#include "StaticAnimator.hpp"

using namespace familyline::graphics;

StaticAnimator::StaticAnimator(VertexDataGroup vdg)
    : _vdg(vdg)
{}

void StaticAnimator::advance(int ms) {}
void StaticAnimator::runAnimation(const char* name) {}

VertexDataGroup StaticAnimator::getCurrentFrame() {
    return _vdg;
}
