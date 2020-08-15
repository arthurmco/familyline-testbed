#pragma once

#include <cairo/cairo.h>

#include <client/graphical/gui/control.hpp>
#include <cstddef>
#include <span>
#include <tuple>

namespace familyline::graphics::gui
{
/**
 * How should we draw the image into the imageview?
 */
enum ImageMode {
    /// Stretch it and distort it so it fits 100% inside the box?
    Stretch,

    /// Render it as-is, without scaling. If it is bigger than the view,
    /// draw the center. If it is not, draw it smaller, but centered
    Center,

    /// Render the image inside the box, as bigger as it can, but without
    /// hiding anything, and without distorting
    Scaled,

};

class ImageView : public Control
{
private:
    unsigned width_, height_;
    double image_width_, image_height_;
    std::vector<unsigned char> file_data_;

    cairo_surface_t *image_surf_ = nullptr;
    ImageMode image_mode_        = ImageMode::Stretch;

public:
    ImageView(unsigned width, unsigned height, ImageMode im = ImageMode::Stretch)
        : width_(width), height_(height), image_mode_(im)
    {
    }

    /**
     * Load the image content from a file
     */
    void loadFromFile(std::string_view path);

    /**
     * Load the image content from a color buffer
     *
     * The color format must be in the ARGB32 format
     * (first byte is alpha channel, then red, then green, then blue)
     *
     * The width and height specified here are the width and height of the
     * *image*, not the ones you set for the control!
     */
    void loadFromBuffer(
        ssize_t width, ssize_t height, std::span<unsigned char> data,
        int format = CAIRO_FORMAT_ARGB32);

    virtual bool update(cairo_t *context, cairo_surface_t *canvas);

    virtual std::tuple<int, int> getNeededSize(cairo_t *parent_context) const
    {
        return std::tie(width_, height_);
    }

    virtual void receiveEvent(const familyline::input::HumanInputAction &ev, CallbackQueue &cq) {}
};

}  // namespace familyline::graphics::gui
