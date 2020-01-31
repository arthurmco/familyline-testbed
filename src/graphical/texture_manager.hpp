/***
    Texture manager

    Copyright (C) 2016, 2018-2019 Arthur Mendes.

***/
#pragma once

#include <map>
#include <string>
#include <cstring> //strcmp()
#include "Texture.hpp"
#include "Log.hpp"

namespace familyline::graphics {

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
    };

}