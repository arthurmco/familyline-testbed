#include <pango/pangocairo.h>

#include <client/graphical/gfx_service.hpp>
#include <client/graphical/opengl/gl_gui_renderer.hpp>
#include <common/logger.hpp>
#include <cstdio>
#include <cuchar>

#ifdef RENDERER_OPENGL

using namespace familyline::graphics::gui;

PangoLayout* getLayoutFromText(
    cairo_t* ctxt, const std::string& text, const GUIAppearance& appearance, bool markup = false);

void GLGUIRenderer::update(const std::vector<ControlPaintData*>& data)
{
    srand(data.size() * data.size() * 1000);

    cairo_set_source_rgba(context_, 0, 0, 0, 0);
    cairo_set_operator(context_, CAIRO_OPERATOR_SOURCE);
    cairo_paint(context_);

    for (const ControlPaintData* d : data) {
        const GLControlPaintData* gld = (const GLControlPaintData*)d;

        cairo_set_operator(context_, CAIRO_OPERATOR_OVER);
        cairo_move_to(context_, gld->x, gld->y);
        cairo_set_source_surface(context_, (cairo_surface_t*)gld->data(), 0, 0);
        cairo_paint(context_);
    }
}

std::optional<GUIGlyphSize> GLGUIRenderer::getCodepointSize(
    char32_t codepoint, std::string_view fontName, size_t fontSize, FontWeight weight)
{
    // Ignore control characters
    if (codepoint < 32) return std::optional<GUIGlyphSize>(GUIGlyphSize{.width = 0, .height = 0});

    std::mbstate_t state{};
    char c[MB_LEN_MAX + 1] = {};
    auto rc                = std::c32rtomb(c, codepoint, &state);
    if (rc == (std::size_t)-1) return std::nullopt;

    std::string ret = c;

    GUIAppearance a;
    a.font     = fontName;
    a.fontsize = fontSize;
    a.weight   = weight;

    PangoLayout* l = getLayoutFromText(context_, ret, a);
    int width = 0, height = 0;
    pango_layout_get_size(l, &width, &height);
    g_object_unref(l);
    
    return std::optional<GUIGlyphSize>(GUIGlyphSize{
        .width  = double(width) / PANGO_SCALE,
        .height = double(height) / PANGO_SCALE,
    });
}

std::unique_ptr<GUIControlPainter> GLGUIRenderer::createPainter()
{
    return std::make_unique<GLControlPainter>(*this);
}

std::unique_ptr<ControlPaintData> GLControlPainter::drawWindow(GUIWindow& w)
{
    return drawControl(w.box());
}

PangoLayout* getLayoutFromText(
    cairo_t* ctxt, const std::string& text, const GUIAppearance& appearance, bool markup)
{
    PangoFontDescription* font_description = pango_font_description_new();
    pango_font_description_set_family(font_description, text.c_str());
    pango_font_description_set_absolute_size(font_description, appearance.fontsize * PANGO_SCALE);

    PangoWeight weight;
    switch (appearance.weight) {
        case FontWeight::Regular: weight = PANGO_WEIGHT_NORMAL; break;
        case FontWeight::Bold: weight = PANGO_WEIGHT_BOLD; break;
    }
    pango_font_description_set_weight(font_description, weight);

    //        pango_font_description_set_style(
    //            font_description, appearance_.italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);

    PangoLayout* layout = pango_cairo_create_layout(ctxt);
    pango_layout_set_font_description(layout, font_description);

    if (markup)
        pango_layout_set_markup(layout, text.c_str(), -1);
    else
        pango_layout_set_text(layout, text.c_str(), -1);

    pango_font_description_free(font_description);

    return layout;
}

