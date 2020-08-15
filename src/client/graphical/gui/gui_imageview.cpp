#include <client/graphical/gui/gui_imageview.hpp>

#include "cairo.h"

using namespace familyline::graphics::gui;

#include <IL/ilu.h>

#include <algorithm>
#include <array>
#include <common/logger.hpp>
#include <cstdio>

/**
 * Load the image content from a file
 */
void ImageView::loadFromFile(std::string_view path)
{
    /* Initialize devIL if not */
    ilInit();

    ILuint handle = 0;
    ilGenImages(1, &handle);
    ilBindImage(handle);

    auto& log = LoggerService::getLogger();
    if (ilLoad(IL_TYPE_UNKNOWN, path.data()) == IL_FALSE) {
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

        log->write("imageview", LogType::Error, "Error '%s' while opening %s", estr, path.data());
        return;
    }

    auto w      = ilGetInteger(IL_IMAGE_WIDTH);
    auto h      = ilGetInteger(IL_IMAGE_HEIGHT);
    auto format = IL_BGRA;  // ilGetInteger(IL_IMAGE_FORMAT);

    file_data_.clear();

    if (format == IL_BGRA || format == IL_RGBA) {
        file_data_.reserve(w * h * 4);
        ilCopyPixels(0, 0, 0, w, h, 1, format, IL_UNSIGNED_BYTE, file_data_.data());

        this->loadFromBuffer(w, h, file_data_);
    } else {
        file_data_.reserve(w * h * 3);

        ilCopyPixels(0, 0, 0, w, h, 1, format, IL_UNSIGNED_BYTE, file_data_.data());
        this->loadFromBuffer(w, h, file_data_, CAIRO_FORMAT_RGB24);
    }
}

/**
 * Load the image content from a color buffer
 *
 * The color format must be in the ARGB32 format
 * (first byte is alpha channel, then red, then green, then blue)
 */
void ImageView::loadFromBuffer(
    ssize_t width, ssize_t height, std::span<unsigned char> buffer, int format)
{
    if (!image_surf_) {
        cairo_surface_destroy(image_surf_);
    }

    image_surf_ = cairo_image_surface_create_for_data(
        (unsigned char*)buffer.data(), CAIRO_FORMAT_ARGB32, width, height, width * 4);

    image_width_  = width;
    image_height_ = height;
}

bool ImageView::update(cairo_t* context, cairo_surface_t* canvas)
{
    if (!image_surf_) return false;

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
            const double xoff   = (xscale >= yscale) ? (width_ - (image_width_ * vscale)) / 2 : 0;
            const double yoff   = (yscale >= xscale) ? (height_ - (image_height_ * vscale)) / 2 : 0;

            cairo_identity_matrix(context);
            cairo_scale(context, vscale, vscale);
            cairo_set_operator(context, CAIRO_OPERATOR_OVER);
            cairo_set_source_surface(context, image_surf_, xoff / vscale, yoff / vscale);
            cairo_paint(context);
            break;
        }
        case ImageMode::Scaled: {
            const double vscale = std::max(std::min(xscale, yscale), 1.0);
            const double xoff =
                (xscale > yscale && vscale > 1) ? (width_ - (image_width_ * vscale)) / 2 : 0;
            const double yoff =
                (yscale > xscale && vscale > 1) ? (height_ - (image_height_ * vscale)) / 2 : 0;

            cairo_identity_matrix(context);
            cairo_scale(context, vscale, vscale);
            cairo_set_operator(context, CAIRO_OPERATOR_OVER);
            cairo_set_source_surface(context, image_surf_, xoff / vscale, yoff / vscale);
            cairo_paint(context);

            break;
        }
    }

    // draw a debug border
    cairo_identity_matrix(context);
    cairo_set_line_width(context, 6);
    cairo_set_source_rgba(context, 0, 0, 0, 0.5);
    cairo_rectangle(context, 0, 0, width_, height_);
    cairo_stroke(context);

    return true;
}
