/***
    Raw vertex data

    Copyright (C) 2016 Arthur M.

***/

#include <vector>
#include <glm/glm.hpp>

#ifndef VERTEXDATA_HPP
#define VERTEXDATA_HPP

namespace Tribalia {
namespace Graphics {

struct VertexData
{
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> TexCoords;
};

}
}

#endif /* end of include guard: VERTEXDATA_HPP */
