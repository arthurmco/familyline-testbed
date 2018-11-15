#include "test_pathfind.hpp"
#include "logic/ObjectEventEmitter.hpp"

using namespace familyline::logic;

void PathfinderTest::SetUp()
{
	_om = new ObjectManager;
	_pf = new PathFinder{ _om };
	_pf->InitPathmap(256, 256);
	
	o = _om->addObject(new TestObject(1, 10, 1, 10));
}

void PathfinderTest::TearDown()
{
    ObjectEventEmitter::clearListeners();
	delete _pf;
	delete _om;
}

namespace glm {
	/* Code for print vector position */
	void PrintTo(const vec2& vec, ::std::ostream *os) {
		*os << "(" << vec.x << ", " << vec.y << ")";
	}
}


TEST_F(PathfinderTest, TestStraightPathfind) {

    ObjectEventEmitter::distributeMessages();
    auto vlist = _pf->CreatePath((AttackableObject*) o.lock().get(), glm::vec2(32, 32));
	EXPECT_EQ(vlist.size(), 23) << "List size mismatch";

	/* Check if we didn't got off the path too far */
	int i = 0;
	for (auto v : vlist) {
		ASSERT_GE(v.x, 10) << "Element index " << i << " with xpos <= xmin";
		ASSERT_LE(v.x, 32) << "Element index " << i << " with xpos >= xmax";
		ASSERT_GE(v.y, 10) << "Element index " << i << " with ypos <= ymax";
		ASSERT_LE(v.y, 32) << "Element index " << i << " with ypos >= ymax";

		i++;
	}

	auto vlast = vlist.back();
	EXPECT_EQ(glm::vec2(32, 32), vlast);

}

TEST_F(PathfinderTest, TestObstaclePathfind) {
    auto c = _om->addObject(new TestObject(1, 21, 1, 21)).lock();

    ObjectEventEmitter::distributeMessages();
    _pf->UpdatePathmap(256, 256);

    auto vlist = _pf->CreatePath((AttackableObject*) o.lock().get(), glm::vec2(32, 32));

	/* Check if we didn't collided */
	int i = 0;
	glm::vec2 colpos = glm::vec2(c->position.x, c->position.z);
	glm::vec2 colposn = glm::vec2(c->position.x + 1, c->position.z + 1);
	glm::vec2 colposp = glm::vec2(c->position.x - 1, c->position.z - 1);
	for (auto v : vlist) {
		EXPECT_NE(v, colposn) << "Collided with object in position (posn) " << i;
		EXPECT_NE(v, colpos) << "Collided with object in position " << i;
		EXPECT_NE(v, colposp) << "Collided with object in position (posp) " << i;

		i++;
	}

	auto vlast = vlist.back();
	EXPECT_EQ(glm::vec2(32, 32), vlast);
}

TEST_F(PathfinderTest, TestTwoObstaclesPathfind) {
    auto c = _om->addObject(new TestObject(1, 21, 1, 21)).lock();
    auto d = _om->addObject(new TestObject(1, 26, 1, 26)).lock();

    ObjectEventEmitter::distributeMessages();
    _pf->UpdatePathmap(256, 256);

    auto vlist = _pf->CreatePath((AttackableObject*) o.lock().get(), glm::vec2(32, 32));
	
	/* Check if we didn't collided */
	int i = 0;
	glm::vec2 ccolpos = glm::vec2(c->position.x, c->position.z);
	glm::vec2 ccolposn = glm::vec2(c->position.x + 1, c->position.z + 1);
	glm::vec2 ccolposp = glm::vec2(c->position.x - 1, c->position.z - 1);
	glm::vec2 dcolpos = glm::vec2(d->position.x, d->position.z);
	glm::vec2 dcolposn = glm::vec2(d->position.x + 1, d->position.z + 1);
	glm::vec2 dcolposp = glm::vec2(d->position.x - 1, d->position.z - 1);
	for (const auto& v : vlist) {
		EXPECT_NE(v, ccolposn) << "Collided with object in position (posn) " << i;
		EXPECT_NE(v, ccolpos) << "Collided with object in position " << i;
		EXPECT_NE(v, ccolposp) << "Collided with object in position (posp) " << i;

		EXPECT_NE(v, dcolposn) << "Collided with object in position (posn) " << i;
		EXPECT_NE(v, dcolpos) << "Collided with object in position " << i;
		EXPECT_NE(v, dcolposp) << "Collided with object in position (posp) " << i;

		i++;
	}

	auto vlast = vlist.back();
	EXPECT_EQ(glm::vec2(32, 32), vlast);
}

TEST_F(PathfinderTest, TestIfPathfindingPutObjectInOtherObjectPosition) {
	/* Test if the pathfinder puts your object in the final destination even
	 * if the final destination has another object in there
	 *
	 * It shouldn't
	 */
    auto c = _om->addObject(new TestObject(1, 32, 1, 32)).lock();

    ObjectEventEmitter::distributeMessages();
    _pf->UpdatePathmap(256, 256);

    auto vlist = _pf->CreatePath((AttackableObject*) o.lock().get(), glm::vec2(32, 32));

    auto vlast = vlist.back();

    /* Check if we didn't go to the start or end points */
    EXPECT_NE(glm::vec2(32, 32), vlast) << "Shouldn't go to obstacle";
    EXPECT_NE(glm::vec2(10, 10), vlast) << "Shouldn't go back to start";

    EXPECT_LE(28, vlast.x);
    EXPECT_GE(36, vlast.x);
    EXPECT_LE(28, vlast.y);
    EXPECT_GE(36, vlast.y);

    _om->removeObject(c);
}
