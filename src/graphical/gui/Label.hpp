/*  Class for creating a label
    
    Copyright (C) 2016 Arthur M
*/

#include "IPanel.hpp"
#include <string>

#ifndef LABEL_HPP
#define LABEL_HPP

namespace Tribalia {
namespace Graphics {
namespace GUI {

class Label : public IPanel {
private:
    std::string _text;

public:
    Label(int x, int y, const char* text);
    Label(int x, int y, int w, int h, const char* text);

    const char* GetText() const;
    void SetText(char* txt);

    virtual void Redraw(cairo_t* ctxt);

};

}
}
}

#endif
