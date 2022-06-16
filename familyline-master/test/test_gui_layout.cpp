#include <gtest/gtest.h>

#include <array>
#include <client/graphical/gui/gui_manager.hpp>
#include <client/input/input_manager.hpp>
#include <memory>

#include "utils/test_gui_renderer.hpp"
#include "utils/test_inputprocessor.hpp"

using namespace familyline::graphics::gui;
using namespace familyline::input;

class GUITestLayout : public ::testing::Test
{
protected:
    void SetUp() override
    {
        auto ipr = std::make_unique<TestInputProcessor>();
        InputService::setInputManager(std::make_unique<InputManager>(*ipr.get()));

        gm = new GUIManager(std::make_unique<TestGUIRenderer>());
    }

    void TearDown() override
    {
        delete gm;
        InputService::setInputManager(std::unique_ptr<InputManager>());
    }

    GUIManager *gm = nullptr;
};

TEST_F(GUITestLayout, TestRenderDynamicResizeWhenControlIsAdded)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(900, 900);

    GUILabel &label1 = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label2 = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Middle"));

    gm->showWindow(w);
    gm->update();
    gm->render();

    TestGUIRenderer &tr           = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop    = tr.query(label1.id());
    TestControlPaintData *lbottom = tr.query(label2.id());

    ASSERT_EQ(0, ltop->x);
    ASSERT_EQ(0, ltop->y);
    ASSERT_EQ(900, ltop->width);
    ASSERT_EQ(450, ltop->height);
    ASSERT_EQ(0, lbottom->x);
    ASSERT_EQ(450, lbottom->y);
    ASSERT_EQ(900, lbottom->width);
    ASSERT_EQ(450, lbottom->height);

    GUILabel &label3 = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Bottom"));

    gm->update();
    gm->render();

    ltop                          = tr.query(label1.id());
    TestControlPaintData *lmiddle = tr.query(label2.id());
    lbottom                       = tr.query(label3.id());

    ASSERT_EQ(0, ltop->x);
    ASSERT_EQ(0, ltop->y);
    ASSERT_EQ(900, ltop->width);
    ASSERT_EQ(300, ltop->height);
    ASSERT_EQ(0, lmiddle->x);
    ASSERT_EQ(300, lmiddle->y);
    ASSERT_EQ(900, lmiddle->width);
    ASSERT_EQ(300, lmiddle->height);
    ASSERT_EQ(0, lbottom->x);
    ASSERT_EQ(600, lbottom->y);
    ASSERT_EQ(900, lbottom->width);
    ASSERT_EQ(300, lbottom->height);
}

TEST_F(GUITestLayout, TestCorrectMinHeightApplication)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(640, 480);

    GUILabel &title = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Settings"));

    GUILabel &lblName = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Player name:"));

    auto a      = lblName.appearance();
    a.maxHeight = 40;
    lblName.setAppearance(a);

    GUITextbox &txtName = (GUITextbox &)w.box().add(gm->createControl<GUITextbox>("DefaultUser"));

    a           = txtName.appearance();
    a.maxHeight = 40;
    txtName.setAppearance(a);

    GUIButton &back = (GUIButton &)w.box().add(gm->createControl<GUIButton>("Back", [&](auto c) {

    }));

    gm->showWindow(w);
    gm->update();
    gm->render();

    TestGUIRenderer &tr          = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltitle = tr.query(title.id());
    TestControlPaintData *lname  = tr.query(lblName.id());
    TestControlPaintData *tname  = tr.query(txtName.id());
    TestControlPaintData *bback  = tr.query(back.id());

    ASSERT_TRUE(ltitle);
    ASSERT_TRUE(lname);
    ASSERT_TRUE(tname);
    ASSERT_TRUE(bback);

    ASSERT_EQ(0, ltitle->x);
    ASSERT_EQ(0, lname->x);
    ASSERT_EQ(0, tname->x);
    ASSERT_EQ(0, bback->x);

    ASSERT_EQ(0, ltitle->y);
    ASSERT_EQ(120, lname->y);
    ASSERT_EQ(160, tname->y);
    ASSERT_EQ(200, bback->y);

    ASSERT_EQ(120, ltitle->height);
    ASSERT_EQ(40, lname->height);
    ASSERT_EQ(40, tname->height);
    ASSERT_EQ(280, bback->height);
}
