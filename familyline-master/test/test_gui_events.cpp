#include <gtest/gtest.h>

#include <client/graphical/gui/gui_manager.hpp>
#include <client/input/input_manager.hpp>
#include <memory>

#include "utils/test_gui_renderer.hpp"
#include "utils/test_inputprocessor.hpp"

using namespace familyline::graphics::gui;
using namespace familyline::input;

class GUITestEvents : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ipr = std::make_unique<TestInputProcessor>();
        InputService::setInputManager(std::make_unique<InputManager>(*ipr.get()));

        gm = new GUIManager(std::make_unique<TestGUIRenderer>());
    }

    void TearDown() override
    {
        delete gm;
        InputService::setInputManager(std::unique_ptr<InputManager>());
    }

    std::unique_ptr<TestInputProcessor> ipr;
    GUIManager *gm = nullptr;
};

TEST_F(GUITestEvents, TestIfButtonRecognizesClick)
{
    using namespace familyline::input;

    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    bool clicked    = false;
    int click_times = 0;
    GUILabel &label = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Test label"));

    GUIButton &button =
        (GUIButton &)w.box().add(gm->createControl<GUIButton>("Test Button", [&](auto c) {
            clicked = true;
            click_times++;
        }));

    gm->showWindow(w);
    gm->update();
    gm->render();

    ASSERT_FALSE(clicked);

    ipr->pushAction(HumanInputAction{0, MouseAction{1, 1}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_FALSE(clicked);

    ipr->pushAction(HumanInputAction{1, MouseAction{1, 1}});
    ipr->pushAction(HumanInputAction{1, ClickAction{1, 1, 0, 1, true, 0}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_FALSE(clicked);

    ipr->pushAction(HumanInputAction{2, MouseAction{1, 400}});
    ipr->pushAction(HumanInputAction{3, ClickAction{1, 400, 0, 1, true, 0}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_FALSE(clicked);

    ipr->pushAction(HumanInputAction{4, ClickAction{1, 400, 0, 1, false, 0}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_TRUE(clicked);
    ASSERT_EQ(1, click_times);

    ipr->pushAction(HumanInputAction{5, ClickAction{1, 400, 0, 1, true, 0}});
    ipr->pushAction(HumanInputAction{6, ClickAction{1, 400, 0, 1, false, 0}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_TRUE(clicked);
    ASSERT_EQ(2, click_times);
}

TEST_F(GUITestEvents, TestIfButtonCanCloseWindowOnEventHandler)
{
    using namespace familyline::input;

    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    bool destroyed = false;
    GUIButton &button =
        (GUIButton &)w.box().add(gm->createControl<GUIButton>("Test Button", [&](auto c) {
            gm->destroyWindow("test");
            destroyed = true;
        }));

    gm->showWindow(w);
    gm->update();
    gm->render();

    ASSERT_FALSE(destroyed);
    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(button.id());
    ASSERT_TRUE(l1t);

    ipr->pushAction(HumanInputAction{0, MouseAction{100, 100}});
    ipr->pushAction(HumanInputAction{1, ClickAction{100, 100, 0, 1, true, 0}});
    ipr->pushAction(HumanInputAction{2, ClickAction{100, 100, 0, 1, false, 0}});
    InputService::getInputManager()->processEvents();
    gm->runEvents();
    gm->update();
    gm->render();

    ASSERT_TRUE(destroyed);
    l1t = tr.query(button.id());
    ASSERT_FALSE(l1t);
}
