#include "mesh.hpp"

using namespace familyline::graphics;

void Mesh::update()
{
	auto translMatrix = glm::mat4(1.0);
	glm::translate(translMatrix, this->worldPosition);

	_worldMatrix = translMatrix;
	
	// update values in the shader state
	for (auto& vi : vinfo) {
		vi.shaderState.matrixUniforms["m_world"] = _worldMatrix;
	}
}

std::string_view Mesh::getName() const
{
	return _name;
}

glm::vec3 Mesh::getPosition() const
{
	return this->worldPosition;
}


glm::mat4 Mesh::getWorldMatrix() const
{
	return _worldMatrix;
}

Animator* Mesh::getAnimator()
{
	return _ani;
}

std::vector<VertexData> Mesh::getVertexData()
{
	return _ani->getCurrentFrame();
}

bool Mesh::isVertexDataDirty()
{
	return _ani->isDirty();
}
