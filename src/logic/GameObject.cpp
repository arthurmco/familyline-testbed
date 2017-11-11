
/***
    Game object class definition

    Copyright 2016 Arthur M.

***/
#include "GameObject.hpp"

using namespace Tribalia::Logic;

GameObject::GameObject(int oid, int tid, const char* name) :
    GameObject(oid, tid, name, 0, 0, 0) {}

GameObject::GameObject(int oid, int tid, const char* name,
		       float x, float y, float z) :
    _oid(oid),
    _tid(tid),
    _name(name),
    _xPos(x), _yPos(y), _zPos(z)
{
    _properties = new std::map<std::string, void*>();
    _sizemap = new std::map<std::string, size_t>();
}

/* Copy an object, alongside with its properties, to another
   object */
void GameObject::CopyObject(GameObject* dst, GameObject& src)
{
    dst->_oid = -1;
    dst->_tid = src._tid;
    dst->_name = src._name;
    dst->_xPos = src._xPos;
    dst->_yPos = src._yPos;
    dst->_zPos = src._zPos;
    dst->_radius = src._radius;
//    dst->_properties = new std::map<std::string, void*>();
//    dst->_sizemap = new std::map<std::string, size_t>();

    for (auto it : (*src._properties)) {

	/* Do not copy some properties */
	if (it.first == "mesh")
	    continue;
	
	size_t s = src._sizemap->at(it.first);

	void* data = (void*)new char[s];
	memcpy(data, (void*)it.second, s);

	dst->_properties->emplace(it.first, data);
	dst->_sizemap->emplace(it.first, s);
    } 

}

GameObject::GameObject(GameObject& o)
{
    CopyObject(this, o);
}

GameObject::~GameObject()
{
    this->_properties->clear();

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

/* Returns true if a property exists */
bool GameObject::HasProperty(const char* name)
{
    std::string sname{name};

    return (_properties->find(sname) != _properties->end());
}
