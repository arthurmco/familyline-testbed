/*
 * The one that runs the GUI rendering code
 *
 * Copyright (C) 2018, 2019 Arthur Mendes
 *
 */

/*
 * TODO: handle resize and resolution changes.
 */

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>
#include <queue>

#include "GUIControl.hpp"
#include "../../input/InputManager.hpp"
#include "../../input/InputListener.hpp"
#include "../shader_manager.hpp"
#include "../window.hpp"

namespace familyline::graphics::gui {

    /**
     * \brief The GUI Manager
     *
     * To use it, you need first to run the render() method, to render the GUI contents to the
     * framebuffer, and then you need to run the renderToScreen() method, to effectively
     * push it into the screen
     */
    class GUIManager : public GUIControl {
    private:
        // Only force redraw on redraw events.
        bool force_redraw = false;

        /* This listener receives the input events from the game and puts them into the control */
        input::InputListener* listener = nullptr;

        ShaderProgram* sGUI;

        // OpenGL interface variables
        GLuint vaoGUI;
        GLuint attrPos, attrTex;
        GLuint vboPos, vboTex;
        GLuint texHandle;

    public:
        std::vector<GUIControl*> controls;

        GUIManager(int width, int height);


        void initShaders(familyline::graphics::Window* w);


        /** 
         * Add the controller (and send the containeradd event) 
         */
        void add(GUIControl* c);

        /**
         * Remove the control 
         */
        void remove(GUIControl* c);

        /**
         * Try to handle the signal. Returns true if handled
         */
        virtual bool processSignal(GUISignal s);

        /**
         * Process signals for all controls
         */
        virtual void update();

        virtual GUICanvas doRender(int absw, int absh) const;


        /**
         * Render this into a graphical framebuffer
         *
         * Screen, for you
         */
        void renderToScreen();

        virtual void render(int absw, int absh);

        virtual ~GUIManager() { }

    };


}


#endif /* GUIMANAGER_H */
