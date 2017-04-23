#include "test_pathfind.hpp"

using namespace Tribalia::Logic;

void PathfinderTest::SetUp()
{
    _om = new ObjectManager;
    _pf = new PathFinder{_om};

    _pf->InitPathmap(256, 256);
}

void PathfinderTest::TearDown()
{
    delete _pf;
    delete _om;
}

TEST_F(PathfinderTest, TestStraightPathfind){
    TestObject* o = new TestObject(1, 10, 1, 10);
    _om->RegisterObject(o);
    ASSERT_EQ(1, _om->GetCount()) << "Object manager with wrong count";

    auto vlist = _pf->CreatePath(o, glm::vec2(32, 32));
    
    /* Check if we didn't got off the path too far */
    int i = 0;
    for (auto v : vlist) {
	EXPECT_GE(v.x, 10) << "Element index " << i << " with xpos <= xmin";
	EXPECT_LE(v.x, 32) << "Element index " << i << " with xpos >= xmax";
	EXPECT_GE(v.y, 10) << "Element index " << i << " with ypos <= ymax";
	EXPECT_LE(v.y, 32) << "Element index " << i << " with ypos >= ymax";

	i++;
    }
   
}

TEST_F(PathfinderTest, TestObstaclePathfind){
    TestObject* o = new TestObject(1, 10, 1, 10);
    TestObject* c = new TestObject(1, 21, 1, 21);
    
    _om->RegisterObject(o);
    _om->RegisterObject(c);

    _pf->UpdatePathmap(256, 256);
    
    ASSERT_EQ(2, _om->GetCount()) << "Object manager with wrong count";

    auto vlist = _pf->CreatePath(o, glm::vec2(32, 32));
    
    /* Check if we didn't collided */
    int i = 0;
    glm::vec2 colpos = glm::vec2(c->GetX(), c->GetZ());
    glm::vec2 colposn = glm::vec2(c->GetX()+1, c->GetZ()+1);
    glm::vec2 colposp = glm::vec2(c->GetX()-1, c->GetZ()-1);
    for (auto v : vlist) {
	EXPECT_NE(v, colposn) << "Collided with object in position (posn)" << i;
	EXPECT_NE(v, colpos) << "Collided with object in position" << i;
	EXPECT_NE(v, colposp) << "Collided with object in position (posp)" << i;
	
	i++;
    }
   

}