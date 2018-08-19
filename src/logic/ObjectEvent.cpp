#include "ObjectEvent.hpp"
#include "City.hpp"

using namespace Familyline::Logic;


ObjectEvent::ObjectEvent()
    : from(nullptr), to(nullptr), type(ObjectEventType::None)
{}
	
ObjectEvent::ObjectEvent(const GameObject* from, const GameObject* to, ObjectEventType type)
    : from(from), to(to), type(type)
{}

	
ObjectEvent::ObjectEvent(const GameObject* from, City* city)
    : from(from), to(city), type(ObjectCityChanged), city(city)
{}
