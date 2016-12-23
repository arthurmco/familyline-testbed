#include "Camera.hpp"

using namespace Tribalia::Graphics;

Camera::Camera(glm::vec3 pos, float aspectRatio, glm::vec3 lookAt)
{
    this->_pos = pos;
    this->_posOriginal = pos;
    this->_lookAt = lookAt;
    this->_lookAtOriginal = lookAt;

    this->_fov = glm::radians(60.0f);
    this->_aspectRatio = aspectRatio;
    this->_distance = 100.0f;

    _isViewChanged = true;
    _isProjectionChanged = true;

    _original_distance = lookAt - pos;

    Log::GetLog()->Write("Created camera at (%.2f, %.2f, %.2f) "
        "looking at (%.2f, %.2f, %.2f)",
        pos.x, pos.y, pos.z, lookAt.x, lookAt.y, lookAt.z);


    CalculateVectors();
}

glm::vec3 Camera::GetPosition() const { return this->_pos; }
void Camera::SetPosition(glm::vec3 pos) { 
    this->_pos = pos;
    this->_posOriginal = pos;
    _rotation = 0;
     _isViewChanged = true;
}

void Camera::AddPosition(glm::vec3 pos) { 
    this->_pos += pos;
    this->_posOriginal += pos;
     _isViewChanged = true;
}

glm::vec3 Camera::GetLookAt() const { return this->_lookAt; }
void Camera::SetLookAt(glm::vec3 pos) { 
    /* Reset all rotations here */
    this->_lookAt = pos; 
    this->_lookAtOriginal = pos;
     _isViewChanged = true;
     _rotation = 0;    
}

void Camera::AddLookAt(glm::vec3 pos) { 
    this->_lookAt += pos; 
    this->_lookAtOriginal += pos;
    _isViewChanged = true;
}

float Camera::GetRotation() const { return _rotation; }

/*  Set position and 'look-at' at the same time,
    giving the impression the camera is 'translating' */
void Camera::AddMovement(glm::vec3 pos)
{
    this->AddPosition(pos);
    this->AddLookAt(pos);

}

void Camera::CalculateVectors()
{

    glm::vec3 pivot = (_lookAt - _pos);
    glm::vec3 front = glm::normalize(pivot);
    printf("front: %.2f %.2f, %.2f\n", front.x, front.y, front.z);
    _right = glm::normalize(glm::cross(front, glm::vec3(0,1,0)));

    printf("right: %.2f %.2f, %.2f\n", _right.x, _right.y, _right.z);
    _up = glm::vec3(0,1,0);// glm::cross(_right, front);
    printf("up: %.2f %.2f %.2f\n", _up.x, _up.y, _up.z);
}


/*  Add rotation to the camera.
    You can rotate the camera by changing the look-at value in a
    'circular way'. I will use the glm rotation functions */
void Camera::AddRotation(glm::vec3 axis, float angle)
{

    glm::vec3 l = this->_lookAt;
    
    _rotation += angle;
    constexpr float max_angle = glm::radians(360.0f);

    float dist = glm::length(glm::vec3(_pos.x - _lookAtOriginal.x, 
         _lookAtOriginal.y, _pos.z - _lookAtOriginal.z)); 
    float vx = sin(_rotation)*dist;
    float vz = cos(_rotation)*dist;

    _lookAt = glm::vec3(_pos.x-vx, _lookAtOriginal.y, _pos.z-vz);



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


/*  Get the cursor position and return a ray to the scene in
    world space */
glm::vec3 Camera::Project(int mouse_x, int mouse_y, int screenw, int screenh) const
{
    /* Normalize mouse coordinates */
    float nx = (2.0f * mouse_x) / screenw - 1.0f;
    float ny = 1.0f - (2.0f * mouse_y) / screenh;
    float nz = 1.0f;
    glm::vec3 ray_norm = glm::vec3 (nx, ny, nz);

    /* Transform into homogeneous coordinates and then into eye coordinates */
    glm::vec4 ray_clip = glm::vec4(ray_norm.x, ray_norm.y, 1.0f, 0.0f);
    glm::vec4 ray_eye = glm::inverse(_projMatrix) * ray_clip;
    ray_eye.z = -1.0;   ray_eye.w = 0.0;
    glm::vec4 world_eye = glm::inverse(_viewMatrix) * ray_eye;

    return glm::normalize(glm::vec3(world_eye.x, world_eye.y, world_eye.z));
}
