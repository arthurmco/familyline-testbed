#include "IPanel.hpp"

using namespace Tribalia::Graphics::GUI;

IPanel::IPanel(){_bgColor = 0xffffff00;}

IPanel::IPanel(int x, int y, int w, int h)
{
    _xPos = x;
    _yPos = y;
    _width = w;
    _height = h;
    _bgColor = 0xffffff00;
}

/* Non-virtual functions...
    fuck off */

int IPanel::GetBackColor() { return _bgColor; }
int IPanel::GetBackColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    a = (uint8_t)_bgColor & 0xff;
    b = (uint8_t)(_bgColor >> 8) & 0xff;
    g = (uint8_t)(_bgColor >> 16) & 0xff;
    r = (uint8_t)(_bgColor >> 24) & 0xff;
    return _bgColor;
}
void IPanel::SetBackColor(int rgba)
{
    _bgColor = rgba;
}
void IPanel::SetBackColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    _bgColor = a | (b << 8) | (g << 16) | (r << 24);
}

int IPanel::GetForeColor() { return _fgColor; }
int IPanel::GetForeColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    a = (uint8_t)_fgColor & 0xff;
    b = (uint8_t)(_fgColor >> 8) & 0xff;
    g = (uint8_t)(_fgColor >> 16) & 0xff;
    r = (uint8_t)(_fgColor >> 24) & 0xff;
    return _fgColor;
}
void IPanel::SetForeColor(int rgba)
{
    _fgColor = rgba;
}
void IPanel::SetForeColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    _fgColor = a | (b << 8) | (g << 16) | (r << 24);
}

void IPanel::SetBounds(int x, int y, int w, int h) 
{
    _xPos = x;
    _yPos = y;
    _width = w;
    _height = h;
}

void IPanel::SetPosition(int x, int y)
{
    _xPos = x;
    _yPos = y;
}

void IPanel::GetBounds(int& x, int& y, int& w, int& h)
{
    x = _xPos;
    y = _yPos;
    w = _width;
    h = _height;
    
}