#pragma once

#include <cairo/cairo.h>
#include <optional>
#include <memory>
#include <vector>
#include <tuple>

namespace familyline::graphics::gui {

    class Control;

    struct ControlData {
        int x, y;
        
        cairo_t* local_context;
        cairo_surface_t* control_canvas;
        std::unique_ptr<Control> control;

        ControlData(int x, int y, cairo_t* ctxt,
                    cairo_surface_t* s, std::unique_ptr<Control> c)
            : x(x), y(y), local_context(ctxt), control_canvas(s), control(std::move(c))
            {}
    };

    /**
     * A container component
     *
     * Every control that has this component can have multiple
     * controls under it
     */
    struct ContainerComponent {
        Control* parent;

        std::vector<ControlData> children;

        void add(int x, int y, std::unique_ptr<Control>);
    };


    /**
     * Base control class
     */
    class Control {
    private:


    protected:
        std::optional<ContainerComponent> cc_ = std::nullopt;

    public:
        /**
         * The parent component calls this if it thinks that this control needs to update
         *
         * Return true to signalize that the control updated the framebuffer, false that it
         * did not, and the parent control can reuse caches
         */
        virtual bool update(cairo_t* context, cairo_surface_t* canvas) = 0;

        /**
         * How much width and height, in pixels, your control will need
         *
         * This will be used as a guide to create the surface that will be
         * passed to this component on the `update()` function
         *
         * We pass the parent context just because some controls might need a context,
         * and we do not have ours ready.
         */
        virtual std::tuple<int, int> getNeededSize(cairo_t* parent_context) const = 0;

        /**
         * This is called by the control component when an object is added to it, so
         * you can create a context for it
         */
        virtual std::tuple<cairo_t*, cairo_surface_t*> createChildContext(Control *c);

        std::optional<ContainerComponent>& getControlContainer() { return cc_; }        

        // see https://stackoverflow.com/a/461224
        virtual ~Control() {}
    };

} // namespace familyline::graphics::gui
