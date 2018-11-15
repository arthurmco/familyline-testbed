/***
    Light data

    Copyright (C) 2016 Arthur Mendes.

***/

#include <glm/glm.hpp>

#include "SceneObject.hpp"

#ifndef LIGHT_HPP
#define LIGHT_HPP

namespace familyline::graphics {

    class Light : public SceneObject {
    private:
        /* Light color */
        glm::vec3 _lightColor;

        /* Light strength */
        float _lightStrength;

    public:
        void SetColor(int r, int g, int b);
        void GetColor(int& r, int& g, int& b);

        void SetStrength(float);
        float GetStrength();

        Light(const char* name, glm::vec3 pos,
            int r, int g, int b, float strength);
    };

} /* familyline::graphics */



#endif /* end of include guard: LIGHT_HPP */
