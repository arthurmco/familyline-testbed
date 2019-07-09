#pragma once

#include <SDL2/SDL.h>
#include "device.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"

namespace familyline::graphics {


    /**
     * The window
     *
     * Should mean rendering context, but they are almost always windows, even if they are fullscreen
     */
    class Window {
    public:
        virtual void getSize(int&, int&) = 0;
        virtual void show() = 0;
        virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI) = 0;
        virtual void update() = 0;

        virtual ~Window() {}
    };


    class GLWindow : public Window {
    private:
        GLDevice* _dev = nullptr;

        SDL_Window* _win = nullptr;
        SDL_GLContext _glctxt = nullptr;
        int _width;
        int _height;

        ShaderProgram* winShader = nullptr;
        GLuint base_vao, base_vbo, base_index_vbo;

        Framebuffer* _f3D;
        Framebuffer* _fGUI;

        void createWindowSquare();
	
    public:
        GLWindow(GLDevice* dev, int width = 800, int height = 600);

        virtual void getSize(int& width, int& height);
        virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI);
        virtual void show();
        virtual void update();
    };

}
