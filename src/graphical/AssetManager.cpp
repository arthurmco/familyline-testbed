#include "AssetManager.hpp"

#include <sys/types.h>
#include <dirent.h>

using namespace Tribalia::Graphics;


void AssetManager::AddAssetGroup(const char* path, const char* tag)
{
    /* Read all files and folders looking for valid assets */
    AssetGroup* grp = new AssetGroup{};
    strcpy(grp->tag, tag);
    strcpy(grp->folder, path);
    Log::GetLog()->Write("Added asset group tag '%s', path '%s'",
        grp->tag, grp->folder);
    _groups.push_back(grp);

    DIR *d, *dp = NULL, *dn = NULL;
    const char* dpath = path;

    while (d) {
        d = opendir(dpath);

        if (!d) {
            Log::GetLog()->Write("Could not open %s: %s",
                dpath, strerror(errno));
        } else {
            struct dirent* de;
            std::string spath{path};

            while (de = readdir(d)) {
                spath += '/';
                spath += dpath;


            }

        }

        d = dn;
        if (!d)
            d = dp;

    }


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
