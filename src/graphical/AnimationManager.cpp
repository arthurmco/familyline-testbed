#include "AnimationManager.hpp"

using namespace Tribalia::Graphics;

void AnimationManager::AddAnimation(AnimationData* a)
{
    Log::GetLog()->Write("Added animation @%p", a);
    _animations.push_back(a);
}
void AnimationManager::RemoveAnimation(AnimationData* a)
{
    Log::GetLog()->Write("Removed animation @%p", a);
    _animations.remove(a);  // it's a pointer, should be fine
}

/* Advance the frame of every single animation here added. */
void AnimationManager::Iterate()
{
    for (auto& a : _animations) {
        a->NextFrame();
        
    }
}
