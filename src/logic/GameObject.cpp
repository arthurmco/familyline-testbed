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

GameObject::GameObject(int oid, int tid, const char* name,
		       float x, float y, float z) :
    _oid(oid),
    _tid(tid),
    _name(name),
    _xPos(x), _yPos(y), _zPos(z)
{
    _properties = std::map<std::string, void*>();
}
    

int GameObject::GetObjectID(){ return _oid; }
int GameObject::GetTypeID(){ return _tid; }
const char* GameObject::GetName() { return _name.c_str(); }
void GameObject::SetName(char* name){ _name = std::string{name}; }

void GameObject::SetX(float v) { _xPos = v;  }
float GameObject::GetX() { return _xPos; }

void GameObject::SetY(float v) { _yPos = v; }
float GameObject::GetY() { return _yPos; }

void GameObject::SetZ(float v) { _zPos = v; }
float GameObject::GetZ() { return _zPos; }
