
#include <string>
#include "scene_object.hpp"
#include "animator.hpp"

#include "../logic/imesh.hpp"
#include "asset_object.hpp"

#ifndef _MESH_HPP
#define _MESH_HPP

namespace familyline::graphics {

    struct BoundingBox {
        float minX, minY, minZ;
        float maxX, maxY, maxZ;
    };

    class Mesh : public SceneObject, public AssetObject, public familyline::logic::IMesh  {
    private:
        std::string _name;
        glm::mat4 _worldMatrix;
        Animator* _ani;

    public:
        Mesh(const char* name, Animator* ani, std::vector<VertexInfo> vinfo)
            : SceneObject(), _name(name), _ani(ani), _worldMatrix(glm::mat4(1.0))
            {
                this->vinfo = vinfo;
            }

        glm::vec3 worldPosition;

        /**
         * Update the world matrix and the animator
         */
        virtual void update();

        virtual void setLogicPosition(glm::vec3 p) { this->worldPosition = p; }

        virtual std::string_view getName() const;
        virtual glm::vec3 getPosition() const;
        virtual glm::mat4 getWorldMatrix() const;
        virtual std::vector<VertexData> getVertexData();
        virtual bool isVertexDataDirty();

        // TODO: create a better function to return bounding boxes
        BoundingBox getBoundingBox();

        Animator* getAnimator();

        virtual AssetType getAssetType() const { return AssetType::MeshAsset; }
    };
}

#endif
