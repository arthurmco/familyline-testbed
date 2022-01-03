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
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    GUILabel &label = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Test label"));

    ASSERT_NE(0, label.id());

    gm->showWindow(w);
    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(label.id());

    ASSERT_TRUE(l1t);
    ASSERT_STREQ("Test label", ((GUILabel &)l1t->control).text().c_str());
}

TEST_F(GUITestBase, TestRenderSingleControlWithoutShowingWindow)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    GUILabel &label = (GUILabel &)w.box().add(gm->createControl<GUILabel>("Test label"));

    ASSERT_NE(0, label.id());

    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(label.id());

    ASSERT_FALSE(l1t);
}

TEST_F(GUITestBase, TestRenderSingleControlAndQueryTheWrongOne)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    GUILabel &label =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("We will not query this"));

    gm->showWindow(w);
    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *l1t = tr.query(label.id() + 1);

    ASSERT_FALSE(l1t);
}

TEST_F(GUITestBase, TestRenderLayoutVertical)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Bottom"));

    gm->showWindow(w);
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

TEST_F(GUITestBase, TestQueryNamedControl)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createNamedControl<GUILabel>("label1", "Top"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createNamedControl<GUILabel>("label2", "Bottom"));

    gm->showWindow(w);
    gm->update();
    gm->render();

    GUILabel* lget1 = gm->getControl<GUILabel>("label1");
    GUILabel* lget3 = gm->getControl<GUILabel>("label3");
    GUILabel* lget2 = gm->getControl<GUILabel>("label2");

    ASSERT_TRUE(lget1);
    ASSERT_TRUE(lget2);
    ASSERT_FALSE(lget3);

    ASSERT_EQ("Top", lget1->text());
    ASSERT_EQ("Bottom", lget2->text());
    ASSERT_EQ(label1.id(), lget1->id());
    ASSERT_EQ(label2.id(), lget2->id());

}

TEST_F(GUITestBase, TestRenderLayoutPartialWindow)
{
    gm->onResize(800, 800);
    
    GUIWindow &w1 = gm->createWindow<FlexLayout<false>>("test");
    w1.onResize(400, 800, 0, 0);
    
    GUILabel &label11 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label12 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Bottom"));

    gm->showWindow(w1);
    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop = tr.query(label11.id());
    TestControlPaintData *lbottom = tr.query(label12.id());

    EXPECT_EQ(0, ltop->x);
    EXPECT_EQ(0, ltop->y);
    EXPECT_EQ(400, ltop->width);
    EXPECT_EQ(400, ltop->height);
    EXPECT_EQ(0, lbottom->x);
    EXPECT_EQ(400, lbottom->y);
    EXPECT_EQ(400, lbottom->width);
    EXPECT_EQ(400, lbottom->height);
}

TEST_F(GUITestBase, TestRenderWindowOneAboveOther)
{
    gm->onResize(800, 800);
    
    GUIWindow &w1 = gm->createWindow<FlexLayout<false>>("test1");
    w1.onResize(800, 800, 0, 0);
    
    GUIWindow &w2 = gm->createWindow<FlexLayout<false>>("test2");
    w2.onResize(800, 800, 0, 0);

    GUILabel &label11 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Top W1"));
    GUILabel &label12 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Bottom W1"));
    
    GUILabel &label21 =
        (GUILabel &)w2.box().add(gm->createControl<GUILabel>("Top W2"));
    GUILabel &label22 =
        (GUILabel &)w2.box().add(gm->createControl<GUILabel>("Bottom W2"));

    gm->showWindow(w1);
    gm->showWindow(w2);
    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop = tr.query(label11.id());
    TestControlPaintData *lbottom = tr.query(label12.id());
    
    ASSERT_TRUE(ltop);
    ASSERT_TRUE(lbottom);

    EXPECT_EQ(0, ltop->x);
    EXPECT_EQ(0, ltop->y);
    EXPECT_EQ(800, ltop->width);
    EXPECT_EQ(400, ltop->height);
    EXPECT_EQ(0, lbottom->x);
    EXPECT_EQ(400, lbottom->y);
    EXPECT_EQ(800, lbottom->width);
    EXPECT_EQ(400, lbottom->height);

    ltop = tr.query(label21.id());
    lbottom = tr.query(label22.id());

    ASSERT_TRUE(ltop);
    ASSERT_TRUE(lbottom);
    
    EXPECT_EQ(0, ltop->x);
    EXPECT_EQ(0, ltop->y);
    EXPECT_EQ(800, ltop->width);
    EXPECT_EQ(400, ltop->height);
    EXPECT_EQ(0, lbottom->x);
    EXPECT_EQ(400, lbottom->y);
    EXPECT_EQ(800, lbottom->width);
    EXPECT_EQ(400, lbottom->height);

}


