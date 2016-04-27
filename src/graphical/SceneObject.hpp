/***
    Simplest representation of an object inside the scene.

    Copyright (C) 2016 Arthur M.

***/

#include <glm/glm.hpp>
#include <string>

#include "BoundingBox.hpp"

#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

namespace Tribalia {
namespace Graphics {

    class SceneObject
    {
        friend class SceneManager;
    protected:
        std::string _name;
        int _id = 0;

        glm::vec3 _position;
        float _rotations[3]; /*  Rotation angles in X, Y
                                and Z axis respectively, in radians */
        BoundingBox _box;    /* Object box, might represent size. */
    public:

        SceneObject(const char* name, glm::vec3 pos,
                float rotX, float rotY, float rotZ);

        void SetBoundingBox(BoundingBox);
        BoundingBox GetBoundingBox() const;

        void SetName(const char*);
        const char* GetName() const;

        int GetID();

        glm::vec3 GetPosition() const;
        void SetPosition(glm::vec3);
        void AddPosition(glm::vec3);

        void GetRotation(float* x, float* y, float* z) const;
        void SetRotation(float x, float y, float z);
        void AddRotation(float x, float y, float z);


    };

}
}



#endif /* end of include guard: SCENEOBJECT_HPP */
