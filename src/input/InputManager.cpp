
#include "InputManager.hpp"

using namespace Tribalia::Input;

InputManager* InputManager::im = nullptr;


void InputManager::Initialize() {
    if (!default_listener) {
	default_listener = new InputListener;
	this->AddListener(EVENT_ALL_EVENTS, default_listener, 0.0001);
    }
}

InputListener* InputManager::GetDefaultListener() { return default_listener; }

/* Get the top event (not taking it off the queue).
    Return false if no elements on queue */
bool InputManager::GetEvent(InputEvent* ev)
{
    if (_evt_queue.empty())
        return false;

    *ev = _evt_queue.front();

    // printf("\t event: ");
    // switch (ev->eventType) {
    //     case EVENT_MOUSEEVENT:
    //         printf("mouse, button %d , status %d ",
    //             ev->event.mouseev.button, ev->event.mouseev.status);
    //         break;
    //
    //     case EVENT_KEYEVENT:
    //         printf("keyboard, scancode %d (char %c), status %d",
    //             ev->event.keyev.scancode,
    //             (char)ev->event.keyev.char_utf8 & 0xff,
    //             ev->event.keyev.status);
    //         break;
    //
    //     case EVENT_FINISH:
    //         printf("finish requested");
    //         break;
    //
    //     default:
    //         printf("unknown");
    //         break;
    // }
    //
    // printf(", mouse coords (%d %d %d)\n",
    //     ev->mousex, ev->mousey, ev->mousez);

    return true;
}

/* Pop off the top element of the queue
    Return false if there's no element to pop off */
bool InputManager::PopEvent(InputEvent* ev)
{

    if (_evt_queue.empty())
        return false;

    if (ev)
        *ev = _evt_queue.front();

    _evt_queue.pop();
    return true;
}

unsigned int InputManager::FindEIDForKeyEvent(InputEvent& ev)
{
    for (const auto& list_item : listener_map) {
	auto other_ev = list_item.second.ev;
	if (ev.eventType == other_ev.eventType &&
	    ev.event.keyev.scancode == other_ev.event.keyev.scancode &&
	    ev.event.keyev.status != other_ev.event.keyev.status)
	    return list_item.first;
    }

    return 0;
}

int lastx, lasty, lastz;

