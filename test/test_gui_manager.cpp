/*  Unit test fixture for the GUI manager
*
*  Copyright (C) 2018 Arthur Mendes.
*/

#include <gtest/gtest.h>

#include "../src/graphical/gui/GUIManager.hpp"
#include "mock_gui_label.hpp"
#include "mock_gui_button.hpp"

using namespace familyline::graphics::gui;


class GUIManagerTest : public ::testing::Test {
protected:
	
public:
};

TEST_F(GUIManagerTest, CheckIfContainerAddReceived) {
    GUIManager gm;

    CGUILabel gl1(0.1, 0.1, "Test 01");
    CGUILabel gl2(0.2, 0.2, "Test 02");

    gm.add(&gl1);
    gm.add(&gl2);

    gm.update();

    ASSERT_NE(gl1.width, -1);
    ASSERT_NE(gl2.width, -1);
    // TODO: Calculate label width
}


TEST_F(GUIManagerTest, TestZIndex) {
    GUIManager gm;

    CGUIButton::resetRenderOrder();
    CGUIButton gl1(0.1, 0.1, 0.3, 0.3, "Test 01");
    CGUIButton gl2(0.1, 0.1, 0.3, 0.3, "Test 02");

    gl2.z_index = 1000;
    
    gm.add(&gl2);
    gm.add(&gl1);
    gm.update();
    
    gm.render(640, 480);
    ASSERT_EQ(gl1.getRenderOrder(), 0);
    ASSERT_EQ(gl2.getRenderOrder(), 1);

}


TEST_F(GUIManagerTest, OnlyRenderUndirty) {
    GUIManager gm;

    CGUILabel gl1(0.1, 0.1, "Test 01");
    CGUILabel gl2(0.2, 0.2, "Test 02");

    gm.add(&gl1);
    gm.add(&gl2);

    gm.render(640, 480);
    ASSERT_EQ(gl1.getCalls(), 1);
    ASSERT_EQ(gl2.getCalls(), 1);

    gm.render(640, 480);
    ASSERT_EQ(gl1.getCalls(), 1);
    ASSERT_EQ(gl2.getCalls(), 1);
 
}

TEST_F(GUIManagerTest, TestControlRemove) {
    GUIManager gm;

    CGUILabel gl1(0.1, 0.1, "Test 01");
    CGUILabel gl2(0.2, 0.2, "Test 02");

    gm.add(&gl1);
    gm.add(&gl2);

    gm.render(640, 480);
    ASSERT_EQ(gl1.getCalls(), 1);
    ASSERT_EQ(gl2.getCalls(), 1);

    GUISignal s;
    s.signal = SignalType::Redraw;
    gm.processSignal(s);
    gm.remove(&gl1);
    
    gm.render(640, 480);
    ASSERT_EQ(gl1.getCalls(), 1);
    ASSERT_EQ(gl2.getCalls(), 2);
 
}
