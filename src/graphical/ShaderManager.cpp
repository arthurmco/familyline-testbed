#include "ShaderManager.hpp"

using namespace Tribalia::Graphics;

std::unordered_map<std::string, ShaderProgram*> ShaderManager::_shaders;
const char* ShaderManager::DefaultShader = "forward";

void ShaderManager::Add(const char* name, ShaderProgram* shader)
{
	ShaderManager::_shaders[std::string{ name }] = shader;
}

/**
* Gets a shader by its name
*
* @returns A pointer to the shader program, or nullptr if not found
*/
ShaderProgram* ShaderManager::Get(const char* name)
{
	auto s = ShaderManager::_shaders.find(name);
	if (s == ShaderManager::_shaders.end())
		return nullptr;

	return s->second;
}