/***
    Light data. For things that emit light.
    Not related to anyone named Yagami

    Copyright (C) 2016, 2019 Arthur Mendes.

***/

#include <glm/glm.hpp>

#include "scene_object.hpp"

#ifndef LIGHT_HPP
#define LIGHT_HPP

namespace familyline::graphics
{
class Light
{
private:
    std::string _name;
    glm::vec3 _position;

    /* Light color */
    glm::vec3 _lightColor;

    /* Light strength */
    float _lightStrength;

public:
    void setColor(int r, int g, int b);
    void getColor(int& r, int& g, int& b);

    void setStrength(float);
    float getStrength();

    virtual std::string_view getName();
    virtual glm::vec3 getPosition() const;
    virtual void setPosition(glm::vec3 pos);

    Light(const char* name, glm::vec3 pos, int r, int g, int b, float strength);
};

}  // namespace familyline::graphics

#endif /* end of include guard: LIGHT_HPP */
