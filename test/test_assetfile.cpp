#include "test_assetfile.hpp"

using namespace Familyline::Graphics;


void AssetFileTest::SetUp()
{
    af = new AssetFile{};
}

void AssetFileTest::TearDown()
{
    delete af;
}

TEST_F(AssetFileTest, CheckIfTestFileLoadedOK) {
    ASSERT_NO_THROW(af->LoadFile("test/test-assets.yml"));
}


TEST_F(AssetFileTest, CheckIfAssetsWereParsed) {
    af->LoadFile("test/test-assets.yml");

    auto soldier = af->GetAsset("soldier.mesh");
    ASSERT_NE(soldier, nullptr);
    EXPECT_EQ(soldier->name, "soldier.mesh");
    EXPECT_EQ(soldier->type, "mesh");
    EXPECT_EQ(soldier->path, "meshes/soldier.obj");
        
    auto tower = af->GetAsset("tower.mesh");
    ASSERT_NE(tower, nullptr);
    EXPECT_EQ(tower->name, "tower.mesh");
    EXPECT_EQ(tower->type, "mesh");
    EXPECT_EQ(tower->path, "meshes/tower.obj");
}


TEST_F(AssetFileTest, CheckIfAssetDependenciesWereParsed) {
    af->LoadFile("test/test-assets.yml");

    auto soldier = af->GetAsset("soldier.mesh");
    EXPECT_NE(soldier, nullptr);
    ASSERT_EQ(soldier->dependencies.size(), 1);

    auto soldiert = af->GetAsset("soldier.texture");
    EXPECT_NE(soldiert, nullptr);
    ASSERT_EQ(soldiert->dependencies.size(), 0);
 
    auto tower = af->GetAsset("tower.mesh");
    EXPECT_NE(tower, nullptr);
    ASSERT_EQ(tower->dependencies.size(), 1);
}
