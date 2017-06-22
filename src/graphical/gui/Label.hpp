/*  Class for creating a label
    
    Copyright (C) 2016 Arthur M
*/

#include "IPanel.hpp"
#include <string>
#include <cstdarg>
#include <cstring>

#ifndef LABEL_HPP
#define LABEL_HPP

namespace Tribalia {
namespace Graphics {
namespace GUI {

class Label : public IPanel {
private:
    std::string _text;
    const char* _font_name;
    unsigned int _font_size;
    bool _dirty = true;
    
public:
    Label(int x, int y, const char* text);
    Label(int x, int y, int w, int h, const char* text);
    Label(double x, double y, double w, double h, const char* text);
    
    const char* GetText() const;
    void SetText(char* txt, ...);

    void SetFontData(const char* name, unsigned int size);

    virtual void Redraw(cairo_t* ctxt) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override;

    virtual void OnFocus() override {}
    virtual void OnLostFocus() override {}

    virtual bool IsDirty() const override { return _dirty; }
};

}
}
}

#endif
