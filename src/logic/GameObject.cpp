/***
    Game object class definition

    Copyright 2016 Arthur M.

***/
#include "GameObject.hpp"

using namespace Tribalia::Logic;

GameObject::GameObject(int oid, int tid, const char* name) :
    _oid(oid),
    _tid(tid),
    _name(name)
{
    _properties = std::map<std::string, void*>();
}

int GameObject::GetObjectID(){ return _oid; }
int GameObject::GetTypeID(){ return _tid; }
const char* GameObject::GetName() { return _name.c_str(); }
void GameObject::SetName(char* name){ _name = std::string{name}; }
