/***
    Representation of a camera

    Copyright (C) 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include "Log.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

#ifndef M_PI
#define M_PI 3.1415
#endif

namespace Tribalia {
namespace Graphics {

    class Camera
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
        float _rotation = M_PI/4;

	float _zoom;
	float _posystart;
	float _lookdelta;

        glm::mat4 _viewMatrix, _projMatrix;

        void CalculateVectors();

    public:
        Camera(glm::vec3 pos, float aspectRatio, glm::vec3 lookAt);

        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3);
        void AddPosition(glm::vec3);

        glm::vec3 GetLookAt() const;
        void SetLookAt(glm::vec3);
        void AddLookAt(glm::vec3);

        /*  Set position and 'look-at' at the same time,
            giving the impression the camera is 'translating' */
        void AddMovement(glm::vec3);

        float GetRotation() const;

        /*  Add rotation to the camera.
            You can rotate the camera by changing the look-at value in a
            'circular way'. I will use the glm rotation functions */
        void AddRotation(glm::vec3 axis, float angle);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix();

	float GetZoomLevel() const;
	void SetZoomLevel(float);
	
        /*  Get the cursor position and return a ray to the scene in
            world space */
        glm::vec3 Project(int mouse_x, int mouse_y, int screenw, int screenh) const;

    };

}
}



#endif /* end of include guard: CAMERA_HPP */
