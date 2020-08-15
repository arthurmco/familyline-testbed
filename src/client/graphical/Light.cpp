#include <client/graphical/Light.hpp>

using namespace familyline::graphics;

void Light::setColor(int r, int g, int b)
{
    this->_lightColor = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

void Light::getColor(int& r, int& g, int& b)
{
    r = _lightColor.r * 255.0f;
    g = _lightColor.g * 255.0f;
    b = _lightColor.b * 255.0f;
}

void Light::setStrength(float s) { this->_lightStrength = s; }

float Light::getStrength() { return this->_lightStrength; }

std::string_view Light::getName() { return this->_name; }
glm::vec3 Light::getPosition() const { return this->_position; }
void Light::setPosition(glm::vec3 pos) { this->_position = pos; }

Light::Light(const char* name, glm::vec3 pos, int r, int g, int b, float strength)
    : _name(name), _position(pos), _lightColor(r, g, b), _lightStrength(strength)
{
    type = SceneObjectType::SceneLight;
}
