/*  
    The asset file plus its loader

    Copyright (C) 2016, 2018-2019 Arthur M
*/

#ifndef ASSETFILE_HPP
#define ASSETFILE_HPP

#include <vector>
#include <cstdio>
#include <string_view>

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

        std::list<std::shared_ptr<AssetItem>> dependencies;

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
        std::list<std::shared_ptr<AssetItem>> assets;

        std::list<std::shared_ptr<AssetItem>> parseFile(yaml_parser_t* parser);

        /**
         * Process the assets and discover the dependencies between them 
         */
        std::list<std::shared_ptr<AssetItem>> processDependencies(
            std::list<std::shared_ptr<AssetItem>>&& assets);
    

    public:
        void loadFile(const char* file);

        /**
         * Get an asset 
         */
        AssetItem* getAsset(std::string_view name) const;
    };


}


#endif
