#include "Mesh.hpp"
#include "Material.hpp"

#include <numeric>

using namespace familyline::graphics;

Mesh::Mesh(Animator* animator, const std::vector<VertexInfo>& vinfo)
    : SceneObject("Mesh", glm::vec3(0,0,0), 0, 0, 0)
{
    _translMatrix = glm::mat4(1.0);
    _scaleMatrix = glm::mat4(1.0);
    _rotMatrix = glm::mat4(1.0);

    _modelMatrix = glm::mat4(1.0);

    this->_animator = animator;
    this->_vinfo = vinfo;
    this->_type = SCENE_MESH;
}

void Mesh::Translate(glm::vec3 pos)
{
    _isModelChanged = true;
    _translMatrix = glm::translate(_translMatrix, pos);
}

void Mesh::Scale(glm::vec3 scalef)
{
    _isModelChanged = true;
    _scaleMatrix = glm::scale(_scaleMatrix, scalef);
}

/* warning: rotate angle should be in radians */
void Mesh::Rotate(glm::vec3 axis, float angle)
{
    _isModelChanged = true;
    _rotMatrix = glm::rotate(_rotMatrix, angle, axis);
}

void Mesh::ApplyTransformations()
{
    if (_isModelChanged) {
        _modelMatrix = _translMatrix * _scaleMatrix * _rotMatrix;
        _isModelChanged = false;
    }
}

glm::mat4 Mesh::GetModelMatrix() const
{
    return _modelMatrix;
}

void Mesh::SetPosition(glm::vec3 pos)
{
    this->Translate(-_position);
    SceneObject::SetPosition(pos);
    this->Translate(_position);
    this->ApplyTransformations();
}

void Mesh::AddPosition(glm::vec3 pos)
{
    SceneObject::AddPosition(pos);
    this->Translate(_position);
    this->ApplyTransformations();
}

void Mesh::SetRotation(float x, float y, float z)
{
    SceneObject::SetRotation(x, y, z);
    this->Rotate(glm::vec3(1,0,0), _rotations[0]);
    this->Rotate(glm::vec3(0,1,0), _rotations[1]);
    this->Rotate(glm::vec3(0,0,1), _rotations[2]);
    this->ApplyTransformations();
}

void Mesh::AddRotation(float x, float y, float z)
{
    SceneObject::AddRotation(x, y, z);
    this->Rotate(glm::vec3(1,0,0), _rotations[0]);
    this->Rotate(glm::vec3(0,1,0), _rotations[1]);
    this->Rotate(glm::vec3(0,0,1), _rotations[2]);
    this->ApplyTransformations();
}

glm::mat4* Mesh::GetModelMatrixPointer()
{
    return &this->_modelMatrix;
}


void Mesh::GenerateBoundingBox()
{
    float minz = 10E6, maxz = 10E-6;
    float miny = 10E6, maxy = 10E-6;
    float minx = 10E6, maxx = 10E-6;

    auto vdgroup = this->_animator->getCurrentFrame();
    
    for (const auto& vd : vdgroup) {
	for (auto it = vd.position.begin();
	     it != vd.position.end(); ++it) {
	    minz = std::min(it->z, minz);
	    miny = std::min(it->y, miny);
	    minx = std::min(it->x, minx);

	    maxz = std::max(it->z, maxz);
	    maxy = std::max(it->y, maxy);
	    maxx = std::max(it->x, maxx);
	}
    }
/*
  printf("m: %s\n", this->_name.c_str());
  printf(" minx miny minz: %.2f %.2f %.2f\n", minx, miny, minz);
  printf(" maxx maxy maxz: %.2f %.2f %.2f\n", maxx, maxy, maxz);
*/
    this->_box.minX = minx;
    this->_box.minY = miny;
    this->_box.minZ = minz;
    this->_box.maxX = maxx;
    this->_box.maxY = maxy;
    this->_box.maxZ = maxz;

    this->_box.points[BOUNDING_BOX_LOWER_LEFT_FRONT]    = glm::vec3(minx, miny, minz);
    this->_box.points[BOUNDING_BOX_LOWER_LEFT_BACK]     = glm::vec3(minx, miny, maxz);
    this->_box.points[BOUNDING_BOX_LOWER_RIGHT_BACK]    = glm::vec3(maxx, miny, minz);
    this->_box.points[BOUNDING_BOX_LOWER_RIGHT_FRONT]   = glm::vec3(maxx, miny, maxz);
    this->_box.points[BOUNDING_BOX_UPPER_RIGHT_FRONT]   = glm::vec3(minx, maxy, minz);
    this->_box.points[BOUNDING_BOX_UPPER_RIGHT_BACK]    = glm::vec3(minx, maxy, maxz);
    this->_box.points[BOUNDING_BOX_UPPER_LEFT_BACK]     = glm::vec3(maxx, maxy, minz);
    this->_box.points[BOUNDING_BOX_UPPER_LEFT_FRONT]    = glm::vec3(maxx, maxy, maxz);

}

/**
 * \brief Get a reference to metainformation about the vertices,
 *
 * Get a reference about a structure that contains immutable data
 * about the vertices: shaders and textures
 */
VertexInfo Mesh::getVertexInfo(unsigned int index)
{
    auto vi = _vinfo.at(index);
    vi.worldMat = this->GetModelMatrixPointer();

    return vi;
}


void Mesh::setVertexInfo(unsigned int index, VertexInfo vi)
{
    vi.worldMat = this->GetModelMatrixPointer();
    if (index >= _vinfo.size())
	return; // No matching index!
    
    _vinfo[index] = vi;
}

