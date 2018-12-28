/***
    Familyline 3D renderer

    Copyright (C) 2016-2018 Arthur Mendes.

***/

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <vector>

#include "GFXExceptions.hpp"

#include "VertexData.hpp"
#include "TerrainRenderer.hpp"
#include "ShaderProgram.hpp"
#include "SceneManager.hpp"
#include "Mesh.hpp"
#include "Light.hpp"
#include "MaterialManager.hpp"
#include "Log.hpp"


namespace familyline::graphics {
    struct VertexHandle {
	GLuint vao;
	GLuint vboPos, vboTex, vboNorm;
	unsigned int vsize;
	VertexInfo vi;
	GLuint vao_bbox = 0;
	ShaderProgram* sp;
    };
    
#define MAX_RENDERER_LIGHTS 4
    struct LightRenderInfo {
	glm::vec3 lightPosition;
	glm::vec3 lightColor;
	float lightStrength;
    };

    struct SceneIDCache {
	int ID;
	int lastcheck;
	VertexHandle vhandle;
	GLuint bbvao;
    };
    
    class Renderer
    {
    protected:
        SceneManager* _scenemng;
        std::vector<VertexHandle> _vertices;
	std::vector<VertexHandle> _bb_vaos;

	ShaderProgram *sForward, *sLines;
	Texture* fake_tex;

	unsigned int lightCount = 0;
	LightRenderInfo lri[MAX_RENDERER_LIGHTS];

	int lastCheck = 0;
	std::vector<SceneIDCache> _last_IDs;
	
	/* Custom properties */
	bool renderBBs = false;

        void InitializeLibraries();
        virtual void InitializeShaders();

        void SetMaterial(int index);

	/* Render object bounding boxes */
	void RenderBoundingBoxes();
	virtual int AddBoundingBox(Mesh*, glm::vec3 color);
	void RemoveBoundingBox(GLuint);

    public:
	/* Virtual functions, so we can mock them*/
	
        Renderer();
        virtual ~Renderer();

	/**
	 * Initialize the renderer shaders and libraries
	 */
	virtual void initialize();
	
        SceneManager* GetSceneManager() const;
        void SetSceneManager(SceneManager*);

        void UpdateFrames();
	void UpdateObjects();

        /** 
	 * Returns true if rendered successfully 
	 */
	bool Render(TerrainRenderer*);

	virtual const VertexHandle addVertexData(const VertexData& vdata,
						 const VertexInfo vinfo);
	void updateVertexData(VertexHandle vhandle,
			      const VertexData& vdata, const VertexInfo vinfo);
	void removeVertexData(VertexHandle&& vhandle);
	

	void SetBoundingBox(bool);
    };

}

#endif /* end of include guard: RENDERER_HPP */
