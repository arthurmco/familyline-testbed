/***
	Texture opening class

	Copyright (C) 2016 Arthur M
***/

/* This engine uses the DevIL library to open images */
#include <IL/il.h>

#include <errno.h>
#include <cmath>
#include <cstdio>

#include "Texture.hpp"
#include "Log.hpp"

#ifndef TEXTUREOPENER_HPP
#define TEXTUREOPENER_HPP


namespace Tribalia {
namespace Graphics {

class TextureFile
{
private:
    ILuint _handle;
    GLenum _format;
public:
    ILuint GetHandle() const;
    GLenum GetFormat() const;

    /* TODO: Make ImageControl read the texture data */

    TextureFile(ILuint handle, GLenum format);

    /* Get a texture from a cut in the loaded image */
    Texture* GetTextureCut(int x = 0, int y = 0, int w = 0, int h = 0);

    /* Get texture raw data from a cut */
    unsigned char* GetTextureRaw(int x = 0, int y = 0, int w = 0, int h = 0);
    
    ~TextureFile();
        
};
    
class TextureOpener
{
private:
    static TextureFile* TextureOpenBMP(FILE*, const char*);
    
public:
    /* 	Opens a image as a texture. Returns a texture
	object, or nullptr if texture wasn't found */
    static TextureFile* OpenFile(const char* path);
    static Texture* OpenTexture(const char* path);


};

} /* Graphics */
} /* Tribalia */

#endif /* end of include guard: TEXTUREOPENER_HPP */
