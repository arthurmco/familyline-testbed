#include "Mesh.hpp"
#include "Material.hpp"

using namespace Tribalia::Graphics;

Mesh::Mesh(VertexData* vd)
    : SceneObject("Mesh", glm::vec3(0,0,0), 0, 0, 0)
{
    _translMatrix = glm::mat4(1.0);
    _scaleMatrix = glm::mat4(1.0);
    _rotMatrix = glm::mat4(1.0);

    _modelMatrix = glm::mat4(1.0);

    this->_vdata = new VertexData(*vd);
    this->_vdata->meshptr = ((uintptr_t)this);
    this->_type = SCENE_MESH;
}

void Mesh::Translate(glm::vec3 pos)
{
    _isModelChanged = true;
    _translMatrix *= glm::translate(pos);
}

void Mesh::Scale(glm::vec3 scalef)
{
    _isModelChanged = true;
    _scaleMatrix = glm::scale(scalef);
}

/* warning: rotate angle should be in radians */
void Mesh::Rotate(glm::vec3 axis, float angle)
{
    _isModelChanged = true;
    _rotMatrix *= glm::rotate(angle, axis);
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

VertexData* Mesh::GetVertexData()
{
    return this->_vdata;
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
    for (auto it = _vdata->Positions.begin();
        it != _vdata->Positions.end(); ++it) {
        minz = std::min(it->z, minz);
        miny = std::min(it->y, miny);
        minx = std::min(it->x, minx);

        maxz = std::max(it->z, maxz);
        maxy = std::max(it->y, maxy);
        maxx = std::max(it->x, maxx);


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


/* Sets material for whole mesh */
void Mesh::SetMaterial(void* m)
{
    Material* mt = (Material*)m;
    _vdata->MaterialIDs.clear();

    for (size_t i = 0; i < _vdata->Positions.size(); i++) {
	_vdata->MaterialIDs.push_back(mt->GetID());
    }

    
    
}
