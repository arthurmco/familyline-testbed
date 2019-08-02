#pragma once

#include "vertexdata.hpp"

namespace familyline::graphics {

    enum SceneObjectType {
        SceneMesh,
        SceneLight
    };

    class SceneObject {
    protected:
        SceneObjectType type;
        std::vector<VertexInfo> vinfo;
        // std::vector<LightHandle> lhandle;
        // Animator* animator;

    public:
        virtual void update() = 0;

        virtual std::string_view getName() const = 0;
        virtual glm::vec3 getPosition() const = 0;
        virtual glm::mat4 getWorldMatrix() const = 0;
    
        virtual std::vector<VertexInfo> getVertexInfo() const { return vinfo;  }
        virtual void setVertexInfo(std::vector<VertexInfo>&& v) { vinfo = v; }

        virtual std::vector<VertexData> getVertexData() = 0;
        virtual bool isVertexDataDirty() = 0;

        virtual ~SceneObject() {}
    };
}
