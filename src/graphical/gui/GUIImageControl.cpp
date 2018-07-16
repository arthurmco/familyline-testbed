#include "GUIImageControl.hpp"
#include <Log.hpp>

using namespace Familyline;
using namespace Familyline::Graphics::GUI;

GUIImageControl::GUIImageControl(float xPos, float yPos, float width,
	float height, const char* filename)
{
	this->x = xPos;
	this->y = yPos;
	this->width = width;
	this->height = height;
	this->filename = filename;
}

GUICanvas GUIImageControl::openImageFromFile(const char* filename)
{
	cairo_surface_t* image_surface = cairo_image_surface_create_from_png(filename);

	Log::GetLog()->InfoWrite("gui-image-control", "opening image '%s'",
		filename);

	auto status = cairo_surface_status(image_surface);
	switch (status) {
	case CAIRO_STATUS_NO_MEMORY:
		Log::GetLog()->Fatal("gui-image-control", "unsufficient memory while opening %s",
			filename);
		return nullptr;

	case CAIRO_STATUS_FILE_NOT_FOUND:
		Log::GetLog()->Fatal("gui-image-control", "'%s' has not been found",
			filename);
		return nullptr;

	case CAIRO_STATUS_READ_ERROR:
		Log::GetLog()->Fatal("gui-image-control", "error while reading '%s'",
			filename);
		return nullptr;
	default:
	    return image_surface;
	}

}


bool GUIImageControl::processSignal(GUISignal s)
{
	cairo_t* image_context = nullptr;
	switch (s.signal) {
	case ContainerAdd:
		// Only load the image on container add

		this->image = this->openImageFromFile(this->filename);
		if (!this->image) {
			// throw GUIException
		}

		image_context = cairo_create(this->image);
		cairo_clip_extents(image_context, nullptr, nullptr, &this->imageW, &this->imageH);
		Log::GetLog()->InfoWrite("gui-image-control", "image has size %.2f x %.2f", 
			imageW, imageH);

		this->setContext(s.absw, s.absh);
		this->dirty = true;

		return true;

	default:
		return GUIControl::processSignal(s);

	}
}

GUICanvas GUIImageControl::doRender(int absw, int absh) const
{
	cairo_save(this->ctxt);

	// Resizes and plots the image into the surface
	auto sw = (absw * this->width) / this->imageW;
	auto sh = (absh * this->height) / this->imageH;

	cairo_scale(ctxt, sw, sh);    
	cairo_set_source_surface(ctxt, this->image, 0, 0);
	cairo_paint(ctxt);

	cairo_restore(this->ctxt);
	return this->canvas;
}



