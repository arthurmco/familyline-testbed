/***
    Representation of a camera

    Copyright (C) 2016 Arthur M.

***/

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
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
        void Translate(glm::vec3);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjectionMatrix();

    };

}
}



#endif /* end of include guard: CAMERA_HPP */
