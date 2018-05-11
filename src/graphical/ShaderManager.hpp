/**
  * The shader manager
  * Manages shaders, and makes you able to refer them by name
  *
  * Copyright (C) 2018 Arthur M
  */

#ifndef SHADER_MANAGER_HPP
#define	SHADER_MANAGER_HPP

#include <unordered_map>
#include <string>
#include "ShaderProgram.hpp"

namespace Familyline::Graphics
{
	class ShaderManager {
	private:
		static std::unordered_map<std::string, ShaderProgram*> _shaders;

	public:
		static const char* DefaultShader;
		static void Add(const char* name, ShaderProgram* shader);

		/**
		 * Gets a shader by its name
		 *
		 * @returns A pointer to the shader program, or nullptr if not found
		 */
		static ShaderProgram* Get(const char* name);
		

	};
}

#endif // !SHADER_MANAGER_HPP
