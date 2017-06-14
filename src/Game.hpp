/*
  Game loop class

  Copyright (C) 2017 Arthur M

*/

#ifndef _GAME_HPP
#define _GAME_HPP

#include "config.h"
#include "EnviroDefs.h"

#include "logic/ObjectRenderer.hpp"
#include "logic/PathFinder.hpp"
#include "logic/ObjectFactory.hpp"
#include "logic/ObjectPathManager.hpp"
#include "logic/TerrainFile.hpp"
#include "logic/Team.hpp"

#include "graphical/Camera.hpp"
#include "graphical/AssetFile.hpp"
#include "graphical/Light.hpp"
#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/materialopener/MTLOpener.hpp"
#include "graphical/TerrainRenderer.hpp"
#include "graphical/TextureOpener.hpp"
#include "graphical/TextureManager.hpp"
#include "graphical/MaterialManager.hpp"
#include "graphical/AssetManager.hpp"
#include "graphical/Window.hpp"
#include "graphical/Framebuffer.hpp"
#include "graphical/AnimationManager.hpp"
#include "graphical/Renderer.hpp"
#include "graphical/GUIRenderer.hpp"

#include "graphical/gui/Panel.hpp"
#include "graphical/gui/Label.hpp"
#include "graphical/gui/Button.hpp"

#include "input/InputPicker.hpp"

#include "Log.hpp"
#include "Timer.hpp"
#include "HumanPlayer.hpp"
#include "objects/WatchTower.hpp"
#include "objects/Tent.hpp"


namespace Tribalia {
    
class Game {
private:
    Logic::ObjectManager* om = nullptr;
    HumanPlayer* hp = nullptr;
    Logic::Terrain* terr = nullptr;
    Logic::TeamCoordinator* tc = nullptr;
    Logic::TerrainFile* terrFile;
    
    Logic::GameContext gctx;
    
    Graphics::Window* win = nullptr;
    Graphics::Renderer* rndr = nullptr;
    Graphics::SceneManager* scenemng = nullptr;

    Graphics::Framebuffer *fbGUI, *fb3D;
    Graphics::GUIRenderer* gr;
    
    bool player = false;

    Graphics::Camera* cam;

    Graphics::AssetManager* am = Graphics::AssetManager::GetInstance();

    Graphics::TerrainRenderer* terr_rend = nullptr;
    Logic::ObjectRenderer* objrend = nullptr;

    Input::InputPicker* ip = nullptr;
    Logic::PathFinder* pathf = nullptr;

    struct {
	Graphics::GUI::Panel *p;
	Graphics::GUI::Label *lbl;
	Graphics::GUI::Label *lblVersion;
	Graphics::GUI::Panel *pnl;
	Graphics::GUI::Button* btn;
    } widgets;
    
public:
    Game(Graphics::Window* w, Graphics::Framebuffer* fb3D,
	 Graphics::Framebuffer* fbGUI, Graphics::GUIRenderer* gr);

    int RunLoop();

};
    
}

#endif



