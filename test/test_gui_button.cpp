/*  Unit test fixture for the GUI button
*
*  Copyright (C) 2018 Arthur Mendes.
*/

#include <gtest/gtest.h>

#include "../src/graphical/gui/GUIManager.hpp"
#include "../src/graphical/gui/GUIButton.hpp"

using namespace familyline::graphics::gui;

class GUIButtonTest : public ::testing::Test {
protected:
	
public:
};

TEST_F(GUIButtonTest, TestClickReceived) {
    GUIButton gb(0.1, 0.1, 0.2, 0.1, "Test of Button");

    bool onClickReceived = false;
    gb.onClickHandler = [&](GUIControl* gc) {
	ASSERT_EQ(gc, (GUIControl*)&gb);
	onClickReceived = true;
    };

    GUIManager gm;
    gm.add(&gb);
    
    GUISignal s;
    s.from = nullptr;
    s.to = &gb;
    s.signal = SignalType::MouseClick;
    s.xPos = 0.25;
    s.yPos = 0.15;

    gm.sendSignal(s);
    gm.update();

    ASSERT_EQ(onClickReceived, true);
}


TEST_F(GUIButtonTest, TestRender) {
    GUIButton gb(0.1, 0.1, 0.2, 0.1, "Test of Button");

    GUIManager gm;
    gm.add(&gb);
    
    GUISignal s;
    s.from = nullptr;
    s.to = &gb;
    s.signal = SignalType::MouseClick;
    s.xPos = 0.25;
    s.yPos = 0.15;

    gm.sendSignal(s);
    gm.update();
    gm.render(640, 480);

}

