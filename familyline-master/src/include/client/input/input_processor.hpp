#pragma once

/**
 * Input processor
 * Processes SDL inputs, and convert them to actions, so
 * we do not have to fill the code with SDL-specific functions.
 *
 * However, we retain SDL keycode constants, because it is easier
 *
 * Copyright (C) 2020 Arthur Mendes
 */

#include <client/input/input_actions.hpp>
#include <queue>
#include <string>
#include <thread>

namespace familyline::input
{
/**
 * Processes input from SDL and generate input actions
 *
 * Usually will not generate events that have something to do with game
 * objects, like BuildAction or ObjectClickAction, but you can enable
 * it when the game starts
 *
 * We virtualize the public functions so we can mock them on tests
 */
class InputProcessor
{
private:
    //        InputPicker& _ip;

    std::queue<HumanInputAction> _actions;

    std::thread _runThread;
    bool _isRunning = false;

    void enqueueEvent(const SDL_Event& e, int& lastX, int& lastY);

    // Did the ctrl, shift or alt keys were pressed by themselves?
    // This is important to know so we can register inputs that combine
    // the mouse and dead keys, like ctrl+scroll.
    bool lastCtrl_ = false, lastShift_ = false, lastAlt_ = false;

    /// Did the cursor is on the window?
    bool cursor_is_on_window_ = true;

public:
    //        InputProcessor(InputPicker& ip)
    //            : _ip(ip)
    //            {}

    /**
     * Starts a thread to receive input, so we do not lose input
     * even if the primary thread hangs
     *
     * We might not be able to process it, but the system will never
     * treat this game as non-responding.
     */
    virtual void startInputReceiver();

    /**
     * Stops the input receiver thread
     */
    virtual void stopInputReceiver();

    /**
     * Get the next action
     *
     * Return true if we have a next action, false if the action queue is
     * empty
     */
    virtual bool pollAction(HumanInputAction& a);

    /**
     * Start receiving text events
     *
     * Call this if, for example, you are inserting text in a
     * textbox
     */
    virtual void enableTextEvents();

    /**
     * Stop receiving text events
     *
     * Call this if, for example, you moved focus from a combobox to
     * another control
     */
    virtual void disableTextEvents();

    virtual std::string getClipboardText();

    virtual ~InputProcessor() { this->stopInputReceiver(); }
};

}  // namespace familyline::input
