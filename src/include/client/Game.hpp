/*
  Game loop class

  Copyright (C) 2017, 2019 Arthur Mendes

*/

#ifndef _GAME_HPP
#define _GAME_HPP

//#include "logic/ObjectRenderer.hpp"
#include <client/graphical/GraphicalPlotInterface.hpp>
#include <client/graphical/Light.hpp>
#include <client/graphical/TextureOpener.hpp>
#include <client/graphical/asset_file.hpp>
#include <client/graphical/asset_manager.hpp>
#include <client/graphical/camera.hpp>
#include <client/graphical/framebuffer.hpp>
#include <client/graphical/gfx_service.hpp>
#include <client/graphical/gui/gui_button.hpp>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/graphical/material_manager.hpp>
#include <client/graphical/materialopener/MTLOpener.hpp>
#include <client/graphical/meshopener/OBJOpener.hpp>
#include <client/graphical/object_renderer.hpp>
#include <client/graphical/renderer.hpp>
#include <client/graphical/scene_manager.hpp>
#include <client/graphical/terrain_renderer.hpp>
#include <client/graphical/texture_manager.hpp>
#include <client/graphical/window.hpp>
#include <common/logic/BuildQueue.hpp>
#include <common/logic/GameActionManager.hpp>
#include <common/logic/ObjectPathManager.hpp>
#include <common/logic/PathFinder.hpp>
#include <common/logic/colony_manager.hpp>
#include <common/logic/debug_drawer.hpp>
#include <common/logic/lifecycle_manager.hpp>
#include <common/logic/object_factory.hpp>
#include <common/logic/player_manager.hpp>
#include <common/logic/terrain_file.hpp>
//#include "graphical/gui/ImageControl.hpp"

//#include <client/input/InputPicker.hpp>
#include <client/HumanPlayer.hpp>
#include <client/Timer.hpp>
#include <client/input/input_manager.hpp>
#include <common/objects/Tent.hpp>
#include <common/objects/WatchTower.hpp>

namespace familyline
{
class Game
{
private:
    unsigned human_id_;

    graphics::Window* win            = nullptr;
    graphics::Renderer* rndr         = nullptr;
    graphics::SceneManager* scenernd = nullptr;
    graphics::Framebuffer *fbGUI = nullptr, *fb3D = nullptr;
    graphics::gui::GUIManager* gr = nullptr;
    logic::PlayerManager* pm      = nullptr;
    logic::ObjectManager* om      = nullptr;

    std::unique_ptr<logic::ColonyManager> cm_;
    logic::Terrain* terr = nullptr;
    logic::TerrainFile* terrFile;

    logic::GameContext gctx;
    logic::GameActionManager gam;

    bool player = false;

    graphics::Camera* cam;

    std::unique_ptr<graphics::AssetManager>& am = graphics::GFXService::getAssetManager();

    graphics::TerrainRenderer* terr_rend = nullptr;
    graphics::ObjectRenderer* objrend    = nullptr;

    input::InputPicker* ip   = nullptr;
    logic::PathFinder* pathf = nullptr;

    logic::ObjectLifecycleManager* olm = nullptr;

    struct {
        //  graphics::gui::GUIPanel *p;
        graphics::gui::Label* lbl;
        graphics::gui::Label* lblVersion;
        //  graphics::gui::GUIPanel *pnl;
        graphics::gui::Button* btn;
    } widgets;

    /* Run input-related code
     * Return false if the player asked to exit the game.
     */
    bool RunInput();

    void RunLogic();
    void RunGraphical(double framems);

    /* Show on-screen debug info
     * (aka the words in monospaced font you see in-game)
     */
    void ShowDebugInfo();

    void showHumanPlayerInfo(logic::Player*);

    std::map<unsigned int /*player_id*/, std::reference_wrapper<logic::Colony>> colonies_;

public:
    Game(
        graphics::Window* w, graphics::Framebuffer* fb3D, graphics::Framebuffer* fbGUI,
        graphics::gui::GUIManager* gr, logic::PlayerManager* pm);

    int RunLoop();
};

}  // namespace familyline

#endif
