#include "IPanel.hpp"

using namespace Tribalia::Graphics::GUI;

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
