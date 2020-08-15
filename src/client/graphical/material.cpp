#include <client/graphical/material.hpp>

using namespace familyline::graphics;

int lastID = 0;
Material::Material(int ID, const char* name, MaterialData data) : _ID(ID), _name{name}, _data(data)
{
    if (ID > lastID) lastID = ID + 1;

    _tex = nullptr;
}

Material::Material(const char* name, MaterialData data) : _name{name}, _data(data)
{
    _tex = nullptr;
    _ID  = lastID++;
}

int Material::getID() const { return _ID; }
const char* Material::getName() const { return _name.c_str(); }
MaterialData& Material::getData() { return _data; }

Texture* Material::getTexture() { return _tex; }
void Material::setTexture(Texture* tex) { _tex = tex; }
