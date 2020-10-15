/*
  Game loop class

  Copyright (C) 2017, 2019 Arthur Mendes

*/

#ifndef _GAME_HPP
#define _GAME_HPP

//#include "logic/ObjectRenderer.hpp"
#include <client/graphical/GraphicalPlotInterface.hpp>
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

    /// Variables used between initLoopData() and runLoop()
    graphics::gui::Label* lblBuilding   = nullptr;
    graphics::gui::Label* lblFPS        = nullptr;
    graphics::gui::Label* lblRange      = nullptr;
    graphics::gui::Label* lblSelected   = nullptr;
    graphics::gui::Label* lblTerrainPos = nullptr;
    graphics::gui::Label* lblKeys = nullptr;
    
    std::chrono::duration<double, std::milli> delta;
    double pms = 0.0;

    double maxdelta = 0, mindelta = 99, sumfps = 0;
    bool started_ = false;

    // Run the logic engine at 60 Hz
#define LOGIC_DELTA 16

    // and the input engine at 120 Hz
#define INPUT_DELTA 8
    
    double logicTime = LOGIC_DELTA;
    double  inputTime = INPUT_DELTA;
    int limax     = 0;

    std::chrono::high_resolution_clock::time_point ticks;
    std::chrono::high_resolution_clock::time_point rendertime;
    unsigned int frame = 0;

public:
    Game(
        graphics::Window* w, graphics::Framebuffer* fb3D, graphics::Framebuffer* fbGUI,
        graphics::gui::GUIManager* gr, logic::PlayerManager* pm);

    void initLoopData();
    
    bool runLoop();

    /// Return maximum, minimum and average fps
    auto getStatisticInfo() {
         // less delta, more fps
        
        if (frame > 0 && started_)
            return std::make_tuple(1000 / mindelta, 1000 / maxdelta, sumfps / frame);
        else
            return std::make_tuple(0.0, 0.0, 0.0);
    }

    ~Game();
};

}  // namespace familyline

#endif
