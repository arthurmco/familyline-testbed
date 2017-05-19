/*
    Class that ensures control re-updating

*/
#include <cairo/cairo.h>
#include <glm/glm.hpp>

#include "../../input/InputEvent.hpp"

#ifndef _GUI_CONTROLUPDATER
#define _GUI_CONTROLUPDATER

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    A panel is the most basic window structore for Tribalia's GUI
*/
class IControl {
protected:
     /* Control back n fore color, in RGBA format */
    glm::vec4 _bgColor, _fgColor;
    
public:
    /* Redraws itself */
    virtual void Redraw(cairo_t* ctxt) = 0;

    /* Process control input */
    virtual bool ProcessInput(Input::InputEvent& ev) = 0;
    
    virtual ~IControl(){}
};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */
