/*  Class for creating a button
    
    Copyright (C) 2017 Arthur M
*/

#include "IPanel.hpp"
#include <string>
#include <cstdarg>
#include <cstring>

#ifndef BUTTON_HPP
#define BUTTON_HPP

namespace Tribalia {
namespace Graphics {
namespace GUI {

class Button : public IPanel {
private:
    std::string _text;

    glm::vec4 _actualbg;
    bool isInput = true;

    cairo_text_extents_t extents;
    bool textChanged = true;
    
public:
    Button(int x, int y, int w, int h, const char* text);

    const char* GetText() const;
    void SetText(char* txt, ...);

    virtual void Redraw(cairo_t* ctxt) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override;
	
};

}
}
}

#endif
