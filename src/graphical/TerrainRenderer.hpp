/***
    Terrain rendering system

    Copyright (C) 2016 Arthur Mendes.

***/

#include <map>

#include "../logic/Terrain.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "VertexData.hpp"

#ifndef TERRAINRENDERER_HPP
#define TERRAINRENDERER_HPP

/* Size of each slot, in OpenGL units */
#define SEC_SIZE 0.50

/* What each unit of height means in OpenGL units */
#define SEC_HEIGHT 0.01

namespace Tribalia {
namespace Graphics {


    struct TerrainDataInfo {
        Tribalia::Logic::TerrainData* data;
        GLint vao;
    };

    struct TerrainMaterial {
	Material* m;
	Shader* pshader;
	Shader* vshader;

	TerrainMaterial();
	TerrainMaterial(Material* m);
	TerrainMaterial(Material* m, Shader* ps, Shader* vs);
    };    

    class TerrainRenderer {
    private:
        TerrainDataInfo* _tdata = nullptr;
        Tribalia::Logic::Terrain* _t;
        Camera* _cam  = nullptr;
        Renderer* _rend;
        glm::mat4 _wmatrix = glm::mat4(1.0f);

	// Maps texture IDs into real textures
	std::map<unsigned int, TerrainMaterial> _texturemap;
	
    public:
        TerrainRenderer(Renderer*);

        void SetTerrain(Tribalia::Logic::Terrain*);
        Tribalia::Logic::Terrain* GetTerrain();

        void SetCamera(Camera*);
        Camera* GetCamera();

	void AddMaterial(unsigned int id, Material* m);

        /* Convert a terrain point from graphical to game space */
        static glm::vec3 GraphicalToGameSpace(glm::vec3 graphical);

	/* Convert a terrain point from game to graphical space*/
	static glm::vec3 GameToGraphicalSpace(glm::vec3 game);

        /*  Check the terrains that needs to be rendered and
            send them to the renderer.
            Will also cache terrain textures too */
        void Update();

    };

}
}



#endif /* end of include guard: TERRAINRENDERER_HPP */
