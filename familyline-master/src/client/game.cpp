#include <SDL2/SDL_timer.h>

#include <client/game.hpp>
#include <client/graphical/animator.hpp>
#include <client/graphical/gfx_debug_drawer.hpp>
//#include <client/graphical/gl_renderer.hpp>
#include <client/graphical/exceptions.hpp>
#include <client/graphical/light.hpp>
#include <client/input/input_service.hpp>
#include <common/logger.hpp>
#include <common/logic/colony.hpp>
#include <common/logic/game_event.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logic/pathfinder.hpp>
#include <exception>

using namespace familyline;
using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::graphics::gui;

/**
 * Initialize a map
 *
 * Be aware that this will not reset anything related to
 * objects, so you should call `initObjects`, or you will
 * be screwed
 */
logic::Terrain& Game::initMap(std::string_view path)
{
    auto& log = LoggerService::getLogger();

    if (!terrFile_->open(path)) {
        throw logic_exception{fmt::format("Could not open terrain '{}'", path)};
    }

    terrain_ = std::move(std::make_unique<Terrain>(*terrFile_.get()));

    /// Initial position of the camera might be part of the map file
    /// in the future, so we start it here too.
    int winW, winH;
    window_->getSize(winW, winH);

    glm::vec3 directionOffset = glm::vec3(6.0f, 36.0f, 6.0f);
    glm::vec3 position        = glm::vec3(0, 0, 0);
    float aspectRatio         = float(winW) / float(winH);
    camera_ = std::make_unique<Camera>(position + directionOffset, aspectRatio, position);

    log->write("game", LogType::Info, "map '{}' loaded", path.data());

    return *terrain_.get();
}

/**
 * Pass the player manager to the game
 *
 * Please add all starting clients to the player manager before starting the
 * game.
 *
 * When we support spectators (of course we will), the add/removal of the
 * spectators need to happen outside
 */
void Game::initPlayers(
    std::unique_ptr<logic::PlayerManager> pm, std::unique_ptr<logic::ColonyManager> cm,
    decltype(colonies_) player_colony_map, uint64_t human_id)
{
    auto& log = LoggerService::getLogger();
    log->write("game", LogType::Info, "configuring player manager");

    cm_.swap(cm);
    pm_.swap(pm);
    pm_->render_add_callback = [&](std::shared_ptr<GameEntity> o) {
        if (objrend_) objrend_->add(o);
    };
    colonies_ = player_colony_map;

    pm_->colony_add_callback = [&](std::shared_ptr<GameEntity> o, uint64_t player_id) {
        auto& col = o->getColonyComponent();
        if (col.has_value()) {
            col->owner = std::make_optional(colonies_.at(player_id));
            log->write(
                "game", LogType::Info, "set object id {} ({}) to city {}", o->getID(),
                o->getName().c_str(), col->owner.value().get().getName().data());
        }
    };

    if (human_id != uint64_t(-1)) {
        HumanPlayer* hp = dynamic_cast<HumanPlayer*>(*pm_->get(human_id));
        if (hp) hp->setCamera(camera_.get());
        human_id_ = human_id;
    }

    log->write("game", LogType::Info, "player manager configured");
}

/**
 * Initialize the asset library
 */
void Game::initAssets()
{
    // TODO: move this outside?
    AssetFile f;
    f.loadFile("assets.yml");
    am->loadFile(f);

    rndr_ = window_->createRenderer();
    if (terr_rend_) delete terr_rend_;
}

/**
 * Initialize the object factory, with all game entitys, and return a reference to it
 *
 * This is good, so we can get the object checksums
 */
logic::ObjectFactory* Game::initObjectFactory()
{
    auto& of = LogicService::getObjectFactory();
    factory_objects_.clear();

    factory_objects_.push_back(std::make_unique<WatchTower>());
    factory_objects_.push_back(std::make_unique<Tent>());

    /* Adds the objects to the factory */
    for (auto& o : factory_objects_) of->addObject(o.get());

    return of.get();
}

void Game::initObjectManager()
{
    auto& log = LoggerService::getLogger();
    log->write("game", LogType::Info, "configuring game entitys");

    terr_rend_ = rndr_->createTerrainRenderer(*camera_.get());
    terr_rend_->setTerrain(terrain_.get());
    terr_rend_->buildVertexData();
    terr_rend_->buildTextures();

    om_      = std::make_unique<ObjectManager>();
    olm_     = std::make_unique<ObjectLifecycleManager>(*om_.get());
    pm_->olm = olm_.get();

    log->write("game", LogType::Info, "game entitys configured");
}

