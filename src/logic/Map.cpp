#include "Map.hpp"

using namespace Tribalia::Logic;

Map::Map(int width, int height)
{
    _width = width;
    _height = height;
    _map = new MapSlot[width*height];
}
Map::Map(int width, int height, MapSlot* slot)
{
    _width = width;
    _height = height;
    _map = slot;
}

MapSlot* Map::GetMapData()
{
    return _map;
}

void Map::SetName(const char* name)
{
    _name = std::string{name};
}

const char* Map::GetName() const
{
    return _name.c_str();
}

int Map::GetWidth() const
{
    return _width;
}

int Map::GetHeight() const
{
    return _height;
}
