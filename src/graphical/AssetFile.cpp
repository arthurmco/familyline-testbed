#include "AssetFile.hpp"
#include "../Log.hpp"

using namespace Tribalia::Graphics;

static std::string Trim(std::string str)
{
    size_t endpos = str.find_last_not_of(" \t");
    if (endpos != std::string::npos)
    {
        str = str.substr(0, endpos+1);
    }

    size_t startpos = str.find_first_not_of(" \t");
    if (startpos != std::string::npos)
    {
        str = str.substr(startpos);
    }

    return str;

}

#define SEPARATOR '/'
#if defined(_WIN32)
    #define SEPARATOR '\\'
#endif

std::string AssetFile::GetAbsolutePath(std::string rel)
{   
    std::string dir = _path.substr(0, _path.find_last_of(SEPARATOR)+1);
    dir.append(rel);
    return dir;    
}

AssetFile::AssetFile(const char* path)
{
    _fAsset = fopen(path, "r");
    if (!_fAsset) {
        char* msg = new char[strlen(path)+48];
        sprintf(msg, "Failure to open %s: %d", path, errno);
        throw new asset_exception(this, msg);
    }

    _path = std::string(path);
}

/*  Represents a material item that wasn't found, but specified to be
    loaded late */
struct DeferredLink {
    AssetFileItem* afi;
    std::string asset;
};

/* Build the file item dependency tree */
void AssetFile::BuildFileItemTree()
{
    bool isInAsset = false;

    // General options
    std::string name, path, type;

    // Mesh-specific
    std::string material_asset, texture_asset;

    std::vector<DeferredLink> deferred_material_assets;
    std::vector<DeferredLink> deferred_texture_assets;

    rewind(_fAsset);
    while (!feof(_fAsset)) {
        // Get char and take out \n and \r
        char line[256];
        fgets(line, 255, _fAsset);
        char* n = strrchr(line, '\n');
        if (n)  *n = 0;
        n = strrchr(line, '\r');
        if (n)  *n = 0;

        std::string l = Trim(std::string(line));

        /* Take out comments */
        if (l[0] == '#') {
            continue;
        }

        size_t comment = l.find_first_of('#');
        if (comment != std::string::npos) {
            if (l[comment-1] != '\\') {
                l = l.substr(0,comment);
            }
        }

        /* Find the '{' corresponding to a new asset */
        size_t brktpos = l.find_last_of('{');
        if (brktpos != std::string::npos) {
            name = Trim(l.substr(0, brktpos-1));
            isInAsset = true;
        }

        if (isInAsset) {
            size_t i;

            i = l.find("type:");
            if (i != std::string::npos) {
                type = Trim(l.substr(i+5));
                continue;
            }

            i = l.find("path:");
            if (i != std::string::npos) {
                path = GetAbsolutePath(Trim(l.substr(i+5)).c_str());
                continue;
            }

            if (type == "mesh") {
                i = l.find("material_asset:"); 
                if (i != std::string::npos) {
                    material_asset = Trim(l.substr(i+15));
                    continue;
                }

                i = l.find("texture_asset:"); 
                if (i != std::string::npos) {
                    texture_asset = Trim(l.substr(i+14));
                    continue;
                }
                
            }

            i = l.find("}");
            if (i != std::string::npos) {
                AssetFileItem* afi = new AssetFileItem;
                afi->name = name;
                afi->path = path;
                afi->type = type;
/*                printf("\t new asset found: %s, %s, %s\n", name.c_str(), type.c_str(), path.c_str());*/
                /* Check if our material asset has been loaded */
                if (type == "mesh") {
                    bool mfound = false;
                    if (material_asset != "") {
                        for (auto& afs : _file_items) {
                            if (afs->name == material_asset) {
                                mfound = true;
                                afi->depends.push_back(afs);
                                break;
                            }
                        }

                        if (!mfound) {
                            DeferredLink dl;
                            dl.afi = afi;
                            dl.asset = material_asset;
			    Log::GetLog()->Warning("%s material not found, deferred load", material_asset.c_str());
                            deferred_material_assets.push_back(dl);
                        }
                        
                    }

                    mfound = false;
                    if (texture_asset != "") {
                        for (auto& afs : _file_items) {
                            if (afs->name == texture_asset) {
                                mfound = true;
                                afi->depends.push_back(afs);
			        break;
                            }
                        }

                        if (!mfound) {
                            DeferredLink dl;
                            dl.afi = afi;
                            dl.asset = texture_asset;
			    Log::GetLog()->Warning("%s texture not found, deferred load", texture_asset.c_str());

                            deferred_texture_assets.push_back(dl);
                        }
                        
                    }
                } else if (type == "material") {
                    /* Check if our deferred material is here */
		    while (true) {
			bool breaks = true;
			for (auto it = deferred_material_assets.begin();
			     it != deferred_material_assets.end();
			     it++) {
			    if (it->asset == name) {
				it->afi->depends.push_back(afi);
				printf("%s material loaded now\n", name.c_str());
				deferred_material_assets.erase(it);
				breaks = false;
				break;
			    }
			}

			if (breaks) {
			    break;
			}
		    }
		    
                    
                } else if (type == "texture") {
                    /* Check if our texture material is here */
		    bool breaks = true;

		    while (true) {
			breaks = true;
			for (auto it = deferred_texture_assets.begin();
			     it != deferred_texture_assets.end();
			     it++) {
			    if (it->asset == name) {
				it->afi->depends.push_back(afi);
				printf("%s texture loaded now\n", name.c_str());
				deferred_texture_assets.erase(it);
				breaks = false;
				break;
			    }
			}
			
			if (breaks) {
			    break;
			}
		    }
                    
                }


                name = "";
                path = "";
                type = "";
                        
                material_asset = "";
		texture_asset = "";

                isInAsset = false;
                _file_items.push_back(afi);
            }
        }


    }

    if (deferred_texture_assets.size() > 0) {
	Log::GetLog()->Warning("Unrecognized textures: %d", deferred_texture_assets.size());
	for (auto& unrecog : deferred_texture_assets) {
	    Log::GetLog()->Warning("Texture name: '%s', referenced in '%s'", unrecog.asset.c_str(),
				   unrecog.afi->name.c_str());
	}
    }

    if (deferred_material_assets.size() > 0) {
	Log::GetLog()->Warning("Unrecognized materials: %d", deferred_material_assets.size());
	for (auto& unrecog : deferred_material_assets) {
	    Log::GetLog()->Warning("Texture name: '%s', referenced in '%s'", unrecog.asset.c_str(),
				   unrecog.afi->name.c_str());
	}
    }

    for (auto& it : _file_items) {
    	printf("%s\n", it->name.c_str());

    	for (auto& dep_it : it->depends) {
    	    printf(" |-> %s\n", dep_it->name.c_str());
    	}
    }
}

AssetFile::~AssetFile()
{
    fclose(_fAsset);
}

/* Get the tree you built */
std::vector<AssetFileItem*>* AssetFile::GetFileItemTree() 
{
    return &_file_items;
}
