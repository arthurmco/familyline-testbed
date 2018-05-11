#include "Light.hpp"

using namespace Familyline::Graphics;

void Light::SetColor(int r, int g, int b)
{
    this->_lightColor = glm::vec3(r/255.0f, g/255.0f, b/255.0f);
}

void Light::GetColor(int& r, int& g, int& b)
{
    r = _lightColor.r * 255.0f;
    g = _lightColor.g * 255.0f;
    b = _lightColor.b * 255.0f;
}

void Light::SetStrength(float s)
{
    this->_lightStrength = s;
}

float Light::GetStrength()
{
    return this->_lightStrength;
}

Light::Light(const char* name, glm::vec3 pos,
    int r, int g, int b, float strength) :
    SceneObject(name, pos, 0, 0, 0)
    {
        this->SetColor(r, g, b);
        this->_lightStrength = strength;
		this->_type = SCENE_LIGHT;
    }
