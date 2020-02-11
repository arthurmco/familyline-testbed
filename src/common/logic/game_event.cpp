#include <common/logic/game_event.hpp>

#include <chrono>
#include <common/logic/action_queue.hpp>

using namespace familyline::logic;

Event::Event(EventType t)
{
	this->timestamp = -1;
	this->emitter = nullptr;
	this->type = t;
	switch (t) {
	case ObjectCreated:
	case ObjectDestroyed:
	case ObjectMoved:
	case ObjectStateChanged:
		this->object = {};
		this->object.name = std::string{};
		break;

	case ObjectAttack:
		this->attack.name = std::string{};
		break;
	}
}

Event::Event(const Event& other)
{
	this->attack = other.attack;
	this->emitter = other.emitter;
	this->object = other.object;
	this->timestamp = other.timestamp;
	this->type = other.type;
}

void EventEmitter::pushEvent(Event& e)
{
	auto epoch = std::chrono::duration_cast<std::chrono::microseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();

	e.timestamp = epoch;
	e.emitter = this;

	this->queue->pushEvent(e);
}


bool EventReceiver::pollEvent(Event& e)
{
	if (events.empty())
		return false;

	Event tmp = events.front();
	events.pop();
	e = tmp;
	return true;
}


void EventReceiver::pushEvent(Event& e)
{
	events.push(e);
}
