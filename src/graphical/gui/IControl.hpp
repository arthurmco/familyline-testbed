/*
    Class that ensures control re-updating

*/
#include <cairo/cairo.h>

#ifndef _GUI_CONTROLUPDATER
#define _GUI_CONTROLUPDATER

namespace Tribalia {
namespace Graphics {
namespace GUI {

/*
    A panel is the most basic window structore for Tribalia's GUI
*/
class IControl {
private:


public:
    /* Redraws itself */
    virtual void Redraw() = 0;
};

}
}
}


#endif /* end of include guard: _GUI_WINDOW */
