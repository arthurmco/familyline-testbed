/***
    Raw vertex data

    Copyright (C) 2016, 2018 Arthur M.

***/

#ifndef VERTEXDATA_HPP
#define VERTEXDATA_HPP

#include <vector>
#include <glm/glm.hpp>
#include <memory>

#include "ShaderProgram.hpp"

namespace Tribalia::Graphics {

	struct VertexData;
	class Mesh;

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

		VertexData();
		VertexData(VertexData& vd);
		VertexData(BaseAnimator* a);

	};

}

#endif /* end of include guard: VERTEXDATA_HPP */
