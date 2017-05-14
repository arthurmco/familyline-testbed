#include "IPanel.hpp"

using namespace Tribalia::Graphics::GUI;

IPanel::IPanel(){_bgColor = glm::vec4(255, 255, 255, 0);}

IPanel::IPanel(int x, int y, int w, int h)
{
    _xPos = x;
    _yPos = y;
    _width = w;
    _height = h;
    _bgColor = glm::vec4(1,1,1,0);
}

/* Non-virtual functions...
    fuck off */

int IPanel::GetBackColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    unsigned int _r, _g, _b, _a;
    _r = int(_bgColor.r * 255) & 0xff;
    _g = int(_bgColor.g * 255) & 0xff;
    _b = int(_bgColor.b * 255) & 0xff;
    _a = int(_bgColor.a * 255) & 0xff;

    r = _r; g = _g; b = _b; a = _a;
    
    int color = r | (g << 8) | (b << 16) | (a << 24);
    return color;
}

int IPanel::GetBackColor()
{
    uint8_t r,g,b,a;
    return this->GetBackColor(r,g,b,a);    
}

void IPanel::SetBackColor(int rgba)
{
    int r,g,b,a;

    r = (rgba & 0xff);
    g = (rgba >> 8) & 0xff;
    b = (rgba >> 16) & 0xff;
    a = (rgba >> 24) & 0xff;
    
    _bgColor = glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}
void IPanel::SetBackColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    _bgColor = glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}

int IPanel::GetForeColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    
    unsigned int _r, _g, _b, _a;
    _r = int(_fgColor.r * 255) & 0xff;
    _g = int(_fgColor.g * 255) & 0xff;
    _b = int(_fgColor.b * 255) & 0xff;
    _a = int(_fgColor.a * 255) & 0xff;

    r = _r; g = _g; b = _b; a = _a;
    
    int color = r | (g << 8) | (b << 16) | (a << 24);
    return color;
    
}

int IPanel::GetForeColor()
{
    uint8_t r,g,b,a;
    return GetForeColor(r,g,b,a);    
}

void IPanel::SetForeColor(int rgba)
{
    int r,g,b,a;

    r = (rgba & 0xff);
    g = (rgba >> 8) & 0xff;
    b = (rgba >> 16) & 0xff;
    a = (rgba >> 24) & 0xff;
    
    _fgColor = glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}
void IPanel::SetForeColor(uint32_t r, uint32_t g, uint32_t b, uint32_t a)
{
    _fgColor = glm::vec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
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
