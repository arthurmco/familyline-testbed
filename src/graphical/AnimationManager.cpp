#include "AnimationManager.hpp"

using namespace Tribalia::Graphics;

static void RunAnimation(void* am, double delta, void* animation_data);

void AnimationManager::AddAnimation(AnimationData* a)
{
    Log::GetLog()->Write("Added animation @%p", a);
    _animations.push_back(a);
    Timer::getInstance()->AddFunctionCall(30, &RunAnimation, (void*)this, (void*)a);
}
void AnimationManager::RemoveAnimation(AnimationData* a)
{
    Log::GetLog()->Write("Removed animation @%p", a);
    _animations.remove(a);  // it's a pointer, should be fine
}

static void RunAnimation(void* am, double delta, void* animation_data)
{
    AnimationData* ad = (AnimationData*) animation_data;
    ad->NextFrame();
}

/* Advance the frame of every single animation here added. */
void AnimationManager::Iterate()
{
    /* for (auto& a : _animations) {
        a->NextFrame();   
    } */
}
