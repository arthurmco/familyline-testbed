/*
 * 	Unit test routines for Familyline
 *
 *  Copyright (C) 2017 Arthur M
 *
 */
#include <gtest/gtest.h>

class PathFindingTest : public ::testing::Test {
};

#include "graphical/meshopener/OBJOpener.hpp"

using namespace Familyline::Graphics;
TEST(MeshTest, OBJOpenTest) {
    OBJOpener om;
    Mesh* m = om.Open("test/test.obj").at(0);
    ASSERT_STREQ("TestCube_Cube", m->GetName());
    ASSERT_EQ(36, m->GetVertexData().at(0)->Positions.size());
    delete m;
}

TEST(MeshTest, DISABLED_MD2OpenTest) {
    OBJOpener om;
    Mesh* m = om.Open("test/test.md2").at(0);
    ASSERT_EQ(36, m->GetVertexData().at(0)->Positions.size());
    delete m;
}

#include "test_obj.hpp"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
