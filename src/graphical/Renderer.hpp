/***
    Tribalia 3D renderer

    Copyright (C) 2016 Arthur M.

***/

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <vector>

#include "GFXExceptions.hpp"

#include "VertexData.hpp"
#include "ShaderProgram.hpp"
#include "SceneManager.hpp"
#include "Mesh.hpp"
#include "Light.hpp"
#include "MaterialManager.hpp"
#include "../Log.hpp"

#ifndef RENDERER_HPP
#define RENDERER_HPP

namespace Tribalia {
namespace Graphics {

    struct VertexRenderInfo {
        VertexData* vd;
        glm::mat4* worldMat;
        GLuint vao;
        GLuint vbo_pos, vbo_norm, vbo_tex;
        int material_offsets[9];
        GLuint vao_bbox = 0;
		ShaderProgram* sp;
    };

	struct LightRenderInfo {
		int lightCount = 0;
		glm::vec3 lightPositions[4];
		glm::vec3 lightColors[4];
		float lightStrengths[4];
		int lightIDs[4];
	};


    class Renderer
    {
    private:
        SceneManager* _scenemng;
        std::vector<VertexRenderInfo> _vertices;
		std::vector<VertexRenderInfo> _bb_vaos;

		ShaderProgram *sForward, *sLines;
		Texture* fake_tex;

		/* Custom properties */
		bool renderBBs = false;

        void InitializeLibraries();
        void InitializeShaders();

        void SetMaterial(int index);

		/* Render object bounding boxes */
		void RenderBoundingBoxes();
		int AddBoundingBox(Mesh*, glm::vec3 color);
		void RemoveBoundingBox(GLuint);

    public:
        Renderer();
        ~Renderer();

        SceneManager* GetSceneManager() const;
        void SetSceneManager(SceneManager*);

        void UpdateFrames();
		void UpdateObjects();
        /* Returns true if rendered successfully */

        bool Render();

        /* Add vertex data structure. Returns its VAO ID */
        GLint AddVertexData(VertexData*, glm::mat4* worldMatrix);
        void RemoveVertexData(GLuint vaoid);
        void UpdateVertexData(int vbo, glm::vec3* data, size_t count);

		void SetBoundingBox(bool);
    };

}
}


#endif /* end of include guard: RENDERER_HPP */
