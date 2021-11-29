#include <gtest/gtest.h>

#include <client/game.hpp>
#include <client/input/command_table.hpp>
#include <client/input/input_manager.hpp>
#include <common/logic/logic_service.hpp>
#include <string>

#include "client/graphical/gfx_service.hpp"
#include "utils.hpp"
#include "utils/test_device.hpp"
#include "utils/test_framebuffer.hpp"
#include "utils/test_gui_manager.hpp"
#include "utils/test_inputprocessor.hpp"
#include "utils/test_renderer.hpp"
#include "utils/test_window.hpp"

using namespace familyline::logic;
using namespace familyline::graphics;
using namespace familyline::input;
using namespace familyline;

TEST(HumanPlayerTest, TestIfCameraMoveWorks)
{
    CommandTable ct;
    ct.loadConfiguration({
        {"w", "CameraMove, up"},
        {"s", "CameraMove, down"},
        {"<right>", "CameraMove, right"},
        {"<left>", "CameraMove, left"},
    });

    PlayerSession session = {};
    session.players       = std::make_unique<PlayerManager>();
    session.colonies      = std::make_unique<ColonyManager>();

    auto ipr = std::make_unique<TestInputProcessor>();
    InputService::setInputManager(std::make_unique<InputManager>(*ipr));
    LogicService::getActionQueue()->clearEvents();

    TerrainFile tf;
    tf.open(TESTS_DIR "/terrain_test.flte");

    Terrain t{tf};

    auto hp    = std::make_unique<HumanPlayer>(*session.players.get(), t, "Player", 1, ct, true);
    auto id    = session.players->add(std::move(hp));
    auto hpptr = (HumanPlayer*)*(session.players->get(id));
    ObjectManager om;
    ObjectLifecycleManager olm{om};
    
    auto winW = 800, winH = 600;
    glm::vec3 directionOffset = glm::vec3(6.0f, 36.0f, 6.0f);
    glm::vec3 position        = glm::vec3(0, 0, 0);
    float aspectRatio         = float(winW) / float(winH);

    Camera c{position + directionOffset, aspectRatio, position};
    hpptr->setCamera(&c);
    hpptr->SetPicker(nullptr);

    ipr->startInputReceiver();

    auto upcmd = HumanInputAction{
        .timestamp = 0,
        .type      = KeyAction{
            .keycode    = SDLK_s,
            .keyname    = "",
            .isPressed  = true,
            .isRepeated = false,
            .modifiers  = 0}};
    ipr->pushAction(upcmd);

    GameContext gctx {
        .om = &om,
        .tick = 1,
        .elapsed_seconds = 0.03,
    };

    session.players->olm = &olm;
    session.players->run(gctx);
    InputService::getInputManager()->processEvents();
    session.players->generateInput();
    ASSERT_EQ(c.GetPosition(), position+directionOffset);

    for (auto i = 0; i < session.players->tickDelta(); i++) {
        gctx.tick++;
        session.players->run(gctx);
    }

    glm::vec3 nposition = glm::vec3(0, 0, 0.1);
    ASSERT_EQ(c.GetPosition(), nposition+directionOffset);

    InputService::setInputManager(std::unique_ptr<InputManager>());
    LogicService::getActionQueue()->clearEvents();
}
