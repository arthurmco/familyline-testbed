#include <client/graphical/scene_manager.hpp>
#include <common/logger.hpp>

using namespace familyline::graphics;

/**
 *  Update the vertices of the visible object into the
 * renderer and, consequently, on the video card.
 */
void SceneManager::updateObjectVertices(SceneObjectInfo& soi)
{
    auto vinfos  = soi.object->getVertexInfo();
    unsigned idx = 0;
    for (auto vhandle : soi.handles) {
        vhandle->vinfo = vinfos[idx];
        idx++;
    }

    if (soi.object->isVertexDataDirty()) {
        idx = 0;

        auto vgroups = soi.object->getVertexData();
        for (auto vhandle : soi.handles) {
            vhandle->update(vgroups[idx]);
            idx++;
        }
    }
}


void SceneManager::updateAnimations(SceneObjectInfo& soi, unsigned int ms)
{
    soi.object->stepAnimation(ms);
}

void SceneManager::update(unsigned int ms)
{
    for (auto& soi : objects_) {
        if (!soi.visible)
            continue;

        soi.object->update();
        this->updateAnimations(soi, ms);
        this->updateObjectVertices(soi);
    }
}

void SceneManager::remove(scene_object_handle_t meshHandle)
{
    auto& log = LoggerService::getLogger();
    auto iter = std::find_if(
        objects_.begin(), objects_.end(),
        [meshHandle](SceneObjectInfo& soi) { return soi.id == meshHandle; });

    if (iter == objects_.end()) return;

    for (auto h : iter->handles) {
        h->remove();
    }
    
    log->write(
        "scene-renderer", LogType::Debug, "removed scene object %s with ID %#x",
        iter->object->getName().data(), meshHandle);

    objects_.erase(iter);
}

int SceneManager::add(std::shared_ptr<SceneObjectBase> so)
{
    auto& log = LoggerService::getLogger();
    int id    = (uintptr_t)so.get();

    auto vdata    = so->getVertexData();
    auto vinfo    = so->getVertexInfo();
    auto vhandles = std::vector<VertexHandle*>();

    unsigned idx = 0;
    for (auto& vgroup : vdata) {
        vhandles.push_back(renderer_.createVertex(vgroup, vinfo[idx]));
        idx++;
    }

    objects_.emplace_back(so, id, true, vhandles);

    log->write(
        "scene-renderer", LogType::Debug, "added scene object %s with ID %#x", so->getName().data(),
        id);

    return id;
}
