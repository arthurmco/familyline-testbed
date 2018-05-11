/*  Window management class
	Also works on joining the GUI with the general game renderer.

	Copyright (C) 2016, 2017, 2018 Arthur M
*/

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ShaderProgram.hpp"
#include "GFXExceptions.hpp"
#include "Framebuffer.hpp"
#include "Device.hpp"

namespace Familyline {
	namespace Graphics {

		/* A list of options to be passed when creating the window  or gl context */
		enum WindowOptions {
			WIN_DEBUG_CONTEXT = 0x1, // Enable OpenGL debugging extensions (aka ARB_debug_output)
		};

		enum MessageBoxInfo {
			Error,
			Warning,
			Information
		};

		class Window {
		private:
			int _width, _height;
			SDL_Window* _win;
			SDL_GLContext _glctxt = nullptr;

			ShaderProgram* winShader = nullptr;

			Framebuffer* _f3D = nullptr;
			Framebuffer* _fGUI = nullptr;

			GLuint base_vao, base_vbo, base_index_vbo;
		public:
			Window(int w, int h, unsigned win_opts);

			/* Shows the window */
			void Show();

			/* Updates the window content to the video card */
			void Update();

			void Set3DFramebuffer(Framebuffer* f);
			void SetGUIFramebuffer(Framebuffer* f);

			void GetSize(int& w, int& h);

			/* Show a message box */
			void ShowMessageBox(const char* message,
				const char* title,
				MessageBoxInfo info);

			~Window();

			/* Get the device used in that window */
			DeviceInfo* GetDevice();
		};


	} /* Graphics */
} /* Familyline */


#endif /* end of include guard: WINDOW_HPP */
