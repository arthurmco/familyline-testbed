/*
    Asset management class

    Copyright (C) 2016 Arthur M
*/

#include "MaterialManager.hpp"
#include "TextureManager.hpp"
#include "TextureOpener.hpp"
#include "Mesh.hpp"

#include "meshopener/OBJOpener.hpp"

#include "materialopener/MTLOpener.hpp"


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
        std::vector<Material*>* material;
        Mesh* mesh;
    } asset;
};

class AssetManager {

private:
    std::vector<Asset*> _assets;
    std::vector<AssetGroup*> _groups;

public:
    AssetGroup* AddAssetGroup(const char* path, const char* tag);

    /* Query group folder for valid assets */
    void QueryAssetGroup(AssetGroup* grp);

    Asset* GetAsset(const char* relpath);
    void AddAsset(AssetGroup* grp, Asset* a);

    /*  Read assets from a Tribalia Asset File (*.taif)
        Returns 'true' if read successfully, 'false' if not
    */
    bool ReadFromFile(const char* file);
};

}
}




#endif /* end of include guard: ASSETMANAGER_H */
