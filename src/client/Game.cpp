#include <GL/glew.h>
#include <SDL2/SDL_timer.h>

#include <client/Game.hpp>
#include <client/graphical/animator.hpp>
#include <client/graphical/gfx_debug_drawer.hpp>
#include <client/graphical/gl_renderer.hpp>
#include <client/graphical/light.hpp>
#include <client/input/input_service.hpp>
#include <common/logger.hpp>
#include <common/logic/colony.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/logic_service.hpp>

#include "client/graphical/exceptions.hpp"

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::input;

using namespace familyline::graphics;
using namespace familyline::graphics::gui;

class GameActionListenerImpl : public GameActionListener
{
public:
    GameActionListenerImpl() : GameActionListener("listener-base") {}

    virtual void OnListen(GameAction& a)
    {
        (void)a;
        LoggerService::getLogger()->write(
            "bogus-listener", LogType::Info, "received from listener");
    }
};

/// A no-op player that will only plot an existing building into the map
/// at the start of the game and do nothing.
///
/// It is only temporary, to test attacks in the future.
class DummyPlayer : public Player
{
private:
    bool done         = false;
    size_t sync_start = 0;
    size_t input_tick = 0;

    void pushActions(std::initializer_list<PlayerInputType> input)
    {
        for (auto& i : input) {
            this->pushAction(i);
        }
    }

public:
    DummyPlayer(PlayerManager& pm, const Terrain& t, const char* name, int code)
        : Player(pm, t, name, code)
    {
    }

    virtual void generateInput()
    {
        if (done) return;

        auto tick = this->getTick();

        if (sync_start == 0) {
            if (this->isTickValid()) {
                sync_start = tick;
                printf("engine is synchronized at tick %zu", sync_start);
            } else {
                return;
            }
        }

        if (input_tick == 2) {
            this->pushActions({EnqueueBuildAction{"tent"}});
        }

        if (input_tick == 6) {
            this->pushActions(
                {CommitLastBuildAction{20.0, 20.0, 2.4, true}, EnqueueBuildAction{"tent"}});
        }

        if (input_tick == 10) {
            this->pushActions({
                CommitLastBuildAction{30.0, 35.0, 2.4, true},
            });

            puts("Dummy player built everything it needed");
            done = true;
        }

        input_tick++;
    }
};

/// TODO: rewrite this and Tribalia.cpp!!!

auto sunlight = std::make_unique<Light>(
    SunLightType{glm::vec3(-0.2, -1.0, -0.2)}, 1.5f, glm::vec3(0, 0.1, 0.5), "sunlight");

auto pointlight = std::make_unique<Light>(
    PointLightType{glm::vec3(30.0, 10.0, 30.0)}, 5.8f, glm::vec3(0, 0.8, 0.2), "greenishlight");

auto pointlight2 = std::make_unique<Light>(
    PointLightType{glm::vec3(50.0, 10.0, 10.0)}, 9.8f, glm::vec3(0.8, 0.2, 0.0), "redishlight");