void drawLabel(cairo_t* ctxt, const GUILabel* label, const GUIAppearance& appearance)
{
    PangoLayout* layout = getLayoutFromText(ctxt, label->text(), appearance, true);

    int width = 0, height = 0;
    pango_layout_get_size(layout, &width, &height);

    width /= PANGO_SCALE;
    height /= PANGO_SCALE;

    int startx = 0, starty = appearance.marginY;
    switch (appearance.horizontalAlignment) {
        case HorizontalAlignment::Left: startx = appearance.marginX; break;
        case HorizontalAlignment::Center:
            startx = glm::max(appearance.marginX, (label->width() / 2) - (width / 2));
            break;
        case HorizontalAlignment::Right:
            startx = label->width() - width - appearance.marginX;
            break;
    }

    auto [fr, fg, fb, fa] = appearance.foreground;
    auto [br, bg, bb, ba] = appearance.background;

    cairo_set_source_rgba(ctxt, fr, fg, fb, fa);
    cairo_move_to(ctxt, startx, starty);
    pango_cairo_show_layout(ctxt, layout);

    g_object_unref(layout);
}

std::unique_ptr<ControlPaintData> GLControlPainter::drawControl(GUIControl& c)
{
    auto controlpaint =
        std::make_unique<GLControlPaintData>(c, c.x(), c.y(), c.width(), c.height());

    cairo_t* ctxt = (cairo_t*)controlpaint->context;

    auto appearance = c.appearance();

    auto [fr, fg, fb, fa] = appearance.foreground;
    auto [br, bg, bb, ba] = appearance.background;

    if (auto box = dynamic_cast<GUIBox*>(&c); box) {
        for (auto* child : *box) {
            auto relx = child->x() - c.x();
            auto rely = child->y() - c.y();

            auto pchild = drawControl(*child);

            cairo_set_operator(ctxt, CAIRO_OPERATOR_OVER);
            cairo_set_source_surface(ctxt, (cairo_surface_t*)pchild->data(), relx, rely);
            cairo_paint(ctxt);
        }
    } else if (auto label = dynamic_cast<GUILabel*>(&c); label) {
        cairo_set_source_rgba(ctxt, br, bg, bb, ba);
        cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctxt);

        drawLabel(ctxt, label, appearance);

    } else if (auto button = dynamic_cast<GUIButton*>(&c); button) {
        if (button->isHover()) {
            br = glm::min(br + 0.2, 1.0);
            bg = glm::min(bg + 0.2, 1.0);
            bb = glm::min(bb + 0.2, 1.0);
            ba = glm::min(ba + 0.2, 1.0);
        }

        cairo_set_source_rgba(ctxt, br, bg, bb, ba);
        cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctxt);

        cairo_set_line_width(ctxt, 4.0);
        cairo_set_source_rgb(ctxt, fr, fg, fb);
        cairo_rectangle(ctxt, 1, 1, c.width() - 1, c.height() - 1);
        cairo_stroke(ctxt);

        GUIAppearance labelap = appearance;
        labelap.marginX += 6;
        labelap.marginY += 6;

        drawLabel(ctxt, &button->getInnerLabel(), labelap);

    } else if (auto textbox = dynamic_cast<GUITextbox*>(&c); textbox) {


        auto [selbefore, selection, selafter] = textbox->getTextAsSelection(false);

        PangoLayout* layoutBefore = getLayoutFromText(ctxt, selbefore, appearance);

        GUIAppearance inverseAppearance = appearance;
        std::swap(inverseAppearance.foreground, inverseAppearance.background);

        PangoLayout* layoutSelection = getLayoutFromText(ctxt, selection, inverseAppearance);
        PangoLayout* layoutAfter     = getLayoutFromText(ctxt, selafter, appearance);

        int selectionoff = 0, height = 0;
        pango_layout_get_size(layoutBefore, &selectionoff, &height);
        int selwidth = 0;
        pango_layout_get_size(layoutSelection, &selwidth, &height);

        selectionoff /= PANGO_SCALE;
        selwidth /= PANGO_SCALE;
        height /= PANGO_SCALE;

        auto control_height = std::min(height+25, c.height());
        
        cairo_move_to(ctxt, 0, 0);
        cairo_set_source_rgba(ctxt, br, bg, bb, ba);
        cairo_set_operator(ctxt, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctxt);

        cairo_set_source_rgba(ctxt, 1.0 - br, 1.0 - bg, 1.0 - bb, ba);
        cairo_set_line_width(ctxt, 4.0);
        cairo_rectangle(ctxt, 1, 1, c.width() - 1, control_height - 1);
        cairo_stroke(ctxt);

        selwidth = std::max(selwidth, 2);

        cairo_set_source_rgba(ctxt, fr, fg, fb, fa);
        cairo_move_to(ctxt, 5, control_height - height - 4);
        pango_cairo_show_layout(ctxt, layoutBefore);

        cairo_set_source_rgba(ctxt, fr, fg, fb, fa);
        cairo_rectangle(ctxt, 5 + selectionoff + 1, 5, selwidth, control_height - 5);
        cairo_fill(ctxt);

        if (selection.size() == 0) {
            cairo_set_source_rgba(ctxt, br, bg, bb, ba);
            cairo_move_to(ctxt, 5 + selectionoff + 1, control_height - height - 4);
            pango_cairo_show_layout(ctxt, layoutSelection);
        }

        cairo_set_source_rgba(ctxt, fr, fg, fb, fa);
        cairo_move_to(ctxt, 5 + selectionoff + selwidth, control_height - height - 4);
        pango_cairo_show_layout(ctxt, layoutAfter);
        
        g_object_unref(layoutBefore);
        g_object_unref(layoutSelection);
        g_object_unref(layoutAfter);

    } else {
        double r = (rand() % 256) / 256.0;
        cairo_set_source_rgb(ctxt, r, 0, 0.5);
        cairo_rectangle(ctxt, 1, 1, c.width() - 1, c.height() - 1);
        cairo_fill(ctxt);

        cairo_set_source_rgb(ctxt, 0.1, 0.0, 0.25);
        auto strsize = fmt::format("{:08x}, ({}, {})", c.id(), c.x(), c.y());
        cairo_move_to(ctxt, 10, 20);
        cairo_set_font_size(ctxt, 15.0);
        cairo_show_text(ctxt, strsize.c_str());
        strsize = fmt::format("{}", c.describe());
        cairo_move_to(ctxt, 10, 40);
        cairo_set_font_size(ctxt, 15.0);
        cairo_show_text(ctxt, strsize.c_str());
    }

    if (c.onFocus()) {
        cairo_set_line_width(ctxt, 1.0);
        cairo_set_source_rgb(ctxt, fr, fg, fb);
        cairo_rectangle(ctxt, 0, 0, c.width(), c.height());
        cairo_stroke(ctxt);
    }

