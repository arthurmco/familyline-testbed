#include <client/graphical/gui/gui_imageview.hpp>

using namespace familyline::graphics::gui;

#include <algorithm>

/**
 * Load the image content from a color buffer
 *
 * The color format must be in the ARGB32 format
 * (first byte is alpha channel, then red, then green, then blue)
 */
void ImageView::loadFromBuffer(ssize_t width, ssize_t height, std::span<unsigned int> buffer)
{
    if (!image_surf_) {
        cairo_surface_destroy(image_surf_);
    }

    image_surf_ = cairo_image_surface_create_for_data(
        (unsigned char*) buffer.data(), CAIRO_FORMAT_ARGB32, width, height, width*4);

    image_width_ = width;
    image_height_ = height;
}

bool ImageView::update(cairo_t *context, cairo_surface_t *canvas)
{
    if (!image_surf_)
        return false;

    const double xscale = width_ / image_width_;
    const double yscale = height_ / image_height_;


    switch (image_mode_) {
    case ImageMode::Stretch:

        // Reset the transformation matrices or the scale will be cumulative
        // (it will continue scaling every frame instead of once)
        cairo_identity_matrix(context);
        cairo_scale(context, xscale, yscale);
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        cairo_set_source_surface(context, image_surf_, 0, 0);
        cairo_paint(context);

        break;

    case ImageMode::Center: {
        const double vscale = std::max(std::max(xscale, yscale), 1.0);
        const double xoff =
            (xscale >= yscale) ? (width_ - (image_width_ * vscale)) / 2 : 0;
        const double yoff =
            (yscale >= xscale) ? (height_ - (image_height_ * vscale)) / 2 : 0;

        cairo_identity_matrix(context);
        cairo_scale(context, vscale, vscale);
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        cairo_set_source_surface(context, image_surf_, xoff / vscale,
                                 yoff / vscale);
        cairo_paint(context);
        break;
    }
    case ImageMode::Scaled: {
        const double vscale = std::max(std::min(xscale, yscale), 1.0);
        const double xoff = (xscale > yscale && vscale > 1)
            ? (width_ - (image_width_ * vscale)) / 2
            : 0;
        const double yoff = (yscale > xscale && vscale > 1)
            ? (height_ - (image_height_ * vscale)) / 2
            : 0;

        cairo_identity_matrix(context);
        cairo_scale(context, vscale, vscale);
        cairo_set_operator(context, CAIRO_OPERATOR_OVER);
        cairo_set_source_surface(context, image_surf_, xoff / vscale,
                                 yoff / vscale);
        cairo_paint(context);

        break;
    }
    }


    // draw a debug border
    cairo_identity_matrix(context);
    cairo_set_line_width (context, 6);
    cairo_set_source_rgba (context, 0, 0, 0, 0.5);
    cairo_rectangle (context, 0, 0, width_, height_);
    cairo_stroke (context);

    return true;
}
