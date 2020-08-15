#pragma once

/**
 * Mesh renderer class
 */

#include <memory>

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
};

class Renderer
{
public:
    virtual VertexHandle* createVertex(VertexData& vd, VertexInfo& vi) = 0;
    virtual void removeVertex(VertexHandle* vh)                        = 0;
    virtual void render(Camera* c)                                     = 0;
    // virtual LightHandle createLight(LightData& ld) = 0;
};

}  // namespace familyline::graphics
