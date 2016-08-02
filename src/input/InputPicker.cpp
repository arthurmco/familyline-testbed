#include "InputPicker.hpp"

using namespace Tribalia::Input;
using namespace Tribalia::Graphics;
using namespace Tribalia::Logic;

InputPicker::InputPicker(TerrainRenderer* terrain, Renderer* renderer, SceneManager* sm, Camera* cam,
	ObjectManager* om)
{
    this->_terrain = terrain;
    this->_renderer = renderer;
    this->_sm = sm;
	this->_cam = cam;
	this->_om = om;
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
	/*
	glm::vec4 cur_world = glm::inverse(_cam->GetViewMatrix()) * this->GetCursorEyeRay();
	glm::vec3 cur_world3 = glm::vec3(cur_world.x, cur_world.y, cur_world.z);
	return glm::normalize(cur_world3);
	*/
	int x, y, w, h;
	_renderer->GetWindowSize(w, h);
	Cursor::GetInstance()->GetPositions(x, y);


	glm::vec3 cStart = glm::unProject(glm::vec3(x, h - y, 0), _cam->GetViewMatrix(), _cam->GetProjectionMatrix(), glm::vec4(0, 0, w, h));
	glm::vec3 cEnd = glm::unProject(glm::vec3(x, h - y, 1), _cam->GetViewMatrix(), _cam->GetProjectionMatrix(), glm::vec4(0, 0, w, h));
	glm::vec3 cur_world = glm::normalize(cEnd - cStart);
	return cur_world;
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
	
	glm::vec3 cur_world = this->GetCursorWorldRay();
	
	/*printf("\ncamera_pos: %.4f %.4f %.4f, ray_pos: %.4f %.4f %.4f\n",
		_cam->GetPosition().x, _cam->GetPosition().y, _cam->GetPosition().z,
		cur_world.x, cur_world.y, cur_world.z);
*/
	float prolong_near = 0.1f, prolong_far = 128.0f;
	float prolong_now = prolong_near + ((prolong_far - prolong_near ) / 2.0f);
		
	glm::vec3 pHalf;
	glm::vec3 pNear = _cam->GetPosition() + (cur_world * prolong_near);
	glm::vec3 pFar = _cam->GetPosition() + (cur_world * prolong_far);

	/*printf("near: %.3f %.3f %.3f, far: %.3f %.3f %.3f, prolongs: { ",
		pNear.x, pNear.y, pNear.z, pFar.x, pFar.y, pFar.z);*/
	for (int i = 0; i < MAX_PICK_ITERATIONS; i++) {


		/*	Here, we'll check if the ray projection is above or below the terrain 
			
			If we're above, we'll adjust pNear to that point
			If we're below, we'll adjust pFar to that point

			To check that, we simply check if pFar is under and
			pNear and pHalf are above 
		*/


		if (this->CheckIfTerrainIntersect(cur_world, prolong_near, prolong_now))
			prolong_far = prolong_now;
		else 
			prolong_near = prolong_now;
			

		prolong_now = prolong_near + ((prolong_far - prolong_near) / 2.0f);
		pHalf = _cam->GetPosition() + (cur_world * prolong_now);
		//printf("%.2f (%.2f %.2f %.2f), ", prolong_now, pHalf.x, pHalf.y, pHalf.z);

	}

	glm::vec3 collide = _terrain->GraphicalToGameSpace(pHalf);
	//printf(" }\nprol: %.2f, pos: %.3f %.3f %.3f, gamespace: %.3f %.3f %.3f\n\n", 
	//	1.0f, pHalf.x, pHalf.y, pHalf.z, collide.x, collide.y, collide.z);
	
	return collide;
}


/*	Get the object that were intersected by the cursor ray */
LocatableObject* InputPicker::GetIntersectedObject()
{
	glm::vec3 direction = this->GetCursorWorldRay();

	glm::vec3 origin = _cam->GetPosition();

	for (auto it = _om->GetObjectList()->begin(); it != _om->GetObjectList()->end(); it++) {
		LocatableObject* loc = dynamic_cast<LocatableObject*>(it->obj);

		if (loc) {
			BoundingBox bb = loc->GetMesh()->GetBoundingBox();
			glm::vec4 vmin = glm::vec4(bb.minX, bb.minY, bb.minZ, 1);
			glm::vec4 vmax = glm::vec4(bb.maxX, bb.maxY, bb.maxZ, 1);

			vmin = loc->GetMesh()->GetModelMatrix() * vmin;
			vmax = loc->GetMesh()->GetModelMatrix() * vmax;

			glm::min(vmin.y, 0.0f);
			glm::max(vmax.y, 0.0f);

			glm::vec3 planePosX, planePosY, planePosZ;

			float tmin = -100000;
			float tmax = 100000;
			
			float dxmin, dxMax;
//			printf("\n%s\n", loc->GetName());
			if (direction.x != 0) {
				dxmin = (vmin.x - origin.x) / direction.x;
				dxMax = (vmax.x - origin.x) / direction.x;

				tmin = fmaxf(tmin, fminf(dxmin, dxMax));
				tmax = fminf(tmax, fmaxf(dxmin, dxMax));
	//			printf("x: %.4f %.4f \t", dxmin, dxMax);
				if (tmax < tmin) continue;
			} 

			if (direction.y != 0) {
				dxmin = (vmin.y - origin.y) / direction.y;
				dxMax = (vmax.y - origin.y) / direction.y;

				tmin = fmaxf(tmin, fminf(dxmin, dxMax));
				tmax = fminf(tmax, fmaxf(dxmin, dxMax));
		//		printf("y: %.4f %.4f \t", dxmin, dxMax);
				if (tmax < tmin) continue;

			}

			if (direction.z != 0) {
				dxmin = (vmin.z - origin.z) / direction.z;
				dxMax = (vmax.z - origin.z) / direction.z;

				tmin = fmaxf(tmin, fminf(dxmin, dxMax));
				tmax = fminf(tmax, fmaxf(dxmin, dxMax));
			//	printf("z: %.4f %.4f \t", dxmin, dxMax);
				if (tmax < tmin) continue;

			}

			//printf("total: %.4f %.4f\n", tmin, tmax);

			/* Ray misses */
			if (tmin < 0) {
				continue;
			}

			/* Collided with both 3 axis! */
			if (tmax >= tmin)
				return loc;		

			
		}
	}

	return nullptr;
}

