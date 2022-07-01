/***
    Material data

    Copyright (C) 2016, 2019 Arthur Mendes.

***/
#pragma once

#include <glm/glm.hpp>
#include <string>

#include <client/graphical/texture_environment.hpp>
#include <client/graphical/asset_object.hpp>

namespace familyline::graphics
{
struct MaterialData {
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
    std::optional<TextureHandle> tex_;

    MaterialData(glm::vec3 dif, glm::vec3 spec, glm::vec3 amb)
        : diffuseColor(dif), specularColor(spec), ambientColor(amb)
    {
    }

    MaterialData(float dif, float spec, float amb)
        : diffuseColor(glm::vec3(dif)), specularColor(glm::vec3(spec)), ambientColor(glm::vec3(amb))

    {
    }

    MaterialData() : diffuseColor(0), specularColor(0), ambientColor(0) {}
};

class Material : public AssetObject
{
    friend class MaterialManager;

private:
    int _ID;
    std::string _name;
    MaterialData _data;
    std::optional<TextureHandle> tex_;

public:
    Material(int ID, const char* name, MaterialData data);
    Material(const char* name, MaterialData data);

    int getID() const;
    const char* getName() const;
    MaterialData& getData();
    std::optional<TextureHandle> getTexture();
    void setTexture(std::optional<TextureHandle>);

    virtual AssetType getAssetType() const { return AssetType::MaterialAsset; }

};

}  // namespace familyline::graphics
