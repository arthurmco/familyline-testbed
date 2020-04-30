#include "gui_label.hpp"

using namespace familyline::graphics::gui;

PangoLayout* Label::getLayout(cairo_t* context) const
{
    PangoLayout *layout = nullptr;
    PangoFontDescription *font_description = nullptr;

    font_description = pango_font_description_new ();
    pango_font_description_set_family (font_description, "Arial");
    pango_font_description_set_absolute_size (font_description, 24 * PANGO_SCALE);

    layout = pango_cairo_create_layout (context);
    pango_layout_set_font_description (layout, font_description);
    pango_layout_set_text (layout, this->text_.c_str(), -1);

    return layout;
}


bool Label::update(cairo_t* context, cairo_surface_t* canvas)
{
    PangoLayout* layout = this->getLayout(context);

    cairo_set_source_rgba(context, 1, 1, 1, 0);
    cairo_set_operator(context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context);

    cairo_set_source_rgba(context, 0, 0, 0, 1);
    cairo_move_to(context, 0, 0);
    pango_cairo_show_layout (context, layout);

    last_context_ = context;

    return true;
}

std::tuple<int, int> Label::getNeededSize(cairo_t* parent_context) const
{
    auto width = 1;
    auto height = 1;

    if (!parent_context) {
        return std::tie(width, height);
    }

    PangoLayout* layout = this->getLayout(parent_context);

    pango_layout_get_size(layout, &width, &height);
    width /= PANGO_SCALE;
    height /= PANGO_SCALE;

    return std::tie(width, height);
}


void Label::setText(std::string v) {
    if (this->text_ != v) {
        this->text_ = v;

        if (last_context_) {
            auto width = 1;
            auto height = 1;

            PangoLayout* layout = this->getLayout(last_context_);
            pango_layout_get_size(layout, &width, &height);

            last_context_ = nullptr;
            this->resize(width / PANGO_SCALE, height / PANGO_SCALE);
        }
    }
}

