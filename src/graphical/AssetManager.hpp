/*
    Asset management class

    Copyright (C) 2016 Arthur M
*/

#include "MaterialManager.hpp"
#include "TextureManager.hpp"
#include "Mesh.hpp"

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

namespace Tribalia {
namespace Graphics {

struct AssetGroup {
    char folder[256];   // Asset group folder
    char tag[32];       // Asset group tag.
};

enum AssetType {
    ASSET_MESH,
    ASSET_MATERIAL,
    ASSET_TEXTURE,
};

struct Asset {
    AssetGroup* group;
    char relpath[128];  // Relative path of the asset file
    int asset_type;
    union {
        Texture* texture;
        Material* material;
        Mesh* mesh;
    } asset;
};

class AssetManager {

private:
    std::vector<Asset*> _assets;
    std::vector<AssetGroup*> _groups;

public:
    void AddAssetGroup(const char* path, const char* tag);

    Asset* GetAsset(const char* relpath);
};

}
}




#endif /* end of include guard: ASSETMANAGER_H */
