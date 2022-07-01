#ifndef MOCK_GUI_LABEL_HPP
#define MOCK_GUI_LABEL_HPP

#include "../src/graphical/gui/GUILabel.hpp"

class CGUILabel : public familyline::graphics::gui::GUILabel
{
private:
    unsigned nCalls = 0;

public:
    CGUILabel(float x, float y, const char* text) : GUILabel(x, y, text) {}

    virtual void render(int absw, int absh) override
    {
        (void)absw;
        (void)absh;

        this->nCalls++;
        this->setContext(absw, absh);
        this->doRender(absw, absh);
        this->dirty = false;
    }

    void resetCalls() { nCalls = 0; }
    unsigned getCalls() const { return nCalls; }
};

#endif /* MOCK_GUI_LABEL_HPP */
