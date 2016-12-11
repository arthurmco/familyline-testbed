/***
    Path finder class.

    Copyright (C) 2016 Arthur M

***/

#include "Terrain.hpp"
#include "ObjectManager.hpp"
#include "LocatableObject.hpp"

#include <vector>
#include <glm/glm.hpp>

#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

namespace Tribalia {
namespace Logic {

/*  The slot used by pathfinder.
    Same thing, but with dificulty levels instead of elevation and type */
struct PathFinderSlot {
    /* Elevation points. Normal is 1.0. More if terrain is steep */
    double elevation_points;

    /* Difficulty for land units. 1.0 if land, more if terrain is hard to walk */
    double terrain_land_points;

     /* Difficulty for water units.
        Normal is 0.0. Land is 1.0. Water/land mixture is between these values */
    double terrain_water_points;

    /* True if there's something in your way */
    bool isObstructed;
};

/* Representation of a map, ready for A*-algorithm */
struct PathMap {

    int width, height;
    struct PathMapElement {
        double f;
        double g,h;
    } *map;

    /* Create the pathmap */
    PathMap(int w, int h){
        map = new PathMapElement[w*h];
        width = w;
        height = h;
    }

    /* Delete the pathmap*/
    ~PathMap() {
        delete map;
    }
};


class PathFinder {
private:
    Terrain* _terr;
    ObjectManager* _om;

    /* Path finder slots for generated data, in the whole map */
    PathFinderSlot* _slots;

    /* Helper subfunctions */

    /*  Calculates the pathmap from the point 'from' to the point 'to', in
        game coordinates
        Get an array of points, who is the path for you get to 'to' from 'from'.
        Both 'to' and 'from' are removed from the final list
    */
    std::vector<glm::vec2> PathFind(glm::vec2 from, glm::vec2 to, bool isWaterUnit);

public:
    PathFinder(Terrain* t, ObjectManager* om);

    /*  Update the pathfinder slot list, for an determined region
        Note that this will only update the buildings, because terrain is
        immutable in this engine.
    */
    void UpdateSlotList(int x, int y, int w, int h);

    /*  Create a path */
    std::vector<glm::vec2> CreatePath(LocatableObject* from, glm::vec2 to);


};

}
}



#endif /* end of include guard: PATHFINDER_HPP */
