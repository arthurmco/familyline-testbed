/*  
    The asset file plus its loader

    Copyright (C) 2016, 2018-2019 Arthur M
*/

#ifndef ASSETFILE_HPP
#define ASSETFILE_HPP

#include <vector>
#include <cstdio>
#include <string_view>

#include <optional>
#include <unordered_map>
#include <list>
#include <memory>

extern "C" {	
#include <yaml.h>
}

#include <string>

namespace familyline::graphics {

    /**
     * The asset itself, as a file, not an object 
     *
     * \see AssetFile
     */
    struct AssetItem {
        std::string name, type, path;
        std::unordered_map<std::string /* key */, std::string> items;

        std::vector<std::shared_ptr<AssetItem>> dependencies;

        bool isLoaded = false;

        std::string getItemOr(const char* key, const char* defaultval);
    };

    class AssetFile {
    private:
        /**
         * The list of assets
         *
         * The dependencies will be evaluated only after all assets are loaded
         * \see AssetItem, AssetManager
         */
        std::vector<std::shared_ptr<AssetItem>> assets;
        int _asset_idx = 0;

        std::vector<std::shared_ptr<AssetItem>> parseFile(yaml_parser_t* parser);

        /**
         * Process the assets and discover the dependencies between them 
         */
        std::vector<std::shared_ptr<AssetItem>> processDependencies(
            std::vector<std::shared_ptr<AssetItem>>&& assets);
    

    public:
        void loadFile(const char* file);

        /**
         * Get an asset 
         */
        AssetItem* getAsset(std::string_view name) const;

        /**
         * Pseudo-iterator functions for this asset, so we do not
         * expose the inner object yet let other classes retrieve
         * all of the assets without specifying a name
         */

        //! Resets an iterator counter 
        void resetAsset() { this->_asset_idx = 0; }

        //! Gets a probable asset. If asset is empty, we reached the end of the list
        std::optional<std::shared_ptr<AssetItem>> nextAsset();        
    };


}


#endif
