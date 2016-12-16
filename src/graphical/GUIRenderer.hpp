/*  GUI rendering class

    Copyright (C) 2016 Arthur M
*/

#include <cairo/cairo.h>
#include <GL/glew.h>

#include <cstdarg>
#include <vector>

#include "Window.hpp"
#include "gui/IPanel.hpp"
#include "gui/IContainer.hpp"

#ifndef GUIRENDERER_HPP
#define GUIRENDERER_HPP

namespace Tribalia {
namespace Graphics {

/*  This class will get the cairo context and
    transform it into a texture, */

class GUIRenderer : public GUI::IContainer {
private:
   cairo_t* cr;
   cairo_surface_t* cr_surface;

   Window* _w;
   std::vector<GUI::IPanel*> _panels;

   Framebuffer* _f;
public:
    GUIRenderer(Window* w);

	void SetFramebuffer(Framebuffer* f);

    /* Write a message in the screen */
    void DebugWrite(int x, int y, const char* fmt, ...);

    /* Render the GUI view */
    bool Render();

    /* Redraw the child controls */
    virtual void Redraw(cairo_t* ctxt) override;

    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(GUI::IPanel* p) override;
    virtual int AddPanel(GUI::IPanel* p, int x, int y) override;

    /* Remove the panel */
    virtual void RemovePanel(GUI::IPanel* p) override;

    void SetBounds(int x, int y, int w, int h);
    void SetPosition(int x, int y);

    

};

} /* Graphics */
} /* Tribalia */


#endif /* end of include guard: GUIRENDERER_HPP */
