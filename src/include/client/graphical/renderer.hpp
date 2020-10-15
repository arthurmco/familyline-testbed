#pragma once

/**
 * Mesh renderer class
 */

#include <memory>
#include <client/graphical/light.hpp>
#include "camera.hpp"
#include "vertexdata.hpp"

namespace familyline::graphics
{
struct VertexHandle {
    VertexInfo vinfo;

    VertexHandle(VertexInfo& vinfo) : vinfo(vinfo) {}

    virtual bool update(VertexData& vd)                   = 0;
    virtual bool remove()                                 = 0;
    virtual bool recreate(VertexData& vd, VertexInfo& vi) = 0;

    virtual ~VertexHandle() {}
};

    struct LightHandle {
        Light& light;
    };

class Renderer
{
public:
    virtual VertexHandle* createVertex(VertexData& vd, VertexInfo& vi) = 0;
    virtual void removeVertex(VertexHandle* vh)                        = 0;
    virtual void render(Camera* c)                                     = 0;
    // virtual LightHandle createLight(LightData& ld) = 0;

    virtual LightHandle* createLight(Light& light) = 0;
    virtual void removeLight(LightHandle* lh) = 0;

    virtual ~Renderer() {}
};

}  // namespace familyline::graphics
