/*  GUI rendering class

    Copyright (C) 2016 Arthur M
*/

#include <cairo/cairo.h>
#include <GLEW/gl.h>

#ifndef GUIRENDERER_HPP
#define GUIRENDERER_HPP

namespace Tribalia {
namespace Graphics {

/*  This class will get the cairo context and
    transform it into a texture, */

class GUIRenderer {
private:
   cairo_t* cr;
   cairo_context_t* cr_ctxt;
 
public:
    GUIRenderer();

    /* Write a message in the screen */
    DebugWrite(const char* fmt, ...);   

    /* Render the GUI view */
    Render();


};

} /* Graphics */
} /* Tribalia */


#endif /* end of include guard: GUIRENDERER_HPP */
