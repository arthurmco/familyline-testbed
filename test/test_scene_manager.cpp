#include <gtest/gtest.h>

#include <client/graphical/mesh.hpp>
#include <client/graphical/camera.hpp>
#include <client/graphical/meshopener/OBJOpener.hpp>
#include <client/graphical/meshopener/MD2Opener.hpp>
#include <common/logic/logic_service.hpp>
#include <common/logic/object_factory.hpp>
#include <common/logger.hpp>

#include <unordered_map>
#include <memory>

#include "utils.hpp"
#include "utils/test_renderer.hpp"


using namespace familyline;
using namespace familyline::graphics;


/**
 * Base class for the scene object trait
 */
class SceneObjectBase {
public:
    virtual void update() = 0;
    virtual void stepAnimation(double ms) = 0;

    virtual std::string_view getName() const = 0;
    virtual glm::vec3 getPosition() const    = 0;
    virtual glm::mat4 getWorldMatrix() const = 0;

    virtual std::vector<VertexInfo> getVertexInfo() const = 0;
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) = 0;

    virtual std::vector<VertexData> getVertexData() = 0;
    virtual bool isVertexDataDirty()                = 0;

    virtual ~SceneObjectBase() {}

};

/**
 * The scene object trait
 *
 * This is only the default implementation, not the specific ones, so
 * it will return only the default values
 */
template <typename T>
class SceneObject : public SceneObjectBase {
public:
    SceneObject(T& ref) { static_assert("SceneObject trait not supported for this type");  }

    virtual void update() {}
    virtual void stepAnimation(double ms) {}

    virtual std::string_view getName() const { return ""; }
    virtual glm::vec3 getPosition() const    { return glm::vec3(-1, -1, -1); }
    virtual glm::mat4 getWorldMatrix() const { return glm::mat4(1.0); }

    virtual std::vector<VertexInfo> getVertexInfo() const { return std::vector<VertexInfo>(); }
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) {}

    virtual std::vector<VertexData> getVertexData() { return std::vector<VertexData>() ;}
    virtual bool isVertexDataDirty()                { return false; }

    virtual ~SceneObject() {}

};

/*
 * The scene object trait
 *
 * This is only the default implementation, not the specific ones, so
 * it will return only the default values
 */
template <>
class SceneObject<Mesh> : public SceneObjectBase {
protected:
    Mesh& ref_;

public:
    SceneObject(Mesh& ref) : ref_(ref) {}

    virtual void update() {
        ref_.update();
    }

    virtual void stepAnimation(double ms) {
        ref_.getAnimator()->advance(ms);
    }

    virtual std::string_view getName() const { return ref_.getName(); }
    virtual glm::vec3 getPosition() const    { return ref_.getPosition(); }
    virtual glm::mat4 getWorldMatrix() const { return ref_.getWorldMatrix(); }

    virtual std::vector<VertexInfo> getVertexInfo() const { return ref_.getVertexInfo(); }
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) { ref_.setVertexInfo(std::move(v));  }

    virtual std::vector<VertexData> getVertexData() { return ref_.getVertexData() ;}
    virtual bool isVertexDataDirty()                { return ref_.isVertexDataDirty(); }

    virtual ~SceneObject() {}

};

struct SceneObjectInfo {
    std::shared_ptr<SceneObjectBase> object;

    int id;

    /// Is the object visible or not?
    bool visible;

    /// The vertex handles, represent data on the video card
    /// about this object
    std::vector<VertexHandle*> handles;
};

typedef int scene_object_handle_t;

/**
 * Manages scene object rendering
 *
 * Gets scene object raw vertex and light handles and updates them, accordingly to
 * some variables.
 * For example, if the scene object is destroyed, we remove the vertex handle (and, therefore, the
 * data) from the video card
 */
class SceneManager {
private:
    Camera& camera_;
    Renderer& renderer_;

    std::vector<SceneObjectInfo> objects_;

    /**
     *  Update the vertices of the visible objects into the
     * renderer and, consequently, on the video card.
     */
    void updateObjectVertices(SceneObjectInfo& soi);

    /**
     * Advance animation frames for all visible frames for the
     * specified scene obejct
     */
    void updateAnimations(SceneObjectInfo& soi, unsigned int ms);

public:
    SceneManager(Renderer& renderer, Camera& camera) : renderer_(renderer), camera_(camera) {}

    /// Add the object to the scene manager, get the ID
    scene_object_handle_t add(std::shared_ptr<SceneObjectBase> o);
    void remove(scene_object_handle_t meshHandle);

    /// Update the scene objects
    void update(unsigned int ms);

};




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
        this->updateObjectVertices(soi);
        this->updateAnimations(soi, ms);
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

template <typename T>
std::shared_ptr<SceneObjectBase> make_scene_object(T& obj) {
    SceneObjectBase* pobj = (SceneObjectBase*) new SceneObject<T>(obj);
    return std::shared_ptr<SceneObjectBase>(pobj);
}


