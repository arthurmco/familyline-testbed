/*  Class for creating a button
    
    Copyright (C) 2017 Arthur M
*/

#include "IPanel.hpp"
#include <string>
#include <functional>
#include <algorithm> // for max()
#include <cstdarg>
#include <cstring>

#ifndef BUTTON_HPP
#define BUTTON_HPP

namespace Familyline::Graphics::GUI {

    typedef std::function<void(IControl*)> OnClickListener;
	     
class Button : public IPanel {
private:
//    std::string _text;
    struct {
	IPanel* panel;
	cairo_surface_t* panel_surf;
	cairo_t* panel_ctxt = nullptr;
    } panel_data;

    glm::vec4 _actualbg;
    bool isInput = true;

    bool panel_changed = true;
    bool _dirty = true;

    bool firstDraw = false;
    bool isHover = false;
    bool isClick = false;

    OnClickListener onClickListener = nullptr;
    
public:
    Button(int x, int y, int w, int h, IPanel* pnl);
    Button(double x, double y, double w, double h, IPanel* pnl);
    Button(int x, int y, int w, int h, const char* text);
    Button(double x, double y, double w, double h, const char* text);

    
    const IPanel* GetPanel();

    virtual void Redraw(cairo_t* ctxt) override;

    virtual bool ProcessInput(Input::InputEvent& ev) override;
   
    virtual void OnFocus() override;
    virtual void OnLostFocus() override;

    void SetOnClickListener(OnClickListener);

    virtual bool IsDirty() const override {
	return _dirty;
    }
};

}

#endif
