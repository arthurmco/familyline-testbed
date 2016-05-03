#include "Camera.hpp"

using namespace Tribalia::Graphics;

Camera::Camera(glm::vec3 pos, glm::vec3 lookAt)
{
    this->_pos = pos;
    this->_lookAt = lookAt;

    this->_fov = glm::radians(45.0f);
    this->_aspectRatio = 4.0f/3.0f;
    this->_distance = 50.0f;

    _isViewChanged = true;
    _isProjectionChanged = true;

    _original_distance = lookAt - pos;

    Log::GetLog()->Write("Created camera at (%.2f, %.2f, %.2f) "
        "looking at (%.2f, %.2f, %.2f)",
        pos.x, pos.y, pos.z, lookAt.x, lookAt.y, lookAt.z);

}

glm::vec3 Camera::GetPosition() const { return this->_pos; }
void Camera::SetPosition(glm::vec3 pos) { this->_pos = pos; _isViewChanged = true;}
void Camera::AddPosition(glm::vec3 pos) { this->_pos += pos; _isViewChanged = true;}

glm::vec3 Camera::GetLookAt() const { return this->_lookAt; }
void Camera::SetLookAt(glm::vec3 pos) { this->_lookAt = pos; _isViewChanged = true;}
void Camera::AddLookAt(glm::vec3 pos) { this->_lookAt += pos; _isViewChanged = true;}

/*  Set position and 'look-at' at the same time,
    giving the impression the camera is 'translating' */
void Camera::AddMovement(glm::vec3 pos)
{
    this->AddPosition(pos);
    this->AddLookAt(pos);
}


/*  Add rotation to the camera.
    You can rotate the camera by changing the look-at value in a
    'circular way'. I will use the glm rotation functions */
void Camera::AddRotation(glm::vec3 axis, float angle)
{
    glm::vec3 l = this->_lookAt;
    glm::vec3 pivot = (_lookAt - _pos);
    glm::mat4 tRotate = glm::rotate(angle, axis);
    glm::mat4 tPivot = glm::translate(pivot);
    glm::mat4 tPivotMinus = glm::translate(-pivot);
    glm::vec4 l4 = tPivotMinus * tRotate * tPivot *
        glm::vec4(l.x, l.y, l.z, 1.0f);

    l = glm::vec3(l4.x, l4.y, l4.z);

    printf("\n%.2f %.2f %.2f\n", l.x, l.y, l.z);
    this->_lookAt = l;
    this->_isViewChanged = true;
}


glm::mat4 Camera::GetViewMatrix()
{
    if (_isViewChanged) {
        this->_viewMatrix = glm::lookAt(_pos, _lookAt, glm::vec3(0,1,0));
        _isViewChanged = false;
    }

    return _viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
    if (_isProjectionChanged) {
        this->_projMatrix = glm::perspective(_fov, _aspectRatio, 0.1f, _distance);
        _isProjectionChanged = false;
    }

    return _projMatrix;
}
