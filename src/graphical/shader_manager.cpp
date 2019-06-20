#include "shader_manager.hpp"
#include "logger.hpp"
#include "exceptions.hpp"

using namespace familyline::graphics;

std::unique_ptr<ShaderManager> GFXService::_manager;

void ShaderManager::use(ShaderProgram& s)
{
	if (lastShader == s.getHandle())
		return;

	auto handle = s.getHandle();
	//LoggerService::getLogger()->write(
	//	"shader-manager", LogType::Debug, "shader %#x is in use now", handle);

	glUseProgram(handle);
	lastShader = handle;
}


void ShaderManager::addShader(ShaderProgram* s)
{
	_shaders[std::string{ s->getName() }] = s;
}

ShaderProgram* ShaderManager::getShader(std::string_view name)
{
	auto res = _shaders.find(std::string{ name });
	if (res == _shaders.end()) {
		char s[256];
		sprintf(s, "Shader %s not found in shader manager", name.data());
		throw shader_exception(std::string_view{ s }, 2);
	}

	return res->second;
}
