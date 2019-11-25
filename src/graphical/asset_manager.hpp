/*
  Asset management class

  Copyright (C) 2016, 2019 Arthur M
*/

#include "asset_file.hpp"
#include "asset_object.hpp"

#include <unordered_map>
#include "asset_object.hpp"
#include "exceptions.hpp"

#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

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
         * If there is an error, the 'error' variable will be set.
         *
         * Please call `loadAssetObject` to load it.
         */
        std::optional<std::shared_ptr<AssetObject>> object;
        std::optional<AssetError> error;

        std::string name; //! Asset name, as identified in the yml file
        AssetType type;   //! Asset type
        std::string path; //! Path to the asset

        std::vector<Asset> dependencies;

        /**
         * Load the asset object
         *
         * Returns the asset object on success, throws asset_exception on error
         */
        std::vector<std::shared_ptr<AssetObject>> loadAssetObject();
    };

    /**
     * Asset manager
     */
    class AssetManager {
    private:
        std::unordered_map<std::string, Asset> _assets;

        Asset processAsset(AssetItem&);


    public:
        AssetManager();
        
        /**
         * Load the asset list file
         */
        void loadFile(AssetFile& file);

        std::shared_ptr<AssetObject> getAsset(std::string_view assetName);
        // void reloadAsset(std::string_view assetName); //TODO: might implement, might not
    };

}




#endif /* end of include guard: ASSETMANAGER_H */

