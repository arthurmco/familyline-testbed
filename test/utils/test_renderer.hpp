#pragma once
/**
 * Those classes above mocks a renderer
 *
 * They are useful to test some aspects of the graphical system without
 * calling any specific API
 */

#include <client/graphical/camera.hpp>
#include <client/graphical/renderer.hpp>

class TestRenderer;

struct TestVertexHandle final : public familyline::graphics::VertexHandle {
private:
    TestRenderer& renderer_;

public:
    familyline::graphics::VertexData vdata;
    int id_ = 0;

    TestVertexHandle(
        familyline::graphics::VertexInfo& vinfo, familyline::graphics::VertexData vdata, int id,
        TestRenderer& renderer)
        : VertexHandle(vinfo), vdata(vdata), id_(id), renderer_(renderer)
    {
    }

    virtual bool update(familyline::graphics::VertexData& vd);
    virtual bool remove();
    virtual bool recreate(
        familyline::graphics::VertexData& vd, familyline::graphics::VertexInfo& vi);

    virtual ~TestVertexHandle() {}
};

class TestRenderer : public familyline::graphics::Renderer
{
private:
    std::unordered_map<int, TestVertexHandle> handles_;
    int lastid_ = 0;

public:
    virtual familyline::graphics::VertexHandle* createVertex(
        familyline::graphics::VertexData& vd, familyline::graphics::VertexInfo& vi);

    virtual void removeVertex(familyline::graphics::VertexHandle* vh);
    virtual void render(familyline::graphics::Camera* c);

    size_t getVertexListCount();
    virtual ~TestRenderer() {}
};
