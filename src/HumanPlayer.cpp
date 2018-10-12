#include "HumanPlayer.hpp"
#include "graphical/meshopener/OBJOpener.hpp"


using namespace Familyline;
using namespace Familyline::Graphics;
using namespace Familyline::Logic;
using namespace Familyline::Input;

HumanPlayer::HumanPlayer(const char* name, int xp, GameActionManager* gam)
    : Player(name, xp, gam)
    {
        /* Initialize input subsystems */
	srand((size_t)name*xp);

    }


void HumanPlayer::SetCamera(Familyline::Graphics::Camera* c) { _cam = c;}
void HumanPlayer::SetPicker(Familyline::Input::InputPicker* ip) { _ip = ip; }
void HumanPlayer::SetPathfinder(Familyline::Logic::PathFinder* p) { _pf = p; }

bool build_tent = false, build_tower = false;

void HumanPlayer::SetGameActionManager(Familyline::Logic::GameActionManager* gam)
{
    this->_gam = gam;

    /* Create a city for it */
    City* c = new City{this, glm::vec3(1, 0, 0)};
    AddCity(c);
}


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


bool remove_object = false;

bool zoom_in = false;
bool zoom_out = false;
bool zoom_mouse = false;
bool build_something = false;

std::weak_ptr<AttackableObject> attacker, attackee;

InputListener ilt;

bool HumanPlayer::ProcessInput()
{
    _updated = false;
    InputManager::GetInstance()->Run();

    if (zoom_mouse) {
	zoom_in = false;
	zoom_out = false;
	zoom_mouse = false;
    }

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

	    case SDLK_PLUS:
	    case SDLK_KP_PLUS:
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    zoom_in = false;
		else
		    zoom_in = true;

		break;

	    case SDLK_MINUS:
	    case SDLK_KP_MINUS:
		if (ev.event.keyev.status != KEY_KEYPRESS)
		    zoom_out = false;
		else
		    zoom_out = true;
		break;



            }

        } else if (ev.eventType == EVENT_MOUSEEVENT ) {
	    if (attack_set && attack_ready) { attack_set = false; }

	    if (ev.event.mouseev.scrolly > 0) {
		zoom_in = true;
		zoom_out = false;
		zoom_mouse = true;
	    }

	    if (ev.event.mouseev.scrolly < 0) {
		zoom_out = true;
		zoom_in = false;
		zoom_mouse = true;
	    }


            if (ev.event.mouseev.button == MOUSE_LEFT) {
	        attack_set = false;
                if (ev.event.mouseev.status == KEY_KEYPRESS)
                    mouse_click = true;
                else
                    mouse_click = false;
            }

            if (ev.event.mouseev.button == MOUSE_RIGHT && !_selected_obj.expired()) {
                if (ev.event.mouseev.status == KEY_KEYPRESS) {

		    if (!attack_set) {
			attack_ready = false;

			/* Move the object to some position */
			glm::vec2 to = _ip->GetGameProjectedPosition();
			auto slock = _selected_obj.lock();

			auto path = _pf->CreatePath(slock.get(), to);
			glm::vec2 lp = path.back();
			Log::GetLog()->InfoWrite("human-player",
						 "moved to %.2fx%.2f", lp.x, lp.y);

			ObjectPathManager::getInstance()->AddPath(slock.get(), &path);

			this->RegisterMove(slock.get(), to);
			_updated = true;
		    } else {
			attack_ready = true;
			attack_set = true;
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

    if (mouse_click && build_something) {
	if (BuildQueue::GetInstance()->GetNext()) {
	    glm::vec3 p = ::GraphicalToGameSpace(_ip->GetTerrainProjectedPosition());

	    auto* build = BuildQueue::GetInstance()->BuildNext(p);
	    AttackableObject* c = dynamic_cast<AttackableObject*>(build);

	    if (c) {
		BuildQueue::GetInstance()->Clear();
		build->position.y = p.y;

                // the object will be added to the city
		Log::GetLog()->InfoWrite("human-player",
					 "creating %s at %.3f %.3f %.3f", c->getName(), p.x, p.y, p.z);

		auto cobj = gctx->om->addObject(c).lock();
		this->RegisterCreation(cobj.get());
		Log::GetLog()->InfoWrite("human-player",
					 "%s has id %d now", c->getName(), cobj->getID());

	    }

	}
    }



    auto l = _ip->GetIntersectedObject().lock();
    if (l) {
        if (mouse_click) {
            _selected_obj = std::dynamic_pointer_cast<AttackableObject>(l);
        }
	//printf("intersected with %s\n", l->GetName());
    } else {
        if (mouse_click)   _selected_obj = std::weak_ptr<AttackableObject>();
    }

    if (build_tent) {
	BuildQueue::GetInstance()->Add( (AttackableObject*)
	    ObjectFactory::GetInstance()->GetObject("tent", 0, 0, 0));

	build_tent = false;
	build_something = true;
    }

    if (build_tower) {
	BuildQueue::GetInstance()->Add( (AttackableObject*)
	    ObjectFactory::GetInstance()->GetObject("watchtower", 0, 0, 0));

	build_tower = false;
	build_something = true;
    }

    if (remove_object) {
	auto l = _ip->GetIntersectedObject().lock();
	if (l) {
	    printf("Deleting object %s", l->getName());
	    this->RegisterDestroy(l.get());
	    gctx->om->removeObject(l);
	}
	remove_object = false;
    }

    if (attack_ready) {
	auto l = _ip->GetIntersectedObject().lock();
	if (l && attack_set)
	    attackee = std::dynamic_pointer_cast<AttackableObject>(l);

	if (!attackee.expired()) {

	    if (attack_set)
		attacker = std::dynamic_pointer_cast<AttackableObject>(_selected_obj.lock());

	    if (!attacker.expired()) {
		CombatManager::getDefault()->AddAttack(attacker, attackee);
		this->RegisterAttack(attacker.lock().get(), attackee.lock().get());
		attack_ready = false;
	    }

	} else {
	    attack_ready = false;
	    attacker = attackee = std::weak_ptr<AttackableObject>();
	}
    }

    if (zoom_in || zoom_out) {
	float zfac = (gctx->elapsed_seconds * 0.25);

	if (zoom_mouse)
	    zfac *= 5;

	float z = _cam->GetZoomLevel();

	if (zoom_in)
	    z += zfac;

	if (zoom_out)
	    z -= zfac;

	_cam->SetZoomLevel(z);
    }


    return true;

}



AttackableObject* HumanPlayer::GetSelectedObject()
{
    return (_selected_obj.expired() ? nullptr : _selected_obj.lock().get());
}

bool HumanPlayer::HasUpdatedObject()
{
    return _updated;
}

HumanPlayer::~HumanPlayer()
{

}
