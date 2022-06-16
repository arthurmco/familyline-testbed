#include <client/graphical/material.hpp>
#include <optional>

using namespace familyline::graphics;

int lastID = 0;
Material::Material(int ID, const char* name, MaterialData data) : _ID(ID), _name{name}, _data(data)
{
    if (ID > lastID) lastID = ID + 1;

    tex_ = std::nullopt;
}

Material::Material(const char* name, MaterialData data) : _name{name}, _data(data)
{
    tex_ = std::nullopt;
    _ID  = lastID++;
}

int Material::getID() const { return _ID; }
const char* Material::getName() const { return _name.c_str(); }
MaterialData& Material::getData() { return _data; }

std::optional<TextureHandle> Material::getTexture() { return tex_; }
void Material::setTexture(std::optional<TextureHandle> tex) { tex_ = tex; }
