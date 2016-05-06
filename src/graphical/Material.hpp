/***
    Material data

    Copyright (C) 2016 Arthur M.

***/

#include <glm/glm.hpp>
#include <string>

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

namespace Tribalia {
namespace Graphics {

    struct MaterialData {
        glm::vec3 diffuseColor;
        float diffuseIntensity;

        glm::vec3 specularColor;
        float specularIntensity;

        glm::vec3 ambientColor;
        float ambientIntensity;
    };

    class Material
    {
        friend class MaterialManager;
    private:
        int _ID;
        std::string _name;
        MaterialData _data;

    public:
        Material(int ID, const char* name, MaterialData data);

        int GetID() const;
        const char* GetName() const;
        MaterialData* GetData();
    };

}
}



#endif /* end of include guard: MATERIAL_HPP */