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
    /*    printf("\tFound object within updt square: %s (%d,%d)\n",
            lobj->GetName(), lx, lz); */

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


#define GET_POS_SLOT(pos, w) (&_slots[(int)(pos.y * w + pos.x)])

void PathFinder::AddNeighborsToOpenList(std::list<PathItem*>* open_list,
    std::list<PathItem*>* closed_list, glm::vec2 point,
    glm::vec2 from, glm::vec2 to)
{
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (y == 0 && x == 0) continue; /* Only the neighbors */

            glm::vec2 p = glm::vec2(point.x - x, point.y - y);

            bool isInClosed = false;
            /* Check if the point isn't on the closed list */
            for (auto& pc : *closed_list) {
                if (pc->point == p) {
                    isInClosed = true;
                    break;
                }
            }
            if (isInClosed) continue;

            PathItem* pi = new PathItem(p, GET_POS_SLOT(p, _terr->GetWidth()));

            pi->calculateAStar(from, to);
            //pi->calculateMult(false);
        /*    printf("%f,%f : f=%.3f, g=%.3f, h=%.3f\n",
                pi->point.x, pi->point.y, pi->f, pi->g, pi->h); */
            open_list->push_back(pi);

        }
    }

    open_list->unique([](PathItem* p1, PathItem* p2)
        { return (p1->point == p2->point); });

}


std::vector<glm::vec2> PathFinder::PathFind(glm::vec2 from,
    glm::vec2 to, bool isWaterUnit)
    {
        glm::vec2 pos = from;
        std::list<PathItem*> open_list;
        std::list<PathItem*> closed_list;

        PathItem* pi = new PathItem(pos, GET_POS_SLOT(pos, _terr->GetWidth()));

        pi->calculateAStar(from, to);
        //pi->calculateMult(false);
        closed_list.push_back(pi);

        while (pos != to) {
            open_list.clear();

            /* Fixes some precision shit */
            if (glm::abs(pos.x - to.x) < 1 && glm::abs(pos.y - to.y) < 1) {
                closed_list.push_back(new PathItem(to, GET_POS_SLOT(to, _terr->GetWidth())));
                break;
            }

            int i = (pos.y * _terr->GetWidth() + pos.x);

            /* Get all neighbors to the open list */
            //open_list.clear();
            this->AddNeighborsToOpenList(&open_list, &closed_list, pos, from, to);

            /* Check who had the lower score */
            PathItem* lower = nullptr;

            for (auto pi : open_list) {
                if (!lower) {
                    lower = pi;
                    continue;
                }

                if (pi->f < lower->f) {
                    //printf("(pi)%.3f menor que (lower)%.3f", pi->f, lower->f);
                    lower = pi;
                }
            }

            /*  Add the lower to the closed item, and bind it to the last item
                on there */
            PathItem* plast = closed_list.back();
            lower->prev = plast;
            plast->next = lower;
            /*printf("\nlower: (%.2f,%.2f), plast: (%.2f,%.2f) |",
                lower->point.x, lower->point.y, plast->point.x, plast->point.y); */
            closed_list.push_back(lower);
            pos = lower->point;

        }

        /* Create the vector pathway */
        auto vecp = std::vector<glm::vec2>();

        for (auto& li : closed_list) {
            vecp.push_back(li->point);
        }

        return vecp;
    }
