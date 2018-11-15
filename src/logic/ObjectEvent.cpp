#include "ObjectEvent.hpp"
#include "City.hpp"

using namespace familyline::logic;


ObjectEvent::ObjectEvent()
    : from(std::weak_ptr<GameObject>()),
      to(std::weak_ptr<GameObject>()),
      type(ObjectEventType::EventNone)
{}
	
ObjectEvent::ObjectEvent(std::weak_ptr<GameObject> from,
			 std::weak_ptr<GameObject> to,
			 ObjectEventType type)
    : from(from), to(to), type(type)
{}

	
ObjectEvent::ObjectEvent(std::weak_ptr<GameObject> from, City* city)
    : from(from),
      to(std::weak_ptr<GameObject>()),
      type(ObjectCityChanged), city(city)
{}


ObjectEvent::ObjectEvent(std::weak_ptr<GameObject> from, ObjectState oldstate,
			 ObjectState newstate)
    : from(from), type(ObjectStateChanged),
      oldstate(oldstate), newstate(newstate)
{}
