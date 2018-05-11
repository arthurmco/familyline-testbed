/*
    Class that ensures control re-updating

*/
#include <cairo/cairo.h>
#include <glm/glm.hpp>

#include "../../input/InputEvent.hpp"

#ifndef _GUI_CONTROLUPDATER
#define _GUI_CONTROLUPDATER

namespace Familyline {
namespace Graphics {
namespace GUI {

/*
    A panel is the most basic window structore for Familyline's GUI
*/
class IControl {
protected:
     /* Control back n fore color, in RGBA format */
    glm::vec4 _bgColor, _fgColor;
    float _opacity = 1.0;

public:
    float GetOpacity() const { return _opacity; }
    void SetOpacity(float f) { _opacity = f; }
    
    /* Redraws itself */
    virtual void Redraw(cairo_t* ctxt) = 0;

    /* Process control input */
    virtual bool ProcessInput(Input::InputEvent& ev) = 0;


    /* Functions called when you gain or lost focus */
    virtual void OnFocus() = 0;
    virtual void OnLostFocus() = 0;

    virtual bool IsDirty() const = 0;
    
    virtual ~IControl(){}
};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */
