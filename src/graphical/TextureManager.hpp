/***
    Texture manager

    Copyright (C) 2016 Arthur M.

***/

#include <map>
#include <string>
#include <cstring> //strcmp()
#include "Texture.hpp"
#include "Log.hpp"

#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

namespace Tribalia {
namespace Graphics {

    class TextureManager
    {
    private:
        std::map<std::string, Texture*> _textures;

        static TextureManager* _tm;

    public:
        /* Add texture, return its ID */
        int AddTexture(const char* name, Texture*);

        Texture* GetTexture(int ID);
        Texture* GetTexture(const char* name);

        static TextureManager* GetInstance()
        {
            if (!_tm)
                _tm = new TextureManager();

            return _tm;
        }
    };

}
}

#endif
