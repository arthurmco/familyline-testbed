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
	cairo_select_font_face(cr, "Consolas", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
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


bool GUIRenderer::Render()
{
	glBindTexture(GL_TEXTURE_2D, _f->GetTextureHandle());

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
