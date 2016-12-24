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
InputListener ilt;

bool HumanPlayer::Play(GameContext* gctx){

    _updated = false;
	InputManager::GetInstance()->Run();
    while (InputManager::GetInstance()->GetDefaultListener()->PopEvent(ev)) {
        if (ev.eventType == EVENT_FINISH) {
            return false;
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
                        goto key_flush;

					glm::vec3 p = TerrainRenderer::GraphicalToGameSpace(_ip->GetTerrainProjectedPosition());

                    AttackableObject* c = (AttackableObject*) 
							ObjectFactory::GetInstance()->GetObject(2)->Clone(
											p.x, 0, p.z);
                    this->GetCity()->AddObject(c);
                    printf("Creating %s at %.3f %.3f %.3f\n", c->GetName(), p.x, 1.0f, p.z);


                    int id = gctx->om->RegisterObject(c);
                    printf("%s has id %d now\n", c->GetName(), id);
                    fflush(stdin);
                }
                break;
                case SDLK_e:
                {
                    if (ev.event.keyev.status != KEY_KEYPRESS)
                        goto key_flush;

                    glm::vec3 p = TerrainRenderer::GraphicalToGameSpace(_ip->GetTerrainProjectedPosition());

					AttackableObject* c = (AttackableObject*)
							ObjectFactory::GetInstance()->GetObject(3)->Clone(
									p.x, 0.0f, p.z);
                    this->GetCity()->AddObject(c);
                    printf("Creating %s at %.3f %.3f %.3f\n", c->GetName(), p.x, 1.0f, p.z);


                    int id = gctx->om->RegisterObject(c);
                    printf("%s has id %d now\n", c->GetName(), id);
                    fflush(stdin);
                }
                break;

                case SDLK_r:
                {
                    if (ev.event.keyev.status != KEY_KEYPRESS)
                        goto key_flush;

					LocatableObject* l = _ip->GetIntersectedObject();
                    if (l) {
                        printf("Deleting object %s", l->GetName());
                        gctx->om->UnregisterObject(l);
                    }
                }
                break;

				case SDLK_b :
				{
					if (ev.event.keyev.status != KEY_KEYPRESS)
						goto key_flush;

					this->renderBBs = !this->renderBBs;
				}
				break;
            }

        } else if (ev.eventType == EVENT_MOUSEMOVE) {

        } else if (ev.eventType == EVENT_MOUSEEVENT ) {

            if (ev.event.mouseev.button == MOUSE_LEFT) {
                if (ev.event.mouseev.status == KEY_KEYPRESS)
                    mouse_click = true;
                else
                    mouse_click = false;
            }

            if (ev.event.mouseev.button == MOUSE_RIGHT && _selected_obj) {
                if (ev.event.mouseev.status == KEY_KEYPRESS) {
                    /* Move the object to some position */
                    glm::vec2 to = _ip->GetGameProjectedPosition();

                    glm::vec2 lp = _pf->CreatePath(_selected_obj, to).back();
                    printf("Moved to %.2fx%.2f", lp.x, lp.y);
                    _selected_obj->SetX(lp.x);
                    _selected_obj->SetZ(lp.y);
                    _updated = true;


                }
            }

        }



//            printf("%d %d \n", ev.mousex, ev.mousey);
    key_flush:
		while(0);

    }

    float unit = 3.2f * gctx->elapsed_seconds;


    if (front)
        _cam->AddMovement(glm::vec3(0, 0, -unit));
    else if (back)
        _cam->AddMovement(glm::vec3(0, 0, unit));

    if (left)
        _cam->AddMovement(glm::vec3(-unit, 0, 0));
    else if (right)
        _cam->AddMovement(glm::vec3(unit, 0, 0));

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
