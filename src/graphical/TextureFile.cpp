#include "TextureOpener.hpp"

using namespace Tribalia::Graphics;

TextureFile::TextureFile(ILuint handle, GLenum format)
    : _handle(handle), _format(format)
    
{ }

Texture* TextureFile::GetTextureCut(int x, int y, int w, int h)
{
    ilBindImage(_handle);
    int s = (_format == IL_BGR || _format == IL_RGB) ? 3 : 4;
    
    char* c = new char[w*h*s];
    Log::GetLog()->Write("texture-file", "Got image with handle %d and format 0x%x, cutting [%d, %d, %d, %d]", _handle, _format, x, y, w, h);
    
    ilCopyPixels(x, y, 0, w, h, 1, _format, IL_UNSIGNED_BYTE, c);
    ilBindImage(0);

    return new Texture(w, h, _format, c);
}

ILuint TextureFile::GetHandle() const
{
    return _handle;
}

TextureFile::~TextureFile()
{
    ilDeleteImage(_handle);
}
