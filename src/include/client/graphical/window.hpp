#pragma once

#include <client/graphical/device.hpp>
#include <client/graphical/framebuffer.hpp>
#include <client/graphical/shader.hpp>
#include <client/graphical/gui/gui_manager.hpp>
#include <memory>

namespace familyline::graphics
{
    class Renderer;    
    
/**
 * The window
 *
 * Should mean rendering context, but they are almost always windows, even if they are fullscreen
 */
class Window
{
public:
    virtual void getSize(int&, int&) const = 0;

    /* Get the window framebuffer size
     *
     * This value can differ from the window size if the
     * OS do some sort of dpi scaling
     *
     * macOS is one example
     */
    virtual void getFramebufferSize(int& width, int& height) = 0;

    virtual void show()                                               = 0;
    virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI) = 0;
    virtual void update()                                             = 0;

    virtual ~Window() {}

    virtual Renderer* createRenderer() = 0;
    virtual Renderer* getRenderer() = 0;

    virtual gui::GUIManager* createGUIManager() = 0;
};

    

}  // namespace familyline::graphics
