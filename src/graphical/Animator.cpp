#include "Animator.hpp"
#include <algorithm>

using namespace familyline::graphics;

std::vector<Animator*> Animator::animators;

Animator::Animator()
{
    animators.push_back(this);
}

Animator::~Animator()
{
    animators.erase(std::find(animators.begin(), animators.end(), this));
}

void Animator::runAllAnimations(int ms)
{
    for (auto a : animators) {
	a->advance(ms);
    }
    
}
