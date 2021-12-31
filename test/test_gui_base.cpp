#include <gtest/gtest.h>

#include <client/graphical/gui/gui_manager.hpp>
#include <client/input/input_manager.hpp>
#include <memory>

#include "utils/test_gui_renderer.hpp"
#include "utils/test_inputprocessor.hpp"

using namespace familyline::graphics::gui;
using namespace familyline::input;

class GUITestBase : public ::testing::Test
{
protected:
    void SetUp() override {
        auto ipr = std::make_unique<TestInputProcessor>();
        InputService::setInputManager(std::make_unique<InputManager>(*ipr.get()));

        gm = new GUIManager(std::make_unique<TestGUIRenderer>());
    }

    void TearDown() override {        
        delete gm;
        InputService::setInputManager(std::unique_ptr<InputManager>());
    }

    GUIManager *gm = nullptr;
};

TEST_F(GUITestBase, TestRenderSingleControl)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>();
    gm->onResize(800, 800);

    GUILabel &label = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Test label"));

    ASSERT_NE(0, label.id());

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(label.id());

    ASSERT_TRUE(l1t);
    ASSERT_STREQ("Test label", ((GUILabel &)l1t->control).text().c_str());
}

TEST_F(GUITestBase, TestRenderSingleControlAndQueryTheWrongOne)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>();
    gm->onResize(800, 800);

    GUILabel &label =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("We will not query this"));

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(label.id() + 1);

    ASSERT_FALSE(l1t);
}

TEST_F(GUITestBase, TestRenderLayoutVertical)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>();
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Bottom"));

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop = tr.query(label1.id());
    TestControlPaintData *lbottom = tr.query(label2.id());

    ASSERT_EQ(0, ltop->x);
    ASSERT_EQ(0, ltop->y);
    ASSERT_EQ(800, ltop->width);
    ASSERT_EQ(400, ltop->height);
    ASSERT_EQ(0, lbottom->x);
    ASSERT_EQ(400, lbottom->y);
    ASSERT_EQ(800, lbottom->width);
    ASSERT_EQ(400, lbottom->height);
}

TEST_F(GUITestBase, TestRenderLayoutHorizontal)
{
    GUIWindow &w = gm->createWindow<FlexLayout<true>>();
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Left"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Right"));

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *lleft = tr.query(label1.id());
    TestControlPaintData *lright = tr.query(label2.id());

    ASSERT_EQ(0, lleft->x);
    ASSERT_EQ(0, lleft->y);
    ASSERT_EQ(400, lleft->width);
    ASSERT_EQ(800, lleft->height);
    ASSERT_EQ(400, lright->x);
    ASSERT_EQ(0, lright->y);
    ASSERT_EQ(400, lright->width);
    ASSERT_EQ(800, lright->height);
}

TEST_F(GUITestBase, TestRenderDynamicResizeWhenControlIsAdded)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>();
    gm->onResize(900, 900);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Middle"));

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop = tr.query(label1.id());
    TestControlPaintData *lbottom = tr.query(label2.id());

    ASSERT_EQ(0, ltop->x);
    ASSERT_EQ(0, ltop->y);
    ASSERT_EQ(900, ltop->width);
    ASSERT_EQ(450, ltop->height);
    ASSERT_EQ(0, lbottom->x);
    ASSERT_EQ(450, lbottom->y);
    ASSERT_EQ(900, lbottom->width);
    ASSERT_EQ(450, lbottom->height);

    GUILabel &label3 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Bottom"));
    
    gm->update();
    gm->render();

    ltop = tr.query(label1.id());
    TestControlPaintData *lmiddle= tr.query(label2.id());
    lbottom = tr.query(label3.id());

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
