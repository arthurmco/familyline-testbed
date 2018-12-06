#include <gtest/gtest.h>

#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/meshopener/MD2Opener.hpp"
#include "graphical/AssetFile.hpp"

class MeshTest : public ::testing::Test {
protected:
    familyline::graphics::AssetFile* af;
    
public:
    void SetUp();
    void TearDown();
};

using namespace familyline::graphics;
TEST(MeshTest, OBJOpenTest) {
    OBJOpener om;
    Mesh* m = om.Open("test/test.obj").at(0);
    ASSERT_STREQ("TestCube_Cube", m->GetName());
    ASSERT_EQ(36, m->getAnimator()->getCurrentFrame().at(0).position.size());
	
    delete m;
}

TEST(MeshTest, OBJWorldMatTest) {
    OBJOpener om;
    Mesh* m = om.Open("test/test.obj").at(0);
    ASSERT_STREQ("TestCube_Cube", m->GetName());
    ASSERT_EQ(m->getVertexInfo(0).worldMat, m->GetModelMatrixPointer());
	
    delete m;
}

TEST(MeshTest, DISABLED_MD2OpenTest) {
    OBJOpener om;
    Mesh* m = om.Open("test/test.md2").at(0);
    ASSERT_EQ(36, m->getAnimator()->getCurrentFrame().at(0).position.size());
    delete m;
}
