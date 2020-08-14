#pragma once

#include <cairo/cairo.h>
#include <optional>
#include <memory>
#include <vector>
#include <tuple>
#include <functional>
#include <queue>

#include <client/input/input_actions.hpp>

#include <SDL2/SDL.h>

namespace familyline::graphics::gui {

    class Control;

    enum ControlPositioning {
        Pixel = 1,
        Relative = 2,
        CenterX = 4,
        CenterY = 8,
        CenterAll = 12
    };


    struct ControlData {
        /* Is the position absolute (you specified an integer pixel value)
         * or not (you specified a float proportional value)?
         */
        ControlPositioning pos_type;

        int x, y;
        float fx, fy;

        cairo_t* local_context;
        cairo_surface_t* control_canvas;
        std::unique_ptr<Control> control;

        ControlData()
            : pos_type(ControlPositioning::Pixel), x(0), y(0), local_context(nullptr), control_canvas(nullptr),
              control(std::unique_ptr<Control>())
            {}

        ControlData(int x, int y, ControlPositioning cpos, cairo_t* ctxt,
                    cairo_surface_t* s, std::unique_ptr<Control> c)
            : pos_type(cpos), x(x), y(y), local_context(ctxt), control_canvas(s),
              control(std::move(c))
            {}

        ControlData(float x, float y, ControlPositioning cpos, cairo_t* ctxt,
                    cairo_surface_t* s, std::unique_ptr<Control> c)
            : pos_type(cpos), fx(x), fy(y), local_context(ctxt), control_canvas(s),
              control(std::move(c))
            {}

        ControlData(const ControlData& other) = delete;
        ControlData& operator=(const ControlData& other) = delete;
        ControlData(ControlData& other) = delete;
        ControlData& operator=(ControlData& other) = delete;

        ControlData(ControlData&& other) noexcept
            : pos_type(other.pos_type), x(other.x), y(other.y), fx(other.fx), fy(other.fy),
              local_context(other.local_context), control_canvas(other.control_canvas)
            {
                this->control = std::move(other.control);

                other.local_context = nullptr;
                other.control_canvas = nullptr;
            }

        ControlData& operator=(ControlData&& other) noexcept {
            this->pos_type = other.pos_type;
            this->x = other.x;
            this->y = other.y;
            this->fx = other.fx;
            this->fy = other.fy;
            this->local_context = other.local_context;
            this->control_canvas = other.control_canvas;

            this->control = std::move(other.control);

            other.local_context = nullptr;
            other.control_canvas = nullptr;

            return *this;
        }

        virtual ~ControlData() {}
    };

    class Control;

    typedef std::function<void(Control*)> EventCallbackFn;

    // TODO: make callbacks async?
    struct CallbackQueueElement {
        EventCallbackFn fn;
        Control* owner;

        // We use the owner ID to check if the control still
        // exists. If it did not exist anymore, we do not run the
        // callback.
        unsigned owner_id;
    };

    struct CallbackQueue {
        std::queue<CallbackQueueElement> callbacks;
    };


    /**
     * A container component
     *
     * Every control that has this component can have multiple
     * controls under it
     */
    struct ContainerComponent {
        Control* parent;



        /**
         * Sort the controls by their z-index values
         */
        void sortZIndex();

        std::vector<ControlData> children;

        /**
         * Get the control that is at the specified pixel coordinate
         */
        std::optional<Control*> getControlAtPoint(int x, int y);

        /**
         * Update the absolute (aka the pixel) positions of a control, so we can keep
         * track of them for box testing, for example
         *
         * When you discover the absolute position based on a relative coordinate
         * (like ControlPositioning::CenterX, or the fractional relative number),
         * you call this function to update it
         */
        void updateAbsoluteCoord(unsigned long long control_id, int absx, int absy);

        void add(int x, int y, std::unique_ptr<Control>);
        void add(float x, float y, ControlPositioning, std::unique_ptr<Control>);
        void add(double x, double y, std::unique_ptr<Control> c) {
            this->add((float)x, (float)y, std::move(c));
        }

        void remove(unsigned long long control_id);
    };


    /**
     * Defines the visual appearance of the controls
     *
     * In this structure are all types of visual customization I could think of.
     * Not all customizations will apply to all controls.
     */
    struct ControlAppearance {
        std::string fontFace;
        int fontSize;                        /// font size, in points

        std::array<double, 4> foreground = {1.0, 1.0, 1.0, 1.0};
        std::array<double, 4> background = {0.0, 0.0, 0.0, 0.0};
    };


    /**
     * Base control class
     */
    class Control {
    private:
        unsigned long long id_;
        std::function<void(Control*, size_t, size_t)> resize_cb_;

    protected:
        std::optional<ContainerComponent> cc_ = std::nullopt;
        ControlAppearance appearance_;

    public:
        Control();

        unsigned long getID() { return id_; }

        const ControlAppearance& getAppearance() { return appearance_; }
        virtual void setAppearance(ControlAppearance& a) { appearance_ = a; }

        void modifyAppearance(std::function<void(ControlAppearance&)> fn) {
            auto a = this->getAppearance();
            fn(a);
            this->setAppearance(a);
        }
        
        /**
         * The control z-index value
         *
         * Bigger the number, more priority it has, more on front of anything
         * else it will appear
         */
        int z_index = 1;

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

        /**
         * Set the resize callback, to notify the parent that you resized
         */
        void setResizeCallback(std::function<void(Control*, size_t, size_t)> cb) { resize_cb_ = cb; }

        /**
         * Resize the control
         */
        void resize(size_t w, size_t h);

        std::optional<ContainerComponent>& getControlContainer() { return cc_; }

        virtual void enqueueCallback(CallbackQueue& cq, EventCallbackFn ec);

        virtual void receiveEvent(const familyline::input::HumanInputAction& ev, CallbackQueue& cq) = 0;

        // see https://stackoverflow.com/a/461224
        virtual ~Control() {}
    };

} // namespace familyline::graphics::gui
