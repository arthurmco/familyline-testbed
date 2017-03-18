#include "PathFinder.hpp"
#include <cstring> //memset()

using namespace Tribalia::Logic;

PathFinder::PathFinder(ObjectManager* om)
    : _om(om)
{ }

void PathFinder::InitPathmap(int w, int h)
{
    _mapWidth = w;
    _mapHeight = h;
    _pathing_slots = new unsigned char[w*h];
}
void PathFinder::UpdatePathmap(int w, int h, int x, int y)
{
    auto objList = _om->GetObjectList();

    memset((void*)_pathing_slots, 0, w*h*sizeof(unsigned char));
    
    for (auto& obj : *objList) {
	LocatableObject* l = dynamic_cast<LocatableObject*>(obj.obj);
	if (!l) {
	    continue; // Not locatable
	}

	int ox = l->GetX(), oy = l->GetY(), oz = l->GetZ();
	float r = l->GetRadius();

	for (int y = oz-r; y < oz+r; y++) {	    
	    for (int x = ox-r; x < ox+r; x++) {
		_pathing_slots[y*_mapWidth+x] = 0xff;
	    }
	}	
    }
    
}


static inline double CalculateF(PathNode* n) {
    return (n->g + n->h) * n->weight;
}

/* Create neighbors and check if they exist in both open and closed list
   If they don't exist in both lists, add it in open
   If they exist in the open, update its values
   If they exist in the closed, don't add it 
*/   
void PathFinder::CreateNeighbors(PathNode* n, std::list<PathNode*>& lopen,
			    std::list<PathNode*>& lclosed, glm::vec2 from,
			    glm::vec2 to)
{
    for (double y = -1; y <= 1; y++) {
	for (double x = -1; x <= 1; x++) {
	    if (x == 0 && y == 0) continue;
	    
	    glm::vec2 neighbor = glm::vec2(n->pos.x + x, n->pos.y + y);

	    bool isInClosed = false;
	    for (auto& closednode : lclosed) {
		if (closednode->pos == neighbor) {
		    isInClosed = true;
		    break;
		}		
	    }
	    if (isInClosed) continue;

	    bool isInOpen = false;
	    for (auto& opennode : lopen) {
		if (opennode->pos == neighbor) {
		    isInOpen = true;
		    CalculateF(opennode);
		    break;
		}
	    }
	    if (isInOpen) continue;

	    PathNode* n = this->CreateNode(neighbor, from, to);
	    lopen.push_back(n);
	    
	}
    }
    
}

PathNode* PathFinder::CreateNode(glm::vec2 pos, glm::vec2 from, glm::vec2 to)
{
    PathNode* n = new PathNode;
    n->pos = pos;
    n->weight = 1.0;
    n->next = nullptr;
    n->prev = nullptr;

    // use euclidian distance as g
    n->g = glm::sqrt(glm::pow(pos.x - from.x, 2) + glm::pow(pos.y - from.y, 2));

    // use Manhattan distance as h
    n->h = glm::abs(to.x - pos.x) + glm::abs(to.y - pos.y);
    
    n->f = CalculateF(n);
    return n;
}





/* Create a path from 'from' to 'to'. 
   Returns true if a path was viable, in this case there are its nodes in 'nodelist'
   Returns false if a path wasn't viable, returns the nodelist until the most approximated location 

   The path is based on A* algorithm. It should follow it, but probably doesn't
*/
bool PathFinder::MakePath(glm::vec2 from, glm::vec2 to, std::list<PathNode*>& nodelist)
{
    PathNode* node;
    node = CreateNode(from, from, to);

    std::list<PathNode*> lopen, lclosed;
    lclosed.push_back(node);

    while (node->pos != to) {
	printf("%.2f %.2f\n", node->pos.x, node->pos.y);
	
	/* Open the neighbors */	
	this->CreateNeighbors(node, lopen, lclosed, from, to);
	
	PathNode* lowernode = nullptr;
	double lowerf = 9E10;
	/* Check the path with the best 'f' */
	for (auto& opennode : lopen) {
	    if (opennode->f < lowerf) {
		printf("\t next: %.2f %.2f (f: %.4f)\n", opennode->pos.x, opennode->pos.y, lowerf);
		lowerf = opennode->f;
		lowernode = opennode;
	    }
	}

	/* Define nodes' relation */
	lowernode->prev = node;
	node->next = lowernode;
	lclosed.push_back(lowernode);
	lopen.remove_if(
	    [&lowernode](PathNode*& n){return (n->pos == lowernode->pos); });

	node = lowernode;
	
	//TODO: remove lowernode from openlist

	/* Check if we approximately reached the final point
	   If yes, just add the 'final touch' */
	if (glm::abs(node->pos.x - to.x) < 1.0 &&
	    glm::abs(node->pos.y - to.y) < 1.0) {
	    PathNode* final = CreateNode(to, from, to);
	    final->prev = lowernode;
	    lowernode->next = final;
	    lclosed.push_back(lowernode);
	    break;
	}
    }

    printf("-- ok, realigning\n");

    /* Find the correct path */
    for (PathNode* n = lclosed.back(); n; n = n->prev) {
	printf("%p %p\n ", n, n->prev);
	nodelist.push_back(n);
    }
    
    return true;   
}

std::vector<glm::vec2> PathFinder::CreatePath(LocatableObject* o, glm::vec2 destination)
{
    std::vector<glm::vec2> vec;
    std::list<PathNode*> nodelist;

    glm::vec2 from(o->GetX(), o->GetZ());
    printf("from: %.2f %.2f, to %.2f %.2f\n\n",
	   from.x, from.y, destination.x, destination.y);
    
    MakePath(from, destination, nodelist);

    /* Reverse the map */
    for (auto node = nodelist.rbegin(); node != nodelist.rend(); node++) {
	vec.push_back((*node)->pos);
    }

    return vec;
}
