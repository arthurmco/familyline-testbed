#pragma once

#include "device.hpp"
#include "framebuffer.hpp"

namespace familyline::graphics {


/**
 * The window
 *
 * Should mean rendering context, but they are almost always windows, even if they are fullscreen
 */
class Window {
public:
	virtual void show() = 0;
	virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI) = 0;
	virtual void update() = 0;
};


#include "shader.hpp"

class GLWindow : public Window {
private:
	GLDevice* _dev = nullptr;

	SDL_Window* _win = nullptr;
	SDL_GLContext _glctxt = nullptr;
	int _width = 800;
	int _height = 600;

	ShaderProgram* winShader = nullptr;
	GLuint base_vao, base_vbo, base_index_vbo;

	Framebuffer* _f3D;
	Framebuffer* _fGUI;

	void createWindowSquare();
	
public:
	GLWindow(GLDevice* dev);

	virtual void setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI);
	virtual void show();
	virtual void update();
};
}