Game::Game(Window* w, Framebuffer* fb3D, Framebuffer* fbGUI, GUIManager* gr, PlayerManager* pm)
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

        cm_ = std::make_unique<ColonyManager>();

        olm               = new ObjectLifecycleManager{*om};
        auto& atk_manager = LogicService::getAttackManager();

        rndr = w->createRenderer();
        // DebugPlotter::pinterface = std::unique_ptr<DebugPlotInterface>
        //    (new GraphicalPlotInterface(rndr));

        gctx.om = om;
        gam.AddListener(new GameActionListenerImpl());
        // hp->SetGameActionManager(&gam);

        terrFile = new TerrainFile();
        if (!terrFile->open(ASSET_FILE_DIR "terrain_test.flte")) {
            throw new logic_exception{
                fmt::format("Could not open terrain " ASSET_FILE_DIR " terrain_test.flte")};
        }

        terr = new Terrain{*terrFile};

        AssetFile f;
        f.loadFile("assets.yml");
        am->loadFile(f);

        cam =
            new Camera{glm::vec3(6.0f, 36.0f, 6.0f), (float)winW / (float)winH, glm::vec3(0, 0, 0)};
        scenernd = new SceneManager(*(Renderer*)rndr, *cam);
        scenernd->add(std::make_shared<SceneObject<Light>>(*sunlight.get()));
        scenernd->add(std::make_shared<SceneObject<Light>>(*pointlight.get()));
        scenernd->add(std::make_shared<SceneObject<Light>>(*pointlight2.get()));

        //        scenernd->add()

        //      scenernd->SetCamera(cam);
        std::unique_ptr<HumanPlayer> hp = std::make_unique<HumanPlayer>(*pm, *terr, "Arthur", 0);
        hp->setCamera(cam);

        //      rndr->SetSceneManager(scenernd);

        //      am->Create();

        //      Light* l = new Light{"mainLight", glm::vec3(16, 16, 16), 0x0, 0x0, 0xff, 200 };
        std::vector<glm::vec3> p = {
            glm::vec3(0, terr->getHeightFromCoords(glm::vec2(0, 0)) + 16, 0),
            glm::vec3(10, terr->getHeightFromCoords(glm::vec2(10, 10)) + 16, 10),
            glm::vec3(10, terr->getHeightFromCoords(glm::vec2(10, 0)) + 16, 0)};

        //        DebugPlotter::pinterface->AddPath(p, glm::vec3(1, 0, 0));
        //      LightManager::AddLight(l);

        terr_rend = new TerrainRenderer{*terr, *cam};
        terr_rend->buildVertexData();
        terr_rend->buildTextures();

        objrend = new ObjectRenderer(*terr, *scenernd);

        pm->render_add_callback = [&](std::shared_ptr<GameObject> o) { objrend->add(o); };

        pm->colony_add_callback = [&](std::shared_ptr<GameObject> o, unsigned player_id) {
            auto& col = o->getColonyComponent();
            if (col.has_value()) {
                col->owner = std::make_optional(colonies_.at(player_id));
                log->write(
                    "game", LogType::Info, "set object id %d (%s) to city %s", o->getID(),
                    o->getName().c_str(), col->owner.value().get().getName().data());
            }
        };

        // gam.AddListener(objrend);

        //        InputManager::GetInstance()->Initialize();

        ip = new InputPicker{terr, win, scenernd, cam, om};
        hp->SetPicker(ip);

        pathf = new PathFinder(om);

        {
            auto [w, h] = terr->getSize();
            pathf->InitPathmap(w, h);
            pathf->UpdatePathmap(w, h);
        }

        widgets.lblVersion = new Label(10, 10, "Familyline " VERSION " commit " COMMIT);

        std::unique_ptr<Player> humanp = std::unique_ptr<Player>(hp.release());
        auto& halliance                = cm_->createAlliance(std::string{humanp->getName()});
        auto& hcolony                  = cm_->createColony(
            *humanp.get(), 0x0000ff, std::optional{std::reference_wrapper<Alliance>{halliance}});

        auto dummyp     = std::unique_ptr<Player>(new DummyPlayer(*pm, *terr, "Dummy Player", 2));
        auto& dalliance = cm_->createAlliance(std::string{dummyp->getName()});
        auto& dcolony   = cm_->createColony(
            *dummyp.get(), 0xff0000, std::optional{std::reference_wrapper<Alliance>{dalliance}});

        human_id_     = pm->add(std::move(humanp));
        auto dummy_id = pm->add(std::move(dummyp));

        colonies_.insert({human_id_, std::reference_wrapper<Colony>(hcolony)});
        colonies_.insert({dummy_id, std::reference_wrapper<Colony>(dcolony)});
        // gr->add(widgets.lblVersion);

        auto& of = LogicService::getObjectFactory();

        /* Adds the objects to the factory */
        of->addObject(new WatchTower);
        of->addObject(new Tent);

        ObjectPathManager::getInstance()->SetTerrain(terr);

        // LogicService::initDebugDrawer(new DummyDebugDrawer(*terr));
        LogicService::initDebugDrawer(new GFXDebugDrawer(*rndr, *terr));

        pm->olm = olm;
        pm->pf  = pathf;

    } catch (renderer_exception& re) {
        log->write("game", LogType::Fatal, "Rendering error: %s [%d]", re.what(), 0xdeadbeef);

        err = new char[192 + strlen(re.what())];
        sprintf(
            err,
            "Familyline found an error in rendering\n"
            "\n"
            "Error: %s\n",
            re.what());
        //      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    } catch (shader_exception& se) {
        log->write("game", LogType::Fatal, "Shader error: %s [%d]", se.what(), 0xbadbeef);
        log->write("game", LogType::Fatal, "Shader file: %s, type %d", "", 0x1e);

        err = new char[512 + strlen(se.what())];
        sprintf(
            err,
            "Familyline found an error in a shader\n"
            "\n"
            "Error: %s\n"
            "File: %s, type: %d, code: %d",
            se.what(), "", 0x1e, 0xbadbeef);
        //      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    } catch (asset_exception& ae) {
        log->write("game", LogType::Fatal, "Asset file error: %s", ae.what());
        err = new char[768 + strlen(ae.what())];

        //      if (ae.assetptr) {
        //          AssetItem* a = (AssetItem*)ae.assetptr;
        //          Log::GetLog()->Fatal("game", "Asset %s, file: %s", a->name.c_str(),
        //          a->path.c_str());

        //          sprintf(err,
        //              "Familyline found an error in an asset\n"
        //              "\n"
        //              "Asset named %s, file: %s\n"
        //              "Error: %s\n",
        //              a->name.c_str(), a->path.c_str(), ae.what());
        //      }

        sprintf(
            err,
            "Familyline found an error in an asset\n"
            "\n"
            "Error: %s\n",
            ae.what());

        //      win->ShowMessageBox(err, "Error", MessageBoxInfo::Error);
        delete[] err;
        exit(EXIT_FAILURE);
    } catch (graphical_exception& we) {
        log->write("game", LogType::Fatal, "Window creation error: %s (%d)", we.what(), 0xdeadc0de);
        exit(EXIT_FAILURE);
    }
}

