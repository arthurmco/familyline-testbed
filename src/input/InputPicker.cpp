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
	int x, y, w, h;
	_renderer->GetWindowSize(w, h);
	Cursor::GetInstance()->GetPositions(x, y);

	glm::vec3 cur_world = this->GetCursorWorldRay();
	
	float prolong_near = 0.1f, prolong_far = 100.0f;
	float prolong_now = prolong_near + ((prolong_far - prolong_near ) / 2.0f);
		
	glm::vec3 pNear = _cam->GetPosition() + (cur_world * prolong_near);
	glm::vec3 pFar = _cam->GetPosition() + (cur_world * prolong_far);
	glm::vec3 pHalf = _cam->GetPosition() + (cur_world * prolong_now);
	
	for (int i = 0; i < MAX_PICK_ITERATIONS; i++) {
		
		/*	Here, we'll check if the ray projection is above or below the terrain 
			
			If we're above, we'll adjust pNear to that point
			If we're below, we'll adjust pFar to that point

			To check that, we simply check if pFar is under and
			pNear and pHalf are above 
		*/

		if (pFar.y < 0.0f && pNear.y > 0.0f && pHalf.y > 0.0f) {
			prolong_near = prolong_now;
		}
		else if (pFar.y < 0.0f && pHalf.y <= 0.0f && pNear.y >= 0.0f) {
			prolong_far = prolong_now;
		} 

		pNear = _cam->GetPosition() + (cur_world * prolong_near);
		pFar = _cam->GetPosition() + (cur_world * prolong_far);
		pHalf = _cam->GetPosition() + (cur_world * prolong_now);

		prolong_now = prolong_near + ((prolong_far - prolong_near) / 2.0f);
	}


	glm::vec3 collide = _terrain->GraphicalToGameSpace(pHalf);
	
	return collide;
}