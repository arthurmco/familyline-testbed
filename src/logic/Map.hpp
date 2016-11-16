/***
    Map positioning class

    Copyright (C) 2016 Arthur M

***/

#include <string>

#ifndef MAP_HPP
#define MAP_HPP

namespace Tribalia {
namespace Logic {

struct MapSlot {
    int16_t elevation;
    uint16_t tile_type;
};

class Map {
private:
    /*  Size, in map blocks.
        The size is defined into a map renderer class.
     */
    int _width, _height;

    MapSlot* _map;

    /* Map name */
    std::string _name;

public:
    Map(int width, int height);
    Map(int width, int height, MapSlot* slot);

    MapSlot* GetMapData();

    void SetName(const char* name);
    const char* GetName() const;

    int GetWidth() const;
    int GetHeight() const;

};

}
}



#endif /* end of include guard: MAP_HPP */
