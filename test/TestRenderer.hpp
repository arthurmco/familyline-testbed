#ifndef TESTRENDERER_HPP
#define TESTRENDERER_HPP

#include "graphical/Renderer.hpp"

class TestRenderer final : public familyline::graphics::Renderer {
private:
    int vertexAdded = 0;
    
public:
    TestRenderer();
    virtual ~TestRenderer() {}

    virtual void initialize();

    virtual const familyline::graphics::VertexHandle addVertexData(
	const familyline::graphics::VertexData& vdata,
	const familyline::graphics::VertexInfo vinfo);

    virtual int AddBoundingBox(
	familyline::graphics::Mesh*, glm::vec3 color);
    
    
    // Functions that retrieve data about the renderer, so we can
    // know its state
    size_t getSceneIDCount() { return _last_IDs.size(); }

    int getAddedVertices() { return vertexAdded; }
};

// Renderer class redesigned to be easier to test, added initialize() method
// to init external things like shaders.

#endif /* TESTRENDERER_HPP */
