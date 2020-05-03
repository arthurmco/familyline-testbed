#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <cairo/cairo.h>
#include <memory>
#include <vector>
#include <string>
#include <future>

#include <pango/pangocairo.h>

#include "root_control.hpp"
#include "gui_label.hpp"

namespace familyline::graphics::gui {

    class Button : public Control {
    private:
        unsigned width_, height_;
        Label label_;

        unsigned label_width_, label_height_;
        cairo_t *l_context_ = nullptr;
        cairo_surface_t *l_canvas_ = nullptr;

        std::function<void(Button*)> click_cb_;

        bool clicked_ = false;
        bool click_active_ = false;
        
        /// Stores the last time the user clicked, so we can make the click animation last more
        /// than the milisseconds between the GUI updates, a time sufficient for the user to
        /// see
        std::chrono::time_point<std::chrono::steady_clock> last_click_ = std::chrono::steady_clock::now();
        
        std::future<void> click_fut_;
        
    public:
        Button(unsigned width, unsigned height, std::string text);

        virtual bool update(cairo_t *context, cairo_surface_t *canvas);

        virtual std::tuple<int, int> getNeededSize(cairo_t *parent_context) const;

        void setClickCallback(std::function<void(Button*)> c) { click_cb_ = c; }
        
        void setText(std::string v);

        virtual void receiveEvent(const SDL_Event &e);

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

        Label* lbl3;
        Label* lbl4;
        
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
                Button* btn = new Button{200, 40, "Test Button"};
                
                lbl3 = new Label{80, 20, "0"};
                lbl4 = new Label{80, 20, "0"};

                btn->setClickCallback([&](Button* b) {
                    static int dvalue = 0;
                    dvalue++;

                    lbl4->setText(std::to_string(dvalue));                    
                });
                
                root_control_->getControlContainer()->add(20, 20, std::unique_ptr<Control>(lbl));
                root_control_->getControlContainer()->add(40, 60, std::unique_ptr<Control>(lbl2));
                root_control_->getControlContainer()->add(60, 100, std::unique_ptr<Control>(lbl3));
                root_control_->getControlContainer()->add(260, 100, std::unique_ptr<Control>(lbl4));
                root_control_->getControlContainer()->add(20, 140, std::unique_ptr<Control>(btn));
            }


        void update();


        /**
         * Receive an event and act on it
         *
         * (In the game, we will probably use input actions, not sdl events directly)
         */
        void receiveEvent(const SDL_Event& ev);
        

        ~GUIManager();

    };

}
