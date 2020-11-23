#pragma once

#include <common/logic/game_event.hpp>
#include <string>

class TestEventReceiver : public familyline::logic::EventReceiver
{
private:
    std::string name_;
    
public:
    TestEventReceiver(std::string name)
        : name_(name)
        {}
    
    virtual const std::string getName() { return name_; }
};
