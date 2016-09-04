#include "HumanPlayer.hpp"
#include "graphical/meshopener/OBJOpener.hpp"

using namespace Tribalia;
using namespace Tribalia::Logic;
using namespace Tribalia::Input;

class ConcreteObject : public AttackableObject
{
public:
    ConcreteObject(int oid, const char* name,
        float x, float y, float z) :
    AttackableObject(oid, 2, name,
        x, y, z, 1000, 1.0f, 1.5f)
        {
            Tribalia::Graphics::OBJOpener op;
            this->SetMesh(op.Open("test.obj"));
			this->GetMesh()->GenerateBoundingBox();
        }

    virtual bool Initialize(){
        this->_tid = 2;
        return true;
    }

    virtual bool DoAction(void)
    {
        printf("Iteration (%d %s) \n", _oid, _name.c_str());
        return true;
    }
};


HumanPlayer::HumanPlayer(const char* name, int elo, int xp)
    : Player(name, elo, xp)
    {
        /* Initialize input subsystems */
		srand((size_t)name*elo);
    }


void HumanPlayer::SetCamera(Tribalia::Graphics::Camera* c) { _cam = c;}
void HumanPlayer::SetPicker(Tribalia::Input::InputPicker* ip) { _ip = ip; }

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

					char cname[32];
					sprintf(cname, "Object%d", rand() % rand());
					glm::vec3 p = _ip->GetTerrainProjectedPosition();

                    printf("Creating %s at %.3f %.3f %.3f\n", cname, p.x, 2.0f, p.z);

                    ConcreteObject* c = new ConcreteObject{0, cname, p.x, 2.0f, p.z};
                    int id = gctx->om->RegisterObject(c);
                    printf("%s has id %d now\n", cname, id);
                    fflush(stdin);
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
            glm::vec3 ray = _cam->Project(ev.mousex, ev.mousey, 640, 480);
            //objr->CheckRayCollide(ray, nullptr);


        } else if (ev.eventType == EVENT_MOUSEEVENT ) {
            
            if (ev.event.mouseev.button == MOUSE_LEFT) {
                if (ev.event.mouseev.status == KEY_KEYPRESS) 
                    mouse_click = true;
                else
                    mouse_click = false;
            }
                    
        }



//            printf("%d %d \n", ev.mousex, ev.mousey);
    key_flush:
		while(0);

    }


    if (front)
        _cam->AddMovement(glm::vec3(0, 0, -3.2f * gctx->elapsed_seconds));
    else if (back)
        _cam->AddMovement(glm::vec3(0, 0, 3.2f * gctx->elapsed_seconds));

    if (left)
        _cam->AddMovement(glm::vec3(-3.2f * gctx->elapsed_seconds, 0, 0));
    else if (right)
        _cam->AddMovement(glm::vec3(3.2f * gctx->elapsed_seconds, 0, 0));

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

HumanPlayer::~HumanPlayer()
{

}
