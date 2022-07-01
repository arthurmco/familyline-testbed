#pragma once

/**
 * Mesh renderer class
 */

#include <algorithm>
#include <client/graphical/camera.hpp>
#include <client/graphical/light.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

#include "vertexdata.hpp"

namespace familyline::graphics
{
class TerrainRenderer;

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

    LightHandle(Light& l) : light(l) {}
};

using render_hook_t = std::function<void(Camera*)>;

class Renderer
{
private:
    /// Render hooks.
    ///
    /// There will be functions that will need render information, and
    /// will need to be run at render time, but might need to run outside
    /// of the render
    ///
    /// The hooks will store them here to run in the right time.
    std::unordered_map<long long int, render_hook_t> hooks_;

protected:
    void runHooks(Camera* c)
    {
        std::for_each(hooks_.begin(), hooks_.end(), [&](auto& hookd) { hookd.second(c); });
    }

public:
    long long int addHook(render_hook_t hook)
    {
        long long int id = ((long long int)rand()) * ((long long int)rand());
        hooks_[id]       = hook;
        return id;
    }

    void removeHook(long long int id) { hooks_.erase(id); }

    virtual VertexHandle* createVertex(VertexData& vd, VertexInfo& vi) = 0;
    virtual void removeVertex(VertexHandle* vh)                        = 0;
    virtual void render(Camera* c)                                     = 0;
    // virtual LightHandle createLight(LightData& ld) = 0;

    virtual LightHandle* createLight(Light& light) = 0;
    virtual void removeLight(LightHandle* lh)      = 0;

    virtual TerrainRenderer* createTerrainRenderer(Camera& camera) = 0;

    virtual ~Renderer() {}
};

}  // namespace familyline::graphics