auto sunlight = std::make_unique<Light>(
    SunLightType{glm::vec3(-0.2, -1.0, -0.2)}, 2.5f, glm::vec3(0.1, 0.2, 0.8), "sunlight");

auto pointlight = std::make_unique<Light>(
    PointLightType{glm::vec3(30.0, 10.0, 30.0)}, 5.8f, glm::vec3(0, 0.8, 0.2), "greenishlight");

auto pointlight2 = std::make_unique<Light>(
    PointLightType{glm::vec3(50.0, 10.0, 10.0)}, 9.8f, glm::vec3(0.8, 0.2, 0.0), "redishlight");

void Game::initLoopData(uint64_t human_id)
{
    auto& log = LoggerService::getLogger();
    log->write("game", LogType::Info, "initializing other data needed by the game");

    LogicService::initDebugDrawer(new GFXDebugDrawer(*rndr_, *terrain_.get()));

    //// Initialize some graphical data

    gctx    = {};
    gctx.om = om_.get();

    scenernd_ = std::make_unique<SceneManager>(*rndr_, *camera_.get());
    scenernd_->add(std::make_shared<SceneObject<Light>>(*sunlight.get()));
    scenernd_->add(std::make_shared<SceneObject<Light>>(*pointlight.get()));
    scenernd_->add(std::make_shared<SceneObject<Light>>(*pointlight2.get()));

    objrend_ = std::make_unique<ObjectRenderer>(*terrain_.get(), *scenernd_.get());

    ///// Initialize input

    ip_ = std::make_unique<input::InputPicker>(
        terrain_.get(), window_, scenernd_.get(), camera_.get(), om_.get());

    if (human_id != -1) {
        auto& of        = LogicService::getObjectFactory();
        pr_             = std::make_unique<PreviewRenderer>(*of.get(), *rndr_, *ip_.get());
        HumanPlayer* hp = dynamic_cast<HumanPlayer*>(*pm_->get(human_id));

        if (hp) {
            hp->SetPicker(ip_.get());
            hp->setPreviewer(pr_.get());
        } else {
            log->write("game", LogType::Warning, "human player id {} not found", human_id);
        }
    }

    LogicService::initPathManager(*terrain_.get());
    started_ = true;

    ticks_ = std::chrono::high_resolution_clock::now();

    log->write("game", LogType::Info, "game class ready");
}

Game::~Game() {}

bool Game::runLoop()
{
    gui_->debugClear();
    gui_->debugWrite("Familyline " VERSION " commit " COMMIT
                     "\n"
                     "----------------------------\n");
    gui_->debugWrite("Press C to build Tent, E to build WatchTower\n");
    gui_->debugWrite(fmt::format(
        "{:.2f} fps, {:.3f} ms/frame \n\t ({:.3f} ms logic, {:.3f} ms input, {:.3f} ms draw) \n\t "
        "tick {}\n",
        float(1000 / pms), float(pms), logictime_.count(), inputtime_.count(), drawtime_.count(),
        pm_->tick()));

    rendertime_ = std::chrono::high_resolution_clock::now();
    bool player = true;

    auto inputstart = std::chrono::high_resolution_clock::now();
    /* Runs the input code at fixed steps, like the logic one below */
    while (inputTime >= INPUT_DELTA) {
        player = this->runInput();
        if (!player) {
            return false;
        }

        inputTime -= INPUT_DELTA;
    }
    inputtime_ = std::chrono::high_resolution_clock::now() - inputstart;

    /* Run the logic code in steps of fixed blocks
     * This is called fixed timestep, and will ensure game consistency
     * on multiplayer games
     */
    int li          = 0;
    auto logicstart = std::chrono::high_resolution_clock::now();

    while (logicTime >= LOGIC_DELTA) {
        this->runLogic();
        logicTime -= LOGIC_DELTA;
        li++;
        gctx.tick++;
    }
    logictime_ = std::chrono::high_resolution_clock::now() - logicstart;

    if (frame_ > 1) limax = std::max(li, limax);

    auto drawstart = std::chrono::high_resolution_clock::now();

    this->showDebugInfo();
    this->runGraphical(double(delta.count()));
    drawtime_ = std::chrono::high_resolution_clock::now() - drawstart;

    Timer::getInstance()->RunTimers(delta.count());

    frame_++;

    ////////////////////////

    auto elapsed                = std::chrono::high_resolution_clock::now();
    delta                       = elapsed - ticks_;
    decltype(delta) renderdelta = std::chrono::high_resolution_clock::now() - rendertime_;

    if (frame_ % 15 == 0) {
        pms = delta.count() * 1.0;
    }

    ticks_ = elapsed;

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
        if (delta.count() < mindelta && frame_ > 2) mindelta = delta.count();

        if (delta.count() > maxdelta && frame_ > 2) maxdelta = delta.count();

        sumfps += (1000 / delta.count());
    }

    logicTime += delta.count();
    inputTime += delta.count();

    return true;
}

