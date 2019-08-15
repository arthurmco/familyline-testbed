#include "asset_manager.hpp"

#include "../config.h"
#include "meshopener/OBJOpener.hpp"
#include "meshopener/MD2Opener.hpp"
#include "materialopener/MTLOpener.hpp"
#include <fmt/format.h>

using namespace familyline::graphics;

std::shared_ptr<AssetObject> loadMeshAsset(Asset& asset) {
    auto ms = MeshOpener::Open(asset.path.c_str());

    return std::shared_ptr<Mesh>(ms[0]);
}

std::shared_ptr<AssetObject> loadTextureAsset(Asset& asset) {

}

std::shared_ptr<AssetObject> loadMaterialAsset(Asset& asset)
{

}

std::shared_ptr<AssetObject> Asset::loadAssetObject()
{
    switch (this->type) {
    case MeshAsset: return loadMeshAsset(*this);
    case MaterialAsset: return loadMaterialAsset(*this);
    case TextureAsset: return loadTextureAsset(*this);
    default:
        this->error = std::make_optional(AssetError::InvalidAssetType);
        throw asset_exception(
            fmt::format("Asset type for '{}' is invalid", this->path),
            AssetError::InvalidAssetType);
    }
}

AssetManager::AssetManager()
{
    new OBJOpener();
    new MD2Opener(); 
}


/**
 * Load the asset list file
 *
 * We could use const, but we have a pseudo-iterator that have an internal
 * pointer that points to the current file, so.....
 */
void AssetManager::loadFile(AssetFile& file)
{
    file.resetAsset();
    for (auto a = file.nextAsset(); a; a = file.nextAsset()) {
        auto av = a.value();

        Asset asset;
        asset.name = av->name;

        if (av->type == "mesh") {
            asset.type = AssetType::MeshAsset;
        } else if (av->type == "texture") {
            asset.type = AssetType::TextureAsset;
        } else if (av->type == "material") {
            asset.type = AssetType::MaterialAsset;
        } else {
            asset.type = AssetType::UnknownAsset;
        }

        asset.path = av->path;
        asset.error = std::optional<AssetError>();
        asset.object = std::optional<std::shared_ptr<AssetObject>>();

        this->_assets[asset.name] = asset;
        Log::GetLog()->InfoWrite("asset-manager", "found asset '%s' at path '%s'",
                                 asset.name.c_str(), asset.path.c_str());
    }
    file.resetAsset();

}

std::shared_ptr<AssetObject> AssetManager::getAsset(std::string_view assetName)
{
    auto asset_it = this->_assets.find(std::string{assetName});
    if (asset_it == this->_assets.end()) {
        throw asset_exception(
            fmt::format("Asset '{}' cannot be found", assetName),
            AssetError::AssetNotFound);
    }

    if (!asset_it->second.object) {
        asset_it->second.object = std::make_optional(asset_it->second.loadAssetObject());
    }

    Log::GetLog()->InfoWrite("asset-manager", "getting asset '%s' at path '%s'",
                             asset_it->second.name.c_str(), asset_it->second.path.c_str());

    return asset_it->second.object.value();
}
