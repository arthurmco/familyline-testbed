#ifndef MOCK_GUI_BUTTON_HPP
#define MOCK_GUI_BUTTON_HPP

#include "../src/graphical/gui/GUIButton.hpp"

class CGUIButton : public familyline::graphics::gui::GUIButton
{
private:
    unsigned nCalls = 0;
    static int n_renderOrder;
    int renderOrder = 0;

public:
    CGUIButton(float x, float y, float w, float h, const char* text) : GUIButton(x, y, w, h, text)
    {
    }

    static void resetRenderOrder() { CGUIButton::n_renderOrder = 0; }

    int getRenderOrder() const { return renderOrder; }

    virtual void render(int absw, int absh) override
    {
        (void)absw;
        (void)absh;

        this->nCalls++;
        this->setContext(absw, absh);
        this->doRender(absw, absh);
        this->renderOrder = CGUIButton::n_renderOrder++;
        this->dirty       = false;
    }

    void resetCalls() { nCalls = 0; }
    unsigned getCalls() const { return nCalls; }
};

int CGUIButton::n_renderOrder = 0;

#endif /* MOCK_GUI_BUTTON_HPP */
