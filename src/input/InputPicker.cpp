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
			glm::vec3 planePosX, planePosY, planePosZ;

			planePosX = (direction.x > 0) ? bb.points[BOUNDING_BOX_FACE_LEFT] : bb.points[BOUNDING_BOX_FACE_RIGHT];
			planePosY = (direction.y > 0) ? bb.points[BOUNDING_BOX_FACE_BOTTOM] : bb.points[BOUNDING_BOX_FACE_TOP];
			planePosZ = (direction.z > 0) ? bb.points[BOUNDING_BOX_FACE_FRONT] : bb.points[BOUNDING_BOX_FACE_BACK];
			
			glm::vec4 planePosX4 = loc->GetMesh()->GetModelMatrix() * glm::vec4(planePosX.x, planePosX.y, planePosX.z, 1.0);
			glm::vec4 planePosY4 = loc->GetMesh()->GetModelMatrix() * glm::vec4(planePosY.x, planePosY.y, planePosY.z, 1.0);
			glm::vec4 planePosZ4 = loc->GetMesh()->GetModelMatrix() * glm::vec4(planePosZ.x, planePosZ.y, planePosZ.z, 1.0);

			planePosX = glm::vec3(planePosX4.x, planePosX4.y, planePosX4.z);
			planePosY = glm::vec3(planePosY4.x, planePosY4.y, planePosY4.z);
			planePosZ = glm::vec3(planePosZ4.x, planePosZ4.y, planePosZ4.z);

			glm::vec4 obNormalX4 = loc->GetMesh()->GetModelMatrix() * glm::vec4((direction.x > 0) ? -1 : 1, 0, 0, 0);
			glm::vec4 obNormalY4 = loc->GetMesh()->GetModelMatrix() * glm::vec4(0, (direction.y > 0) ? -1 : 1, 0, 0);
			glm::vec4 obNormalZ4 = loc->GetMesh()->GetModelMatrix() * glm::vec4(0, 0, (direction.z > 0) ? -1 : 1, 0);

			glm::vec3 obNormalX = glm::vec3(obNormalX4.x, obNormalX4.y, obNormalX4.z);
			glm::vec3 obNormalY = glm::vec3(obNormalY4.x, obNormalY4.y, obNormalY4.z);
			glm::vec3 obNormalZ = glm::vec3(obNormalZ4.x, obNormalZ4.y, obNormalZ4.z);

			/* Test with X axis */
			float denomx = glm::dot(obNormalX, direction);
			printf("\n %s", loc->GetName());
			printf("\ndenomx: %.3f", denomx);
			if (glm::abs(denomx) < 0.001) {
				continue;
			}

			/* The rays aren't perpendicular, i.e, they may collide */
			float tx = glm::dot(loc->GetMesh()->GetPosition() - origin, obNormalX) / denomx;
			printf("\t t: %.3f\n", tx);

			if (tx < 0) {
				/* There's not a collision */
				continue;
			}

			/* Test with Y axis */
			float denomy = glm::dot(obNormalY, direction);
			printf("\ndenomy: %.3f", denomy);
			if (glm::abs(denomy) < 0.001) {
				continue;
			}

			/* The rays aren't perpendicular, i.e, they may collide */
			float ty = glm::dot(loc->GetMesh()->GetPosition() - origin, obNormalY) / denomy;
			printf("\t t: %.3f\n", ty);

			if (ty < 0) {
				/* There's not a collision */
				continue;
			}

			/* Test with Z axis */
			float denomz = glm::dot(obNormalY, direction);
			printf("\ndenomz: %.3f", denomz);
			if (glm::abs(denomz) < 0.001) {
				continue;
			}

			/* The rays aren't perpendicular, i.e, they may collide */
			float tz = glm::dot(loc->GetMesh()->GetPosition() - origin, obNormalZ) / denomz;
			printf("\t t: %.3f\n", ty);

			if (tz < 0) {
				/* There's not a collision */
				continue;
			}

			/* Collided with both 3 axis! */
			return loc;

			

			
		}
	}

	return nullptr;
}

