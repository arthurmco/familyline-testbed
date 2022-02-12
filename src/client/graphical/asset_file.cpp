#include <algorithm>
#include <client/graphical/asset_file.hpp>
#include <client/graphical/exceptions.hpp>
#include <common/logger.hpp>
#include <string>

#include <config.h>
#include <fmt/format.h>

using namespace familyline::graphics;

std::string AssetItem::getItemOr(const char* key, const char* defaultval)
{
    auto it = this->items.find(std::string{key});
    if (it == this->items.end()) return std::string{defaultval};

    return it->second;
}

void AssetFile::loadFile(const char* ofile)
{
    std::string sfile{ofile};
#ifdef _WIN32

    // Replace slashes
    std::replace(sfile.begin(), sfile.end(), '/', '\\');
#endif

    try {
        YAML::Node asset = YAML::LoadFile(sfile);

        auto& log = LoggerService::getLogger();
        log->write("asset-file-loader", LogType::Info, "loaded file {}", sfile);
        auto lassets = this->parseFile(asset);
        log->write("asset-file-loader", LogType::Info, "loaded {} assets", lassets.size());
        
        this->assets = this->processDependencies(std::move(lassets));
        
    } catch (YAML::BadFile& b) {
        throw asset_exception(fmt::format("Failed to open asset file list %s", sfile),
                              AssetError::AssetFileOpenError);
    }
}

std::vector<std::shared_ptr<AssetItem>> AssetFile::parseFile(YAML::Node& root)
{
    auto& log = LoggerService::getLogger();

    std::vector<std::shared_ptr<AssetItem>> alist;

    bool asset_str  = false;
    bool asset_list = false;

    YAML::Node assetsnode = root["assets"];

    if (!assetsnode.IsSequence()) {
        return alist;
    }

    for (size_t i = 0; i < assetsnode.size(); i++) {
        if (!assetsnode[i]["name"] || !assetsnode[i]["type"] || !assetsnode[i]["path"]) {
            log->write(
                "asset-file-loader", LogType::Info, "asset {} does not have all required parameters", i);
            continue;
        }

        auto replace_token = [](std::string& str, const char* from, const char* to) {
            auto find_str = str.find(from);
            if (find_str != std::string::npos)
                str.replace(find_str, find_str + strlen(from), to);
        };

        
        AssetItem asset;
        asset.name = assetsnode[i]["name"].as<std::string>();
        asset.type = assetsnode[i]["type"].as<std::string>();
        asset.path = assetsnode[i]["path"].as<std::string>();

        std::string cpath = asset.path;
        replace_token(cpath, "${MODELS_DIR}/", MODELS_DIR);
        replace_token(cpath, "${MATERIALS_DIR}/", MATERIALS_DIR);
        replace_token(cpath, "${TEXTURES_DIR}/", TEXTURES_DIR);
        asset.path = cpath;
        
        for (auto it = assetsnode[i].begin(); it != assetsnode[i].end(); ++it) {
            std::string key = it->first.as<std::string>();
            if (key != "name" && key != "type" && key != "path")
                asset.items[key] = it->second.as<std::string>();
        }

        log->write(
            "asset-file-loader", LogType::Info, "found asset {} type {} path {}",
            asset.name, asset.type,
            asset.path);

        alist.push_back(std::shared_ptr<AssetItem>{new AssetItem{asset}});

    }        
    
    return alist;
}

/* Process the assets and discover the dependencies between them */
std::vector<std::shared_ptr<AssetItem>> AssetFile::processDependencies(
    std::vector<std::shared_ptr<AssetItem>>&& assets)
{
    auto& log = LoggerService::getLogger();

    /* Callback for mesh dependency.
     * Return true if the mesh 'asset_name' is a child of mesh.
     *
     * If it's true, we need to load the childs before
     */
    auto fnMeshDep = [](const char* asset_name, AssetItem* const mesh) {
        auto meshtext = mesh->items.find("mesh.texture");
        if (meshtext != mesh->items.end()) {
            if (!strcmp(asset_name, meshtext->second.c_str())) return true;
        }

        meshtext = mesh->items.find("mesh.material");
        if (meshtext != mesh->items.end()) {
            if (!strcmp(asset_name, meshtext->second.c_str())) return true;
        }

        return false;
    };

    // TODO: Texture dependency.

    for (auto asset : assets) {
        auto fnDependency = [fnMeshDep, asset](std::shared_ptr<AssetItem> dependent) {
            if (asset->type == "mesh")
                return fnMeshDep(dependent->name.c_str(), asset.get());
            else
                return false;
        };

        asset->dependencies.resize(assets.size());
        auto it_dep =
            std::copy_if(assets.begin(), assets.end(), asset->dependencies.begin(), fnDependency);

        asset->dependencies.erase(it_dep, asset->dependencies.end());
        log->write(
            "asset-file-loader", LogType::Info, "asset {} has {} dependencies",
            asset->name, asset->dependencies.size());

        for (auto dep : asset->dependencies) {
            log->write("", LogType::Info, "\t{}", dep->name);
        }
    }

    return assets;
}

/* Get an asset */
AssetItem* AssetFile::getAsset(std::string_view name) const
{
    for (const auto asset : this->assets) {
        if (asset->name == name) {
            return asset.get();
        }
    }

    return nullptr;
}

std::optional<std::shared_ptr<AssetItem>> AssetFile::nextAsset()
{
    if ((unsigned)this->_asset_idx >= assets.size())
        return std::optional<std::shared_ptr<AssetItem>>();

    return make_optional(this->assets[this->_asset_idx++]);
}
