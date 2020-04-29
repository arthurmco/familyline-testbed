#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cairo/cairo.h>
#include <memory>
#include <vector>
#include <string>

#include "root_control.hpp"

namespace familyline::graphics::gui {

    class Label : public Control {
    private:
        unsigned width_, height_;
        std::string text_;
        cairo_text_extents_t te = {};
        
    public:
        Label(unsigned width, unsigned height, std::string text)
            : width_(width), height_(height), text_(text)  {}

        virtual bool update(cairo_t* context, cairo_surface_t* canvas);

        virtual std::tuple<int, int> getNeededSize(cairo_t* parent_context) const;

    };
    
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
                root_control_->getControlContainer()->add(20, 20, std::unique_ptr<Control>(lbl));
                root_control_->getControlContainer()->add(40, 50, std::unique_ptr<Control>(lbl2));
            }


        void update();


        ~GUIManager();

    };

}
