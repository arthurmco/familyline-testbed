/***
    Representation of a camera

    Copyright (C) 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Log.hpp"

#ifndef CAMERA_HPP
#define CAMERA_HPP

namespace Tribalia {
namespace Graphics {

    class Camera
    {
    private:
        glm::vec3 _pos;
        glm::vec3 _lookAt;
        float _fov;
        float _aspectRatio;
        float _distance;

        bool _isViewChanged;
        bool _isProjectionChanged;

        /* Original distance between look-at and positon */
        glm::vec3 _original_distance;

        glm::mat4 _viewMatrix, _projMatrix;
    public:
        Camera(glm::vec3 pos, glm::vec3 lookAt);

        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3);
        void AddPosition(glm::vec3);

        glm::vec3 GetLookAt() const;
        void SetLookAt(glm::vec3);
        void AddLookAt(glm::vec3);

        /*  Set position and 'look-at' at the same time,
            giving the impression the camera is 'translating' */
        void AddMovement(glm::vec3);


        /*  Add rotation to the camera.
            You can rotate the camera by changing the look-at value in a
            'circular way'. I will use the glm rotation functions */
        void AddRotation(glm::vec3 axis, float angle);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix();

    };

}
}



#endif /* end of include guard: CAMERA_HPP */
