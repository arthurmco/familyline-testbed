#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cstdio>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>
#include <cmath>

#include "GUIManager.hpp"

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
        SDL_PollEvent(&e);

        switch (e.type) {
        case SDL_QUIT:
            running = false;
            break;
        }

        gm.update();
        SDL_RenderPresent(renderer);

        c++;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    return 0;
}
