#pragma once

#include <cairo/cairo.h>
#include <memory>
#include <vector>
#include <string>
#include <future>

#include <pango/pangocairo.h>
#include <client/graphical/gui/gui_label.hpp>
#include <client/graphical/gui/control.hpp>

namespace familyline::graphics::gui {

    class Button : public Control {
    private:
        unsigned width_, height_;
        Label label_;

        unsigned label_width_, label_height_;
        cairo_t *l_context_ = nullptr;
        cairo_surface_t *l_canvas_ = nullptr;

        std::function<void(Button*)> click_cb_;

        std::chrono::time_point<std::chrono::steady_clock> last_hover_ = std::chrono::steady_clock::now();
        bool hovered_ = false;
        
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

        virtual void receiveEvent(const familyline::input::HumanInputAction& ev);

        virtual ~Button() {}
    };
    

    
}
