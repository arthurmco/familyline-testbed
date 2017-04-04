#include "HumanPlayer.hpp"
#include "graphical/meshopener/OBJOpener.hpp"


using namespace Tribalia;
using namespace Tribalia::Graphics;
using namespace Tribalia::Logic;
using namespace Tribalia::Input;

HumanPlayer::HumanPlayer(const char* name, int elo, int xp)
    : Player(name, elo, xp)
    {
        /* Initialize input subsystems */
		srand((size_t)name*elo);

         /* Create a city for it */
         City* c = new City{name, nullptr};
         AddCity(c);
    }


void HumanPlayer::SetCamera(Tribalia::Graphics::Camera* c) { _cam = c;}
void HumanPlayer::SetPicker(Tribalia::Input::InputPicker* ip) { _ip = ip; }
void HumanPlayer::SetPathfinder(Tribalia::Logic::PathFinder* p) { _pf = p; }

/***
    Virtual function called on each iteration.

    It allows player to decide its movement
    (input for humans, AI decisions for AI... )

    Returns true to continue its loop, false otherwise.
***/

#include <cstdio>

InputEvent ev;
bool front = false, back = false;
bool left = false, right = false;
bool rotate_left = false, rotate_right = false;
bool mouse_click = false;
bool exit_game = false;

bool attack_set = false, attack_ready = false;

bool build_tent = false, build_tower = false;
bool remove_object = false;

InputListener ilt;

bool HumanPlayer::ProcessInput()
{
    _updated = false;
    InputManager::GetInstance()->Run();
    
    while (InputManager::GetInstance()->GetDefaultListener()->PopEvent(ev)) {
        if (ev.eventType == EVENT_FINISH) {
	    exit_game = true;
            return true;
        }


        if (ev.eventType == EVENT_KEYEVENT) {
            switch (ev.event.keyev.scancode) {
	    case SDLK_w:
		if (ev.event.keyev.status == KEY_KEYPRESS)
		    front = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    front = false;
                break;
	    case SDLK_s:
		if (ev.event.keyev.status == KEY_KEYPRESS)
		    back = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    back = false;
                break;
	    case SDLK_a:
		if (ev.event.keyev.status == KEY_KEYPRESS)
		    left = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    left = false;
                break;
	    case SDLK_d:
		if (ev.event.keyev.status == KEY_KEYPRESS)
                        right = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    right = false;
                break;
	    case SDLK_LEFT:
		if (ev.event.keyev.status == KEY_KEYPRESS)
		    rotate_left = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    rotate_left = false;
                break;
	    case SDLK_RIGHT:
		if (ev.event.keyev.status == KEY_KEYPRESS)
		    rotate_right = true;
		else if (ev.event.keyev.status == KEY_KEYRELEASE)
		    rotate_right = false;
                break;
		
		
	    case SDLK_c:
	    {
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    return false;

		build_tent = true;
	    }
	    break;
	    case SDLK_e:
	    {
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    return false;

		build_tower = true;
	    }
	    break;
	    case SDLK_r:
	    {
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    return false;

		remove_object = true;
	    }
	    break;
	    
	    case SDLK_b :
	    {
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    return false;
		
		this->renderBBs = !this->renderBBs;
	    }
	    break;

	    case SDLK_k:
		attack_set = true;
		break;
            }
	    
        } else if (ev.eventType == EVENT_MOUSEEVENT ) {
	    
            if (ev.event.mouseev.button == MOUSE_LEFT) {
		attack_set = false;
                if (ev.event.mouseev.status == KEY_KEYPRESS)
                    mouse_click = true;
                else
                    mouse_click = false;
            }
	    
            if (ev.event.mouseev.button == MOUSE_RIGHT && _selected_obj) {
                if (ev.event.mouseev.status == KEY_KEYPRESS) {

		    if (!attack_set) {
		    
			/* Move the object to some position */
			glm::vec2 to = _ip->GetGameProjectedPosition();
		    
			auto path = _pf->CreatePath(_selected_obj, to);
			glm::vec2 lp = path.back();
			printf("Moved to %.2fx%.2f", lp.x, lp.y);
					
			ObjectPathManager::getInstance()->AddPath(
			    _selected_obj, &path);
		    
			//_selected_obj->SetX(lp.x);
			//_selected_obj->SetZ(lp.y);
			_updated = true;
		    } else {
			attack_ready = true;
			attack_set = false;
		    }
		    

                }
            }
	    
        }
    }
    
    return true;

}

