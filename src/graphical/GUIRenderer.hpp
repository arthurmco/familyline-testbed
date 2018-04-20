/*  GUI rendering class

    Copyright (C) 2016, 2017 Arthur M
*/

#ifndef GUIRENDERER_HPP
#define GUIRENDERER_HPP

#include <GL/glew.h>
#include <cairo/cairo.h>

#include <cstdarg>
#include <vector>

#include <algorithm>
#include "Window.hpp"
#include "gui/IPanel.hpp"
#include "gui/IContainer.hpp"

#include "../input/InputListener.hpp"
#include "../input/InputManager.hpp"


namespace Tribalia::Graphics {

/* Stores everything needed to render a panel, including panel data itself */
struct PanelRenderObject {
    // Vertex pointers
    GLuint vao, vbo_vert, vbo_tex;

    // The panel
    GUI::IPanel* panel;

    // The texture ID and panel pixel sizes (for easy lookup)
    GLuint tex_id;
    int pw, ph;

    /* Each panel has its own cairo context and surface */
    cairo_surface_t* csurf;
    cairo_t* ctxt;

    /* is debug?  */
    unsigned int is_debug = 0;
    
};
    
/*  This class will get the cairo context and
    transform it into a texture, */

class GUIRenderer : public GUI::IContainer {
private:
    
    Window* _w;
    std::vector<PanelRenderObject> _panels;

    /* Cairo context for debug messages */
    cairo_t* debug_ctxt;
    
    Framebuffer* _f;

    ShaderProgram* sGUI;

    Input::InputListener* _il = nullptr;
    GUI::IPanel* oldPanel = nullptr;

    GLint attrPos, attrTex;
public:
    GUIRenderer(Window* w);

    void SetFramebuffer(Framebuffer* f);
    
    /* Write a message in the screen */
    void DebugWrite(int x, int y, const char* fmt, ...);

    /* Initialize the input subsystem */
    void InitInput();

    /* Send input to other controls 
     * Return true if we processed the event, false if not
     */
    virtual bool ProcessInput(Tribalia::Input::InputEvent& ev) override;
    
    /* Render the GUI view */
    bool Render();

    /* Redraw the child controls */
    virtual void Redraw(cairo_t* ctxt) override;

    /* Add a panel using the panel position or a new position */
    virtual int AddPanel(GUI::IPanel* p) override;
    virtual int AddPanel(GUI::IPanel* p, int x, int y) override;
    virtual int AddPanel(GUI::IPanel* p, double x, double y) override;
    
    /* Remove the panel */
    virtual void RemovePanel(GUI::IPanel* p) override;

    void SetBounds(int x, int y, int w, int h);
    void SetPosition(int x, int y);

    virtual void OnFocus() override {}
    virtual void OnLostFocus() override {}

    virtual bool IsDirty() const override { return true; }

    ~GUIRenderer();
};

} /* Tribalia::Graphics */


#endif /* end of include guard: GUIRENDERER_HPP */
