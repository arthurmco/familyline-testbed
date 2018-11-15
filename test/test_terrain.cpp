#include "test_terrain.hpp"

using namespace familyline::logic;

void TerrainTest::SetUp()
{
    tf = new TerrainFile("test/terrain_test.trtb");
}

void TerrainTest::TearDown()
{
    delete tf;
}


TEST_F(TerrainTest, TestIfTerrainOpens) {
    Terrain* t = nullptr;
    ASSERT_NO_THROW(t = tf->GetTerrain());

    ASSERT_EQ(t->GetWidth(), 640);
    ASSERT_EQ(t->GetHeight(), 400);
    ASSERT_EQ(t->GetSectionCount(), 6); //terrain slot is 256x256

    ASSERT_EQ(t->GetHeightFromPoint(0, 0), 238);
    ASSERT_EQ(t->GetHeightFromPoint(10, 10), 238);
    ASSERT_EQ(t->GetHeightFromPoint(320, 32), 246);
    ASSERT_EQ(t->GetHeightFromPoint(260, 260), 183);
}


TEST_F(TerrainTest, TestIfTerrainSectionLoads) {
    Terrain* t = nullptr;
    ASSERT_NO_THROW(t = tf->GetTerrain());

    auto t1 = t->GetHeightFromPoint(90, 90);
    auto t2 = t->GetHeightFromPoint(356, 90);
    auto t3 = t->GetHeightFromPoint(90, 356);
    auto t4 = t->GetHeightFromPoint(260, 356);

    auto dataptr = t->GetAllSections();

    ASSERT_NE(dataptr, nullptr);
    ASSERT_NE(dataptr[0], nullptr);
    ASSERT_NE(dataptr[1], nullptr);
    ASSERT_NE(dataptr[3], nullptr);
    ASSERT_NE(dataptr[4], nullptr);

    constexpr unsigned section_side = 256;
    
    ASSERT_EQ(t1, dataptr[0]->data[90*section_side+90].elevation);
    ASSERT_EQ(t2, dataptr[1]->data[90*section_side+100].elevation);
    ASSERT_EQ(t3, dataptr[3]->data[100*section_side+90].elevation);
    ASSERT_EQ(t4, dataptr[4]->data[100*section_side+4].elevation);

    ASSERT_EQ(t1, t->GetSection(0)->data[90*section_side+90].elevation);
    ASSERT_EQ(t2, t->GetSection(1)->data[90*section_side+100].elevation);
    ASSERT_EQ(t3, t->GetSection(3)->data[100*section_side+90].elevation);
    ASSERT_EQ(t4, t->GetSection(4)->data[100*section_side+4].elevation);

}
