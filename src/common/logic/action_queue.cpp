#include <common/logic/action_queue.hpp>

#include <algorithm>

using namespace familyline::logic;

void ActionQueue::addEmitter(EventEmitter* e)
{
	//printf("\tadded event emitter %s\n", e->getName().c_str());

	e->queue = this;
}

void ActionQueue::addReceiver(EventReceiver* r, std::initializer_list<EventType> events)
{
	ReceiverData rd;
	rd.events = events;
	rd.receiver = r;

	//printf("\tadded event receiver %s\n", r->getName().c_str());


	this->receivers.push_back(rd);
}


void ActionQueue::removeReceiver(EventReceiver* r)
{
    auto newend = std::remove_if(
        receivers.begin(), receivers.end(),
        [r](ReceiverData rec) {
            return (rec.receiver->getName() == r->getName());
        });
    receivers.erase(newend);
}

void ActionQueue::removeEmitter(EventEmitter* e)
{
    e->queue = nullptr;
}
        


void ActionQueue::pushEvent(const Event& e)
{
	const char* evnames[] = {"null", "ObjectCreated", "ObjectDestroyed", "ObjectMoved", "ObjectStateChanged", "ObjectAttack"};

	//printf("event received: %s (%#x), timestamp=%lld id=%d name='%s'\n", evnames[e.type], e.type,
	//	e.timestamp, e.object.id, e.object.name.c_str());
	this->events.push(e);
}

void ActionQueue::processEvents()
{
	while (!this->events.empty()) {
		Event e = this->events.front();

        // printf("event %x !\n", e.type);

		for (auto& rec : this->receivers) {
			if (std::find(rec.events.begin(), rec.events.end(), e.type) != rec.events.end()) {

                //printf("\t received by %s\n", rec.receiver->getName().c_str());
				rec.receiver->pushEvent(e);
			}
		}

		this->events.pop();
	}
}
