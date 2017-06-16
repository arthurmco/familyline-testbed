/***
    Texture representation class

    Copyright (C) 2016,2017 Arthur M

***/
#include <string>
#include "../Log.hpp"

#include <glm/glm.hpp>
#include <SDL2/SDL_opengl.h>

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

namespace Tribalia {
namespace Graphics {

    /* Texture coordinate areas, in relative/clamped coordinates */
    struct TextureArea {
	glm::vec2 pos = glm::vec2(0,0);
	glm::vec2 size = glm::vec2(1,1);
    };
    
class Texture
{
private:
    GLuint _tex_handle;
    std::string _name;

    TextureArea _area;
    /* The size of the texture */
    int _w, _h;

    /* The size of the image itself, preserved by multiple subtextures */
    int _totalw, _totalh;
    bool _handleowner = true;
    
public:
    Texture(int width, int height, GLenum format, void* data);
    Texture(GLuint handle, int w, int h, glm::vec2 uv_pos, glm::vec2 uv_size);

    GLint GetHandle() const;

    glm::vec2 GetAreaPosition(bool clamp = true);
    glm::vec2 GetAreaSize(bool clamp = true);
    
    void SetName(const char* name);
    const char* GetName() const;

    Texture* GetSubTexture(int xpos, int ypos, int w, int h);

    /* Retrieves a offset matrix, for automatic conversion between local
       and global texcoords */
    glm::mat4 GetOffsetMatrix();
};

} /* Graphics */
} /* Tribalia */

#endif /* end of include guard: TEXTURE_HPP */
