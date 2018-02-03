/*
    The animation manager class

    Copyright (C) 2016 Arthur M
*/

#include "AnimationData.hpp"
#include "Log.hpp"
#include "../Timer.hpp"


#ifndef ANIMATIONMANAGER_HPP
#define ANIMATIONMANAGER_HPP

namespace Tribalia {
namespace Graphics {

class AnimationManager {
private:
    /* The animation list */
    std::list<AnimationData*> _animations;

public:
    void AddAnimation(AnimationData* a);
    void RemoveAnimation(AnimationData* a);

    /* Advance the frame of every single animation here added. */
    void Iterate();

    static AnimationManager* GetInstance() {
        static AnimationManager* am = nullptr;
        if (!am)    am = new AnimationManager;
        return am;
    }

};


}
}



#endif /* end of include guard: ANIMATIONMANAGER_HPP */
