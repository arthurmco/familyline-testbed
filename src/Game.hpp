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
#include "logic/BuildQueue.hpp"
#include "logic/GameActionManager.hpp"
#include "logic/PlayerManager.hpp"
#include "logic/DebugPlot.hpp"

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
#include "graphical/Renderer.hpp"
#include "graphical/GraphicalPlotInterface.hpp"

#include "graphical/gui/GUIManager.hpp"
#include "graphical/gui/GUILabel.hpp"
#include "graphical/gui/GUIButton.hpp"
//#include "graphical/gui/ImageControl.hpp"

#include "input/InputPicker.hpp"

#include "Log.hpp"
#include "Timer.hpp"
#include "HumanPlayer.hpp"
#include "objects/WatchTower.hpp"
#include "objects/Tent.hpp"

namespace Familyline {
    
class Game {
private:
    Logic::ObjectManager* om = nullptr;
    HumanPlayer* hp = nullptr;
    Logic::Terrain* terr = nullptr;
    Logic::PlayerManager* pm = nullptr;
    Logic::TerrainFile* terrFile;
    
    Logic::GameContext gctx;
    Logic::GameActionManager gam;
    
    Graphics::Window* win = nullptr;
    Graphics::Renderer* rndr = nullptr;
    Graphics::SceneManager* scenemng = nullptr;

    Graphics::Framebuffer *fbGUI = nullptr, *fb3D = nullptr;
    Graphics::GUI::GUIManager* gr = nullptr;
    
    bool player = false;

    Graphics::Camera* cam;

    Graphics::AssetManager* am = Graphics::AssetManager::GetInstance();

    Graphics::TerrainRenderer* terr_rend = nullptr;
    Logic::ObjectRenderer* objrend = nullptr;

    Input::InputPicker* ip = nullptr;
    Logic::PathFinder* pathf = nullptr;


    struct {
//	Graphics::GUI::GUIPanel *p;
	Graphics::GUI::GUILabel *lbl;
	Graphics::GUI::GUILabel *lblVersion;
//	Graphics::GUI::GUIPanel *pnl;
	Graphics::GUI::GUIButton* btn;
    } widgets;
    
public:
    Game(Graphics::Window* w, Graphics::Framebuffer* fb3D,
	 Graphics::Framebuffer* fbGUI, Graphics::GUI::GUIManager* gr,
	 Logic::PlayerManager* pm, HumanPlayer* hp);

    int RunLoop();

};
    
}

#endif



