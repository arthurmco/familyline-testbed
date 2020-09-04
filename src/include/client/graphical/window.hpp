#pragma once

#if !(defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__X_GL_H))
#include <GL/glew.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "device.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"


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
};

    
class GLWindow : public Window
{
private:
    GLDevice* _dev = nullptr;

    SDL_Window* _win      = nullptr;
    SDL_GLContext _glctxt = nullptr;
    int _width, _height;
    int _fwidth, _fheight;

    ShaderProgram* winShader = nullptr;
    GLuint base_vao, base_vbo, base_index_vbo;

    Framebuffer* _f3D;
    Framebuffer* _fGUI;

    void createWindowSquare();

    std::unique_ptr<Renderer> renderer_;
    
public:
    GLWindow(GLDevice* dev, int width = 800, int height = 600);

    /* Get the window size, in pixels */
    virtual void getSize(int& width, int& height) const;

    /* Get the window framebuffer size
     *
     * This value can differ from the window size if the
     * OS do some sort of dpi scaling
     *
     * macOS is one example
     */
    virtual void getFramebufferSize(int& width, int& height);

    virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI);
    virtual void show();
    virtual void update();

    virtual Renderer* createRenderer();
    virtual Renderer* getRenderer();

    virtual ~GLWindow() {}
};

}  // namespace familyline::graphics
