/*  Unit test fixture for the GUI labels
*
*  Copyright (C) 2018 Arthur Mendes.
*/

#include <gtest/gtest.h>

#include "test_lights.hpp"
#include "mock_gui_label.hpp"

using namespace familyline::graphics::gui;


class GUILabelTest : public ::testing::Test {
protected:
	
public:
};
TEST_F(GUILabelTest, UnDirtyAfterRender) {
    GUILabel l(0.2, 0.2, "Test01");
    ASSERT_TRUE(l.isDirty());
    l.setContext(640, 480);
   
    l.render(640, 480);
    ASSERT_FALSE(l.isDirty());
    
}

TEST_F(GUILabelTest, SetDirtyAfterTextChange) {
    GUILabel l(0.2, 0.2, "Test01");
    ASSERT_TRUE(l.isDirty());
    l.setContext(640, 480);
    
    l.render(640, 480);
    ASSERT_FALSE(l.isDirty());

    l.setText("Test02");
    ASSERT_TRUE(l.isDirty());
    
}
