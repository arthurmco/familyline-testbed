#include <gtest/gtest.h>

#include "graphical/meshopener/OBJOpener.hpp"
#include "graphical/SceneManager.hpp"
#include "TestRenderer.hpp"

class RendererTest : public ::testing::Test {
protected:

public:
    void SetUp() {

    }

    void TearDown() {

    }
};

using namespace familyline::graphics;

TEST(RendererTest, TestGetObjects) {

    OBJOpener om;
    SceneManager sm;

    Mesh* m = om.Open("test/test.obj").at(0);
    Camera c(glm::vec3(0, 5, 0), 16/9, glm::vec3(5, 1, 5));
    
    m->SetPosition(glm::vec3(5, 1, 5));
    sm.AddObject(m);
    sm.SetCamera(&c);

    TestRenderer* r = new TestRenderer;
    r->initialize();
    r->SetSceneManager(&sm);
    
    r->UpdateObjects();

    ASSERT_EQ(1, r->getSceneIDCount());
    ASSERT_EQ(1, r->getAddedVertices());
    
    delete m;
    delete r;
}
