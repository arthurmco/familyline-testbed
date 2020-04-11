#include <client/HumanPlayer.hpp>
#include <client/graphical/meshopener/OBJOpener.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logger.hpp>

#include <client/input/input_service.hpp>

using namespace familyline;
using namespace familyline::graphics;
using namespace familyline::logic;
using namespace familyline::input;


#include <cstdio>

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
bool build_tent = false, build_tower = false;

std::weak_ptr<GameObject> attacker, attackee;


HumanPlayer::HumanPlayer(PlayerManager &pm, const char *name, int code)
     : Player(pm, name, code) 
{
    /* Initialize input subsystems */
	srand((size_t)name*code);
    auto& log = LoggerService::getLogger();

    _listener = [&](HumanInputAction hia) {
        
        if (std::holds_alternative<KeyAction>(hia.type)) {
            auto event = std::get<KeyAction>(hia.type);

            switch (event.keycode) {
            case SDLK_w:
                if (event.isPressed)
                    front = true;
                else
                    front = false;
                break;
            case SDLK_s:
                if (event.isPressed)
                    back = true;
                else
                    back = false;
                break;
            case SDLK_a:
                if (event.isPressed)
                    left = true;
                else
                    left = false;
                break;
            case SDLK_d:
                if (event.isPressed)
                    right = true;
                else
                    right = false;
                break;
            case SDLK_LEFT:
                if (event.isPressed)
                    rotate_left = true;
                else
                    rotate_left = false;
                break;
            case SDLK_RIGHT:
                if (event.isPressed)
                    rotate_right = true;
                else
                    rotate_right = false;
                break;

            case SDLK_c:
            {
                if (!event.isPressed)
                    return false;

                build_tent = true;
            }
            break;
            case SDLK_e:
            {
                if (!event.isPressed)
                    return false;

                build_tower = true;
            }
            break;
            case SDLK_r:
            {
                if (!event.isPressed)
                    return false;

                remove_object = true;
            }
            break;

            case SDLK_b: {
                if (!event.isPressed)
                    return false;

                this->renderBBs = !this->renderBBs;
            } break;

            case SDLK_k:
                attack_set = true;
                break;

            case SDLK_PLUS:
            case SDLK_KP_PLUS:
                if (!event.isPressed)
                    zoom_in = false;
                else
                    zoom_in = true;

                break;

            case SDLK_MINUS:
            case SDLK_KP_MINUS:
                if (!event.isPressed)
                    zoom_out = false;
                else
                    zoom_out = true;
                break;
            }
            return true;

        } else if (std::holds_alternative<ClickAction>(hia.type)) {
            auto event = std::get<ClickAction>(hia.type);

            if (attack_set && attack_ready) {
                attack_set = false;
            }
            
            if (event.buttonCode == SDL_BUTTON_LEFT) {
                attack_set = false;
                if (event.isPressed)
                    mouse_click = true;
                else
                    mouse_click = false;

				return true;
            }

            if (event.buttonCode == SDL_BUTTON_RIGHT && !_selected_obj.expired()) {
                if (event.isPressed) {

                    if (!attack_set) {
                        attack_ready = false;

                        /* Move the object to some position */
                        glm::vec2 to = _ip->GetGameProjectedPosition();
                        auto slock = _selected_obj.lock();

                        auto path = _pf->CreatePath(*slock.get(), to);
                        glm::vec2 lp = path.back();
                        log->write("human-player", LogType::Debug,
                                   "moved to %.2fx%.2f", lp.x, lp.y);

                        ObjectPathManager::getInstance()->AddPath(slock.get(), path);

//                        this->RegisterMove(slock.get(), to);
                        _updated = true;
                    } else {
                        attack_ready = true;
                        attack_set = true;
                    }
                }

                return true;
            }
            
        } else if (std::holds_alternative<WheelAction>(hia.type)) {
            auto event = std::get<WheelAction>(hia.type);

            if (event.scrollY > 0) {
                zoom_in = true;
                zoom_out = false;
                zoom_mouse = true;
            }

            if (event.scrollY <= 0) {
                zoom_out = true;
                zoom_in = false;
                zoom_mouse = true;
            }

            return true;
        } else if (std::holds_alternative<GameExit>(hia.type)) {
            exit_game = true;
            return true;
        }
        

        return false;

    };

    input::InputService::getInputManager()->addListenerHandler(_listener, 10);

}

void HumanPlayer::SetCamera(familyline::graphics::Camera* c) { _cam = c;}
void HumanPlayer::SetPicker(familyline::input::InputPicker* ip) { _ip = ip; }
void HumanPlayer::SetPathfinder(familyline::logic::PathFinder* p) { _pf = p; }

#if 0
void HumanPlayer::SetGameActionManager(familyline::logic::GameActionManager* gam)
{
    this->_gam = gam;

    /* Create a city for it */
    City* c = new City{this, glm::vec3(1, 0, 0)};
    AddCity(c);
}
#endif



/**
 * Generate the input actions.
 *
 * They must be pushed to the input manager
 */
void HumanPlayer::generateInput()
{
    double camera_speed = 0.1;
    double zoom_speed = 0.1;

    
    if (front) {
        this->pushAction(CameraMove{0, camera_speed, 0});
    }
    if (back) {
        this->pushAction(CameraMove{0, -camera_speed, 0});
    }
    if (left) {
        this->pushAction(CameraMove{-camera_speed, 0, 0});
    }
    if (right) {
        this->pushAction(CameraMove{-camera_speed, 0, 0});
    }


}


/**
 * Does this player requested game exit?
 */
bool HumanPlayer::exitRequested()
{
    return exit_game;
}

GameObject* HumanPlayer::GetSelectedObject()
{
    return (_selected_obj.expired() ? nullptr : _selected_obj.lock().get());
}

/*
bool HumanPlayer::HasUpdatedObject()
{
    return _updated;
}
*/

HumanPlayer::~HumanPlayer()
{

}
