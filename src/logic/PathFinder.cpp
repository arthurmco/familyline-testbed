#include "PathFinder.hpp"
#include <iterator> //std::advance()
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
        //int ly = lobj->GetY();
        int lz = lobj->GetZ();

        /* Take off everything that isn't on the square */
        if (lx < x || lx > (x+w)) continue;
        if (lz < y || lz > (y+h)) continue;

        int radius = (int)lobj->GetRadius();

	/* This is impossible */
	if (radius > _terr->GetHeight())
	    continue;

	printf("\tFound object within updt square: %s (%d,%d) r:%d\n",
	       lobj->GetName(), lx, lz, radius);

        /* For now, let just assume radius=box side/2 */
        for (int ry = -radius; ry < radius; ry++) {
	    int ay = (lz+ry);
	    if (ay < 0)	continue; 
            if (ry > _terr->GetHeight()) break;
	    
            for (int rx = -radius; rx < radius; rx++) {
                if (rx < 0 || rx > _terr->GetWidth()) continue;
		
                int ax = (lx+rx);
		if (ax < 0)	continue;

		printf("[[%d %d]]\n", ay, ax);
                _slots[ay*_terr->GetWidth()+ax].isObstructed = true;
            }
        }

    }
}


#define GET_POS_SLOT(pos, w) (&_slots[(int)(floor(pos.y) * w + floor(pos.x))])

void PathFinder::AddNeighborsToOpenList(std::list<PathItem*>* open_list,
    std::list<PathItem*>* closed_list, glm::vec2 point,
    glm::vec2 from, glm::vec2 to)
{
    for (int y = -1; y <= 1; y++) {
        for (int x = -1; x <= 1; x++) {
            if (y == 0 && x == 0) continue; /* Only the neighbors */

			/* Checks for the closed list */	
            glm::vec2 p = glm::vec2(point.x + x, point.y + y);
	    bool isclosed = false;
	    for (auto& i : *closed_list) {
		if (i->point.x == p.x && i->point.y == p.y) {
		    isclosed = true;
		    break;
		}
	    }
	    
	    bool isopen = false;
	    for (auto& i : *open_list) {
		if (i->point == p) {
		    isopen = true;
		    break;
		}
	    }
	    
	    if (isclosed || isopen)	continue;
			
            PathItem* pi = new PathItem(p, GET_POS_SLOT(p, _terr->GetWidth()));

	    if (pi->slot->isObstructed) {
		delete pi;
		continue;
	    }
	    
            pi->calculateAStar(from, to);
            pi->calculateMult(false);
        /*    printf("%f,%f : f=%.3f, g=%.3f, h=%.3f\n",
                pi->point.x, pi->point.y, pi->f, pi->g, pi->h); */
			
	    open_list->push_back(pi);

        }
    }

	
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
	double pos_f = pi->f;

        while (pos != to) {
	    
            /* Fixes some precision shit */
            if (glm::abs(pos.x - to.x) < 1 && glm::abs(pos.y - to.y) < 1) {
		PathItem* l = new PathItem{to, GET_POS_SLOT(to, _terr->GetWidth())};
		PathItem* pl = (PathItem*)closed_list.back();
		l->prev = pl;
		pl->next = l;
		l->next = nullptr;
		closed_list.push_back(l);
                break;
            }

            /* Get all neighbors to the open list */
            open_list.clear();
            this->AddNeighborsToOpenList(&open_list, &closed_list, pos, from, to);

            /* Check who had the lower score */
            PathItem* lower = nullptr;
	    PathItem* replace = nullptr;
	    
	    for (auto& pi : open_list) {
		if (!lower) {
                    lower = pi;
                    continue;
                }
		
		/* If we're repeating some point, then make it the point we are now */
		if ((pi->point.x >= (pos.x-1) && pi->point.x <= (pos.x+1) &&
		     pi->point.y > (pos.y-1) && pi->point.y <= (pos.y+1))) {
		    pi->calculateAStar(from, to);
		    replace = pi;
		}
		
                if (pi->h < lower->h) {
                    printf("\n(pi)%.3f < (lower)%.3f (%.2f, %.2f)", pi->f, lower->f,
			   pi->point.x, pi->point.y);
                    lower = pi;
		    if (pos_f > lower->f) {
			continue;
		    }
                }
            }
	    
	    if (replace == lower) {
		replace->next = lower;
		lower->prev = replace;
	    }
	    
            /*  Add the lower to the closed item, and bind it to the last item
                on there */
            PathItem* plast = closed_list.back();
            lower->prev = plast;
            plast->next = lower;
	    //          printf("\nlower: (%.2f,%.2f), plast: (%.2f,%.2f)|\n",
	    //              lower->point.x, lower->point.y, plast->point.x, plast->point.y); 
	    
	    /* Removes it from open list and adds it into closed */
	    for (auto it = open_list.begin(); it != open_list.end(); it++) {
		if ((*it)->point == lower->point) {
//					printf("(%u) - moved out %.f %.f", open_list.size(), lower->point.x, lower->point.y);
		    open_list.erase(it);
		    break;
		}
	    }	
	    
	    closed_list.push_back(lower);
            pos = lower->point;
	    pos_f = lower->f;
        }

        /* Create the vector pathway */
        auto vecp = std::vector<glm::vec2>();
	vecp.reserve(closed_list.size()+1);
	auto item = closed_list.front();
		
        while (item->next) {
	    printf("%p (%.2f %.2f) [%s] -> %p (%.2f %.2f)\n", item, item->point.x, item->point.y, item->slot->isObstructed ? "ob" : "NOB", item->next, item->next->point.x, item->next->point.y);
            vecp.push_back(item->point);
	    item = item->next;
        }


        return vecp;
    }