void Game::initLoopData()
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

    lblBuilding   = new Label(0.05 * 640, 0.1 * 480, "!!!");
    lblFPS        = new Label(0.05 * 640, 0.9 * 480, "0 fps, 0 ms/frame");
    lblRange      = new Label(0.05 * 640, 0.13 * 480, "--");
    lblSelected   = new Label(0.05 * 640, 0.17 * 480, "---");
    lblTerrainPos = new Label(0.05 * 640, 0.21 * 480, "---");
    lblKeys       = new Label(
        0.05 * 640, 0.05 * 480,
        "Press C to build Tent, E to build WatchTower, B to draw bounding boxes");

    lblBuilding->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {1, 1, 1, 1};
        ca.background = {0, 0, 0, 0.4};
    });

    lblFPS->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {1, 1, 1, 1};
        ca.background = {0, 0, 0, 0.4};
    });

    lblRange->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {1, 1, 1, 1};
        ca.background = {0, 0, 0, 0.4};
    });

    lblSelected->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {1, 1, 1, 1};
        ca.background = {0, 0, 0, 0.4};
    });

    lblKeys->modifyAppearance([](ControlAppearance& ca) {
        ca.foreground = {0.9, 0.8, 1, 1};
        ca.background = {0, 0, 0, 0.4};
    });

    lblTerrainPos->modifyAppearance([](ControlAppearance& ca) { ca.background = {0, 0, 0, 0.4}; });

    GUIWindow& gw = gr->getDebugWindow();
    gw.add(5, 5, ControlPositioning::Pixel, std::unique_ptr<Control>(lblFPS));
    gw.add(5, 35, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)lblTerrainPos));
    gw.add(5, 65, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)lblBuilding));
    gw.add(5, 95, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)lblRange));
    gw.add(5, 125, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)lblSelected));
    gw.add(5, 155, ControlPositioning::Pixel, std::unique_ptr<Control>((Control*)lblKeys));

    started_ = true;

    ticks = std::chrono::high_resolution_clock::now();
}

Game::~Game()
{
    if (started_) {
        GUIWindow& gw = gr->getDebugWindow();
        gw.remove(lblFPS);
        gw.remove(lblTerrainPos);
        gw.remove(lblBuilding);
        gw.remove(lblRange);
        gw.remove(lblSelected);
        gw.remove(lblKeys);
    }
}

