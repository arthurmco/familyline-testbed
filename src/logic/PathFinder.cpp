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

    std::vector<glm::vec2> v;
    bool vend = true;
    int flags;

    int limit = 0;
    do {
	vend = true;
	auto vi = this->PathFind(vFrom, to, isWater, flags);
	v.insert(v.end(), vi.begin(), vi.end());
	
	if (flags & PATHF_LOOP)
	    vend = false; //Path in loop, need to recalculate again

	/* The first item here will always be the previous 'vFrom' so we don't get without
	   items never. */
	vFrom = *(--(vi.end()));
	limit++;

	if (limit > LOOP_POINTS_MAX)
	    break; // prevents infinite loops
	
    } while (!vend);
    
    return v;
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
	if (radius > h)
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

/* Add neighbors to open list.

   Add only those who are not in the list */
void PathFinder::AddNeighborsToOpenList(std::list<PathItem*>* open_list,
					std::list<PathItem*>* closed_list,
					glm::vec2 point, glm::vec2 from,
					glm::vec2 to)
{
    for (auto x = -1; x <= 1; x++) {
	for (auto y = -1; y <= 1; y++) {

	    glm::vec2 p(point.x+x, point.y+y);
	    
	    /* Do not add yourself into the list */
	    if (x == 0 && y == 0)
		continue;

	    bool has_item = false;

	    /* Check if this item is on closed list */
	    for (auto& it : *closed_list) {
		if ((*it) == p) {
		    /* If we find, same thing: recalculate item and break */
		    it->calculateAStar(from, to);
		    it->calculateMult(false);
		    has_item = true;
		    break;
		}
	    }

	    if (has_item) continue;
	    
	    /* Add the item */
	    PathItem* pit = new PathItem(p, GET_POS_SLOT(p, _terr->GetWidth()));
	    pit->calculateAStar(from, to);
	    pit->calculateMult(false);
	    open_list->push_back(pit);
	    
	}
    }
    
}


std::vector<glm::vec2> PathFinder::PathFind(glm::vec2 from,
	   glm::vec2 to, bool isWaterUnit, int& retflags)
{
    this->UpdateSlotList(0, 0, 256, 256);
    
    std::list<PathItem*> open_list;
    std::list<PathItem*> closed_list;

    retflags = 0;
    
    PathItem* now = new PathItem(from, GET_POS_SLOT(from, _terr->GetWidth()));
    now->calculateAStar(from, to);
    now->calculateMult(false);
    closed_list.push_back(now);

    int loop_points = 0;
    bool loop_ovflw = false;
	
    while (now->point != to) {

	open_list.clear();
	
	/* Check if distance is lower than one point.
	   If is, then just add the "final point" there */
	if (glm::abs(now->point.x - to.x) < 1.0 &&
	    glm::abs(now->point.y - to.y) < 1.0) {
	    PathItem* it = new PathItem(to, GET_POS_SLOT(to, _terr->GetWidth()));
	    it->calculateAStar(from, to);
	    it->calculateMult(false);
	    closed_list.push_back(it);
	    printf("over!\n");
	    break;
	}

	
	/* Add neighbors to open list */
	this->AddNeighborsToOpenList(&open_list, &closed_list, now->point, from, to);
	printf("-> %zu : %zu\n", open_list.size(), closed_list.size());

	/* Check who has the lowest f score */
	
	double lower_f = 9e12; // the diagonal?

	auto lower_it = open_list.begin();
	PathItem* lower = nullptr;

	for (auto it = open_list.begin(); it != open_list.end(); ++it) {
	    (*it)->calculateAStar(from, to);
	    (*it)->calculateMult(false);
	    
	    if ((*it)->f < lower_f) {
		lower_f = (*it)->f;
		lower_it = it;
	    }
	}

	/* Remove the lowest from the open list and put it into closed list */
	lower = *lower_it;
	open_list.erase(lower_it);
	lower->prev = nullptr;

	size_t i = 0;
	/* Check if we have had crossed this node before.
	   This helps avoiding zig-zag and 'walking in circle' pathing bugs 
	*/
	for (auto it = closed_list.begin(); it != closed_list.end(); ++it) {
        
	
	    if ((*it) == lower) {
		// Immediate node
		lower = (*it);
		closed_list.erase(it, closed_list.end());
		break;
	    }

	    if (i+3 < closed_list.size()) {

		bool has_found = false;
		
		/* Check neighbors too, but only if this node isn't the last */
		for (int x = -1; x <= 1; x++) {
		    if (has_found) break;
		    
		    for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) continue;

			glm::vec2 pos(lower->point.x+x, lower->point.y+y);
			if (*(*it) == pos) {
			    printf("\n\n!!!!\n\n");

			    /* Recalculate start position.
			       Might help with infinite loops and walking in circle bugs even more, because
			       the A* values would change. */
			    from = pos;
			    loop_points++;
			    printf("-- %d", loop_points);

			    if (loop_points > LOOP_POINTS_MAX) {
				/* We looped to much.
				   Time to give up.

				   TODO: Maybe add some sort of flag to the path, to
				   indicate it broke and we should restart it again */
				printf("Too many breaks\n ");
				to = now->point;
				closed_list.push_back(now);
				has_found = true;
				loop_ovflw = true;
				retflags |= PATHF_LOOP;
				break;
			    }

			    if (loop_ovflw)
				break;
			    
			    
			    (*it)->next = lower;
			    lower->prev = (*it);
			    closed_list.erase(++it, --closed_list.end());
			    has_found = true;
			    break;
			}
		    
			if (loop_ovflw)
			    break;
		    
		    }

		    if (loop_ovflw)
			break;
		}

		if (loop_ovflw)
		    break;
		
		if (has_found) break;

	    }

	    if (loop_ovflw)
		break;

	    i++;

	}

	if (loop_ovflw)
	    break;


	if (!lower->prev) {
	    lower->prev = now;
	    now->next = lower;
	}
	
	now = lower;
	closed_list.push_back(now);
    }
    
    /* Retrieve points and return */
    std::vector<glm::vec2> points;

    for (auto& p : closed_list) {
	points.push_back(p->point);
    }

    return points;
}