#ifdef DRAW_DEBUG_BORDER
    cairo_set_line_width(ctxt, 1.0);
    cairo_set_source_rgb(ctxt, 0.0, 0.0, 1.0);
    cairo_rectangle(ctxt, 1, 1, c.width() - 1, c.height() - 1);
    cairo_stroke(ctxt);
#endif
    return controlpaint;
}

/* The panel vertex square coordinates.
   It's a big rectangle, that fills the entire screen  */
static const float window_pos_coord[][3] = {{-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},
                                            {-1, 1, 1}, {1, 1, 1},   {1, -1, 1}};

/* Coordinates for every panel texture.
   Since they have the same vertex order, we don't need to declare multiple
   texture coordinates
   Also send the y coordinate inverted, because in OpenGL, Y positive is down, not up
*/
static const float window_texture_coord[][2] = {{-1, 1}, {-1, -1}, {1, -1},
                                                {-1, 1}, {1, 1},   {1, -1}};

GLuint attrPos_, attrTex_;
GLuint vboPos, vboTex;

GLGUIRenderer::GLGUIRenderer()
{
    canvas_  = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, screenWidth_, screenHeight_);
    context_ = cairo_create(canvas_);
    this->initShaders();
    tex_gui_ = this->initTexture(screenWidth_, screenHeight_);
}

