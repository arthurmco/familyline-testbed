#include "scene_renderer.hpp"
#include "Log.hpp"
#include "mesh.hpp"

using namespace familyline::graphics;

// TODO: texture animation
// TODO: differentiate static from dynamic meshes?
// TODO: put vertex info into the vertex handle to make
//       getting the world matrix easier (and can solve texture anim too)

int SceneRenderer::add(std::shared_ptr<SceneObject> so)
{
	int id = nextID++;

	auto vdata = so->getVertexData();
	auto vinfo = so->getVertexInfo();
	auto vhandles = std::vector<VertexHandle*>();

	unsigned idx = 0;
	for (auto& vgroup : vdata) {
		vhandles.push_back(_renderer->createVertex(vgroup, vinfo[idx]));
		idx++;
	}

	_sceneObjects.emplace_back(so, id, vhandles);
    
    //	LoggerService::getLogger()->write("scene-renderer", LogType::Debug,
	//	"added scene object %s with ID %#x", so->getName().data(), id);
    auto l = Log::GetLog();
    l->InfoWrite("scene-renderer", "added scene object %s with ID %#x",
                 so->getName().data(), id);
    
	return id;
}

void SceneRenderer::remove(int meshHandle)
{
    (void)meshHandle; //TODO: implement
}

void SceneRenderer::update()
{
	this->updateVisibleObjects();
	this->changeVertexStatusOnRenderer();
}

void SceneRenderer::updateVisibleObjects()
{
	for (auto& si : this->_sceneObjects) {
		if (!si.visible) {
			continue;
		}

		si.object->update();

		auto vinfos = si.object->getVertexInfo();
		unsigned idx = 0;
		for (auto vhandle : si.handles) {
			vhandle->vinfo = vinfos[idx];
			idx++;
		}

		if (si.object->isVertexDataDirty()) {
			idx = 0;

			auto vgroups = si.object->getVertexData();
			for (auto vhandle : si.handles) {
				vhandle->update(vgroups[idx]);
				idx++;
			}
		}
	}

}

void SceneRenderer::changeVertexStatusOnRenderer()
{
	// Check if the scene object can be seen by the camera
	// If it can't, set the visible attribute to false.
	// If it can, set it to true.
}
