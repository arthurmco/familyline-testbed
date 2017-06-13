#include "TextureOpener.hpp"
#include <cstring>

using namespace Tribalia::Graphics;

static bool isDevilOn = false;


Texture* TextureOpener::TextureOpenBMP(FILE* f, const char* path) {
    Texture* t = nullptr;

    char bmp_header[14];
    if (fread((void*)bmp_header, 1, 14, f) < 14) {
	Log::GetLog()->Warning("texture-opener", "Unexpected EOF in BMP header while opening %s", path);	
	return nullptr;
    }

    if (bmp_header[0] != 'B' && bmp_header[1] != 'M') {
	Log::GetLog()->Warning("texture-opener", "Invalid magic number while opening %s", path);	
	return nullptr;
    }

    unsigned off_pixels = *(unsigned int*)&bmp_header[0x0a];

    if (off_pixels == 0) {
	Log::GetLog()->Warning("texture-opener", "Wrong pixel header while opening %s", path);	
	return nullptr;
    }

    char dib_header[40];
    if (fread((void*)dib_header, 1, 40, f) < 40) {
	Log::GetLog()->Warning("texture-opener", "Unexpected EOF in DIB header while opening %s", path);	
	return nullptr;
    }
   
    unsigned hsize, width = 0, height = 0, bpp = 0;
    hsize = *(unsigned int*)&dib_header[0];

    if (hsize <= 12) {
		width = *(unsigned short*)&dib_header[4];
		height = *(unsigned short*)&dib_header[6];
		bpp = *(unsigned short*)&dib_header[10];
    } else {
		width = *(unsigned int*)&dib_header[4];
		height = *(unsigned int*)&dib_header[8];
		bpp = *(unsigned short*)&dib_header[14];

		if (dib_header[16] != 0) {
			Log::GetLog()->Warning("texture-opener", "BMP Compression is not supported opening %s", path);	
			return nullptr;
		}
    }

    if (width == 0 || height == 0 || bpp == 0) {
		Log::GetLog()->Warning("texture-opener", "Invalid size while opening %s", path);	
		return nullptr;
    }

    printf("sz %ux%ux%u\n", width, height, bpp);
    
    /* Read the rows */
    const int bytespp = bpp/8;

    // The row size of a BMP file
    size_t row_size = (size_t) floor(((bpp * width + 31) / 32.0)) * 4.0;
    auto* image_grid = new unsigned char[width*height*bytespp];
    fseek(f, off_pixels, SEEK_SET);

    for (size_t y = 0; y < height; y++) {
		/* We read row by row, because the each BMP row is padded by a multiple
		   of 4. */
		char* row = new char[row_size];
		if (fread((void*)row, 1, row_size, f) < row_size) {
			printf("EEE %d", y);
			return t;
		}

		for (size_t x = 0; x < width; x ++) {
			/* BMP stores image upside down.
			   Here we rotate back to the normal orientation, hence the (height-y-1) */
	    
			image_grid[ (height-y-1)*width*bytespp + x*bytespp ] = row[x*bytespp];
			image_grid[ (height-y-1)*width*bytespp + x*bytespp + 1 ] = row[x*bytespp+1];
			image_grid[ (height-y-1)*width*bytespp + x*bytespp + 2 ] = row[x*bytespp+2];
		}

		delete[] row;
    }
    
    const GLenum iformat = GL_BGR; //default format for bitmap images.
    t = new Texture(width, height, iformat, image_grid);
    
    fclose(f);
    return t;
}


Texture* TextureOpener::Open(const char* path)
{
    	Log::GetLog()->Write("texture-opener", "Opening %s", path);
	FILE* f = fopen(path, "rb");
	
	if (!f) {
		Log::GetLog()->Warning("texture-opener", "File %s not found"
			" (error %d)", path, errno);
		return nullptr;
	}

	/* For some reason, BMP files don't load.
	   We need to do a fast custom BMP loader, then */
	if (strstr(path, ".bmp") || strstr(path, ".BMP")) {
	    return TextureOpenBMP(f, path);
	}

	fclose(f);
	
	/* Initialize devIL if not */
	if (!isDevilOn) {
		ilInit();	
	}

	/* 	Initialize a handle for the image and
		open it */
	ILuint handle = 0;
	ilGenImages(1, &handle);
	ilBindImage(handle);	

	if (ilLoad(IL_TYPE_UNKNOWN, path) == IL_FALSE) {
		int e = ilGetError();
		const char* estr;

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
		    char* eestr = new char[128];
		    sprintf(eestr, "Unknown error %#x", e);
		    estr = eestr;
		    break;
		}

		Log::GetLog()->Warning("texture-opener",
				       "Error '%s' while opening %s",
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