TEST(SceneManager, TestMeshAdd) {
    ShaderProgram s{"forward", {}};
    GFXService::getShaderManager()->addShader(&s);

    TestRenderer renderer;
    Camera camera(glm::vec3(-30, 30, -30), 16/9.0f, glm::vec3(0, 0, 0));
    SceneManager sm(renderer, camera);

    OBJOpener o;
    std::vector<Mesh*> meshes = o.OpenSpecialized(TESTS_DIR "/assets/test2.obj");
    Mesh* m = meshes[0];

    ASSERT_EQ(0, renderer.getVertexListCount());
    sm.add(make_scene_object(*m));
    ASSERT_EQ(3, renderer.getVertexListCount());

    sm.update(16);
    ASSERT_EQ(3, renderer.getVertexListCount());
}


TEST(SceneManager, Test2MeshAdd) {
    ShaderProgram s{"forward", {}};
    GFXService::getShaderManager()->addShader(&s);

    TestRenderer renderer;
    Camera camera(glm::vec3(-30, 30, -30), 16/9.0f, glm::vec3(0, 0, 0));
    SceneManager sm(renderer, camera);

    OBJOpener o;
    std::vector<Mesh*> meshes = o.OpenSpecialized(TESTS_DIR "/assets/test.obj");
    Mesh* m1 = meshes[0];
    std::vector<Mesh*> meshes2 = o.OpenSpecialized(TESTS_DIR "/assets/test2.obj");
    Mesh* m2 = meshes2[0];

    ASSERT_EQ(0, renderer.getVertexListCount());
    sm.add(make_scene_object(*m1));
    sm.add(make_scene_object(*m2));
    ASSERT_EQ(4, renderer.getVertexListCount());

    sm.update(16);
    ASSERT_EQ(4, renderer.getVertexListCount());
}

TEST(SceneManager, TestMeshPlot) {
    ShaderProgram s{"forward", {}};
    GFXService::getShaderManager()->addShader(&s);

    TestRenderer renderer;
    Camera camera(glm::vec3(-30, 30, -30), 16/9.0f, glm::vec3(0, 0, 0));
    SceneManager sm(renderer, camera);

    OBJOpener o;
    std::vector<Mesh*> meshes = o.OpenSpecialized(TESTS_DIR "/assets/test2.obj");
    Mesh* m = meshes[0];
    m->setLogicPosition(glm::vec3(10, 0, 15));

    sm.add(make_scene_object(*m));
    sm.update(16);

    ASSERT_FLOAT_EQ(10, m->getPosition().x);
    ASSERT_FLOAT_EQ(0, m->getPosition().y);
    ASSERT_FLOAT_EQ(15, m->getPosition().z);
}


TEST(SceneManager, TestMeshRemove) {
    ShaderProgram s{"forward", {}};
    GFXService::getShaderManager()->addShader(&s);

    TestRenderer renderer;
    Camera camera(glm::vec3(-30, 30, -30), 16/9.0f, glm::vec3(0, 0, 0));
    SceneManager sm(renderer, camera);

    OBJOpener o;
    std::vector<Mesh*> meshes = o.OpenSpecialized(TESTS_DIR "/assets/test.obj");
    std::vector<Mesh*> meshes2 = o.OpenSpecialized(TESTS_DIR "/assets/test2.obj");
    std::vector<Mesh*> meshes3 = o.OpenSpecialized(TESTS_DIR "/assets/test2.obj");
    std::vector<Mesh*> meshes4 = o.OpenSpecialized(TESTS_DIR "/assets/test.obj");

    sm.add(make_scene_object(*meshes[0]));
    sm.add(make_scene_object(*meshes2[0]));
    auto removeid = sm.add(make_scene_object(*meshes3[0]));
    sm.add(make_scene_object(*meshes4[0]));
    sm.update(16);

    ASSERT_EQ(8, renderer.getVertexListCount());

    sm.remove(removeid);    
    sm.update(16);

    ASSERT_EQ(5, renderer.getVertexListCount());
}


TEST(SceneManager, TestSceneManagerAnimate) {
    ShaderProgram s{"forward", {}};
    GFXService::getShaderManager()->addShader(&s);

    TestRenderer renderer;
    Camera camera(glm::vec3(-30, 30, -30), 16/9.0f, glm::vec3(0, 0, 0));
    SceneManager sm(renderer, camera);

    OBJOpener oo;
    MD2Opener om;
    std::vector<Mesh*> meshes = oo.OpenSpecialized(TESTS_DIR "/assets/test.obj");
    std::vector<Mesh*> meshes2 = om.OpenSpecialized(TESTS_DIR "/assets/anim_test.md2");
    Mesh* animmesh = meshes2[0];
    
    sm.add(make_scene_object(*meshes[0]));
    sm.add(make_scene_object(*animmesh));

    ASSERT_FLOAT_EQ(0.0, animmesh->getAnimator()->getCurrentTime());

    for (auto i = 0; i < 120; i++) {
        sm.update(15);
    }

    ASSERT_FLOAT_EQ(1800.0, animmesh->getAnimator()->getCurrentTime());
}