bool Game::runLoop()
{
    rendertime = std::chrono::high_resolution_clock::now();
    player     = true;

    /* Runs the input code at fixed steps, like the logic one below */
    while (inputTime >= INPUT_DELTA) {
        player = this->RunInput();
        if (!player) {
            return false;
        }

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

    if (frame > 1) limax = std::max(li, limax);

    this->ShowDebugInfo();
    this->RunGraphical(double(delta.count()));
    Timer::getInstance()->RunTimers(delta.count());

    frame++;

    ////////////////////////

    auto elapsed                = std::chrono::high_resolution_clock::now();
    delta                       = elapsed - ticks;
    decltype(delta) renderdelta = std::chrono::high_resolution_clock::now() - rendertime;

    if (frame % 15 == 0) {
        pms = delta.count() * 1.0;
    }

    ticks = elapsed;

    char sfps[128];
    sprintf(sfps, "%.3f fps, %.3f ms/frame", float(1000 / pms), float(pms));
    lblFPS->setText(sfps);
    //      gr->DebugWrite(0, 420, "%.2f ms, %.2f fps", pms, 1000 / pms);

#define FPS_LOCK 120.0

    // Locked in ~120 fps
    if (renderdelta.count() < (1000.0 / FPS_LOCK)) {
        auto sleepdelta = (1000.0 / FPS_LOCK) - 5;
        SDL_Delay(unsigned(sleepdelta));
    }

    // Make the mininum and maximum frame calculation more fair
    // because usually the first frame is when we load things, and
    // its the slowest.

    if (delta.count() > 0) {
        if (delta.count() < mindelta && frame > 2) mindelta = delta.count();

        if (delta.count() > maxdelta && frame > 2) maxdelta = delta.count();

        sumfps += (1000 / delta.count());
    }

    logicTime += delta.count();
    inputTime += delta.count();

    return true;
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

    gctx.elapsed_seconds = INPUT_DELTA / 1000.0;

    pm->generateInput();
    gr->update();

    return !pm->exitRequested();
}

void Game::RunLogic()
{
    pm->run(gctx);
    olm->update();

    LogicService::getObjectListener()->updateObjects();

    /* Logic & graphical processing */
    gam.ProcessListeners();
    // terr_rend->Update();
    om->update();

    LogicService::getActionQueue()->processEvents();

    bool objupdate = objrend->willUpdate();
    if (objupdate) {
        objrend->update();
        auto [w, h] = terr->getSize();
        pathf->UpdatePathmap(w, h);
    }

    LogicService::getAttackManager()->processAttacks(*olm);
    ObjectPathManager::getInstance()->UpdatePaths(LOGIC_DELTA);

    LogicService::getDebugDrawer()->update();
}

void Game::RunGraphical(double framems)
{
    /* Rendering */

    fb3D->startDraw();
    terr_rend->render(*rndr);

    //  rndr->SetBoundingBox(hp->renderBBs);

    scenernd->update(framems);

    //    rndr->UpdateObjects();

    //    rndr->UpdateFrames();
    rndr->render(cam);

    fb3D->endDraw();

    fbGUI->startDraw();
    gr->render(0, 0);
    //    gr->renderToScreen();
    fbGUI->endDraw();

    win->update();
}

void Game::showHumanPlayerInfo(Player* hp)
{
    auto selections = hp->getSelections();
    std::shared_ptr<GameObject> selected =
        selections.size() == 1 ? selections[0].lock() : std::shared_ptr<GameObject>();

    if (BuildQueue::GetInstance()->getNext()) {
        char s[256];
        sprintf(
            s, "Click to build %s",
            BuildQueue::GetInstance()->getNext().value()->getName().c_str());
        lblBuilding->setText(s);
    } else {
        lblBuilding->setText("");
    }

    auto locc = ip->GetIntersectedObject().lock();
    if (locc && selected) {
        auto alocc = locc.get();
        // gr->DebugWrite(10, 100, "Hovering object '%s'", locc->getName());

        bool attackable = selected->getAttackComponent().has_value();

        if (alocc && attackable && selected && alocc->getMaxHealth()) {
            auto inRange = selected->getAttackComponent()->isInAttackRange(
                alocc->getAttackComponent().value());

            lblRange->setText(inRange ? "In range" : "Not in range");
        }
    }

    //   {
    //       int qx, qy;
    //       scenernd->GetCameraQuadrant(qx, qy);
    //       gr->DebugWrite(10, 160, "Camera quadrant: %d x %d", qx, qy);
    //   }

    lblSelected->setText("");
    if (selected) {
        char s[150];
        auto& acomp = selected->getAttackComponent();

        if (acomp) {
            sprintf(
                s, "Selected object: '%s' (%4f/%4d)", selected->getName().c_str(),
                selected->getHealth(), selected->getMaxHealth());
        } else {
            sprintf(s, "Selected object: '%s'", selected->getName().c_str());
        }

        lblSelected->setText(s);
    }

    glm::vec3 p = ip->GetTerrainProjectedPosition();
    glm::vec2 q = ip->GetGameProjectedPosition();

    char texs[256];
    sprintf(
        texs,
        "Terrain pos: "
        "(ogl: %.3f,%.3f,%.3f | Game: %.2f, %.2f), rotation %.1f",
        p.x, p.y, p.z, q.x, q.y, cam->GetRotation() * 180 / M_PI);
    lblTerrainPos->setText(texs);
    // gr->DebugWrite(10, 65, "Bounding box: %s", hp->renderBBs ?
    //  "Enabled" : "Disabled");
}

/* Show on-screen debug info
 * (aka the words in monospaced font you see in-game)
 */
void Game::ShowDebugInfo()
{
    pm->iterate([&](Player* p) {
        if (p->getCode() == human_id_) {
            this->showHumanPlayerInfo(p);
        }
    });
}
