#include "Material.hpp"

using namespace Tribalia::Graphics;

Material::Material(int ID, const char* name, MaterialData data)
    :_ID(ID), _name{name}, _data(data)
{
	_tex = nullptr;
}

int Material::GetID() const { return _ID; }
const char* Material::GetName() const { return _name.c_str(); }
MaterialData* Material::GetData() { return &_data; }

Texture* Material::GetTexture() { return _tex; }
void Material::SetTexture(Texture* tex) { _tex = tex; }