bool HumanPlayer::Play(GameContext* gctx)
{
    if (exit_game)
	return false;
    
    float unit = 5.0f * gctx->elapsed_seconds;
    float rot_sin = glm::sin(_cam->GetRotation());
    float rot_cos = glm::cos(_cam->GetRotation());
    
    if (front)
        _cam->AddMovement(glm::vec3
			  (-(unit*rot_sin), 0, -(unit*rot_cos)));
    else if (back)
        _cam->AddMovement(glm::vec3(
			      unit*rot_sin, 0, unit*rot_cos));
    
    if (left)
        _cam->AddMovement(glm::vec3(
			      -(unit*rot_cos), 0, (unit*rot_sin)));
    else if (right)
        _cam->AddMovement(glm::vec3(
			      unit*rot_cos, 0, -(unit*rot_sin)));
    
    if (rotate_left)
        _cam->AddRotation(glm::vec3(0, 1, 0), glm::radians(1.0f));
    else if (rotate_right)
        _cam->AddRotation(glm::vec3(0, 1, 0), glm::radians(-1.0f));


    LocatableObject* l = _ip->GetIntersectedObject();
    if (l) {
        if (mouse_click) {
            _selected_obj = l;
        }
	//printf("intersected with %s\n", l->GetName());
    } else {
        if (mouse_click)   _selected_obj = nullptr;
    }

    if (build_tent) {
	
	glm::vec3 p = TerrainRenderer::GraphicalToGameSpace(_ip->GetTerrainProjectedPosition());
	
	AttackableObject* c = (AttackableObject*) 
	    ObjectFactory::GetInstance()->GetObject(2)->Clone(
		p.x, p.y, p.z);
	this->GetCity()->AddObject(c);
	printf("Creating %s at %.3f %.3f %.3f\n", c->GetName(), p.x, 1.0f, p.z);
	
	int id = gctx->om->RegisterObject(c);
	printf("%s has id %d now\n", c->GetName(), id);
	build_tent = false;
    }

    if (build_tower) {
	glm::vec3 p = TerrainRenderer::GraphicalToGameSpace(_ip->GetTerrainProjectedPosition());
	
	AttackableObject* c = (AttackableObject*)
	    ObjectFactory::GetInstance()->GetObject(3)->Clone(
		p.x, p.y, p.z);
	this->GetCity()->AddObject(c);
	printf("Creating %s at %.3f %.3f %.3f\n", c->GetName(), p.x, 1.0f, p.z);
	
	int id = gctx->om->RegisterObject(c);
	printf("%s has id %d now\n", c->GetName(), id);
	build_tower = false;
    }

    if (remove_object) {	
	LocatableObject* l = _ip->GetIntersectedObject();
	if (l) {
	    printf("Deleting object %s", l->GetName());
	    
	    gctx->om->UnregisterObject(l);
	}
	remove_object = false;
    }

    if (attack_ready) {
	AttackableObject* ao = dynamic_cast<AttackableObject*>(_ip->GetIntersectedObject());

	if (ao) {
	    AttackableObject* sel = dynamic_cast<AttackableObject*>(_selected_obj);
	    if (sel && sel->CheckAttackRange(ao)) {
		float f = sel->Hit(ao);
		printf("%s dealt %.3f damage on %s\n", sel->GetName(), f,
		       ao->GetName());
		       
	    }
	} 

	attack_ready = false;
	
    }
    
    return true;

}



LocatableObject* HumanPlayer::GetSelectedObject()
{
    return _selected_obj;
}

bool HumanPlayer::HasUpdatedObject()
{
    return _updated;
}

HumanPlayer::~HumanPlayer()
{

}
