#include "PathFinder.hpp"

using namespace Tribalia::Logic;

PathFinder::PathFinder(Terrain* t, ObjectManager* om)
{
    _terr = t;
    _om = om;

    _slots = new PathFinderSlot[t->GetHeight()*t->GetWidth()];

    /* Load terrain data here */
    for (int y = 0; y < t->GetHeight(); y++) {
        for (int x = 0; x < t->GetWidth(); x++) {
            int i = (y*t->GetWidth())+x;

            _slots[i].elevation_points = 1.0;   /* No elevation yet */
            _slots[i].terrain_land_points = 1.0;    /* No diff terrain types yet */
            _slots[i].terrain_water_points = 0.0;
            _slots[i].isObstructed = false;

        }
    }

    Log::GetLog()->Write("PathFinder: initialized terrain %dx%d, %.3f kB approx.",
        t->GetWidth(), t->GetHeight(), (t->GetWidth()*t->GetHeight()*sizeof(PathFinderSlot)));
}

std::vector<glm::vec2> PathFinder::CreatePath(LocatableObject* from, glm::vec2 to)
{
    glm::vec2 vFrom = glm::vec2(from->GetX(), from->GetZ());
    bool isWater = false;   // Water units unsupported for now.

    Log::GetLog()->Write("[PathFinder] Finding path for %s (%.1fx%.1f) to "
        "(%.1fx%.1f)", from->GetName(), vFrom.x, vFrom.y, to.x, to.y);

    return PathFind(vFrom, to, isWater);
}

/*  Update the pathfinder slot list, for an determined region
    Note that this will only update the buildings, because terrain is
    immutable in this engine.
*/
void PathFinder::UpdateSlotList(int x, int y, int w, int h)
{
    /* Clean every obstruction thing about other slots */
    for (int ry = y; ry < y+h; ry++) {
        for (int rx = x; rx < x+w; rx++) {
            _slots[ry*_terr->GetWidth()+rx].isObstructed = false;
        }
    }

    for (auto& obj : *_om->GetObjectList()) {
        LocatableObject* lobj = dynamic_cast<LocatableObject*>(obj.obj);

        /* Not locatable */
        if (!lobj) continue;

        int lx = lobj->GetX();
        int ly = lobj->GetY();
        int lz = lobj->GetZ();

        /* Take off everything that isn't on the square */
        if (lx < x || lx > (x+w)) continue;
        if (lz < y || lz > (y+h)) continue;

        float radius = lobj->GetRadius();
        printf("\tFound object within updt square: %s (%d,%d)\n",
            lobj->GetName(), lx, lz);

        /* For now, let just assume radius=box side/2 */
        for (int ry = -radius; ry < radius; ry++) {
            if (ry < 0 || ry > _terr->GetHeight()) continue;
            for (int rx = -radius; ry > radius; ry++) {
                if (rx < 0 || rx > _terr->GetWidth()) continue;

                int ay = (ly+ry);
                int ax = (lx+rx);
                _slots[ay*_terr->GetWidth()+ax].isObstructed = true;
            }
        }

    }
}

std::vector<glm::vec2> PathFinder::PathFind(glm::vec2 from,
    glm::vec2 to, bool isWaterUnit)
    {
        return std::vector<glm::vec2>();
    }
