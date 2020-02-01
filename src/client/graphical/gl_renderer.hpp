#pragma once

#include <vector>
#include "renderer.hpp"

namespace familyline::graphics {

class GLRenderer;

struct GLVertexHandle final : public VertexHandle {
private:
	GLRenderer& _renderer;
	
public:
    int vao;
	int vboPos, vboNorm, vboTex;
	size_t vsize;

    
    GLVertexHandle(int vao, GLRenderer& renderer, VertexInfo& vinfo)
		: VertexHandle(vinfo), vao(vao), _renderer(renderer)
	{}


	virtual bool update(VertexData& vd);
	virtual bool remove();
	virtual bool recreate(VertexData& vd, VertexInfo& vi);
};

class GLRenderer : public Renderer {
private:
	std::vector<std::unique_ptr<GLVertexHandle>> _vhandle_list;
	
	ShaderProgram* _sForward = nullptr;
	ShaderProgram* _sLines = nullptr;

public:
	GLRenderer();

	virtual VertexHandle* createVertex(VertexData& vd, VertexInfo& vi);
	virtual void render(Camera* c);

	/**
	 * Create a raw VAO, with the vbox for position, normal and texture. 
	 *
	 * Useful when we need to only retrieve the basic elements VAO, without an object
	 */
	std::tuple<int, int, int, int> createRaw(VertexData& vd);

    
    virtual void removeVertex(VertexHandle* vh);
	
	//virtual LightHandle createLight(LightData& ld) = 0;
};
}
