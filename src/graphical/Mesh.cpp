#include "Mesh.hpp"

using namespace Tribalia::Graphics;

Mesh::Mesh(VertexData* vd)
    : SceneObject("Mesh", glm::vec3(0,0,0), 0, 0, 0)
{
    _translMatrix = glm::mat4(1.0);
    _scaleMatrix = glm::mat4(1.0);
    _rotMatrix = glm::mat4(1.0);

    _modelMatrix = glm::mat4(1.0);

    this->_vdata = vd;
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