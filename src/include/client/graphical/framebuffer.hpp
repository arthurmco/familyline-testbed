#pragma once

#include <string_view>

namespace familyline::graphics
{
/**
 * Represents an memory area where you can draw 3D data
 *
 * This memory area will have 2D data, an image, which you can
 * use as a texture for something, like an gui overlay, or as a
 * map for something like shadows
 */
class Framebuffer
{
protected:
    std::string_view name_;
    int width_;
    int height_;
    
public:
    Framebuffer(std::string_view name, int width, int height)
        : name_(name), width_(width), height_(height)
        {}

    /// Call this before you start drawing data
    virtual void startDraw() = 0;

    /// Call this after you end drawing data
    virtual void endDraw() = 0;

    virtual int getTextureHandle() = 0;

    virtual ~Framebuffer() {};
};
}  // namespace familyline::graphics
