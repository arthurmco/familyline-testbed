#include "InputPicker.hpp"

using namespace Tribalia::Input;
using namespace Tribalia::Graphics;

InputPicker::InputPicker(TerrainRenderer* terrain, Renderer* renderer, SceneManager* sm, Camera* cam)
{
    this->_terrain = terrain;
    this->_renderer = renderer;
    this->_sm = sm;
	this->_cam = cam;
}

/* Get cursor ray in screen space */
glm::vec4 InputPicker::GetCursorScreenRay()
{
    int w = 0, h = 0, x = 0, y = 0;
    _renderer->GetWindowSize(w, h);
    Cursor::GetInstance()->GetPositions(x, y);

    // Create ray data.
    glm::vec4 ray = glm::vec4( (2.0f * x) / w - 1.0f,
        1.0f - (2.0f - y) / h, -1.0f, 1.0f);
    return ray;
}

/* Get cursor ray in eye space */
glm::vec4 InputPicker::GetCursorEyeRay()
{
	glm::vec4 cur_eye = glm::inverse(_cam->GetProjectionMatrix()) *
		this->GetCursorScreenRay();
	return glm::vec4(cur_eye.x, cur_eye.y, -1.0, 0.0);
}

/* Get cursor ray in world space */
glm::vec3 InputPicker::GetCursorWorldRay()
{
	glm::vec4 cur_world = glm::inverse(_cam->GetViewMatrix()) * this->GetCursorEyeRay();
	glm::vec3 cur_world3 = glm::vec3(cur_world.x, cur_world.y, cur_world.z);
	return glm::normalize(cur_world3);
}

/*	Get position where the cursor collides with the
terrain, in render coordinates */
glm::vec3 InputPicker::GetTerrainProjectedPosition()
{
	int w, h;
	_renderer->GetWindowSize(w, h);

	glm::vec3 pNear, pFar;
	glm::vec3 cur_world = this->GetCursorWorldRay();
	pNear = glm::unProject(glm::vec3(cur_world.x, cur_world.y, 0.1), _cam->GetViewMatrix() * glm::mat4(1.0f),
		_cam->GetProjectionMatrix(), glm::vec4(0, 0, w, h));
	pFar = glm::unProject(glm::vec3(cur_world.x, cur_world.y, 1.0f), _cam->GetViewMatrix() * glm::mat4(1.0f),
		_cam->GetProjectionMatrix(), glm::vec4(0, 0, w, h));
	printf("near: %.3f %.3f %.3f, far: %.3f %.3f %.3f\n",
		pNear.x, pNear.y, pNear.z, pFar.x, pFar.y, pFar.z);

	glm::vec3 gNear = _terrain->GraphicalToGameSpace(pNear);
	printf("gamespace: %.3f %.3f %.3f\n\n", gNear.x, gNear.y, gNear.z);

	return gNear;
}