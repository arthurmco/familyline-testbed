/***
    Definitions for human player

    Copyright (C) 2016 Arthur M

***/

#include "Player.hpp"
#include "../graphical/Camera.hpp"
#include "ObjectRenderer.hpp"
#include "../input/InputPicker.hpp"
#include "../input/InputManager.hpp"

#ifndef HUMAN_PLAYER
#define HUMAN_PLAYER

namespace Tribalia {
namespace Logic {

    class HumanPlayer : public Player
    {
    private:
        Tribalia::Graphics::Camera* _cam;
		Tribalia::Input::InputPicker* _ip;
        
    public:
        ObjectRenderer* objr;

        HumanPlayer(const char* name, int elo=0, int xp=0);

        /***
            Virtual function called on each iteration.

            It allows player to decide its movement
            (input for humans, AI decisions for AI... )

            Returns true to continue its loop, false otherwise.
        ***/
        virtual bool Play(GameContext*);

        void SetCamera(Tribalia::Graphics::Camera*);
		void SetPicker(Tribalia::Input::InputPicker* ip);
        void SetInputManager(Tribalia::Input::InputManager*);

        ~HumanPlayer();
    };

}
}



#endif /* end of include guard: HUMAN_PLAYER */
