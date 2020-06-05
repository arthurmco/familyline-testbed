// build command: g++ --std=c++2a -Wall $(sdl2-config --cflags) $(pkg-config pangocairo --cflags) gui.cpp control.cpp root_control.cpp gui_label.cpp gui_button.cpp GUIManager.cpp $(sdl2-config --libs) $(pkg-config cairo --libs) $(pkg-config pangocairo --libs) -lpthread -g -o gui

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cstdio>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <cmath>

#include <client/graphical/gui/gui_manager.hpp>

using namespace familyline::graphics::gui;

int main(int argc, char *argv[])
{
    auto width=800, height=600;
    SDL_Window* win;
    SDL_Renderer* renderer;

    SDL_Init(SDL_INIT_EVERYTHING);

	win = SDL_CreateWindow("GUI Test",
                           SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                           SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);

	if (!win) {
		auto err = std::string("OpenGL context creation error: ");
		err.append(SDL_GetError());

        fprintf(stderr, "%s", err.c_str());
        return 1;
	}

    SDL_ShowWindow(win);

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Clear the entire screen to our selected color.
    SDL_RenderClear(renderer);

    // Up until now everything was drawn behind the scenes.
    // This will show the new, red contents of the window.
    SDL_RenderPresent(renderer);


    SDL_Event e;
    bool running = true;

    GUIManager gm(win, width, height, renderer);

    auto c = 0;
    while (running) {
        if (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            default:
                gm.receiveEvent(e);
            }
    
        }
        
        gm.update();
        SDL_RenderPresent(renderer);

        c++;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    return 0;
}
