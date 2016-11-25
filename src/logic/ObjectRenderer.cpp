#include "ObjectRenderer.hpp"

using namespace Tribalia::Logic;


ObjectRenderer::ObjectRenderer(ObjectManager* om, Tribalia::Graphics::SceneManager* sm)
    : _om(om), _sm(sm)
{

}

/*	Check for new objects, add them to the list
	Return true if we have new objects, false if we haven't */
bool ObjectRenderer::Check()
{
    int object_found = 0;
    for (auto it = _om->_objects.begin(); it != _om->_objects.end(); ++it) {

        bool exists = false;
        /* Check if ID already exists.
            If yes, then the object is already added */
        for (auto id_it = _IDs.begin(); id_it != _IDs.end(); ++id_it) {
            if (*id_it == it->oid) {
                exists = true;
                break;
            }
        }

        if (exists) continue;

        /* easy way to check if we have a locatable object
            I don't know if is fast */
        LocatableObject* loc = dynamic_cast<LocatableObject*>(it->obj);
        if (loc) {

            /* check if mesh is valid */
            if (!loc->GetMesh()) continue;

            object_found++;
            _objects.emplace_back(loc);
            _sm->AddObject(loc->GetMesh());
            _IDs.push_back(it->oid);
        }
    }

    if (object_found > 0) {
        Log::GetLog()->Write("Object renderer updated. %d objects found",
            object_found);
    }

	return (object_found == 0);

}

/* Update object meshes */
void ObjectRenderer::Update()
{
    for (auto it = _objects.begin(); it != _objects.end(); it++) {
        double x, y, z;
        x = (*it)->GetX() * SEC_SIZE;
        y = 0; //change this
        z = (*it)->GetZ() * SEC_SIZE;

        (*it)->GetMesh()->SetPosition(glm::vec3(x,y,z));
        (*it)->GetMesh()->ApplyTransformations();

        /*
        printf("\tobject %s id %d is at %.3f %.3f %.3f\n",
            (*it)->GetName(), (*it)->GetObjectID(),
            (*it)->GetMesh()->GetPosition().x,
            (*it)->GetMesh()->GetPosition().y,
            (*it)->GetMesh()->GetPosition().z);
        */
    }
}

/* Check if the world-space ray collides with any rendered object.
    Returns the object, or nullptr if any.
    Also return the collided world-space coords on world_pos vec3. */
LocatableObject* ObjectRenderer::CheckRayCollide(glm::vec3 eye_ray,
        glm::vec3* world_pos)
{
    glm::vec3 camLook = this->_sm->GetCamera()->GetLookAt();
    glm::vec3 camPos = this->_sm->GetCamera()->GetPosition();

    /* Check distance */
    float distance = glm::distance(camPos, camLook);
    glm::vec3 bbnormal = glm::vec3(0,1,0);

    glm::vec3 eye_prolong = (eye_ray * bbnormal);

    if (glm::length(eye_prolong) == 0.0f) {
        return nullptr; //non-collidable, perpendicular, infinity distance.
    }

    float raydist = ((camPos * bbnormal + distance) / eye_prolong).y;

    if (raydist == 0 || raydist == INFINITY)
        return nullptr; //intersection before 0

    glm::vec3 reachpoint = camPos + (eye_ray * raydist);

    printf("dist: %.2f %.2f %.2f \n", reachpoint.x, reachpoint.y, reachpoint.z);

    if (world_pos)
        *world_pos = reachpoint;

    return nullptr;
}


/* Check the terrain position that the cursor is projected at, in
    OpenGL coordinates */
glm::vec3 ObjectRenderer::CheckTerrainPositions(glm::vec2 positions, int winwidth, int winheight)
{
    positions.y = (winheight - positions.y);

    GLfloat depth = 0;

    glm::vec3 pos_near = glm::vec3(positions.x, positions.y, 0.0f);
    glm::vec3 pos_far = glm::vec3(positions.x, positions.y, 1.0f);

    glm::vec3 unproj_near = glm::unProject(pos_near,
        this->_sm->GetCamera()->GetViewMatrix(),
        this->_sm->GetCamera()->GetProjectionMatrix(),
        glm::vec4(0,0,winwidth,winheight));

    glm::vec3 unproj_far = glm::unProject(pos_far,
        this->_sm->GetCamera()->GetViewMatrix(),
        this->_sm->GetCamera()->GetProjectionMatrix(),
        glm::vec4(0,0,winwidth,winheight));

    // printf("> %.2f %.2f %.2f -> %.2f %.2f %.2f ",
    //     unproj_near.x, unproj_near.y, unproj_near.z,
    //     unproj_far.x, unproj_far.y, unproj_far.z);

    glm::vec3 unproj_direction = glm::normalize(unproj_far - unproj_near);

    printf("%.2f %.2f %.2f ", unproj_direction.x, unproj_direction.y, unproj_direction.z);

    glm::vec3 cameraLook = this->_sm->GetCamera()->GetLookAt();
    float terrain_dist = glm::distance(unproj_near, cameraLook);

    glm::vec3 terrain_pos = unproj_near + (terrain_dist * unproj_direction);

    return terrain_pos;
}
