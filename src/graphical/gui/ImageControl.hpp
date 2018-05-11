/*  Class for creating a control that draws an image
    
    Copyright (C) 2016 Arthur M
*/

#include "IPanel.hpp"
#include "../TextureOpener.hpp"
#include <string>
#include <cstdarg>
#include <cstring>

#ifndef IMAGECONTROL_HPP
#define IMAGECONTROL_HPP

namespace Familyline {
namespace Graphics {
namespace GUI {

class ImageControl : public IPanel {
private:
    double _imgW, _imgH;
    
    cairo_surface_t* _image;
    cairo_t* _image_ctxt;
    
public:
    /* Load the image from a filename.
       Because it uses Cairo file loading functions directly, it only 
       supports loading from PNG files */
    ImageControl(int x, int y, int w, int h, const char* file);
    ImageControl(double x, double y, double w, double h, const char* file);

    /* Load the image from a texture file object */
    ImageControl(int x, int y, int w, int h, TextureFile* f,
		 int cx, int cy, int cw, int ch);
    ImageControl(double x, double y, double w, double h, TextureFile* f,
		 int cx, int cy, int cw, int ch);
    
    virtual void Redraw(cairo_t* ctxt) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override {
	(void) ev;
	return false;
    }

    virtual void OnFocus() override {}
    virtual void OnLostFocus() override {}

    virtual bool IsDirty() const override { return true; }

    int GetDataWidth() const override; 
    int GetDataHeight() const override;

    virtual ~ImageControl();
};

}
}
}

#endif