/**
 * Initialize the GUI shaders
 */
void GLGUIRenderer::initShaders()
{
    auto& d = GFXService::getDevice();

    sGUI_ = d->createShaderProgram(
        "gui", {d->createShader("shaders/GUI.vert", ShaderType::Vertex),
                d->createShader("shaders/GUI.frag", ShaderType::Fragment)});

    sGUI_->link();

    auto fnGetAttrib = [&](const char* name) {
        return glGetAttribLocation(sGUI_->getHandle(), name);
    };

    attrPos_ = fnGetAttrib("position");
    attrTex_ = fnGetAttrib("in_uv");

    // Create the vertices
    glGenVertexArrays(1, &(this->vao_gui_));
    glBindVertexArray(this->vao_gui_);

    /* Create vertex information */
    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, window_pos_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(attrPos_, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attrPos_);

    glGenBuffers(1, &vboTex);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, window_texture_coord, GL_STATIC_DRAW);
    glVertexAttribPointer(attrTex_, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attrTex_);

    glBindVertexArray(0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        auto e = fmt::format("error {:#x} while setting vertices and shaders for GUI content", err);
        throw graphical_exception(e);
    }
}

GLuint GLGUIRenderer::initTexture(int width, int height)
{
    GLuint handle;
    glGenTextures(1, &handle);

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    cairo_surface_flush(this->canvas_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
        (void*)cairo_image_surface_get_data(canvas_));

    glBindTexture(GL_TEXTURE_2D, 0);
    return handle;

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        auto e = fmt::format("error {:#x} while setting texture for GUI content", err);
        throw graphical_exception(e);
    }
}

/**
 * Resize the GUI texture
 */
GLuint GLGUIRenderer::resizeTexture(int width, int height)
{
    glDeleteTextures(1, &tex_gui_);

    cairo_surface_destroy(canvas_);
    cairo_destroy(context_);

    canvas_  = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    context_ = cairo_create(canvas_);
    return initTexture(width, height);
}

void GLGUIRenderer::render()
{
    auto& log = LoggerService::getLogger();

    cairo_set_source_rgb(context_, 1.0, 1.0, 1.0);
    auto strsize = fmt::format("{} x {}", screenWidth_, screenHeight_);
    cairo_move_to(context_, 50, 50);
    cairo_set_font_size(context_, 15.0);
    cairo_show_text(context_, strsize.c_str());

    cairo_surface_flush(this->canvas_);
    auto* canvas_data = cairo_image_surface_get_data(this->canvas_);
    if (!canvas_data) {
        log->write(
            "gui-renderer", LogType::Warning, "canvas data points to a null pointer, weird...");
        return;
    }

    // Make the GUI texture transparent
    glClearColor(0.0, 0.0, 0.0, 0.0);
    GLint depthf;
    glGetIntegerv(GL_DEPTH_FUNC, &depthf);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    GFXService::getShaderManager()->use(*sGUI_);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->tex_gui_);

    sGUI_->setUniform("texPanel", 0);
    sGUI_->setUniform("opacity", 1.0f);

    glBindVertexArray(this->vao_gui_);

    glEnableVertexAttribArray(attrPos_);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glVertexAttribPointer(attrPos_, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(attrTex_);
    glBindBuffer(GL_ARRAY_BUFFER, vboTex);
    glVertexAttribPointer(attrTex_, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0, screenWidth_, screenHeight_, GL_BGRA, GL_UNSIGNED_BYTE,
        (void*)canvas_data);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        log->write("gui-renderer", LogType::Error, "OpenGL error %#x", err);
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthFunc(depthf);
    glDisable(GL_BLEND);

    glClearColor(0.0, 0.0, 0.0, 1.0);
}

#endif
