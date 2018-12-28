#include "TestRenderer.hpp"
#include <cstdio>

using namespace familyline::graphics;


TestRenderer::TestRenderer() {
}

void TestRenderer::initialize()
{
}

const VertexHandle TestRenderer::addVertexData(const VertexData& vdata,
					       const VertexInfo vinfo)
{
    this->vertexAdded++;
    
    auto v =  VertexHandle{};
    v.vsize = vdata.position.size();
    v.vi = vinfo;
    v.sp = nullptr;

    _vertices.push_back(v);
    return v;
}

int TestRenderer::AddBoundingBox(Mesh*, glm::vec3 color) {
    return _vertices.size();
}
