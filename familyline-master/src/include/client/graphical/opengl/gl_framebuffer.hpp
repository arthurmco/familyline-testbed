#pragma once

#include <client/graphical/framebuffer.hpp>
#include <client/graphical/opengl/gl_headers.hpp>
#include <string_view>

#ifdef RENDERER_OPENGL

namespace familyline::graphics
{
/**
 * Represents an memory area where you can draw 3D data
 *
 * This memory area will have 2D data, an image, which you can
 * use as a texture for something, like an gui overlay, or as a
 * map for something like shadows
 */
class GLFramebuffer : public Framebuffer
{
private:
    GLuint _handle;

    // texture, for color data, read-write
    GLuint _textureHandle;

    // renderbuffer, for depth data, write-only
    GLuint _rboHandle;

    void setupTexture(int width, int height);

public:
    GLFramebuffer(std::string_view name, int width, int height);

    /// Call this before you start drawing data
    virtual void startDraw();

    /// Call this after you end drawing data
    virtual void endDraw();

    virtual int getTextureHandle();

    virtual ~GLFramebuffer();
};
}  // namespace familyline::graphics

#endif
