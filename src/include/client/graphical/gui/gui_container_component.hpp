#pragma once

/**
 * Controls container component classes
 *
 * Copyright 2020 Arthur Mendes
 */

#include <cairo/cairo.h>

#include <memory>
#include <optional>
#include <tuple>
#include <vector>

namespace familyline::graphics::gui
{
class Control;

enum ControlPositioning { Pixel = 1, Relative = 2, CenterX = 4, CenterY = 8, CenterAll = 12 };

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
        : pos_type(ControlPositioning::Pixel),
          x(0),
          y(0),
          fx(0.0f),
          fy(0.0f),
          local_context(nullptr),
          control_canvas(nullptr),
          control(std::unique_ptr<Control>())
    {
    }

    ControlData(
        int x, int y, ControlPositioning cpos, cairo_t* ctxt, cairo_surface_t* s,
        std::unique_ptr<Control> c)
        : pos_type(cpos),
          x(x),
          y(y),
          fx(0.0f),
          fy(0.0f),
          local_context(ctxt),
          control_canvas(s),
          control(std::move(c))
    {
    }

    ControlData(
        float x, float y, ControlPositioning cpos, cairo_t* ctxt, cairo_surface_t* s,
        std::unique_ptr<Control> c)
        : pos_type(cpos),
          fx(x),
          fy(y),
          x(0),
          y(0),
          local_context(ctxt),
          control_canvas(s),
          control(std::move(c))
    {
    }

    ControlData(const ControlData& other) = delete;
    ControlData& operator=(const ControlData& other) = delete;
    ControlData(ControlData& other)                  = delete;
    ControlData& operator=(ControlData& other) = delete;

    ControlData(ControlData&& other) noexcept
        : pos_type(other.pos_type),
          x(other.x),
          y(other.y),
          fx(other.fx),
          fy(other.fy),
          local_context(other.local_context),
          control_canvas(other.control_canvas)
    {
        this->control = std::move(other.control);

        other.local_context  = nullptr;
        other.control_canvas = nullptr;
    }

    ControlData& operator=(ControlData&& other) noexcept
    {
        this->pos_type       = other.pos_type;
        this->x              = other.x;
        this->y              = other.y;
        this->fx             = other.fx;
        this->fy             = other.fy;
        this->local_context  = other.local_context;
        this->control_canvas = other.control_canvas;

        this->control = std::move(other.control);

        other.local_context  = nullptr;
        other.control_canvas = nullptr;

        return *this;
    }

    virtual ~ControlData()
    {
        cairo_surface_destroy(control_canvas);
        cairo_destroy(local_context);
    }
};

/**
 * A container component
 *
 * Every control that has this component can have multiple
 * controls under it
 */
struct ContainerComponent {
    Control* parent = nullptr;

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
    void add(double x, double y, std::unique_ptr<Control> c)
    {
        this->add((float)x, (float)y, std::move(c));
    }

    void remove(unsigned long long control_id);
};


}  // namespace familyline::graphics::gui
