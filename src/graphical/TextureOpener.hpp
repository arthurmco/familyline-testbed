/***
	Texture opening class

	Copyright (C) 2016 Arthur M
***/

/* This engine uses the DevIL library to open images */
#include <IL/il.h>

#include <errno.h>
#include <cstdio>

#include "Texture.hpp"
#include "../Log.hpp"

#ifndef TEXTUREOPENER_HPP
#define TEXTUREOPENER_HPP


namespace Tribalia {
namespace Graphics {

class TextureOpener
{
public:
	/* 	Opens a image as a texture. Returns a texture
		object, or nullptr if texture wasn't found */
	static Texture* Open(const char* path);


};

} /* Graphics */
} /* Tribalia */

#endif /* end of include guard: TEXTUREOPENER_HPP */
