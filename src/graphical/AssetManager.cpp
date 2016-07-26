#include "AssetManager.hpp"

#include <sys/types.h>

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

Asset* AssetManager::GetAsset(const char* relpath)
{
    for (auto it = _assets.begin(); it != _assets.end(); it++) {
        if (!strcmp(relpath, (*it)->relpath)) {
            return (*it);
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
        a->relpath, aname[a->asset_type % 3], a->asset_type, grp->folder);
}

#include <functional>	// std::ptr_fun()
#include <cstdio>
/*  Read assets from a Tribalia Asset File (*.taif)
    Returns 'true' if read successfully, 'false' if not
*/
bool AssetManager::ReadFromFile(const char* file)
{
    FILE* f = fopen(file, "r");
    if (!f) return false;

    /*  Get file path. This will be the base for the relative paths
        later */
    std::string start_path{file};
    start_path = start_path.substr(0,start_path.find_last_of('/'));

    AssetGroup* grp = this->AddAssetGroup(file, "fromfile");
    while (!feof(f)) {
        char line[512];
        fgets(line, 512, f);

        /* Take off the newline */
        size_t l = strlen(line);
        if (l > 0)
            if (line[l-1] == '\n')
                line[l-1] = 0;

        l--;
        if (l > 0)
            if (line[l-1] == '\r')
                line[l-1] = 0;

        std::string sline{line};

        /* Take off comments */
        if (sline.find_first_of('#') != std::string::npos) {
            continue;
        }

        std::string file, type;
        size_t ipos, tpos;

        ipos = sline.find("file");
        if (ipos == std::string::npos) {
            continue;
        }

        file = sline.substr(ipos+5,
            sline.find_first_of(',', ipos+5) - (ipos+5));

        tpos = sline.find("type", ipos+5);
        if (tpos == std::string::npos) {
            continue;
        }

        type = sline.substr(tpos+5);

        /* trim type and file */
        file.erase(file.begin(), std::find_if(file.begin(), file.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
        type.erase(type.begin(), std::find_if(type.begin(), type.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));

        Log::GetLog()->Write("Found asset '%s', type '%s'", file.c_str(), type.c_str());

        Asset* asset = new Asset();
        strcpy(asset->relpath, file.c_str());

        if (type == "mesh") {
            asset->asset_type = ASSET_MESH;

        } else if (type == "texture") {
            asset->asset_type = ASSET_TEXTURE;

        } else if (type == "material") {
            asset->asset_type = ASSET_MATERIAL;
        }

        this->AddAsset(grp, asset);


    }

    /* Now load the data */
    int processer = ASSET_TEXTURE;
    while (processer >= 0) {
        for (auto it = _assets.begin(); it != _assets.end(); it++) {
            Asset* at = (*it);

            /* Process the assets in the right order:
                First textures, then materials and meshes */
            if (at->asset_type != processer) continue;

            switch (at->asset_type) {
                case ASSET_TEXTURE: {
                    TextureOpener o;
                    at->asset.texture = o.Open(at->relpath);

                    std::string fname{at->relpath};
                    std::string tname = fname.substr(fname.find_last_of('/')+1);
                    TextureManager::GetInstance()->AddTexture(tname.c_str(), at->asset.texture);
                } break;
                case ASSET_MATERIAL: {
                    std::string ext{at->relpath};
                    ext = ext.substr(ext.find_last_of('.')+1);

                    if (ext == "mtl") {
                        MTLOpener o;
                        std::vector<Material*>* m = new std::vector<Material*>;
                        *m = o.Open(at->relpath);
                        at->asset.material = m;
                        MaterialManager::GetInstance()->AddMaterials(*m);
                    }
                } break;
                case ASSET_MESH: {

                    std::string ext{at->relpath};
                    ext = ext.substr(ext.find_last_of('.')+1);

                    if (ext == "obj") {
                        OBJOpener o;
                        at->asset.mesh = o.Open(at->relpath);
                    }
                } break;
            }
        }
        processer--;

    }

    fclose(f);
    return true;
}
