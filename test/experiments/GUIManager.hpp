#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cairo/cairo.h>
#include <memory>
#include <vector>
#include <string>

#include <pango/pangocairo.h>

#include "root_control.hpp"
#include "gui_label.hpp"

namespace familyline::graphics::gui {
    
    /**
     * Manages the graphical interface state and rendering
     */
    class GUIManager {
    private:
        // sdl things, just to put on a window
        // will be changed to opengl things for the real game
        SDL_Window* win_;
        SDL_Texture* framebuffer_;
        SDL_Renderer* renderer_;

        unsigned width_, height_;

        // Cairo things, to actually do the rendering work
        // Since Cairo can use hardware acceleration, we do not need to worry much about speed.
        cairo_t* context_;
        cairo_surface_t* canvas_;

        std::unique_ptr<RootControl> root_control_;

        Label* lbl3;
        
    public:
        GUIManager(SDL_Window* win, unsigned width, unsigned height,
                   SDL_Renderer* renderer)
            : win_(win),
              framebuffer_(SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING, width, height)),
              renderer_(renderer),
              width_(width), height_(height),
              canvas_(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height))
            {
                context_ = cairo_create(this->canvas_);
                root_control_ = std::make_unique<RootControl>(width, height);

                Label *lbl = new Label{100, 60, "Test"};
                Label *lbl2 = new Label{100, 60, "AI QUE DELICIA CARA - 家族の人"};
                
                lbl3 = new Label{80, 20, "0"};
                root_control_->getControlContainer()->add(20, 20, std::unique_ptr<Control>(lbl));
                root_control_->getControlContainer()->add(40, 50, std::unique_ptr<Control>(lbl2));
                root_control_->getControlContainer()->add(60, 80, std::unique_ptr<Control>(lbl3));
            }


        void update();


        ~GUIManager();

    };

}
