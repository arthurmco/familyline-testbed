/*
  Asset management class

  Copyright (C) 2016, 2019 Arthur M
*/

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "asset_file.hpp"

#include <unordered_map>
#include "asset_object.hpp"

namespace familyline::graphics {

    /**
     * The asset itself
     */
    struct Asset {
        /**
         * A pointer to the internal asset
         *
         * An empty optional means "not loaded". Might be not loaded because
         * it really was not loaded, or because of an error
         *
         * Please call `loadAssetObject` to be sure.
         */
        std::optional<std::shared_ptr<AssetObject>> object;

        std::string name;        //! Asset name, as identified in the yml file
        AssetType type;          //! Asset type
        std::string path;        //! Path to the asset

        std::vector<Asset> dependencies;

        /**
         * Load the asset object
         *
         * Returns the asset object on success, throws asset_exception on error
         */
        std::shared_ptr<AssetObject> loadAssetObject();
    };


    /**
     * Asset manager
     */
    class AssetManager {
    private:
        std::map<std::string, Asset>  assets;

    public:
        void loadFile(const AssetFile& file);

        std::shared_ptr<AssetObject> getAsset(std::string_view assetName);
        // void reloadAsset(std::string_view assetName); //TODO: might implement, might not
    };

}




#endif /* end of include guard: ASSETMANAGER_H */
