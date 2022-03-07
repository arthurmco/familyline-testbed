#include <gtest/gtest.h>

#include <array>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/input/input_manager.hpp>
#include <memory>
#include <common/logic/script_environment.hpp>

#include "utils/test_gui_renderer.hpp"
#include "utils/test_inputprocessor.hpp"

using namespace familyline::graphics::gui;
using namespace familyline::input;

class GUITestScript : public ::testing::Test
{
protected:
    void SetUp() override
    {
        scm_init_guile();
        auto ipr = std::make_unique<TestInputProcessor>();
        InputService::setInputManager(std::make_unique<InputManager>(*ipr.get()));

        gm = new GUIManager(std::make_unique<TestGUIRenderer>());
        gsr = new GUIScriptRunner(gm);
    }

    void TearDown() override
    {
        delete gsr;
        delete gm;
        InputService::setInputManager(std::unique_ptr<InputManager>());
    }

    GUIManager *gm = nullptr;
    GUIScriptRunner* gsr = nullptr;
};


TEST_F(GUITestScript, TestBasicLayout)
{
    gsr->load(TESTS_DIR "/scripts/gui-base.scm");
    gm->onResize(640, 480);
    GUIWindow* w = gsr->openMainWindow();
    ASSERT_TRUE(w);

    gm->update();
    gm->render();

    GUILabel* ltitle = gm->getControl<GUILabel>("ltitle");
    GUILabel* l = gm->getControl<GUILabel>("ltitle");
    GUILabel* lother = gm->getControl<GUILabel>("ltitle");
    GUILabel* l1 = gm->getControl<GUILabel>("ltitle");
    GUIButton* bt = gm->getControl<GUIButton>("ltitle");
    GUIButton* start = gm->getControl<GUIButton>("ltitle");

    ASSERT_TRUE(ltitle);
    ASSERT_TRUE(l);
    ASSERT_TRUE(lother);
    ASSERT_TRUE(l1);
    ASSERT_TRUE(bt);
    ASSERT_TRUE(start);

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *pltitle = tr.query(ltitle->id());

    ASSERT_EQ(0, pltitle->y);
    ASSERT_EQ(45, pltitle->height);
    

}
    
