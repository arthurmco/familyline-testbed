#include "AssetManager.hpp"

#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/stat.h>
#include <unistd.h>
#endif
using namespace Tribalia::Graphics;


AssetGroup* AssetManager::AddAssetGroup(const char* path, const char* tag)
{
    /* Read all files and folders looking for valid assets */
    AssetGroup* grp = new AssetGroup{};
    strcpy(grp->tag, tag);
    strcpy(grp->folder, path);
    Log::GetLog()->Write("Added asset group tag '%s', path '%s'",
        grp->tag, grp->folder);
    _groups.push_back(grp);
    return grp;
}

/* Query group folder for valid assets */
void AssetManager::QueryAssetGroup(AssetGroup* grp)
{
/*    DIR *d, *dp = NULL, *dn = NULL;
    const char* dpath = grp->folder;

    while (d) {
        d = opendir(dpath);

        if (!d) {
            Log::GetLog()->Write("Could not open %s: %s",
                dpath, strerror(errno));
        } else {
            struct dirent* de;
            std::string spath{grp->folder};

            while (de = readdir(d)) {
                spath += '/';
                spath += dpath;


            }

        }

        d = dn;
        if (!d)
            d = dp;

    }

	*/
}

Asset* AssetManager::GetAsset(const char* path)
{
    for (auto it = _assets.begin(); it != _assets.end(); it++) {
        if (!strcmp(path, (*it)->path)) {
			Asset* at = (*it);
			if (!at->asset.mesh) {
				this->LoadAsset(at);
			}
			return at;
        }
    }

    return nullptr;
}

void AssetManager::AddAsset(AssetGroup* grp, Asset* a)
{
    a->group = grp;
    _assets.push_back(a);

    static const char* aname[] =
        {"mesh", "material", "texture"};
    Log::GetLog()->Write("Added asset %s type %s (%d) to group %s",
        a->path, aname[a->asset_type % 3], a->asset_type, grp->folder);
}


bool CheckIfExists(const char* path) {
#ifndef _MSC_VER
    struct stat statbuf;
    if (stat(path, &statbuf) < 0) {
      switch (errno) {
        case EBADF:
        case ENOENT:  return false;
      }
    }

    return (statbuf.st_size > 0);
#else
    #pragma message("Tribalia is unable to find a stat() version!")
    #pragma message(" Using less destructive return value")
    return true;
#endif

}

Asset* AssetManager::RetrieveAsset(AssetGroup* grp, AssetFileItem*& afi)
{
	std::string extension = afi->path.substr(afi->path.find_last_of('.')+1);
	Asset* a = new Asset();
	strcpy(a->path, afi->path.c_str());
		
	if (afi->type == "mesh") {
		a->asset_type = ASSET_MESH;
	} else if (afi->type == "texture") {
		a->asset_type = ASSET_TEXTURE;
	} else if (afi->type == "material") { 
		a->asset_type = ASSET_MATERIAL;
	}

	/** Treat dependencies */
	for (auto& dep : afi->depends) {
		if (!dep->isProcessed) {
			AddAsset(grp, RetrieveAsset(grp, dep));
		}
		
	}
	
	afi->isProcessed = true;
	if (!LoadAsset(a)) {
		throw asset_exception(afi, "Error loading asset");
	}
	return a;
}


#include <functional>	// std::ptr_fun()
#include <cstdio>
/*  Read assets from a Tribalia Asset File (*.taif)
    Returns 'true' if read successfully, 'false' if not
*/
bool AssetManager::ReadFromFile(const char* file)
{
	AssetGroup* grp = this->AddAssetGroup(file, "fromfile");
	
	AssetFile* af = new AssetFile(file);
	af->BuildFileItemTree();

	for (auto& afi : *af->GetFileItemTree()) {
		if (!afi->isProcessed) {
			AddAsset(grp, RetrieveAsset(grp, afi));
		}
	}

	return true;
}



bool AssetManager::LoadAsset(Asset* at)
{

	switch (at->asset_type) {
	case ASSET_TEXTURE: {
		TextureOpener o;
		at->asset.texture = o.Open(at->path);

		if (at->asset.texture) {
			std::string fname{ at->path };
			int i = fname.find_last_of('/');
			if (i == std::string::npos)	i = -1;
			std::string tname = fname.substr(i + 1);
			TextureManager::GetInstance()->AddTexture(tname.c_str(), at->asset.texture);
			return true;
		}

	} break;
	case ASSET_MATERIAL: {
		std::string ext{ at->path };
		ext = ext.substr(ext.find_last_of('.') + 1);

		if (ext == "mtl") {
			MTLOpener o;
			std::vector<Material*>* m = new std::vector<Material*>;
			*m = o.Open(at->path);

			if (m) {
				at->asset.material = m;
				MaterialManager::GetInstance()->AddMaterials(*m);
				return true;
			}

		}
	} break;
	case ASSET_MESH: {

		std::string ext{ at->path };
		ext = ext.substr(ext.find_last_of('.') + 1);
        MeshOpener* o;

		if (ext == "obj") {
            o = new OBJOpener{};
        } else if (ext == "md2") {
            o = new MD2Opener{};
        } else {
            Log::GetLog()->Write("%s uses an unsupported extension",
                at->path);
            break;
        }

		at->asset.mesh = o->Open(at->path);
		if (at->asset.mesh) {
			return true;
		}

	} break;
	}

	return false;
}
