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

IPanel::IPanel(double x, double y, double w, double h, bool relative)
{
    _fxPos = x;
    _fyPos = y;
    if (relative) {
	_fwidth = w;
	_fheight = h;
    } else {
	_width = w;
	_height = h;
    }
    _bgColor = glm::vec4(1,1,1,0);
}


/* Non-virtual functions...
    fuck off */

int IPanel::GetBackColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
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

int IPanel::GetBackColor() const
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

int IPanel::GetForeColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const
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

int IPanel::GetForeColor() const
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

void IPanel::GetBounds(int& x, int& y, int& w, int& h) const
{
    x = _xPos;
    y = _yPos;
    w = _width;
    h = _height;
    
}

void IPanel::SetBounds(double x, double y, double w, double h) 
{
    _xPos = x*_width;
    _yPos = y*_height;;
    _fwidth = w;
    _fheight = h;
}

void IPanel::SetPosition(double x, double y)
{
    _xPos = x*_width;
    _yPos = y*_height;
}

void IPanel::GetBounds(double& x, double& y, double& w, double& h) const
{
    x = _xPos/_width;
    y = _yPos/_height;
    w = _fwidth;
    h = _fheight;
    
}

void IPanel::ResizePanelAbsolute(IPanel* _p)
{
    if (_p->_fwidth <= 1 && _p->_fheight <= 1) {
	_p->_width = double(_p->_fwidth * _width);
	_p->_height = double(_p->_fheight * _height);

	if (_p->_fxPos <= 1 && _p->_fyPos <= 1) {
	    _p->_xPos = double(_p->_fxPos * _width);
	    _p->_yPos = double(_p->_fyPos * _height);
	}
    }

}


void IPanel::SetZIndex(double z) { _zindex = z; }
double IPanel::GetZIndex() const { return _zindex; }
