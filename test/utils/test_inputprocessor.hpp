#pragma once

#include <client/input/input_processor.hpp>
#include <client/input/input_actions.hpp>

/**
 * A class that mocks input receiving.
 *
 * We can use this class to test classes that depend on human input, such as
 * the human player class, or the GUI classes.
 */
class TestInputProcessor : public familyline::input::InputProcessor {

public:
    
    /**
     * Starts a thread to receive input, so we do not lose input
     * even if the primary thread hangs
     *
     * We might not be able to process it, but the system will never
     * treat this game as non-responding.
     */
    virtual void startInputReceiver() {}

    /**
     * Stops the input receiver thread
     */
    virtual void stopInputReceiver() {}

    /**
     * Get the next action
     *
     * Return true if we have a next action, false if the action queue is
     * empty
     */
    virtual bool pollAction(familyline::input::HumanInputAction& a);

    /**
     * Start receiving text events
     *
     * Call this if, for example, you are inserting text in a
     * textbox
     */
    virtual void enableTextEvents() { text_events_ = true;}

    /**
     * Stop receiving text events
     *
     * Call this if, for example, you moved focus from a combobox to
     * another control
     */
    virtual void disableTextEvents() { text_events_ = false; }

    virtual std::string getClipboardText() { return clipboard_; }
    
    virtual ~TestInputProcessor() { }

    void pushAction(familyline::input::HumanInputAction action);

    void setClipboardText(std::string v) { clipboard_ = v; };
    
private:
    std::queue<familyline::input::HumanInputAction> actions_;

    bool text_events_ = false;
    std::string clipboard_ = "";
};
