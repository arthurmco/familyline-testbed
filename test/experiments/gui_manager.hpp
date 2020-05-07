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
#include "gui_button.hpp"
#include "gui_imageview.hpp"

#include <span>

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
        std::array<unsigned int, 32*32> ibuf;

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
                Label *lbl2 = new Label{100, 60, "AI QUE DELÍCIA CARA - 家族の人"};
                Button* btn = new Button{200, 40, "Test Button"};
                ImageView *img = new ImageView{200, 100};

                ImageView *img2 = new ImageView{100, 125, ImageMode::Scaled};
                ImageView *img3 = new ImageView{150, 100, ImageMode::Scaled};
                ImageView *img4 = new ImageView{64, 64, ImageMode::Center};
                ImageView *img5 = new ImageView{16, 16, ImageMode::Center};

                for (auto i = 0; i < 32*32; i++){
                    if (i%3 == 0)
                        ibuf[i] = 0xffff0000;
                    else
                        ibuf[i] = 0xff00ff00;

                    if (i%5 == 0)
                        ibuf[i] = 0xff0000ff;

                    if (i < 32)
                        ibuf[i] = 0xffffffff;
                }
               
                
                
                img->loadFromBuffer(32, 32, std::span{ibuf});
                img2->loadFromBuffer(32, 32, std::span{ibuf});
                img3->loadFromBuffer(32, 32, std::span{ibuf});
                img4->loadFromBuffer(32, 32, std::span{ibuf});
                img5->loadFromBuffer(32, 32, std::span{ibuf});
                
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
                root_control_->getControlContainer()->add(400, 120, std::unique_ptr<Control>(img));
                root_control_->getControlContainer()->add(400, 240, std::unique_ptr<Control>(img2));
                root_control_->getControlContainer()->add(510, 240, std::unique_ptr<Control>(img3));
                root_control_->getControlContainer()->add(400, 400, std::unique_ptr<Control>(img4));
                root_control_->getControlContainer()->add(510, 400, std::unique_ptr<Control>(img5));
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
