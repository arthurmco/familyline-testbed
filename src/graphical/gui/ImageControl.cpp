#include "ImageControl.hpp"

using namespace Familyline::Graphics;
using namespace Familyline::Graphics::GUI;

/* Load the image from a filename.
   Because it uses Cairo file loading functions directly, it only 
   supports loading from PNG files */
ImageControl::ImageControl(int x, int y, int w, int h, const char* file)
    : IPanel(x,y,w,h)
{
    _bgColor = glm::vec4(255, 255, 255, 0);
    _image = cairo_image_surface_create_from_png(file);
    _image_ctxt = cairo_create(_image);
    double x2, x1, y2, y1;
    cairo_clip_extents(_image_ctxt, &x1, &y1, &x2, &y2);

    _imgW = x2-x1;
    _imgH = y2-y1;

    printf("%s - %.1f %.1f\n", file, _imgW, _imgH);
}

ImageControl::ImageControl(double x, double y, double w, double h,
			   const char* file)
    : IPanel(x,y,w,h, true)
{
    _bgColor = glm::vec4(255, 255, 255, 0);
    
    _image = cairo_image_surface_create_from_png(file);

    auto st = cairo_surface_status(_image);
    if (st != CAIRO_STATUS_SUCCESS) {
        const char* ststr;

	switch (st) {
	case CAIRO_STATUS_FILE_NOT_FOUND: ststr = "File not found"; break;
	default: ststr = "Unknown error"; break;
	}

	 Log::GetLog()->Fatal("gui-image-control", "Error trying to open %s: %s",
			     file, ststr);
    }

    _image_ctxt = cairo_create(_image);
    double x2, x1, y2, y1;
    cairo_clip_extents(_image_ctxt, &x1, &y1, &x2, &y2);

    _imgW = x2-x1;
    _imgH = y2-y1;


}

/* Load the image from a texture file object */
ImageControl::ImageControl(int x, int y, int w, int h, TextureFile* f,
			   int cx, int cy, int cw, int ch)
    : IPanel(x,y,w,h)
{
    auto* idata = f->GetTextureRaw(cx, cy, cw, ch);
    _bgColor = glm::vec4(255, 255, 255, 0);

    auto ofmt = f->GetFormat();
    auto fmt = (ofmt == IL_BGR || ofmt == IL_RGB) ? CAIRO_FORMAT_RGB24 :
	CAIRO_FORMAT_ARGB32;
        
    _image = cairo_image_surface_create_for_data(
	idata, fmt, cw, ch,  cairo_format_stride_for_width(fmt, cw));

    _image_ctxt = cairo_create(_image);
    double x2, x1, y2, y1;
    cairo_clip_extents(_image_ctxt, &x1, &y1, &x2, &y2);

    _imgW = x2-x1;
    _imgH = y2-y1;

    
}

ImageControl::ImageControl(double x, double y, double w, double h, TextureFile* f,
			   int cx, int cy, int cw, int ch)
    : IPanel(x,y,w,h,true)
{
    auto* idata = f->GetTextureRaw(cx, cy, cw, ch);
    _bgColor = glm::vec4(255, 255, 255, 0);

    auto ofmt = f->GetFormat();
    auto fmt = (ofmt == IL_BGR || ofmt == IL_RGB) ? CAIRO_FORMAT_RGB24 :
	CAIRO_FORMAT_ARGB32;
        
    _image = cairo_image_surface_create_for_data(
	idata, fmt, cw, ch,  cairo_format_stride_for_width(fmt, cw));

    _image_ctxt = cairo_create(_image);
    double x2, x1, y2, y1;
    cairo_clip_extents(_image_ctxt, &x1, &y1, &x2, &y2);

    _imgW = x2-x1;
    _imgH = y2-y1;
}

void ImageControl::Redraw(cairo_t* ctxt)
{
    cairo_save(ctxt); 
    double sw = _width/_imgW;;
    double sh = _height/_imgH;

    cairo_scale(ctxt, sw, sh);    
    cairo_set_source_surface(ctxt, _image, 0.0, 0.0);
    cairo_paint(ctxt);
    
    cairo_restore(ctxt);
}

int ImageControl::GetDataWidth() const
{
    return _width;
}
int ImageControl::GetDataHeight() const
{
    return _height;
}

ImageControl::~ImageControl()
{
    cairo_surface_destroy(_image);
}

