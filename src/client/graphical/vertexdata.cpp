#include <client/graphical/vertexdata.hpp>

using namespace familyline::graphics;

VertexInfo::VertexInfo(int index, int materialID, ShaderProgram* shader, VertexRenderStyle style)
	: index(index), materialID(materialID), renderStyle(style)
{
	this->shaderState.shader = shader;
}

void ShaderState::updateShader()
{
	for (auto& state : this->matrixUniforms) {
		this->shader->setUniform(state.first, state.second);
	}

	for (auto& state : this->vec3Uniforms) {
		this->shader->setUniform(state.first, state.second);
	}
}
