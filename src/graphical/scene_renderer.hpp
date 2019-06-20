#pragma once

#include <memory>
#include "renderer.hpp"
#include "scene_object.hpp"
#include "camera.hpp"

using namespace familyline::graphics;

struct SceneObjectInfo {
	std::shared_ptr<SceneObject> object;
	int id;
	std::vector<VertexHandle*> handles;
	bool visible = true;

	SceneObjectInfo(std::shared_ptr<SceneObject> o, int id,
		std::vector<VertexHandle*> h)
		: object(o), id(id), handles(h)
	{}
};

class Mesh;

/**
 * Manages scene object rendering
 *
 * Gets scene object raw vertex and light handles and updates them, accordingly to
 * some variables.
 * For example, if the scene object is destroyed, we remove the vertex handle (and, therefore, the data) 
 * from the video card
 */
class SceneRenderer {
private:
	Renderer* _renderer;

	std::vector<SceneObjectInfo> _sceneObjects;

	int nextID = 1;

	void updateVisibleObjects();
	void changeVertexStatusOnRenderer();

public:
	Camera& camera;

	SceneRenderer(Renderer* renderer, Camera& camera)
		: _renderer(renderer), camera(camera)
	{}

	int add(std::shared_ptr<SceneObject> so);
	void remove(int meshHandle);
	void update();
};
