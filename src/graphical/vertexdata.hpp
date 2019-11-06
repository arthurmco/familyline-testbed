#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <any>
#include <map>

#include "shader.hpp"

namespace familyline::graphics {

	/**
	 * Contains base vertex data
	 */
	struct VertexData {
		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texcoords;

		/// The indices (Not used yet)
		///
		/// Each one of them corresponds to an unique set of vertices, normals and
		/// texcoords
		/// i.e, the index 3 means that the vertex is made of the position at index 2,
		/// normal at index 2 and texcoord at index 2 (the index start at 1)
		std::vector<unsigned int> indices;
	};

	enum class VertexRenderStyle {
		Triangles, ///< The vertices make triangles
		PlotLines  ///< The vertices make lines. Useful for debug lines
	};

	/**
	 * Stored shader configurations
	 *
	 * We store a set of configurations separately from each shader, so we can use
	 * different uniform values for the same shader. Doing that, we do not create
	 * a shader for each unique value set, preserving video memory space.
	 */
	struct ShaderState {
		ShaderProgram* shader;
		std::map<std::string, glm::mat4> matrixUniforms;
		std::map<std::string, glm::vec3> vec3Uniforms;

		// Set the stored shader uniforms in the shader
		void updateShader();
	};

    struct VertexHandle;

	/**
	 * Contains metainformation about the vertex set
	 */
	struct VertexInfo {

		/**
		 * When you retrieve the vertex list from an animation, it
		 * will come as a list of VertexDatas
		 * \see VertexDataGroup
		 *
		 * Each VertexData corresponds to a group of vertices that
		 * use a unique combination of material and shader, and are
		 * grouped together
		 *
		 * The index is a reference to that list
		 */
		int index = 0;

		int materialID = 0;     ///< Material ID for this vertex set
		ShaderState shaderState;

		VertexRenderStyle renderStyle = VertexRenderStyle::Triangles;
        
		VertexInfo(int index, int materialID, ShaderProgram* shader,
                   VertexRenderStyle style);
	};
}
