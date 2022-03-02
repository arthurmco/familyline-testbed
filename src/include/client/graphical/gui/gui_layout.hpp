#pragma once

#include <cassert>
#include <functional>
#include <vector>

namespace familyline::graphics::gui
{
/*
 * BaseLayout
 *
 * A subclass for all types of layout configuration
 * Each layout class will have its own `add` method, because each one will know
 * what it needs to require to position a client Ones might not need to know
 * nothing (the flex ones, for example), and others will need to know all of the
 * clients positional data (the absolute ones, for example)
 *
 * In the base layout, we receive two functions
 * The first function will retrieve a list of control IDs.
 * The second function will get the ID and return a reference to its GUIControl.
 * They should be filled by the window.
 */
class BaseLayout
{
protected:
    std::function<std::vector<int>()> fnGetIDs_;
    std::function<GUIControl *(int)> fnGetControl_;

public:
    void initialize(decltype(fnGetIDs_) fnGetIDs, decltype(fnGetControl_) fnGetControl)
    {
        fnGetIDs_     = fnGetIDs;
        fnGetControl_ = fnGetControl;
    }

    /// The window resized.
    virtual void resize(int windoww, int windowh, int parentx = 0, int parenty = 0) = 0;

    /// You need to update the positions of the controls you have inside you.
    virtual void update() {}

    virtual ~BaseLayout() {}
};

/**
 * FlexLayout
 *
 * Allow the user to set the controls one after other, similar (but not equal)
 * to the `display: flex` CSS attribute
 *
 * If horizontal is true, flex is horizontal. If not, flex is vertical
 */
template <bool horizontal>
class FlexLayout : public BaseLayout
{
private:
    std::vector<int> controlIDs_;

    int windoww_ = 0;
    int windowh_ = 0;

    int parentx_ = 0;
    int parenty_ = 0;

public:
    /// The window resized.
    virtual void resize(int windoww, int windowh, int parentx = 0, int parenty = 0)
    {
        windoww_ = windoww;
        windowh_ = windowh;

        parentx_ = parentx;
        parenty_ = parenty;
    }

    void add(int id) { controlIDs_.push_back(id); }

    /**
     * With the addition of maximum width and maximum height to our layout components,
     * guessing the size of them by dividing the total space by the number of controls
     * will not be possible
     *
     * But we can still reuse that strategy to recalculate the remaining space.
     * We do exacly that here: calculate the width for the remaining controls
     */
    std::tuple<int, int> recalculateRemainingSize(int count, int processedcontrols, int currentval)
    {
        int ncount = count - processedcontrols;
        if (ncount == 0) {
            return std::make_tuple(0, 0);
        }

        return std::make_tuple(
            (horizontal ? (windoww_ - currentval) / ncount : windoww_),
            (horizontal ? windowh_ : (windowh_ - currentval) / ncount));
    }

    /**
     * Update the positions of controls we have here
     */
    virtual void update()
    {
        assert(windoww_ > 0);
        assert(windowh_ > 0);

        auto ccount = controlIDs_.size();
        int border  = 0;

        int currentx = parentx_ + border;
        int currenty = parenty_ + border;

        int currentw = (horizontal ? windoww_ / ccount : windoww_) - border;
        int currenth = (horizontal ? windowh_ : windowh_ / ccount) - border;

        bool shouldRecalculateRemainingWidth  = false;
        bool shouldRecalculateRemainingHeight = false;

        int cidx = -1;
        for (auto cid : controlIDs_) {
            cidx++;
            GUIControl *c = fnGetControl_(cid);
            if (!c) {
                continue;
            }

            GUIAppearance a = c->appearance();
            auto mx         = a.marginX;
            auto my         = a.marginY;

            if (a.minWidth || a.maxWidth) {
                shouldRecalculateRemainingWidth = true;
                if (a.minWidth) {
                    currentw = std::max((unsigned)currentw, *a.minWidth);
                }
                if (a.maxWidth) {
                    currentw = std::min((unsigned)currentw, *a.maxWidth);
                }
            }

            if (a.minHeight || a.maxHeight) {
                shouldRecalculateRemainingHeight = true;
                if (a.minHeight) {
                    currenth = std::max((unsigned)currenth, *a.minHeight);
                }
                if (a.maxHeight) {
                    currenth = std::min((unsigned)currenth, *a.maxHeight);
                }
            }

            c->onResize(currentw - mx, currenth - my, currentx + mx, currenty + my);

            if (horizontal) {
                currentx += currentw + border;
            } else {
                currenty += currenth + border;
            }

            if (shouldRecalculateRemainingWidth || shouldRecalculateRemainingHeight) {
                auto [cw, ch] =
                    recalculateRemainingSize(ccount, cidx + 1, horizontal ? currentx : currenty);
                if (shouldRecalculateRemainingWidth) {
                    currentw = cw;
                }
                if (shouldRecalculateRemainingHeight) {
                    currenth = ch;
                }
            }
        }
    }
};

}  // namespace familyline::graphics::gui
