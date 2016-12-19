/*  
    Represents an asset file

    Copyright (C) 2016 Arthur M
*/

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

#include "GFXExceptions.hpp"

#ifndef ASSETFILE_HPP
#define ASSETFILE_HPP


namespace Tribalia {
namespace Graphics {

struct AssetFileItem {
    std::string name;
    std::string path;
    std::string type;

    /* The items who this file item depends */
    std::vector<AssetFileItem*> depends;

    /* The childs of this item */
    std::vector<AssetFileItem*> childs;

    /* Is the asset fully added? */
    bool isProcessed = false;
};

class AssetFile {
private:
    FILE* _fAsset;
    std::string _path;
    std::vector<AssetFileItem*> _file_items;

    std::string GetAbsolutePath(std::string rel);
    

public:
    AssetFile(const char* path);

    /* Build the file item dependency tree */
    void BuildFileItemTree();

    /* Get the tree you built */
    std::vector<AssetFileItem*>* GetFileItemTree();

    ~AssetFile();

};

}
}


#endif