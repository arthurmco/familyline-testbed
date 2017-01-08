/***
    Material data

    Copyright (C) 2016 Arthur M.

***/

#include <glm/glm.hpp>
#include <string>

#include "Texture.hpp"

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

namespace Tribalia {
namespace Graphics {

    struct MaterialData {
        glm::vec3 diffuseColor;
        glm::vec3 specularColor;
        glm::vec3 ambientColor;
        Texture* texture = nullptr;

        MaterialData(glm::vec3 dif, glm::vec3 spec, glm::vec3 amb) {
            diffuseColor = dif;
            specularColor = spec;
            ambientColor = amb;
        }

        MaterialData() {
            diffuseColor = glm::vec3(0);
            specularColor = glm::vec3(0);
            ambientColor = glm::vec3(0);
        }
    };

    class Material
    {
        friend class MaterialManager;
    private:
        int _ID;
        std::string _name;
        MaterialData _data;
	Texture* _tex;

    public:
        Material(int ID, const char* name, MaterialData data);
        Material(const char* name, MaterialData data);

        int GetID() const;
        const char* GetName() const;
        MaterialData* GetData();
	Texture* GetTexture();
	void SetTexture(Texture*);
    };

}
}



#endif /* end of include guard: MATERIAL_HPP */
