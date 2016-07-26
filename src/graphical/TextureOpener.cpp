#include "TextureOpener.hpp"

using namespace Tribalia::Graphics;

static bool isDevilOn = false;


Texture* TextureOpener::Open(const char* path)
{
	/* Initialize devIL if not */
	if (!isDevilOn) {
		ilInit();	
	}

	/* 	Initialize a handle for the image and
		open it */
	ILuint handle;
	ilGenImages(1, &handle);
	ilBindImage(handle);	

	Log::GetLog()->Write("TextureOpener: Opening %s", path);
	FILE* f = fopen(path, "rb");
	
	if (!f) {
		Log::GetLog()->Warning("TextureOpener: File %s not found"
			" (error %d)", path, errno);
		return nullptr;
	}

	fclose(f);
	if (ilLoad(IL_TYPE_UNKNOWN, path) == IL_FALSE) {
		Log::GetLog()->Warning("TextureOpener: Error while opening %s",
			path);
		return nullptr;
	}

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	GLenum format;

	switch (ilGetInteger(IL_IMAGE_FORMAT)) {
		case IL_RGB: format = GL_RGB; break;
		case IL_BGR: format = GL_BGR; break;
		default: format = GL_RGB; break;
	}

	unsigned char* data = ilGetData();
	Texture* tex = new Texture(width, height, format, data);
		
	/* Clean up image */
	ilBindImage(0);
	ilDeleteImage(handle);	

	/* Returns texture */
	return tex;
}
