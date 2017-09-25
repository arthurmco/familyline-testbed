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

unsigned int InputManager::FindEIDForMouseEvent(InputEvent& ev)
{
    /* Check the listeners, in reverse order.
       We want to get the newer ones first */
    for (auto it = listener_map.crbegin(); it != listener_map.crend(); ++it) {
	auto list_item = *it;
	auto other_ev = list_item.second.ev;
	if (ev.eventType == other_ev.eventType &&
	    ev.event.mouseev.button == other_ev.event.mouseev.button &&
	    ev.event.mouseev.status != other_ev.event.mouseev.status)
	    return list_item.first;
    }

    return 0;
}

unsigned int InputManager::FindEIDForKeyEvent(InputEvent& ev)
{
    for (auto it = listener_map.crbegin(); it != listener_map.crend(); ++it) {
	auto list_item = *it;
	auto other_ev = list_item.second.ev;
	if (ev.eventType == other_ev.eventType &&
	    ev.event.keyev.scancode == other_ev.event.keyev.scancode &&
	    ev.event.keyev.status != other_ev.event.keyev.status)
	    return list_item.first;
    }

    return 0;
}

void InputManager::ConvertEvents()
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
	ev.isPaired = false;
	
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

	case SDL_KEYUP:
	    std::remove_if(lastscancodes.begin(), lastscancodes.end(),
			   [&e](int val) { return val == e.key.keysym.sym; });
	    [[fallthrough]];
	    
        case SDL_KEYDOWN:
            ev.eventType = EVENT_KEYEVENT;
	    ev.isPaired = true;

            ev.event.keyev.scancode = e.key.keysym.sym;
            ev.event.keyev.status = (e.key.state == SDL_PRESSED) ?
                KEY_KEYPRESS : KEY_KEYRELEASE;

            if (e.key.repeat > 0) {
                ev.event.keyev.status = KEY_KEYREPEAT;

		if (std::find(lastscancodes.begin(), lastscancodes.end(),
			      ev.event.keyev.scancode) == lastscancodes.end()) {
		    lastscancodes.push_back(ev.event.keyev.scancode);	    
		} else {
		    /* Only let one repeat event pass */
		    continue;		    		    
		}
		
	    }

            ev.event.keyev.char_utf8 = ' ';
	    if (e.key.state == SDL_PRESSED) {
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

	    if ((last_motion_timestamp + 1) > e.motion.timestamp)
		continue;		

	    last_motion_timestamp = e.motion.timestamp;
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
	    ev.isPaired = true;
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

	    if (e.button.state == SDL_PRESSED) {
		ev.eventid = next_eid;
		listener_map[next_eid] = InputListenerMap(ev, nullptr);
	    } else {
		auto eid = FindEIDForMouseEvent(ev);
		if (eid > 0) {
		    ev.eventid = eid;
		} else {
		    ev.eventid = next_eid;
		}
	    }


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

}


/* Receive events and send them to the listener queues. */
void InputManager::Run()
{
    this->ConvertEvents();
    
    /* Send events to appropriate listeners */
    if (_listeners.size() <= 0) return;

    default_listener->SetAccept();
    if (!current_listener) {
	current_listener = default_listener;
    }
   
    while (!_evt_queue.empty()) {
	InputEvent ev = _evt_queue.front();
	_evt_queue.pop();

	/* Resolve paired events */
	for (auto it = listener_map.begin();
	     it != listener_map.end();
	    ) {
	    if (it->first == ev.eventid) {
		if (it->second.l) {
		    it->second.l->OnListen(ev);

		    /* Small fix to solve that weird bug when camera was
		       forever going */
		    default_listener->OnListen(ev);
		}
	    }

	    it++;
	}

	/* Check the avaliable listeners. */
	for (auto& it : _listeners) {
	    if (it.type_mask & ev.eventType) {
		it.listener->OnListen(ev);
		if (!it.listener->GetAcception())
		    continue;

		current_listener = it.listener;

		/* Attribute paired events to the current listener */
		for(auto& list : listener_map) {
		    if (!list.second.l) {
			list.second.l = current_listener;
		    }
		}

		break;
	    }
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
