#include "SceneObject.hpp"

using namespace familyline::graphics;


SceneObject::SceneObject(const char* name, glm::vec3 pos,
        float rotX, float rotY, float rotZ)
        {
            this->_name = std::string{name};
            this->_position = pos;

            this->_rotations[0] = rotX;
            this->_rotations[1] = rotY;
            this->_rotations[2] = rotZ;
        }

void SceneObject::SetBoundingBox(BoundingBox b)
{
    this->_box = b;
}
BoundingBox SceneObject::GetBoundingBox() const
{
    return this->_box;
}


int SceneObject::GetType()
{
    return this->_type;
}

void SceneObject::SetName(const char* name)
{
    this->_name = std::string{name};
}

const char* SceneObject::GetName() const
{
    return this->_name.c_str();
}

glm::vec3 SceneObject::GetPosition() const
{
    return this->_position;
}
void SceneObject::SetPosition(glm::vec3 pos)
{
    this->_position = pos;
}
void SceneObject::AddPosition(glm::vec3 pos)
{
    this->_position += pos;
}

void SceneObject::GetRotation(float* x, float* y, float* z) const
{
    *x = this->_rotations[0];
    *y = this->_rotations[1];
    *z = this->_rotations[2];
}
void SceneObject::SetRotation(float x, float y, float z)
{
    this->_rotations[0] = x;
    this->_rotations[1] = y;
    this->_rotations[2] = z;
}
void SceneObject::AddRotation(float x, float y, float z)
{
    this->_rotations[0] += x;
    this->_rotations[1] += y;
    this->_rotations[2] += z;
}


int SceneObject::GetID()
{
    return this->_id;
}
