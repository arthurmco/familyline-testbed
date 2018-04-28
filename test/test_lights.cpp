#include "test_lights.hpp"

using namespace Tribalia::Graphics;

void LightTest::SetUp()
{
	LightManager::RemoveAll();
}

void LightTest::TearDown()
{

	LightManager::RemoveAll();
}

TEST_F(LightTest, TestIfManagerGetsTheBiggerLight) {
	
	Light l = Light{ "test01", glm::vec3(10, 10, 10), 255, 255, 255, 9 };
	LightManager::AddLight(&l);

	auto lights = LightManager::GetBestLights(glm::vec3(10, 10, 10), 10, 1);
	
	ASSERT_EQ(lights.size(), 1);
	ASSERT_STREQ(l.GetName(), lights.front()->GetName());
}

TEST_F(LightTest, TestIfManagerGetsTheBiggerLightWhenTwo) {

	Light l1 = Light{ "test02", glm::vec3(10, 10, 10), 255, 255, 255, 12 };
	Light l2 = Light{ "test03", glm::vec3(10, 10, 10), 255, 255, 255, 7 };
	LightManager::AddLight(&l1);
	LightManager::AddLight(&l2);

	auto lights = LightManager::GetBestLights(glm::vec3(10, 10, 10), 10, 1);

	ASSERT_EQ(lights.size(), 1);
	ASSERT_STREQ(l1.GetName(), lights.front()->GetName());
}

TEST_F(LightTest, TestIfManagerGetsTheBiggerTwoLightWhenThree) {

	Light l1 = Light{ "test04", glm::vec3(10, 10, 10), 255, 255, 255, 12 };
	Light l2 = Light{ "test05", glm::vec3(11, 11, 11), 255, 255, 255, 10 };
	Light l3 = Light{ "test06", glm::vec3(12, 12, 12), 255, 255, 255, 14 };
	LightManager::AddLight(&l1);
	LightManager::AddLight(&l2);
	LightManager::AddLight(&l3);

	auto lights = LightManager::GetBestLights(glm::vec3(10, 10, 10), 10, 2);

	ASSERT_EQ(lights.size(), 2);
	ASSERT_STREQ(l1.GetName(), lights.front()->GetName());
	ASSERT_STREQ(l3.GetName(), lights.back()->GetName());
}

TEST_F(LightTest, TestIfManagerCutsOffLightsTooFar) {

	Light l1 = Light{ "test01", glm::vec3(10, 10, 10), 255, 255, 255, 12 };
	Light l2 = Light{ "test02", glm::vec3(20, 10, 20), 255, 255, 255, 10 };
	LightManager::AddLight(&l1);
	LightManager::AddLight(&l2);

	auto lights = LightManager::GetBestLights(glm::vec3(10, 10, 10), 10, 2);

	ASSERT_EQ(lights.size(), 1);
	ASSERT_STREQ(l1.GetName(), lights.front()->GetName());
}
