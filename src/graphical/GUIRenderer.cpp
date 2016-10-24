#include "GUIRenderer.hpp"

using namespace Tribalia::Graphics;

int ww, wh;
GUIRenderer::GUIRenderer(Window* w)
{
	/* Create the cairo context */
	w->GetSize(ww, wh);
	cr_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, ww, wh);
	cr = cairo_create(cr_surface);

	/* Create the framebuffer */
	cairo_rectangle(cr, 20, 20, 200, 100);
	cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);

	cairo_fill(cr);

	_w = w;
}

bool surfaceChanged = false;

void GUIRenderer::DebugWrite(int x, int y, const char* fmt, ...)
{
	char* ch = new char[512];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(ch, fmt, vl);
	va_end(vl);

	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
	cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 14.0);
	cairo_move_to(cr, x*1.0, y*1.0);
	cairo_show_text(cr, ch);
	surfaceChanged = true;
	delete ch;
}

void GUIRenderer::SetFramebuffer(Framebuffer* f) {
	_f = f;
	Log::GetLog()->Write("[GUIRenderer] Framebuffer now it's the texture #%d", f->GetTextureHandle());

}

bool fb_setup = false;
bool GUIRenderer::Render()
{
	this->Redraw();

	glBindTexture(GL_TEXTURE_2D, _f->GetTextureHandle());
	if (!fb_setup) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		fb_setup = true;
	}

	cairo_surface_flush(cr_surface);
	unsigned char* c = cairo_image_surface_get_data(cr_surface);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ww, wh, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)c);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (surfaceChanged) {
		cairo_save(cr);
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);
		surfaceChanged = false;
	}

	return true;
}

/* Redraw the child controls */
void GUIRenderer::Redraw()
{
    for (GUI::IPanel* p : _panels) {
        p->Redraw();

		int x,y,w,h;
		uint8_t r,g,b,a;

		p->GetBounds(x,y,w,h);
		p->GetBackColor(r,g,b,a);
		cairo_set_line_width (cr, 1);
		cairo_set_source_rgba (cr, r/255.0, g/255.0, b/255.0, a/255.0);
		cairo_rectangle (cr, x, y, w, h);
		cairo_fill (cr);
    }
}

/* Add a panel using the panel position or a new position */
 int GUIRenderer::AddPanel(GUI::IPanel* p)
 {
	 Log::GetLog()->Write("GUIRenderer: Added panel (%#p)", p);
	 _panels.push_back(p);
	 return 1;
 }

 int GUIRenderer::AddPanel(GUI::IPanel* p, int x, int y)
 {
	 p->SetPosition(x, y);
	 AddPanel(p);
 }

/* Remove the panel */
 void GUIRenderer::RemovePanel(GUI::IPanel* p)
 {

 }

 void GUIRenderer::SetBounds(int x, int y, int w, int h) {}
 void GUIRenderer::SetPosition(int x, int y) {}
 void GUIRenderer::GetBounds(int& x, int& y, int& w, int& h)
 {
	x = 0;	y = 0;	w = ww;	h = wh;
 }
