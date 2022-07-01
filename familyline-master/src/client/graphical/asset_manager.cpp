#include <fmt/format.h>

#include <algorithm>
#include <client/graphical/asset_manager.hpp>
#include <client/graphical/gfx_service.hpp>
#include <client/graphical/materialopener/MTLOpener.hpp>
#include <client/graphical/meshopener/MD2Opener.hpp>
#include <client/graphical/meshopener/OBJOpener.hpp>
#include <client/graphical/texture_asset.hpp>
#include <common/logger.hpp>
#include <iterator>  // for std::back_inserter

using namespace familyline;
using namespace familyline::graphics;

/// TODO: handle multiple assets in one file
std::vector<std::shared_ptr<AssetObject>> loadMeshAsset(Asset& asset)
{
    auto ms   = MeshOpener::Open(asset.path.c_str());
    auto& log = LoggerService::getLogger();

    // Check if we have a texture and a material in the file
    // If we have neither, leave as it is, the renderer will set
    // a default material
    // If we have material, but no texture, leave as it is
    // If we have texture, but no material, set the material from the
    // texture as our material
    // If we have both, set our material's texture as the specified texture.

    auto textureAsset = std::find_if(
        asset.dependencies.begin(), asset.dependencies.end(),
        [&](Asset& a) { return (a.type == AssetType::TextureAsset); });

    auto materialAsset = std::find_if(
        asset.dependencies.begin(), asset.dependencies.end(),
        [&](Asset& a) { return (a.type == AssetType::MaterialAsset); });

    int materialID = ms[0]->getVertexInfo()[0].materialID;
    if (materialID < 0 && materialAsset != asset.dependencies.end()) {
        // Get the material from the assets.yml

        auto mats              = materialAsset->loadAssetObject();
        Material* meshMaterial = GFXService::getMaterialManager()->getMaterial(
            std::dynamic_pointer_cast<Material>(mats[0])->getName());
        materialID = meshMaterial->getID();
        log->write(
            "asset-manager", LogType::Debug, "loaded material for {}: {}\n", asset.name,
            materialID);
        auto vdata = ms[0]->getVertexInfo();

        vdata[0].materialID = materialID;
        ms[0]->setVertexInfo(std::move(vdata));
    }

    // We have a texture
    if (textureAsset != asset.dependencies.end()) {
        char* matname = new char[textureAsset->name.size() + 10];
        sprintf(matname, "texture:%s", textureAsset->name.c_str());
        Material* texMaterial = GFXService::getMaterialManager()->getMaterial(matname);

        if (materialID < 0 && texMaterial) {
            // No material, but textured
            auto vdata = ms[0]->getVertexInfo();

            vdata[0].materialID = texMaterial->getID();
            ms[0]->setVertexInfo(std::move(vdata));

        } else if (materialID >= 0 && texMaterial) {
            // With both material and texture
            Material* meshMaterial = GFXService::getMaterialManager()->getMaterial(materialID);
            meshMaterial->setTexture(texMaterial->getTexture());
        }

        delete[] matname;
    }

    for (auto i = 1; i < ms.size(); i++) delete ms[i];

    return {std::shared_ptr<Mesh>(ms[0])};
}

std::vector<std::shared_ptr<AssetObject>> loadTextureAsset(Asset& asset)
{
    return {std::make_shared<TextureAsset>(asset.name, asset.path)};
}

std::vector<std::shared_ptr<AssetObject>> loadMaterialAssets(Asset& asset)
{
    MTLOpener mo;
    auto ms = mo.Open(asset.path.c_str());

    std::vector<std::shared_ptr<AssetObject>> mlist;
    for (auto& m : ms) {
        mlist.push_back(std::shared_ptr<Material>(m));
    }

    return mlist;
}

std::vector<std::shared_ptr<AssetObject>> Asset::loadAssetObject()
{
    switch (this->type) {
        case AssetType::MeshAsset: return loadMeshAsset(*this);
        case AssetType::MaterialAsset: return loadMaterialAssets(*this);
        case AssetType::TextureAsset: return loadTextureAsset(*this);
        default:
            this->error = std::make_optional(AssetError::InvalidAssetType);
            throw asset_exception(
                fmt::format("Asset type for '{}' is invalid", this->path),
                AssetError::InvalidAssetType);
    }
}

