#include "window.hpp"

#include "shader_manager.hpp"
#include "exceptions.hpp"
#include "gfx_service.hpp"

using namespace familyline::graphics;

GLWindow::GLWindow(GLDevice* dev, int width, int height)
	: _dev(dev), _width(width), _height(height)
{
	
	/* Setup SDL GL context data */
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	auto fflags = 0;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &fflags);
	fflags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

	if (true) {
		fflags |= SDL_GL_CONTEXT_DEBUG_FLAG;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, fflags);
	_win = SDL_CreateWindow("Familyline",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _width, _height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

	if (!_win) {
		auto err = std::string("OpenGL context creation error: ");
		err.append(SDL_GetError());
        
		SDL_Quit();
		throw graphical_exception(err);
	}
}

void GLWindow::getSize(int& width, int& height)
{
    width = _width;
    height = _height;
}

void GLWindow::show()
{

	/* Create the context */
	_glctxt = SDL_GL_CreateContext(_win);

	if (_glctxt == NULL) {
		auto err = std::string("OpenGL context creation error: ");
		err.append(SDL_GetError());
		SDL_Quit();
		throw renderer_exception(err, 0);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glewExperimental = GL_TRUE;
	GLenum glewStatus = glewInit();

	/* glewInit() can emit GL_INVALID_ENUM, but this doesn't seem to affect
	   the operation of the library
	   (check https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library) */

	glGetError();

	if (glewStatus != GLEW_OK) {
		auto err = std::string("OpenGL context creation error: ");
		err.append(SDL_GetError());
		SDL_Quit();
		throw renderer_exception(err, glewStatus);
	}

	this->createWindowSquare();
	SDL_ShowWindow(_win);
}

static const GLfloat base_win_square_points[] =
{ -1.0, 1.0, 1.0,        1.0, 1.0, 1.0,
1.0, -1.0, 1.0,        -1.0, -1.0, 1.0 };

static const GLuint base_win_square_elements[] =
{ 0, 3, 2, 0, 1, 2 };

void GLWindow::createWindowSquare()
{
	/* Compile the shader */
	if (!winShader) {
		winShader = new ShaderProgram("window", {
			Shader("shaders/Window.vert", ShaderType::Vertex),
			Shader("shaders/Window.frag", ShaderType::Fragment)
		});

		winShader->link();
	}

	/* Create the basic VAO for the window */

	glGenVertexArrays(1, &base_vao);
	glBindVertexArray(base_vao);
	
	glGenBuffers(1, &base_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, base_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(base_win_square_points),
		base_win_square_points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &base_index_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base_index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(base_win_square_elements),
		base_win_square_elements, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void GLWindow::setFramebuffers(Framebuffer* f3D, Framebuffer* fGUI)
{
	_f3D = f3D;
	_fGUI = fGUI;
}

void GLWindow::update()
{
	glClearColor(1.0, 0, 0, 1.0);
	glViewport(0, 0, _width, _height);

	glDisable(GL_DEPTH_TEST);
	GFXService::getShaderManager()->use(*winShader);
    
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	if (_f3D)
		glBindTexture(GL_TEXTURE_2D, _f3D->getTextureHandle());

	glActiveTexture(GL_TEXTURE1);
	if (_fGUI)
		glBindTexture(GL_TEXTURE_2D, _fGUI->getTextureHandle());

	winShader->setUniform("texRender", 0);
	winShader->setUniform("texGUI", 1);

	glBindVertexArray(base_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, base_vbo);
	glVertexAttribPointer(
		0,                  // attribute 0. 
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
    
	// Draw the screen triangle
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base_index_vbo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	SDL_GL_SwapWindow(_win);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {

		switch (err) {
		case GL_OUT_OF_MEMORY:
			throw graphical_exception("Out of memory while rendering");
		default:
			printf("\rGL error %#x\n", err);
		}

	}

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}
