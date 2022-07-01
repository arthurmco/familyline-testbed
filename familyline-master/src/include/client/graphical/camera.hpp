#pragma once

#include <cmath>
#include <common/logic/icamera.hpp>
#include <glm/glm.hpp>

#ifndef M_PI
#define M_PI 3.1415f
#endif

namespace familyline::graphics
{
class Camera : public familyline::logic::ICamera
{
private:
    glm::vec3 _pos;
    glm::vec3 _posOriginal;
    glm::vec3 _lookAt;
    glm::vec3 _lookAtOriginal;

    float _fov;
    float _aspectRatio;
    float _distance;

    bool _isViewChanged;
    bool _isProjectionChanged;

    /* Original distance between look-at and positon */
    glm::vec3 _original_distance;

    /* Up and right vectors */
    glm::vec3 _up;
    glm::vec3 _right;

    /* Camera default rotation, in radians */
    float _rotation = M_PI / 4;

    float _zoom;
    float _posystart;
    float _lookdelta;

    glm::mat4 _viewMatrix, _projMatrix;

    void CalculateVectors();

public:
    Camera(glm::vec3 pos, float aspectRatio, glm::vec3 lookAt);

    virtual glm::vec3 GetPosition() const;
    virtual void SetPosition(glm::vec3);
    virtual void AddPosition(glm::vec3);

    virtual glm::vec3 GetLookAt() const;
    virtual void SetLookAt(glm::vec3);
    virtual void AddLookAt(glm::vec3);

    /*  Set position and 'look-at' at the same time,
        giving the impression the camera is 'translating' */
    void AddMovement(glm::vec3);

    float GetRotation() const;

    /*  Add rotation to the camera.
        You can rotate the camera by changing the look-at value in a
        'circular way'. I will use the glm rotation functions */
    virtual void AddRotation(glm::vec3 axis, float angle);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

    float GetZoomLevel() const;
    void SetZoomLevel(float);

    virtual void AddZoomLevel(float v) { this->SetZoomLevel(_zoom + v); }

    /*  Get the cursor position and return a ray to the scene in
        world space */
    glm::vec3 Project(int mouse_x, int mouse_y, int screenw, int screenh) const;

    virtual ~Camera() {}
};
}  // namespace familyline::graphics
