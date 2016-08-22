/*  GUI rendering class

    Copyright (C) 2016 Arthur M
*/

#include <cairo/cairo.h>
#include <GL/glew.h>

#include <cstdarg>

#include "Window.hpp"

#ifndef GUIRENDERER_HPP
#define GUIRENDERER_HPP

namespace Tribalia {
namespace Graphics {

/*  This class will get the cairo context and
    transform it into a texture, */

class GUIRenderer {
private:
   cairo_t* cr;
   cairo_surface_t* cr_surface;
 
   Window* _w;

   Framebuffer* _f;
public:
    GUIRenderer(Window* w);

	void SetFramebuffer(Framebuffer* f);

    /* Write a message in the screen */
    void DebugWrite(int x, int y, const char* fmt, ...);   

    /* Render the GUI view */
    bool Render();


};

} /* Graphics */
} /* Tribalia */


#endif /* end of include guard: GUIRENDERER_HPP */