AssetManager::AssetManager()
{
    openers.push_back(new OBJOpener());
    openers.push_back(new MD2Opener());
}

/**
 * We also add the textures and materials automatically to their respective
 * managers
 */
Asset AssetManager::processAsset(AssetItem& av)
{
    Asset asset;
    asset.name = av.name;

    std::transform(
        av.dependencies.begin(), av.dependencies.end(), std::back_inserter(asset.dependencies),
        [&](std::shared_ptr<AssetItem>& adep) { return this->_assets[adep->name]; });

    asset.path   = av.path;
    asset.error  = std::optional<AssetError>();
    asset.object = std::optional<std::shared_ptr<AssetObject>>();

    if (av.type == "mesh") {
        asset.type = AssetType::MeshAsset;

    } else if (av.type == "texture") {
        asset.type        = AssetType::TextureAsset;
        auto texasset     = std::dynamic_pointer_cast<TextureAsset>(asset.loadAssetObject()[0]);
        TextureHandle tex = *texasset->getHandle(*GFXService::getTextureManager().get());
        
        asset.object = std::make_optional(texasset);
        GFXService::getTextureManager()->registerTexture(asset.name.c_str(), tex);
        GFXService::getTextureManager()->uploadTexture(tex);

        char* matname = new char[asset.name.size() + 10];
        sprintf(matname, "texture:%s", asset.name.c_str());
        Material* m = new Material{matname, MaterialData(0.1, 0.8, 0.5)};
        m->setTexture(tex);

        // addMaterial copies the object
        // TODO: make this explicit in typing (for example, by using an unique_ptr)
        GFXService::getMaterialManager()->addMaterial(m);
        delete m;
        delete[] matname;

    } else if (av.type == "material") {
        asset.type = AssetType::MaterialAsset;

        auto ms      = asset.loadAssetObject();
        asset.object = std::make_optional(ms[0]);
        for (auto& m : ms) {
            GFXService::getMaterialManager()->addMaterial((Material*)m.get());
        }
    } else {
        asset.type = AssetType::UnknownAsset;
    }

    return asset;
}

/**
 * Load the asset list file
 *
 * We could use const, but we have a pseudo-iterator that have an internal
 * pointer that points to the current file, so.....
 *
 */
void AssetManager::loadFile(AssetFile& file)
{
    auto& log = LoggerService::getLogger();

    file.resetAsset();
    for (auto a = file.nextAsset(); a; a = file.nextAsset()) {
        auto av = a.value();

        if (_assets.find(av->name) != _assets.end()) {
            continue;
        }

        for (auto& dep : av->dependencies) {
            if (_assets.find(dep->name) != _assets.end()) {
                continue;
            }

            Asset dasset               = this->processAsset(*dep.get());
            this->_assets[dasset.name] = dasset;
            log->write(
                "asset-manager", LogType::Info, "found asset '{}' at path '{}' ({} dependencies)",
                dasset.name, dasset.path, dasset.dependencies.size());
        }

        Asset asset = this->processAsset(*av);

        this->_assets[asset.name] = asset;
        log->write(
            "asset-manager", LogType::Info, "found asset '{}' at path '{}' ({} dependencies)",
            asset.name, asset.path, asset.dependencies.size());
    }
    file.resetAsset();
}

// TODO: copy the asset data at each load
std::shared_ptr<AssetObject> AssetManager::getAsset(std::string_view assetName)
{
    auto& log = LoggerService::getLogger();

    auto asset_it = this->_assets.find(std::string{assetName});
    if (asset_it == this->_assets.end()) {
        throw asset_exception(
            fmt::format("Asset '{}' cannot be found", assetName), AssetError::AssetNotFound);
    }

    //    if (!asset_it->second.object) {
    asset_it->second.object = std::make_optional(asset_it->second.loadAssetObject()[0]);
    //    }
    log->write(
        "asset-manager", LogType::Info, "getting asset '{}' at path '{}'",
        asset_it->second.name, asset_it->second.path);

    return asset_it->second.object.value();
}
