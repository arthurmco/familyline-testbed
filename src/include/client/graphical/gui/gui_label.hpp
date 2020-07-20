#pragma once

#include <cairo/cairo.h>
#include <memory>
#include <vector>
#include <string>

#include <pango/pangocairo.h>
#include <client/graphical/gui/control.hpp>

namespace familyline::graphics::gui {

    /**
     * The label GUI control
     */
    class Label : public Control {
    private:
        unsigned width_, height_;
        std::string text_;

        PangoLayout* getLayout(cairo_t* context) const;

        cairo_t* last_context_;        
    public:
        Label(unsigned width, unsigned height, std::string text)
            : width_(width), height_(height), text_(text) {}

        virtual bool update(cairo_t *context, cairo_surface_t *canvas);

        virtual std::tuple<int, int> getNeededSize(cairo_t *parent_context) const;

        void setText(std::string v);

        virtual void receiveEvent(const familyline::input::HumanInputAction& ev, CallbackQueue& cq) {}

        virtual ~Label() {
            fprintf(stderr, "label ptr=%lp text=%s\n", this, text_.c_str());
        }
    };

} // namespace familyline::graphics::gui
