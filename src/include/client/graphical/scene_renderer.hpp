#pragma once

#include <memory>

#include "camera.hpp"
#include "renderer.hpp"
#include "scene_object.hpp"
#include "vertexdata.hpp"


namespace familyline::graphics
{
class Mesh;

struct SceneObjectInfo {
    std::shared_ptr<Mesh> object;
    int id;
    std::vector<VertexHandle*> handles;
    bool visible = true;

    SceneObjectInfo(std::shared_ptr<Mesh> o, int id, std::vector<VertexHandle*> h)
        : object(o), id(id), handles(h)
    {
    }
};


/**
 * Manages scene object rendering
 *
 * Gets scene object raw vertex and light dles and updates them, accordingly to
 * some variables.
 * For example, if the scene object is destroyed, we remove the vertex handle (and, therefore, the
 * data) from the video card
 */
class SceneRenderer
{
private:
    Renderer* _renderer;

    std::vector<SceneObjectInfo> _sceneObjects;

    int nextID = 1;

    void updateVisibleObjects();
    void changeVertexStatusOnRenderer();

public:
    Camera& camera;

    SceneRenderer(Renderer* renderer, Camera& camera) : _renderer(renderer), camera(camera) {}

    int add(std::shared_ptr<Mesh> so);
    void remove(int meshHandle);
    void update();
};
}  // namespace familyline::graphics
