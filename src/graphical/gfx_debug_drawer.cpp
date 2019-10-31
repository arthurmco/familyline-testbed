#include "gfx_debug_drawer.hpp"
#include <string_view>
#include "vertexdata.hpp"
#include "TerrainRenderer.hpp"
#include "gfx_service.hpp"

using namespace familyline::graphics;


/**
 * Implementation of the Fowler-Noll-Vo 1 hash function
 *
 * This will be used to hash the positions
 */
uint64_t hash_fnv1(std::string_view s)
{
    uint64_t off_basis = 0xcbf29ce484222325;
    uint64_t hash = off_basis;

    for (auto vchar : s) {
        hash *= 0x100000001b3;
        hash ^= ((uint8_t)vchar);
    }

    return hash;
}

/**
 * These draw* functions are called once per frame.
 *
 * But the vertices are stored in the videocard
 * We need to know the time we need to stop storing those vertices
 * in the videocard and removing it
 * Those hashes uniquely identify the vertices
 */
uint64_t hashPath(glm::vec3 start, glm::vec3 end)
{
    char posval[100] = {};
    snprintf(posval, 99, "%.5f%.4f%.5fx%.5f%.4f%.5f",
             start.x, start.y, start.z, end.x, end.y, end.z);
    
    return hash_fnv1(posval);
}

#define Game2GFX GameToGraphicalSpace

void GFXDebugDrawer::drawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color)
{
    auto hash = hashPath(start, end);
    if (_vhandles.find(hash) != _vhandles.end()) {
        // TODO: only update color
        return;
    }

    glm::vec3 color3 = color;
    VertexData vdata;
    vdata.position = { Game2GFX(start), Game2GFX(end) };
    vdata.normals = { color3, color3 };
    vdata.texcoords = { glm::vec2(0, 0), glm::vec2(0, 0)};

    VertexInfo vinfo(0, 0, GFXService::getShaderManager()->getShader("lines"),
                     VertexRenderStyle::PlotLines);
    //vinfo.shaderState.vec3Uniforms["color"] = color;

    VertexHandle* vhandle = _renderer.createVertex(vdata, vinfo);
    _vhandles[hash] = {vhandle, this->last_tick};
}

void GFXDebugDrawer::drawSquare(glm::vec3 start, glm::vec3 end,
                                glm::vec4 foreground, glm::vec4 background)
{
    auto hash = hashPath(start, end);
    if (_vhandles.find(hash) != _vhandles.end()) {
        // TODO: only update color
        return;
    }

    glm::vec3 x0 = start;
    glm::vec3 x1 = glm::vec3(end.x, start.y, start.z);
    glm::vec3 y0 = glm::vec3(start.x, end.y, end.z);
    glm::vec3 y1 = end;
    
    glm::vec3 fore3 = foreground;
    VertexData vdata;
    vdata.position = { Game2GFX(x0), Game2GFX(x1), Game2GFX(y1),
                       Game2GFX(x0), Game2GFX(y0), Game2GFX(y1)};
    vdata.normals = { fore3, fore3, fore3, fore3, fore3, fore3};
    vdata.texcoords = { glm::vec2(0, 0), glm::vec2(0, 0),
                        glm::vec2(0, 0), glm::vec2(0, 0),
                        glm::vec2(0, 0), glm::vec2(0, 0)};

    VertexInfo vinfo(0, 0, GFXService::getShaderManager()->getShader("lines"),
                     VertexRenderStyle::PlotLines);
    //vinfo.shaderState.vec3Uniforms["color"] = foreground;

    VertexHandle* vhandle = _renderer.createVertex(vdata, vinfo);
    _vhandles[hash] = {vhandle, this->last_tick};
}

void GFXDebugDrawer::drawCircle(glm::vec3 point, glm::vec3 radius,
                                glm::vec4 foreground, glm::vec4 background)
{

}

void GFXDebugDrawer::update()
{
    for (auto [hash, vdata] : _vhandles) {
        
    }
    
    this->last_tick++;
}