TEST_F(GUITestBase, TestRenderLayoutMultiWindow)
{
    gm->onResize(800, 800);
    
    GUIWindow &w1 = gm->createWindow<FlexLayout<false>>("test1");
    w1.onResize(400, 800, 0, 0);
    
    GUIWindow &w2 = gm->createWindow<FlexLayout<false>>("test2");
    w2.onResize(400, 800, 400, 0);
    
    GUILabel &label11 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Top W1"));
    GUILabel &label12 =
        (GUILabel &)w1.box().add(gm->createControl<GUILabel>("Bottom W1"));
    
    GUILabel &label21 =
        (GUILabel &)w2.box().add(gm->createControl<GUILabel>("Top W2"));
    GUILabel &label22 =
        (GUILabel &)w2.box().add(gm->createControl<GUILabel>("Bottom W2"));

    gm->showWindow(w1);
    gm->showWindow(w2);
    gm->update();
    gm->render();

    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    TestControlPaintData *ltop = tr.query(label11.id());
    TestControlPaintData *lbottom = tr.query(label12.id());

    ASSERT_TRUE(ltop);
    ASSERT_TRUE(lbottom);

    EXPECT_EQ(0, ltop->x);
    EXPECT_EQ(0, ltop->y);
    EXPECT_EQ(400, ltop->width);
    EXPECT_EQ(400, ltop->height);
    EXPECT_EQ(0, lbottom->x);
    EXPECT_EQ(400, lbottom->y);
    EXPECT_EQ(400, lbottom->width);
    EXPECT_EQ(400, lbottom->height);

    ltop = tr.query(label21.id());
    lbottom = tr.query(label22.id());

    ASSERT_TRUE(ltop);
    ASSERT_TRUE(lbottom);
    
    EXPECT_EQ(400, ltop->x);
    EXPECT_EQ(0, ltop->y);
    EXPECT_EQ(400, ltop->width);
    EXPECT_EQ(400, ltop->height);
    EXPECT_EQ(400, lbottom->x);
    EXPECT_EQ(400, lbottom->y);
    EXPECT_EQ(400, lbottom->width);
    EXPECT_EQ(400, lbottom->height);

}


TEST_F(GUITestBase, TestRenderLayoutHorizontal)
{
    GUIWindow &w = gm->createWindow<FlexLayout<true>>("test");
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Left"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Right"));

    gm->showWindow(w);
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

TEST_F(GUITestBase, TestWindowRestoreWindowBelowWhenAboveIsDestroyed)
{
    GUIWindow &w = gm->createWindow<FlexLayout<true>>("test");
    gm->onResize(800, 800);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Left"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Right"));

    gm->showWindow(w);
    gm->update();
    gm->render();

    GUIWindow &w2 = gm->createWindow<FlexLayout<true>>("another");
    w2.onResize(800, 800, 0, 0);
    
    GUILabel &labelother =
        (GUILabel &)w2.box().add(gm->createControl<GUILabel>("Another"));
    
    TestGUIRenderer &tr       = (TestGUIRenderer &)gm->getRenderer();
    
    gm->showWindow(w2);
    gm->update();
    gm->render();

    ASSERT_TRUE(tr.query(labelother.id()));

    
    gm->destroyWindow(w2);

    gm->update();
    gm->render();
        
    TestControlPaintData *lleft = tr.query(label1.id());
    TestControlPaintData *lright = tr.query(label2.id());

    ASSERT_TRUE(lleft);
    ASSERT_TRUE(lright);
    
    EXPECT_EQ(0, lleft->x);
    EXPECT_EQ(0, lleft->y);
    EXPECT_EQ(400, lleft->width);
    EXPECT_EQ(800, lleft->height);
    EXPECT_EQ(400, lright->x);
    EXPECT_EQ(0, lright->y);
    EXPECT_EQ(400, lright->width);
    EXPECT_EQ(800, lright->height);
}


TEST_F(GUITestBase, TestRenderDynamicResizeWhenControlIsAdded)
{
    GUIWindow &w = gm->createWindow<FlexLayout<false>>("test");
    gm->onResize(900, 900);

    GUILabel &label1 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Top"));
    GUILabel &label2 =
        (GUILabel &)w.box().add(gm->createControl<GUILabel>("Middle"));

    gm->showWindow(w);
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
