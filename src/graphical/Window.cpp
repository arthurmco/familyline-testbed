#include "Window.hpp"
#include "../config.h"

using namespace Tribalia::Graphics;

Window::Window(int w, int h)
{
    _width = w;
    _height = h;

    /* Initialize SDL */
    int sdl_ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    if (sdl_ret != 0) {
        /* SDL hasn't been initialized correctly */
        char err[1024] = "Error while initializing SDL: ";
        strcat(err, SDL_GetError());
        throw window_exception(err, sdl_ret);
    }

    /* Setup SDL GL context data */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,  16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    _win = SDL_CreateWindow("Tribalia",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    if (!_win) {
        char err[1024] = "SDL window creation error: ";
        strcat(err, SDL_GetError());
        SDL_Quit();
        throw window_exception(err, 0);
    }
}

static const GLfloat base_win_square_points[] =
{ -1.0, 1.0, 1.0,        1.0, 1.0, 1.0,
1.0, -1.0, 1.0,        -1.0, -1.0, 1.0 };

static const GLuint base_win_square_elements[] =
{ 0, 3, 2, 0, 1, 2 };

/* Shows the window */
void Window::Show()
{
    /* Create the context */
    _glctxt = SDL_GL_CreateContext(_win);

    if (_glctxt == NULL) {
        char err[1024] = "OpenGL context creation error: ";
        strcat(err, SDL_GetError());
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
        char err[1024] = "GLEW initialization error: ";
        strcat(err, (char*)glewGetErrorString(glewStatus));
        SDL_Quit();
        throw renderer_exception(err, glewStatus);
    }

    /* Compile the shader */
    if (!winShader) {
	Shader *sv = new Shader{ SHADERS_DIR "Window.vert", SHADER_VERTEX };
	Shader *sf = new Shader{ SHADERS_DIR "Window.frag", SHADER_PIXEL };
	
	sv->Compile();
	sf->Compile();
	
	winShader = new ShaderProgram{ sv, sf };
	winShader->Link();
	
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
    
    SDL_ShowWindow(_win);

}

/* Updates the window content to the video card */
void Window::Update()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _width, _height);

    glDisable(GL_DEPTH_TEST);
    winShader->Use();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glActiveTexture(GL_TEXTURE0);
    if (_f3D)
	glBindTexture(GL_TEXTURE_2D, _f3D->GetTextureHandle());

    glActiveTexture(GL_TEXTURE1);
    if (_fGUI)
	glBindTexture(GL_TEXTURE_2D, _fGUI->GetTextureHandle());

    winShader->SetUniform("texRender", 0);
    winShader->SetUniform("texGUI", 1);
    
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
    // Draw the triangle !

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, base_index_vbo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    SDL_GL_SwapWindow(_win);
    
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
	printf("GL error %#x\n", err);
    }
    

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_DEPTH_TEST);
}

void Window::GetSize(int& w, int& h)
{
	w = _width;
	h = _height;
}

void Window::Set3DFramebuffer(Framebuffer* f)
{
	_f3D = f;
}

void Window::SetGUIFramebuffer(Framebuffer* f)
{
	_fGUI = f;
}
