#include "AssetFile.hpp"
#include "Log.hpp"
#include "../config.h"

#include <yaml.h>
#include <algorithm>

using namespace Tribalia::Graphics;

void AssetFile::LoadFile(const char* file)
{
    FILE* fAsset = fopen(file, "r");
    if (!fAsset) {
	throw std::runtime_error("Failed to open file");
    }

    yaml_parser_t parser;
    if (!yaml_parser_initialize(&parser)) {
	throw std::runtime_error("Failed to initialize parser");
    }

    yaml_parser_set_input_file(&parser, fAsset);

    auto assets = std::move(this->ParseFile(&parser));
    Log::GetLog()->Write("asset-file-loader",
			 "loaded %zu assets", assets.size());

    this->assets = std::move(this->ProcessDependencies(std::move(assets)));
    
    yaml_parser_delete(&parser);
    fclose(fAsset);

}

std::list<std::shared_ptr<AssetItem>> AssetFile::ParseFile(yaml_parser_t* parser)
{
    std::list<std::shared_ptr<AssetItem>> alist;

    bool asset_str = false;
    bool asset_list = false;
    
    yaml_event_t event;

    // Try to find the 'assets' list
    do {
	if (!yaml_parser_parse(parser, &event)) {
	    char* s;
	    asprintf(&s, "Asset file parsing error: %d",
		     parser->error);
	    throw std::runtime_error(s);
	}

	if (event.type == YAML_SCALAR_EVENT) {
	    if (!strcmp((const char*)event.data.scalar.value, "assets")) {
		asset_str = true;

	    }
	} else if (asset_str && event.type == YAML_SEQUENCE_START_EVENT) {
	    
	    asset_list = true;
	    break;
	    
	} else {
	    asset_str = false;
	    asset_list = false;
	}
	
    } while (event.type != YAML_STREAM_END_EVENT);

    if (!asset_str || !asset_list) {
	throw std::runtime_error("Could not find the asset list in the file");
    }

    AssetItem current_asset;
    bool is_key = false, is_val = false;
    std::string current_key;

    int list_sequences = 1;
	
    // Parse the list contents
    do {
	if (!yaml_parser_parse(parser, &event)) {
	    char* s;
	    asprintf(&s, "Asset list parsing error: %d",
		     parser->error);
	    throw std::runtime_error(s);
	}

	switch (event.type) {
	    // The sequence start has already been processed before, so let's
	    // direct into the mapping

	case YAML_MAPPING_START_EVENT:
	    if (!asset_list) continue;
	    
	    current_asset = {};
	    is_key = true;
	    break;

	case YAML_SCALAR_EVENT:
	{
	    if (!asset_list) continue;
	    
	    const char* str = (const char*)event.data.scalar.value;

	    // If is key, then save the key so we can remember later
	    if (is_key) {
		current_key = std::string{str};
	    }
	    
	    // If is value, then save the old key+value in the file.
	    if (is_val) {
		if (current_key == "name") {
		    current_asset.name = str;
		} else if (current_key == "type") {
		    current_asset.type = str;
		} else if (current_key == "path") {
		    current_asset.path = str;
		} else {
		    current_asset.items[current_key] = std::string{str};
		}
	    }
	    
	    is_key = !is_key;
	    is_val = !is_val;
	}
	break;

	case YAML_MAPPING_END_EVENT:
	    if (!asset_list) continue;
	    // Save the resulting asset in the alist

	    Log::GetLog()->InfoWrite("asset-file-loader", 
				     "found asset %s type %s path %s",
				     current_asset.name.c_str(), current_asset.type.c_str(),
				     current_asset.path.c_str());

	    alist.push_back(std::shared_ptr<AssetItem>{new AssetItem{current_asset}});
	    is_key = false;
	    is_val = false;
	    break;
	

	    // Handle unplanned lists correctly
	case YAML_SEQUENCE_START_EVENT:
	    list_sequences++;
	    break;
	    
	case YAML_SEQUENCE_END_EVENT:
	    list_sequences--;

	    if (list_sequences <= 0) {
		// End of sequence. End of file

		asset_list = false;
	    }
	    
	    break;

	    
	default:
	    is_key = false;
	    is_val = false;
	    break;
	    
	} // switch(...)	    
	
    } while (event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    return alist;
}

/* Process the assets and discover the dependencies between them */
std::list<std::shared_ptr<AssetItem>> AssetFile::ProcessDependencies(
    std::list<std::shared_ptr<AssetItem>>&& assets)
{
    /* Callback for mesh dependency.
     * Return true if the mesh 'asset_name' is a child of mesh.
     * 
     * If it's true, we need to load the childs before
     */
    auto fnMeshDep = [](const char* asset_name, AssetItem* const mesh) {
	auto meshtext = mesh->items.find("mesh.texture");

	if (meshtext != mesh->items.end()) {
	    if (!strcmp(asset_name, meshtext->second.c_str()))
		return true;
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
	auto it_dep = std::copy_if(assets.begin(), assets.end(),
				   asset->dependencies.begin(),
				   fnDependency);

	asset->dependencies.erase(it_dep, asset->dependencies.end());
	Log::GetLog()->InfoWrite("asset-file-loader",
				 "asset %s has %zu dependencies", asset->name.c_str(),
				 asset->dependencies.size());
	
	for (auto dep : asset->dependencies) {
	    Log::GetLog()->InfoWrite("asset-file-loader",
				     "\t%s", dep->name.c_str());
	}
	
	
    }

    return assets;
}
    

/* Get an asset */
AssetItem* AssetFile::GetAsset(const char* name) const
{
    for (const auto asset : this->assets) {
	if (!strcmp(asset->name.c_str(), name)) {
	    return asset.get();
	}
    }

    return nullptr;
}

