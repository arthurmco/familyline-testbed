#include "AssetFile.hpp"

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

/* Build the file item dependency tree */
void AssetFile::BuildFileItemTree()
{
    bool isInAsset = false;
    std::string name, path, type;
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

            i = l.find("}");
            if (i != std::string::npos) {
                AssetFileItem* afi = new AssetFileItem;
                afi->name = name;
                afi->path = path;
                afi->type = type;
                printf("\t new asset found: %s, %s, %s\n", name.c_str(), type.c_str(), path.c_str());

                name = "";
                path = "";
                type = "";

                isInAsset = false;
                _file_items.push_back(afi);
            }
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