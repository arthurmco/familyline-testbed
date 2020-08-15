#include <algorithm>
#include <client/graphical/deform_animator.hpp>

using namespace familyline::graphics;

DeformAnimator::DeformAnimator(
    std::map<std::string /*animation-name*/, std::vector<VertexDataGroup>> animation_frames,
    int framerate)
    : _animation_frames(animation_frames), _framerate(framerate)
{
}

void DeformAnimator::advance(int ms)
{
    const double frametime = 1000.0 / _framerate;

    auto& avector = _animation_frames[_animation_name];
    auto maxframe = unsigned(avector.size()) - 1;
    if (_frameptr >= maxframe) {
        // Cannot advance anymore.
        return;
    }

    this->dirtyFrame = true;
    _frameptr += (ms / frametime);
}
void DeformAnimator::runAnimation(const char* name)
{
    // TODO: check if 'name' exists
    _animation_name = name;
    _frameptr       = 0.0;
}

VertexDataGroup DeformAnimator::getCurrentFrame()
{
    // TODO: interpolate
    auto& avector = _animation_frames[_animation_name];

    auto currptr = unsigned(_frameptr);
    auto nextptr = std::min(currptr + 1, unsigned(avector.size()) - 1);

    /* No frame after here? Return the last one */
    if (nextptr >= _frameptr + 1) return avector[int(_frameptr)];

    auto vdcurrent = avector[int(_frameptr)];
    auto vdnext    = avector[int(nextptr)];
    auto framemix  = double(_frameptr - currptr);

    auto vdret = vdcurrent;

    for (unsigned vidx = 0; vidx < vdret.size(); vidx++) {
        for (unsigned i = 0; i < vdret[vidx].position.size(); i++) {
            vdret[vidx].position[i] =
                glm::mix(vdcurrent[vidx].position[i], vdnext[vidx].position[i], framemix);

            // Support texcoord animation?
            // We could enable some nice effects. (like 2d sprite animation, if we need)
            // Remember to change the GLVertexHandle::updateVertex too!!!
        }
    }

    this->dirtyFrame = false;
    return vdret;
}