int inputruns = 0;
bool Game::runInput()
{
    /* Input processing  */

    input::InputService::getInputManager()->processEvents();

    ip_->UpdateIntersectedObject();
    ip_->UpdateTerrainProjectedPosition();

    gctx.elapsed_seconds = INPUT_DELTA / 1000.0;

    pm_->generateInput();

    gui_->update();

    inputruns++;
    return !pm_->exitRequested();
}

void Game::runLogic()
{
    if (irepr_) {
        irepr_->dispatchEvents((1000 / LOGIC_DELTA));
    }

    pm_->run(gctx);
    olm_->update();

    //    LogicService::getObjectListener()->updateObjects();

    /* Logic & graphical processing */
    // terr_rend->Update();
    om_->update();

    LogicService::getActionQueue()->processEvents();

    LogicService::getAttackManager()->update(*om_.get(), *olm_.get());
    LogicService::getPathManager()->update(*om_.get());

    bool objupdate = objrend_->willUpdate();
    if (objupdate) {
        objrend_->update();
        auto [w, h] = terrain_->getSize();
    }

    LogicService::getDebugDrawer()->update();
}

ObjectManager* Game::getObjectManager() const { return om_.get(); }

void Game::runGraphical(double framems)
{
    /* Rendering */

    fb3D_->startDraw();
    terr_rend_->render(*rndr_);

    scenernd_->update(framems);

    rndr_->render(camera_.get());

    fb3D_->endDraw();

    fbGUI_->startDraw();
    gui_->render();
    fbGUI_->endDraw();

    window_->update();
}

/* Show on-screen debug info
 * (aka the words in monospaced font you see in-game)
 */
void Game::showDebugInfo()
{
    if (irepr_) {
        gui_->debugWrite("Reproducing input...\n");

        if (irepr_->isReproductionEnded()) {
            gui_->debugWrite("End of replay!\n");
        }
    }

    pm_->iterate([&](Player* p) {
        if (p->getCode() == human_id_) {
            this->showHumanPlayerInfo(p);
        }
    });
}

void Game::showHumanPlayerInfo(logic::Player* hp)
{
    auto selections = hp->getSelections();
    std::shared_ptr<GameEntity> selected =
        selections.size() >= 1 ? selections[0].lock() : std::shared_ptr<GameEntity>();

    if (BuildQueue::GetInstance()->getNext()) {
        gui_->debugWrite(fmt::format(
            "Click to build {}\n",
            BuildQueue::GetInstance()->getNext().value()->getName().c_str()));
    }

    auto locc = ip_->GetIntersectedObject().lock();
    if (locc && selected) {
        auto alocc = locc.get();

        bool attackable = selected->getAttackComponent().has_value();

        if (alocc && attackable && selected && alocc->getMaxHealth()) {
            auto inRange =
                selected->getAttackComponent()->isInRange(alocc->getAttackComponent().value());

            gui_->debugWrite(inRange ? "\tIn range\n" : "\tNot in range\n");
        }
    }

    if (selected) {
        char s[150] = {};
        auto& acomp = selected->getAttackComponent();

        if (acomp) {
            gui_->debugWrite(fmt::format(
                "Selected object: '{}' ({:.1f}/{:4d})\n", selected->getName().c_str(),
                selected->getHealth(), selected->getMaxHealth()));
        } else {
            gui_->debugWrite(fmt::format("Selected object: '{}'\n", selected->getName().c_str()));
        }

    } else {
        gui_->debugWrite("No object selected\n");
    }

    glm::vec3 p = ip_->GetTerrainProjectedPosition();
    glm::vec2 q = ip_->GetGameProjectedPosition();

    char texs[256];
    gui_->debugWrite(fmt::format(
        "Terrain pos under the cursor: \n"
        "\t(ogl: {:.3f},{:.3f},{:.3f}, game: {:.2f}, {:.2f}), rotation {:.2f})\n",
        p.x, p.y, p.z, q.x, q.y, camera_->GetRotation() * 180 / M_PI));
}

/// Return maximum, minimum and average fps
std::tuple<double, double, double> Game::getStatisticInfo()
{
    // less delta, more fps

    if (frame_ > 0 && started_)
        return std::make_tuple(1000 / mindelta, 1000 / maxdelta, sumfps / frame_);
    else
        return std::make_tuple(0.0, 0.0, 0.0);
}
