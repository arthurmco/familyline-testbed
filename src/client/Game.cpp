#include <GL/glew.h>
#include <client/Game.hpp>
#include <client/graphical/gl_renderer.hpp>
#include <client/graphical/LightManager.hpp>
#include <client/graphical/animator.hpp>
#include <client/graphical/gfx_debug_drawer.hpp>

#include <common/logger.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/logic_service.hpp>

#include <client/input/input_service.hpp>

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::input;

using namespace familyline::graphics;
using namespace familyline::graphics::gui;

class GameActionListenerImpl : public GameActionListener {
public:
    GameActionListenerImpl() : GameActionListener("listener-base") {}

    virtual void OnListen(GameAction& a) {
        (void)a;
        LoggerService::getLogger()->write("bogus-listener", LogType::Info,
                                          "received from listener");
    }
};

/// TODO: rewrite this and Tribalia.cpp!!!

Game::Game(Window* w, Framebuffer* fb3D, Framebuffer* fbGUI,
           GUIManager* gr, PlayerManager* pm, std::unique_ptr<HumanPlayer> hp)
    : win(w), fbGUI(fbGUI), fb3D(fb3D), gr(gr), pm(pm)
{
    //    DebugPlotter::Init();
    auto& log = LoggerService::getLogger();

    int winW, winH;
    w->getSize(winW, winH);
    char* err = nullptr;

    try {
        /* Initialise things */
        om = new ObjectManager{};
        //        ObjectManager::setDefault(om);

        olm = new ObjectLifecycleManager {*om};
        auto& atk_manager = LogicService::getAttackManager();

        rndr = new GLRenderer{};
        // DebugPlotter::pinterface = std::unique_ptr<DebugPlotInterface>
        //    (new GraphicalPlotInterface(rndr));

        //gctx.om = om;

        gam.AddListener(new GameActionListenerImpl());
        //hp->SetGameActionManager(&gam);
        hp->olm = olm;

        terrFile = new TerrainFile(ASSET_FILE_DIR "terrain_test.trtb");
        terr = terrFile->GetTerrain();

        auto tteam = std::make_shared<Team>(1, "Test team");
        printf("%s -- %#x\n", tteam->name.c_str(), tteam->number);

        AssetFile f;
        f.loadFile("assets.yml");
        am->loadFile(f);


//      scenemng = new SceneManager(terr->GetWidth() * SEC_SIZE, terr->GetHeight() * SEC_SIZE);

        cam = new Camera{ glm::vec3(6.0f, 36.0f, 6.0f), (float)winW / (float)winH, glm::vec3(0,0,0) };
        scenernd = new SceneRenderer((Renderer*)rndr, *cam);

        //      scenernd->SetCamera(cam);
        hp->SetCamera(cam);

//      rndr->SetSceneManager(scenernd);

//      am->Create();

//      Light* l = new Light{"mainLight", glm::vec3(16, 16, 16), 0x0, 0x0, 0xff, 200 };
        std::vector<glm::vec3> p = {
            glm::vec3(0, terr->GetHeightFromPoint(0, 0) + 16, 0),
            glm::vec3(10, terr->GetHeightFromPoint(10, 10) + 16, 10),
            glm::vec3(10, terr->GetHeightFromPoint(10, 0) + 16, 0)
        };

        //        DebugPlotter::pinterface->AddPath(p, glm::vec3(1, 0, 0));
//      LightManager::AddLight(l);

        terr_rend = new TerrainRenderer{ };
        terr_rend->SetTerrain(terr);
        terr_rend->SetCamera(cam);

        objrend = new ObjectRenderer(*terr, *scenernd);
        hp->objr = objrend;
        //gam.AddListener(objrend);

//        InputManager::GetInstance()->Initialize();


        ip = new InputPicker{ terr_rend, win, scenernd, cam, om };
        hp->SetPicker(ip);

        pathf = new PathFinder(om);
        pathf->InitPathmap(terr->GetWidth(), terr->GetHeight());
        pathf->UpdatePathmap(terr->GetWidth(), terr->GetHeight());
        hp->SetPathfinder(pathf);

        widgets.lblVersion = new GUILabel(10, 10, "Familyline " VERSION " commit " COMMIT);

        std::unique_ptr<Player> humanp = std::unique_ptr<Player>(hp.release());
        pm->add(std::move(humanp));

        gr->add(widgets.lblVersion);

        auto& of = LogicService::getObjectFactory();

        /* Adds the objects to the factory */
        of->addObject(new WatchTower);
        of->addObject(new Tent);

        ObjectPathManager::getInstance()->SetTerrain(terr);

        LogicService::initDebugDrawer(new GFXDebugDrawer(*rndr));

    }
    catch (renderer_exception& re) {
        log->write("game", LogType::Fatal,
                   "Rendering error: %s [%d]",
                             re.what(), 0xdeadbeef);

        err = new char[192 + strlen(re.what())];
        sprintf(err,
                "Familyline found an error in rendering\n"
                "\n"
                "Error: %s\n", re.what());
//      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    }
    catch (shader_exception& se) {
        log->write("game", LogType::Fatal,
                   "Shader error: %s [%d]", se.what(), 0xbadbeef);
        log->write("game", LogType::Fatal,
                   "Shader file: %s, type %d", "", 0x1e);

        err = new char[512 + strlen(se.what())];
        sprintf(err,
                "Familyline found an error in a shader\n"
                "\n"
                "Error: %s\n"
                "File: %s, type: %d, code: %d",
                se.what(), "", 0x1e, 0xbadbeef);
//      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    }
    catch (asset_exception& ae) {
        log->write("game", LogType::Fatal,
                   "Asset file error: %s", ae.what());
        err = new char[768 + strlen(ae.what())];

//      if (ae.assetptr) {
//          AssetItem* a = (AssetItem*)ae.assetptr;
//          Log::GetLog()->Fatal("game", "Asset %s, file: %s", a->name.c_str(), a->path.c_str());

//          sprintf(err,
//              "Familyline found an error in an asset\n"
//              "\n"
//              "Asset named %s, file: %s\n"
//              "Error: %s\n",
//              a->name.c_str(), a->path.c_str(), ae.what());
//      }

        sprintf(err,
                "Familyline found an error in an asset\n"
                "\n"
                "Error: %s\n",
                ae.what());

//      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    }
    catch (graphical_exception& we) {
        log->write("game", LogType::Fatal,
                   "Window creation error: %s (%d)", we.what(), 0xdeadc0de);
        exit(EXIT_FAILURE);
    }

}

GUILabel lblBuilding = GUILabel(0.05, 0.1, "!!!");
GUILabel lblFPS = GUILabel(0.05, 0.9, "0 fps, 0 ms/frame");
GUILabel lblRange = GUILabel(0.05, 0.13, "--");
GUILabel lblSelected = GUILabel(0.05, 0.17, "---");
GUILabel lblTerrainPos = GUILabel(0.05, 0.21, "---");

GUILabel lblKeys = GUILabel(0.05, 0.05, "Press C to build Tent, E to build WatchTower, B to draw bounding boxes");


// Run the logic engine at 60 Hz
#define LOGIC_DELTA 16

// and the input engine at 120 Hz
#define INPUT_DELTA 8

int Game::RunLoop()
{
    auto& log = LoggerService::getLogger();

    // Panel pnl = Panel(0.0, 0.8, 1.0, 0.2, true);
    // pnl.SetBackColor(0, 0, 0, 185);
    // gr->AddPanel(&pnl);

    // GUIActionManager* guam = new GUIActionManager(&pnl);
    // hp->SetGUIActionManager(guam);

    /*    Button btnExit = Button(0.7, 0.05, 0.2, 0.5, "Exit");
          btnExit.SetOnClickListener([&](GUI::IControl* cc) {
          (void)cc;
          player = false;
          });
          pnl.AddPanel(&btnExit); */


    lblBuilding.format.foreground = glm::vec4(1, 1, 1, 1);
    lblBuilding.format.background = glm::vec4(0, 0, 0, 0.4);

    lblFPS.format.foreground = glm::vec4(1, 1, 1, 1);
    lblFPS.format.background = glm::vec4(0, 0, 0, 0.4);

    lblRange.format.foreground = glm::vec4(1, 1, 1, 1);
    lblRange.format.background = glm::vec4(0, 0, 0, 0.4);

    lblSelected.format.foreground = glm::vec4(1, 1, 1, 1);
    lblSelected.format.background = glm::vec4(0, 0, 0, 0.4);

    lblKeys.format.foreground = glm::vec4(0.9, 0.8, 1, 1);
    lblKeys.format.background = glm::vec4(0, 0, 0, 0.4);

    lblTerrainPos.format.background = glm::vec4(0, 0, 0, 0.4);

    gr->add(&lblBuilding);
    gr->add(&lblFPS);
    gr->add(&lblRange);
    gr->add(&lblSelected);
    gr->add(&lblKeys);
    gr->add(&lblTerrainPos);

    int delta = 1;
    double pms = 0.0;

    double maxdelta = 0, mindelta = 99, sumfps = 0;

    int logicTime = LOGIC_DELTA;
    int inputTime = INPUT_DELTA;
    int limax = 0;

    // Update the terrain first, so the terrain textures can load
    // TODO: generate the terrain textures before? in another function?
    terr_rend->Update();
    
    unsigned int ticks = SDL_GetTicks();
    unsigned int frame = 0;

    do {
        player = true;

        /* Runs the input code at fixed steps, like the logic one below */
        while (inputTime >= INPUT_DELTA) {
            player = this->RunInput();
            if (!player)
                break;

            inputTime -= INPUT_DELTA;
        }

        /* Run the logic code in steps of fixed blocks
         * This is called fixed timestep, and will ensure game consistency
         * on multiplayer games
         */
        int li = 0;
        while (logicTime >= LOGIC_DELTA) {
            this->RunLogic();
            logicTime -= LOGIC_DELTA;
            li++;
            gctx.tick++;
        }

        if (frame > 1)
            limax = std::max(li, limax);

        this->ShowDebugInfo();
        this->RunGraphical();

        frame++;

        unsigned int elapsed = SDL_GetTicks();
        delta = elapsed - ticks;

        ticks = SDL_GetTicks();
        Timer::getInstance()->RunTimers(delta);

        if (frame % 15 == 0) {
            pms = delta * 1.0;
        }

        char sfps[128];
        sprintf(sfps, "%.3f fps, %.3f ms/frame", float(1000 / pms), float(pms));
        lblFPS.setText(sfps);
//      gr->DebugWrite(0, 420, "%.2f ms, %.2f fps", pms, 1000 / pms);

#define FPS_LOCK 120.0

        //Locked in ~120 fps
        if (delta < 1000 / FPS_LOCK) {
            auto sleepdelta = int((1000 / FPS_LOCK) - delta);
            SDL_Delay(sleepdelta);
        }

        // Make the mininum and maximum frame calculation more fair
        // because usually the first frame is when we load things, and
        // its the slowest.
        
        if (delta < mindelta && frame > 2)
            mindelta = delta;

        if (delta > maxdelta && frame > 2)
            maxdelta = delta;

        sumfps += (1000 / delta);
        logicTime += delta;
        inputTime += delta;
    } while (player);

    double maxfps = 1000 / mindelta;// less delta, more fps
    double minfps = 1000 / maxdelta;
    double avgfps = sumfps / frame;
    log->write("game", LogType::Info,
               "fps max: %.3f (%.3f ms), min: %.3f (%.3f ms), avg: %.3f",
               maxfps, mindelta, minfps, maxdelta, avgfps);

    log->write("game", LogType::Info, "Total frames: %d", frame);

    return 0;
}


/* Run input-related code
 * Return false if the player asked to exit the game.
 */
bool Game::RunInput()
{
    /* Input processing  */

    InputService::getInputManager()->processEvents();

    ip->UpdateIntersectedObject();
    ip->UpdateTerrainProjectedPosition();

    gctx.elapsed_seconds = INPUT_DELTA/1000.0;

    pm->generateInput();
    gr->update();

    return !pm->exitRequested();
}


void Game::RunLogic()
{
    pm->run(gctx.tick);
    olm->update();

    LogicService::getObjectListener()->updateObjects();

    /* Logic & graphical processing */
    gam.ProcessListeners();
    terr_rend->Update();
    om->update();

    LogicService::getActionQueue()->processEvents();

    bool objupdate = objrend->willUpdate();
    if (objupdate) {
        objrend->update();
        pathf->UpdatePathmap(terr->GetWidth(), terr->GetHeight());
    }

    LogicService::getAttackManager()->processAttacks(*olm);
    ObjectPathManager::getInstance()->UpdatePaths(LOGIC_DELTA);

    LogicService::getDebugDrawer()->update();
}

void Game::RunGraphical()
{

    /* Rendering */

    fb3D->startDraw();
    terr_rend->Update();
    terr_rend->Render();

    //  rndr->SetBoundingBox(hp->renderBBs);

    scenernd->update();

//    Animator::runAllAnimations(16); // TODO: get correct frame time
//    rndr->UpdateObjects();

//    rndr->UpdateFrames();
    rndr->render(cam);

    fb3D->endDraw();

    fbGUI->startDraw();
    gr->render(0, 0);
    gr->renderToScreen();
	fbGUI->endDraw();

    win->update();
}


/* Show on-screen debug info
 * (aka the words in monospaced font you see in-game)
 */
void Game::ShowDebugInfo()
{
    GameObject* selected = nullptr; //hp->GetSelectedObject();
    
    if (BuildQueue::GetInstance()->getNext()) {
        char s[256];
        sprintf(s, "Click to build %s",
                BuildQueue::GetInstance()->getNext().value()
                ->getName().c_str());
        lblBuilding.setText(s);
    }
    else {
        lblBuilding.setText("");
    }

    auto locc = ip->GetIntersectedObject().lock();
    if (locc && selected) {
        auto alocc = locc.get();
//          gr->DebugWrite(10, 100, "Hovering object '%s'", locc->getName());

        bool attackable = selected->getAttackComponent().has_value();

        if (alocc && attackable && selected && alocc->getMaxHealth()) {

            auto inRange = selected->getAttackComponent()->isInAttackRange(
                alocc->getAttackComponent().value());

            lblRange.setText(inRange ? "In range" : "Not in range");
        }
    }

    //   {
    //       int qx, qy;
    //       scenernd->GetCameraQuadrant(qx, qy);
    //       gr->DebugWrite(10, 160, "Camera quadrant: %d x %d", qx, qy);
    //   }


    lblSelected.setText("");
    if (selected) {
        char s[150];
        auto& acomp = selected->getAttackComponent();

        if (acomp) {
            sprintf(s, "Selected object: '%s' (%4f/%4d)",
                    selected->getName().c_str(),
                    selected->getHealth(),
                    selected->getMaxHealth());
        } else {
            sprintf(s, "Selected object: '%s'",
                    selected->getName().c_str());
        }

        lblSelected.setText(s);
    }

    glm::vec3 p = ip->GetTerrainProjectedPosition();
    glm::vec2 q = ip->GetGameProjectedPosition();

    char texs[256];
    sprintf(texs, "Terrain pos: "
            "(ogl: %.3f,%.3f,%.3f | Game: %.2f, %.2f), rotation %.1f",
            p.x, p.y, p.z, q.x, q.y, cam->GetRotation() * 180 / M_PI);
    lblTerrainPos.setText(texs);
    // gr->DebugWrite(10, 65, "Bounding box: %s", hp->renderBBs ?
    //  "Enabled" : "Disabled");

}
