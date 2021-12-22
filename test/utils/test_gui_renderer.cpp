#include "test_gui_renderer.hpp"

#include <cstdio>

using namespace familyline::graphics::gui;

void TestGUIRenderer::update(const std::vector<ControlPaintData *> &data)
{
    data_ = data;
}

TestControlPaintData* TestGUIRenderer::query(int id)
{
    for (auto* d : data_) {
        TestControlPaintData *cd = (TestControlPaintData *)d;
        if (cd->control.id() == id)
            return cd;

        if (cd->children().size() > 0)
            return queryInto(id, cd);
    }
    
    return nullptr;    
}


TestControlPaintData* TestGUIRenderer::queryInto(int id, TestControlPaintData * parent)
{
    for (auto& c : parent->children()) {
        if (c->control.id() == id)
            return c.get();

        if (c->children().size() > 0)
            return queryInto(id, c.get());
    }

    return nullptr;
}


void TestGUIRenderer::render() { }

std::optional<GUIGlyphSize> TestGUIRenderer::getCodepointSize(
    char32_t codepoint, std::string_view fontName, size_t fontSize, FontWeight weight)
{
    // Ignore control characters
    if (codepoint < 32) return std::optional<GUIGlyphSize>(GUIGlyphSize{.width = 0, .height = 0});

    return std::optional<GUIGlyphSize>(GUIGlyphSize{
        .width  = 10,
        .height = 10,
    });
}

std::unique_ptr<GUIControlPainter> TestGUIRenderer::createPainter()
{    
    return std::make_unique<TestControlPainter>(*this);
}

std::unique_ptr<ControlPaintData> TestControlPainter::drawWindow(GUIWindow &w)
{
    //    printf("drawing window %04x: %s\n", w.id(), w.describe().c_str());
    return drawControl(w.box());
}

std::unique_ptr<ControlPaintData> TestControlPainter::drawControl(GUIControl &c)
{
    //    printf("\tdrawing control %04x: %s\n", c.id(), c.describe().c_str());
    if (auto box = dynamic_cast<GUIBox *>(&c); box) {
        std::vector<std::unique_ptr<TestControlPaintData>> children;
        for (auto *child : *box) {
            auto pchild = drawControl(*child).release();
            children.push_back(
                std::unique_ptr<TestControlPaintData>((TestControlPaintData *)pchild));
        }

        return std::make_unique<TestControlPaintData>(
            *box, box->width(), box->height(), box->x(), box->y(), std::move(children));
    } else {
        return std::make_unique<TestControlPaintData>(c, c.width(), c.height(), c.x(), c.y());
    }
}
