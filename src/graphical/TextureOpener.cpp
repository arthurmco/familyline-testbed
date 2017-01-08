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
	if (ilLoadImage(path) == IL_FALSE) {
		int e = ilGetError();
		char* estr;

		switch (e) {
			case IL_COULD_NOT_OPEN_FILE:
				estr = "Could not open file";
				break;

			case IL_INVALID_EXTENSION:
			case IL_INVALID_FILE_HEADER:
				estr = "Invalid file format.";
				break;

			case IL_INVALID_PARAM:
				estr = "Unrecognized file.";
				break;			
			
			default: 
				estr = new char[128];
				sprintf(estr, "Unknown error %#x", e);
				break;
		}

		Log::GetLog()->Warning("TextureOpener: Error '%s' while opening %s",
			estr, path);
		return nullptr;
	}

	int width = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);
	GLenum format;

	format = ilGetInteger(IL_IMAGE_FORMAT);

	unsigned char* data = ilGetData();
	Texture* tex = new Texture(width, height, format, data);
		
	/* Clean up image */
	ilBindImage(0);
	ilDeleteImage(handle);	

	/* Returns texture */
	return tex;
}
