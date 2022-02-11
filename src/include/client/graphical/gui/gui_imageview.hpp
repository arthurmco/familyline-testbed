#pragma once

#include <SDL2/SDL_surface.h>

#include <client/graphical/gui/gui_control.hpp>
#include <client/graphical/texture.hpp>

#include <tuple>

namespace familyline::graphics::gui {


/**
 * GUIImageView
 *
 * An image visualization control
 * You use this control to draw some image.
 *
 * The image view will accept a texture as a constructor, but it will use
 * it only to build a local copy of the image. This copy is the same image,
 * but converted to ARGB32 pixel format, so that our current and first backend,
 * Cairo, could render it easily.
 */
class GUIImageView : public GUIControl
{
public:
    GUIImageView(Texture& image, GUIControlRenderInfo i = {}) :
        GUIControl(i),
        image_(make_surface_unique_ptr(SDL_ConvertSurfaceFormat(image.data.get(),
                                                                SDL_PIXELFORMAT_ARGB8888,
                                                                0)))
        {
            draw_width_ = image.data->w;
            draw_height_ = image.data->h;
        }

    /**
     * A textual way of describing the control
     * If we were in Python, this would be its `__repr__` method
     *
     * Used *only* for debugging purposes.
     */
    virtual std::string describe() const;

    /// Called when this control is resized or repositioned
    virtual void onResize(int nwidth, int nheight, int nx, int ny);

    virtual void autoresize();

    /// Called when the parent need to update
    virtual void update() { dirty_ = false; };

    SDL_Surface* getImageData() const { return image_.get();  }

    std::tuple<int, int> getImageSize() const { return std::make_tuple(draw_width_, draw_height_); }
    
private:
    /**
     * The width and height that the control will resize the image
     * to and draw them
     */
    int draw_width_, draw_height_;

    /**
     * The position of where we will draw the image
     */
    int draw_x = 0, draw_y = 0;

    surface_unique_ptr image_;
    
    void calculateNeededSize();
};

}
