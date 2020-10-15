#pragma once

#include <client/graphical/mesh.hpp>
#include <client/graphical/light.hpp>
#include <client/graphical/vertexdata.hpp>

/**
 * Scene object class
 * Abstracts the different scene objects (currently only the mesh) to a single class
 *
 * Copyright (C) 2020 Arthur Mendes
 */

namespace familyline::graphics
{
enum class SceneObjectType { Mesh, Light, Invalid, Other };

/**
 * Base class for the scene object trait
 */
class SceneObjectBase
{
public:
    virtual SceneObjectType getType() { return SceneObjectType::Invalid; }

    virtual void update()                 = 0;
    virtual void stepAnimation(double ms) = 0;

    virtual std::string_view getName() const = 0;
    virtual glm::vec3 getPosition() const    = 0;
    virtual glm::mat4 getWorldMatrix() const = 0;

    virtual std::vector<VertexInfo> getVertexInfo() const   = 0;
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
class SceneObject : public SceneObjectBase
{
public:
    virtual SceneObjectType getType() { return SceneObjectType::Other; }

    SceneObject(T& ref) { static_assert("SceneObject trait not supported for this type"); }

    virtual void update() {}
    virtual void stepAnimation(double ms) {}

    virtual std::string_view getName() const { return ""; }
    virtual glm::vec3 getPosition() const { return glm::vec3(-1, -1, -1); }
    virtual glm::mat4 getWorldMatrix() const { return glm::mat4(1.0); }

    virtual std::vector<VertexInfo> getVertexInfo() const { return std::vector<VertexInfo>(); }
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) {}

    virtual std::vector<VertexData> getVertexData() { return std::vector<VertexData>(); }
    virtual bool isVertexDataDirty() { return false; }

    virtual T& getInner() = 0;

    virtual ~SceneObject() {}
};

/*
 * The scene object trait
 *
 * This is only the default implementation, not the specific ones, so
 * it will return only the default values
 */
template <>
class SceneObject<Mesh> : public SceneObjectBase
{
protected:
    Mesh& ref_;

public:
    virtual SceneObjectType getType() { return SceneObjectType::Mesh; }

    SceneObject(Mesh& ref) : ref_(ref) {}

    virtual void update() { ref_.update(); }

    virtual void stepAnimation(double ms) { ref_.getAnimator()->advance(ms); }

    virtual std::string_view getName() const { return ref_.getName(); }
    virtual glm::vec3 getPosition() const { return ref_.getPosition(); }
    virtual glm::mat4 getWorldMatrix() const { return ref_.getWorldMatrix(); }

    virtual std::vector<VertexInfo> getVertexInfo() const { return ref_.getVertexInfo(); }
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) { ref_.setVertexInfo(std::move(v)); }

    virtual std::vector<VertexData> getVertexData() { return ref_.getVertexData(); }
    virtual bool isVertexDataDirty() { return ref_.isVertexDataDirty(); }

    virtual Mesh& getInner() { return ref_; }

    virtual ~SceneObject() {}
};

template <>
class SceneObject<Light> : public SceneObjectBase
{
protected:
    Light& ref_;

public:
    virtual SceneObjectType getType() { return SceneObjectType::Light; }

    SceneObject(Light& ref) : ref_(ref) {}

    virtual void update() {  }

    virtual void stepAnimation(double ms) {  }

    virtual std::string_view getName() const { return ref_.getName(); }
    virtual glm::vec3 getPosition() const { return glm::vec3(0, 0, 0); }
    virtual glm::mat4 getWorldMatrix() const { return glm::mat4(1.0); }

    virtual std::vector<VertexInfo> getVertexInfo() const { return std::vector<VertexInfo>(); }
    virtual void setVertexInfo(std::vector<VertexInfo>&& v) {  }

    virtual std::vector<VertexData> getVertexData() { return std::vector<VertexData>(); }
    virtual bool isVertexDataDirty() { return true; }

    virtual Light& getInner() { return ref_; }

    virtual ~SceneObject() {}

};

template <typename T>
std::shared_ptr<SceneObjectBase> make_scene_object(T& obj)
{
    SceneObjectBase* pobj = (SceneObjectBase*)new SceneObject<T>(obj);
    return std::shared_ptr<SceneObjectBase>(pobj);
}

}  // namespace familyline::graphics
