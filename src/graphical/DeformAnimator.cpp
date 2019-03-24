#include "DeformAnimator.hpp"

using namespace familyline::graphics;

DeformAnimator::DeformAnimator(
	std::map<std::string /*animation-name*/, std::vector<VertexDataGroup>> animation_frames,
	int framerate)
	: _animation_frames(animation_frames), _framerate(framerate)
{}

void DeformAnimator::advance(int ms)
{
	const double frametime = 1000.0 / _framerate;
	_frameptr += (ms / frametime);

	if (_frameptr > _animation_frames.size())
		_frameptr = (size_t(_frameptr) % _animation_frames.size());
}
void DeformAnimator::runAnimation(const char* name) {
	// TODO: check if 'name' exists
	_animation_name = name;
	_frameptr = 0.0;
}

VertexDataGroup DeformAnimator::getCurrentFrame() {
	// TODO: interpolate
	auto& avector = _animation_frames[_animation_name];
	printf("[[ %.2f %d ]] \n", _frameptr, int(_frameptr));


	auto currptr = unsigned(_frameptr);
	auto nextptr = std::min(currptr + 1, unsigned(avector.size()));

	printf("[[ %d %d %d ]]", currptr, nextptr, int(_frameptr));

	/* No frame after here? Return the last one */
	if (nextptr >= currptr+1)
		return avector[int(_frameptr)];

	auto vdcurrent = avector[int(_frameptr)];
	auto vdnext = avector[int(_frameptr)+1];
	auto framemix = double(_frameptr - currptr);

	auto vdret = vdcurrent;

	printf("[[ %.2f ]]", framemix);

	for (unsigned vidx = 0; vidx < vdret.size(); vidx++) {

		for (unsigned i = 0; i < vdret[vidx].position.size(); i++) {
			vdret[vidx].position[i] = glm::mix(
				vdcurrent[vidx].position[i],
				vdnext[vidx].position[i],
				framemix);

			// Support texcoord animation?
			// We could enable some nice effects. (like 2d sprite animation, if we need)
		}
	}

	return vdret;
}
