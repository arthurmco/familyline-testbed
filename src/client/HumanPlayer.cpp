#include <client/HumanPlayer.hpp>
#include <client/graphical/meshopener/OBJOpener.hpp>
#include <client/input/input_service.hpp>
#include <common/logger.hpp>
#include <common/logic/logic_service.hpp>

using namespace familyline;
using namespace familyline::graphics;
using namespace familyline::logic;
using namespace familyline::input;

#include <cstdio>


HumanPlayer::HumanPlayer(PlayerManager& pm, const Terrain& t, const char* name, uint64_t code,
                         const CommandTable& ctable,
                         bool can_control = true)
    : Player(pm, t, name, code), ctable_(ctable), can_control_(can_control)
{
    /* Initialize input subsystems */
    srand((size_t)name * code);
    auto& log = LoggerService::getLogger();

    _listener = [&](HumanInputAction hia) {
        if (std::holds_alternative<KeyAction>(hia.type)) {
            auto event = std::get<KeyAction>(hia.type);
            auto action = ctable_.actionToCommand(hia);

            if (!action)
                return false;
            
            bool pressed = event.isPressed;
            auto [actiontype, actionparam] = *action;
            fprintf(stderr, "\n\n%s\n", front ? "f+" : "f-");
            
            switch (actiontype) {
            case PlayerCommandType::CameraMove:
                if (actionparam == "up")
                    front = pressed;
                else if (actionparam == "down")
                    back = pressed;
                else if (actionparam == "left")
                    left = pressed;
                else if (actionparam == "right")
                    right = pressed;

                break;
            case PlayerCommandType::CameraRotate:
                if (actionparam == "left")
                    rotate_left = pressed;
                else if (actionparam == "right")
                    rotate_right = pressed;
                break;
                
            case PlayerCommandType::CameraZoom:
                if (actionparam == "in")
                    zoom_in = pressed;
                else if (actionparam == "out")
                    zoom_out = pressed;
                
                break;

            case PlayerCommandType::DebugCreateEntity:
                if (pressed) {
                    if (actionparam == "tent")
                        build_tent = true;
                    else if (actionparam == "tower")
                        build_tower = true;
                    
                }
                break;
            case PlayerCommandType::DebugDestroyEntity:
                if (pressed) {
                    remove_object = true;                    
                }
                break;

            case PlayerCommandType::DebugShowBoundingBox:
                if (pressed) {
                    this->renderBBs = !this->renderBBs;
                }
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

            if (event.buttonCode == SDL_BUTTON_RIGHT) {
                if (event.isPressed) {
                    do_something = true;

                } else {
                    do_something = false;
                }

                return true;
            }

        } else if (std::holds_alternative<WheelAction>(hia.type)) {
            auto event = std::get<WheelAction>(hia.type);

            fprintf(stderr, "\n%d\n", event.scrollY);
            if (event.scrollY > 0) {
                zoom_in     = true;
                zoom_out    = false;
                zoom_mouse  = true;
                zoom_factor = event.scrollY;
            }

            if (event.scrollY <= 0) {
                zoom_out    = true;
                zoom_in     = false;
                zoom_mouse  = true;
                zoom_factor = -event.scrollY;
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

void HumanPlayer::setCamera(familyline::graphics::Camera* c)
{
    camera_ = std::optional(dynamic_cast<ICamera*>(c));
}

void HumanPlayer::SetPicker(familyline::input::InputPicker* ip) { _ip = ip; }

/**
 * Generate the input actions.
 *
 * They must be pushed to the input manager
 */
void HumanPlayer::generateInput()
{
    double camera_speed = 0.1;
    double rotate_speed = glm::radians(1.0);
    double zoom_speed   = 0.01;

    glm::vec2 cameraSpeedVec = glm::vec2(0, 0);
    bool isCameraMove        = front || back || left || right || zoom_mouse;
    double zoom_val          = 0;

    if (front) {
        cameraSpeedVec.y -= camera_speed;
    }

    if (back) {
        cameraSpeedVec.y += camera_speed;
    }

    if (left) {
        cameraSpeedVec.x -= camera_speed;
    }

    if (right) {
        cameraSpeedVec.x += camera_speed;
    }
    
    if (zoom_mouse) {
        if (zoom_in) zoom_val = +(zoom_speed * zoom_factor);

        if (zoom_out) zoom_val = -(zoom_speed * zoom_factor);

        zoom_mouse = false;
    }

    if (isCameraMove) {
        this->pushAction(CameraMove{cameraSpeedVec.x, cameraSpeedVec.y, zoom_val});
        zoom_factor = 0;
    }

    if (rotate_left) {
        this->pushAction(CameraRotate{rotate_speed});
    } else if (rotate_right) {
        this->pushAction(CameraRotate{-rotate_speed});
    }
    
    if (build_tent) {
        nextBuild_ = "tent";
        build_tent      = false;
        mouse_click     = false;
        build_something = true;
        preview_building = false;        
    }

    if (build_tower) {
        nextBuild_ = "watchtower";
        build_tower     = false;
        mouse_click     = false;
        build_something = true;
        preview_building = false;        
    }

    bool has_selection = !_ip->GetIntersectedObject().expired();

    if (build_something && mouse_click && can_control_) {
        // Something is queued to be built.
        glm::vec2 to = _ip->GetGameProjectedPosition();
        glm::vec3 p  = terr_.graphicalToGame(_ip->GetTerrainProjectedPosition());

        this->pushAction(CreateEntity{nextBuild_, int(p.x), int(p.z)});
        build_something = false;
        mouse_click     = false;
        if (pr_)
            pr_->reset();

        preview_building = false;        
        nextBuild_ = "";
    } else if (has_selection && mouse_click) {
        // Individual object selection, click-based
        // TODO: do multiple object selection, drag-based
        //       this can be started by adding a DragAction on input_actions.hpp
        auto l = _ip->GetIntersectedObject().lock();
        this->pushAction(SelectAction{{(unsigned long int)l->getID()}});
        mouse_click = false;
    } else if (!has_selection && mouse_click) {
        // Object deselection, aka click on anything.
        this->pushAction(SelectAction{{}});
        mouse_click = false;
    }

    if (build_something && !preview_building && nextBuild_.size() > 0) {
        preview_building = true;
        if (pr_) {
            pr_->add(nextBuild_, true);
        }
    }

    if (selected_.size() > 0 && do_something && can_control_) {
        // Requested to move a selected object
        glm::vec2 to = _ip->GetGameProjectedPosition();

        if (has_selection) {
            // Requested to run the default action on the selected object
            // Can be harvest or attack.
            auto l = _ip->GetIntersectedObject().lock();
            if (l)
                this->pushAction(CommandInput{"attack", l->getID()});

            // TODO: how to treat things when the object is too far from the
            // destination to perform an action?
        } else {
            this->pushAction(ObjectMove{int(to.x), int(to.y)});
        }

        do_something = false;
    }
}

/**
 * Does this player requested game exit?
 */
bool HumanPlayer::exitRequested() { return exit_game; }

HumanPlayer::~HumanPlayer() {}
