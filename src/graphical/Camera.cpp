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
