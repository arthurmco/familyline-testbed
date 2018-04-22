/***
    Raw vertex data

    Copyright (C) 2016, 2018 Arthur M.

***/

#ifndef VERTEXDATA_HPP
#define VERTEXDATA_HPP

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <functional>

#include "ShaderProgram.hpp"


namespace Tribalia::Graphics {

	struct VertexData;
	class Mesh;
	class Camera;

	/* Base animator class */
	class BaseAnimator {
	protected:
		VertexData* vdata;
	public:
		BaseAnimator(VertexData* v)
			: vdata(v)
		{}

		/* Load a vertex */
		virtual const std::vector<glm::vec3>& getVertices(unsigned frameno);
		virtual size_t getFrameCount() const { return 1; }

		virtual ~BaseAnimator() {}
	};
	
	enum VertexRenderStyle {
		Triangles,

		PlotLines
	};

	/* Function pointer to a shader setup function
	   This function receives five parameters: 
	    - the shader itself
	    - a model matrix, used to put the model somewhere
		- a view matrix, used to transform the camera on the scene (the pointing angle)
		- a projection matrix, used to transform the way you see the scene.
		- the camera
		
	   Those parameters serves to setup the shader for that specific shader, so the
	   renderer doesn't need to worry about the shader itself.
	*/
	typedef std::function<void(ShaderProgram*, glm::mat4, glm::mat4, 
		glm::mat4, Camera*)> ShaderSetupFunction;
	
	/* Basic vertex information */
	struct VertexData {
		std::vector<glm::vec3> Positions;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexCoords;
		std::vector<int> MaterialIDs;

		VertexRenderStyle render_format = VertexRenderStyle::Triangles;

		// The shader used
		ShaderProgram* shader;
		std::unique_ptr<BaseAnimator> animator;
		int vbo_pos;

		// Pointer to the owning mesh, so we can draw the bounding boxes
		Mesh* meshptr;

		// Shader setup function
		ShaderSetupFunction fnShaderSetup;

		VertexData();
		VertexData(VertexData& vd);
		VertexData(BaseAnimator* a);

		/* Sets the shader and the shader setup function together, so we
		 * don't fuck up something 
		 */
		void SetShader(ShaderProgram* s);
	};

}

#endif /* end of include guard: VERTEXDATA_HPP */
