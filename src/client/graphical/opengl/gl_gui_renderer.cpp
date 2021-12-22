#include <client/graphical/opengl/gl_gui_renderer.hpp>

#include <cstdio>

using namespace familyline::graphics::gui;

void GLGUIRenderer::update(const std::vector<ControlPaintData *> &data)
{
    data_ = data;
}

GLControlPaintData* GLGUIRenderer::query(int id)
{
    for (auto* d : data_) {
        GLControlPaintData *cd = (GLControlPaintData *)d;
        if (cd->control.id() == id)
            return cd;

        if (cd->children().size() > 0)
            return queryInto(id, cd);
    }
    
    return nullptr;    
}


GLControlPaintData* GLGUIRenderer::queryInto(int id, GLControlPaintData * parent)
{
    for (auto& c : parent->children()) {
        if (c->control.id() == id)
            return c.get();

        if (c->children().size() > 0)
            return queryInto(id, c.get());
    }

    return nullptr;
}


void GLGUIRenderer::render() { }

std::optional<GUIGlyphSize> GLGUIRenderer::getCodepointSize(
    char32_t codepoint, std::string_view fontName, size_t fontSize, FontWeight weight)
{
    // Ignore control characters
    if (codepoint < 32) return std::optional<GUIGlyphSize>(GUIGlyphSize{.width = 0, .height = 0});

    return std::optional<GUIGlyphSize>(GUIGlyphSize{
        .width  = 10,
        .height = 10,
    });
}

std::unique_ptr<GUIControlPainter> GLGUIRenderer::createPainter()
{    
    return std::make_unique<GLControlPainter>(*this);
}

std::unique_ptr<ControlPaintData> GLControlPainter::drawWindow(GUIWindow &w)
{
    //    printf("drawing window %04x: %s\n", w.id(), w.describe().c_str());
    return drawControl(w.box());
}

std::unique_ptr<ControlPaintData> GLControlPainter::drawControl(GUIControl &c)
{
    //    printf("\tdrawing control %04x: %s\n", c.id(), c.describe().c_str());
    if (auto box = dynamic_cast<GUIBox *>(&c); box) {
        std::vector<std::unique_ptr<GLControlPaintData>> children;
        for (auto *child : *box) {
            auto pchild = drawControl(*child).release();
            children.push_back(
                std::unique_ptr<GLControlPaintData>((GLControlPaintData *)pchild));
        }

        return std::make_unique<GLControlPaintData>(
            *box, box->width(), box->height(), box->x(), box->y(), std::move(children));
    } else {
        return std::make_unique<GLControlPaintData>(c, c.width(), c.height(), c.x(), c.y());
    }
}