/* Receive events and send them to queues */
void InputManager::Run()
{
    /* Get event data from SDL */
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
	auto next_eid = ++_last_eid;
	if (next_eid == 0) {
	    Log::GetLog()->Warning("input-manager",
				   "Event ID overflew");
	}
	
        InputEvent ev = {};
        switch (e.type) {
        case SDL_QUIT:
        case SDL_APP_TERMINATING:
	    ev.eventid = next_eid;
            ev.eventType = EVENT_FINISH;
            break;

        case SDL_WINDOWEVENT:
	    ev.eventid = next_eid;
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                ev.eventType = EVENT_FINISH;
            else
                continue;
            break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:    
            ev.eventType = EVENT_KEYEVENT;

            ev.event.keyev.scancode = e.key.keysym.sym;
            ev.event.keyev.status = (e.key.state == SDL_PRESSED) ?
                KEY_KEYPRESS : KEY_KEYRELEASE;
            if (e.key.repeat > 0)
                ev.event.keyev.status = KEY_KEYREPEAT;

            ev.event.keyev.char_utf8 = ' ';
	    if (e.type == SDL_KEYDOWN) {
		ev.eventid = next_eid;
		listener_map[next_eid] = InputListenerMap(ev, nullptr);
	    } else {
		auto eid = FindEIDForKeyEvent(ev);
		if (eid > 0) {
		    ev.eventid = eid;
		} else {
		    ev.eventid = next_eid;
		}
	    }

            break;
        case SDL_MOUSEMOTION:
	    ev.eventid = next_eid;
            /* Only update the last* variables */
            lastx = e.motion.x;
            lasty = e.motion.y;
            lastz = 0;
            ev.eventType = EVENT_MOUSEMOVE;
            break;
	case SDL_MOUSEWHEEL:
	    ev.eventid = next_eid;
	    ev.eventType = EVENT_MOUSEEVENT;
	    ev.event.mouseev.scrolly = e.wheel.y;
	    break;	    
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
	    ev.eventid = next_eid;
            ev.eventType = EVENT_MOUSEEVENT;
            //i will not depend of hacks.
            switch (e.button.button) {
	    case SDL_BUTTON_LEFT:
		ev.event.mouseev.button = MOUSE_LEFT;
		break;
	    case SDL_BUTTON_MIDDLE:
		ev.event.mouseev.button = MOUSE_MIDDLE;
		break;
	    case SDL_BUTTON_RIGHT:
		ev.event.mouseev.button = MOUSE_RIGHT;
		break;
	    default:
		ev.event.mouseev.button = e.button.button - MOUSE_LEFT;
            }

            lastx = e.button.x;
            lasty = e.button.y;

            ev.event.mouseev.status = (e.button.state == SDL_PRESSED) ?
                KEY_KEYPRESS : KEY_KEYRELEASE;

            if (e.button.clicks % 2 == 0)
                ev.event.keyev.status = KEY_KEYREPEAT;

            break;
        default:
            continue;
	}

        ev.mousex = lastx;
        ev.mousey = lasty;
        ev.mousez = lastz;

        _evt_queue.push(ev);

	if (_evt_queue.size() > MAX_INPUT_QUEUE) {
	    _evt_queue.pop();
	}

    }

    /* Send events to appropriate listeners */
    if (_listeners.size() <= 0) return;

    if (!current_listener)
	current_listener = default_listener;
    
    while (!_evt_queue.empty()) {
        InputEvent ev = _evt_queue.front();
        bool event_received = false;

        for (auto& l : _listeners) {
	    
	    /* We'll change the current listener
	       Flush all pending events for this listener */
	    for (auto it = listener_map.begin();
		 it != listener_map.end();
		 ++it) {
		InputListenerMap m = it->second;
		if (m.l == l.listener) {
		    m.l->OnListen(ev);
			printf("Erasing %d\n", it->first);
			listener_map.erase(it->first);
		}
		
	    }
	
	
	    event_received = false;
            if (l.type_mask & ev.eventType) {
                l.listener->OnListen(ev);
                event_received = true;
	    } else {
		continue;
	    }

	    if (l.listener->GetAcception() && event_received) {
		
		current_listener = l.listener;

		/* Check the pending event and see if the event is pending but
		   isn't 'closed', i.e, the 'starting' event has no 'ending'
		   event (like a keydown isn't accompained with a keyup event) */
		auto listmap = listener_map.find(ev.eventid);
		if (listmap != listener_map.end()) {
		    if (listmap->first == ev.eventid) {
			if (!listmap->second.l) {
			    printf("\t registered (id %d) to listener %p\n",
				   ev.eventid, current_listener);
			    listmap->second.l = l.listener;
			}
		    }
		}
	    }

        }

        if (event_received) {
            _evt_queue.pop();
	}
    }

}


void InputManager::AddListener(int types, InputListener* listener, float order)
{
    InputListenerData ild;
    ild.type_mask = types;
    ild.listener = listener;
    ild.order = order;
    Log::GetLog()->Write("input-manager",
			 "Adding listener order %.3f for event mast %#x",
			 order, types);
    _listeners.push_back(ild);

    /* Order by decreasing order number */
    std::sort(_listeners.begin(), _listeners.end(),
	      [](const InputListenerData& i1, const InputListenerData& i2) {
		  return (i1.order >= i2.order);
	      });
    

}
void InputManager::RemoveListener(InputListener* listener)
{
    for (auto it = _listeners.begin(); it != _listeners.end(); it++) {
        if (it->listener == listener) {
            _listeners.erase(it);
            return;
        }
    }
}

InputListener* InputManager::GetCurrentListener()
{
    return current_listener;
}
