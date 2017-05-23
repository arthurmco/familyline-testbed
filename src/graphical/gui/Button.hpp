/*  Class for creating a button
    
    Copyright (C) 2017 Arthur M
*/

#include "IPanel.hpp"
#include <string>
#include <functional>
#include <cstdarg>
#include <cstring>

#ifndef BUTTON_HPP
#define BUTTON_HPP

namespace Tribalia {
namespace Graphics {
namespace GUI {

    typedef std::function<void(IControl*)> OnClickListener;
	     
class Button : public IPanel {
private:
    std::string _text;

    glm::vec4 _actualbg;
    bool isInput = true;

    cairo_text_extents_t extents;
    bool textChanged = true;

    bool isHover = false;

    OnClickListener onClickListener = nullptr;
    
public:
    Button(int x, int y, int w, int h, const char* text);

    const char* GetText() const;
    void SetText(char* txt, ...);

    virtual void Redraw(cairo_t* ctxt) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override;
   
    virtual void OnFocus() override;
    virtual void OnLostFocus() override;

    void SetOnClickListener(OnClickListener);
};

}
}
}

#endif
