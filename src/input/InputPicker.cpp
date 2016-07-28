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
	return glm::vec4(cur_eye.x, cur_eye.y, -1.0f, 0.0f);
}

/* Get cursor ray in world space */
glm::vec3 InputPicker::GetCursorWorldRay()
{
	glm::vec4 cur_world = glm::inverse(_cam->GetViewMatrix()) * this->GetCursorEyeRay();
	glm::vec3 cur_world3 = glm::vec3(cur_world.x, cur_world.y, cur_world.z);
	return glm::normalize(cur_world3);
}

/* Check if we intersect with the terrain between between start and end */
bool InputPicker::CheckIfTerrainIntersect(glm::vec3 ray, float start, float end) {
	glm::vec3 pStart = _cam->GetPosition() + (ray * start);
	glm::vec3 pEnd = _cam->GetPosition() + (ray * end);

	if (pStart.y >= 0 && pEnd.y < 0) {
		return true;
	}

	return false;
}


/*	Get position where the cursor collides with the
terrain, in render coordinates */
glm::vec3 InputPicker::GetTerrainProjectedPosition()
{
	int x, y, w, h;
	_renderer->GetWindowSize(w, h);
	Cursor::GetInstance()->GetPositions(x, y);

	/*
	glm::vec3 cur_world = this->GetCursorWorldRay();
	//cur_world += (glm::normalize(_cam->GetLookAt() - _cam->GetPosition()));
	cur_world = glm::normalize(cur_world);
	
	printf("\ncamera_pos: %.4f %.4f %.4f, ray_pos: %.4f %.4f %.4f\n",
		_cam->GetPosition().x, _cam->GetPosition().y, _cam->GetPosition().z,
		cur_world.x, cur_world.y, cur_world.z);

	float prolong_near = 0.1f, prolong_far = 128.0f;
	float prolong_now = prolong_near + ((prolong_far - prolong_near ) / 2.0f);
		
	glm::vec3 pHalf;
	glm::vec3 pNear = _cam->GetPosition() + (cur_world * prolong_near);
	glm::vec3 pFar = _cam->GetPosition() + (cur_world * prolong_far);

	printf("near: %.3f %.3f %.3f, far: %.3f %.3f %.3f, prolongs: { ",
		pNear.x, pNear.y, pNear.z, pFar.x, pFar.y, pFar.z);
	for (int i = 0; i < MAX_PICK_ITERATIONS; i++) {


		/*	Here, we'll check if the ray projection is above or below the terrain 
			
			If we're above, we'll adjust pNear to that point
			If we're below, we'll adjust pFar to that point

			To check that, we simply check if pFar is under and
			pNear and pHalf are above 
		*//*


		if (this->CheckIfTerrainIntersect(cur_world, prolong_near, prolong_now))
			prolong_far = prolong_now;
		else 
			prolong_near = prolong_now;
			

		prolong_now = prolong_near + ((prolong_far - prolong_near) / 2.0f);
		pHalf = _cam->GetPosition() + (cur_world * prolong_now);
		printf("%.2f (%.2f %.2f %.2f), ", prolong_now, pHalf.x, pHalf.y, pHalf.z);

	}
	*/

	glm::vec3 gPos = glm::vec3(x, h-y, 1);
	glReadPixels(x, h-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &gPos.z);
	glm::vec3 pHalf = glm::unProject(gPos, _cam->GetViewMatrix(), _cam->GetProjectionMatrix(), glm::vec4(0, 0, w, h));
	

	glm::vec3 collide = _terrain->GraphicalToGameSpace(pHalf);
	printf(" }\nprol: %.2f, pos: %.3f %.3f %.3f, gamespace: %.3f %.3f %.3f\n\n", 
		1.0f, pHalf.x, pHalf.y, pHalf.z, collide.x, collide.y, collide.z);
	
	return collide;
}