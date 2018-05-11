/*  
    The asset file plus its loader

    Copyright (C) 2016, 2018 Arthur M
*/

#ifndef ASSETFILE_HPP
#define ASSETFILE_HPP

#include <vector>
#include <cstdio>
#include <cstring>

#include "GFXExceptions.hpp"

#include <unordered_map>
#include <list>
#include <memory>

extern "C" {	
	#include <yaml.h>
}

#include <string>

namespace Familyline::Graphics {

    /* The asset itself, as a file, not an object */
    struct AssetItem {
	std::string name, type, path;
	std::unordered_map<std::string /* key */, std::string> items;

	std::list<std::shared_ptr<AssetItem>> dependencies;

	bool isLoaded = false;

	std::string GetItemOr(const char* key, const char* defaultval);
    };

    class AssetFile {
    private:
	/* The list of assets
	 *
	 * The dependencies will be evaluated only after all assets are loaded
	 */
	std::list<std::shared_ptr<AssetItem>> assets;

	std::list<std::shared_ptr<AssetItem>> ParseFile(yaml_parser_t* parser);

	/* Process the assets and discover the dependencies between them */
	std::list<std::shared_ptr<AssetItem>> ProcessDependencies(
	    std::list<std::shared_ptr<AssetItem>>&& assets);
    

    public:
	void LoadFile(const char* file);

	/* Get an asset */
	AssetItem* GetAsset(const char* name) const;
    };


}


#endif
