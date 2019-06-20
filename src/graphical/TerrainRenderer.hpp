/***
    Terrain rendering system

    Renders the terrain using a custom shader

    Copyright (C) 2016 Arthur Mendes.

***/
#ifndef TERRAINRENDERER_HPP
#define TERRAINRENDERER_HPP


#include <map>

#include "../logic/Terrain.hpp"
#include "camera.hpp"
#include "vertexdata.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>

/* Size of each slot, in OpenGL units */
#define SEC_SIZE 0.50

/* What each unit of height means in OpenGL units */
#define SEC_HEIGHT 0.01

namespace familyline::graphics {

    /**
     * A stripped-down version of a vertex data.
     *
     * We have only the vertex and normals.
     * We do not have material info here, only the terrain types, because the terrain material
     * can be guessed by the ID
     */
    struct TerrainVertexData {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texcoords; //Just for texture mapping

	std::vector<unsigned int> indices;
	std::vector<unsigned int> terrain_ids;
    };

    struct TerrainDataInfo {
	unsigned int secidx;
	size_t vcount;
	GLuint vao;
	Texture* texture;
    };

    /* Width and height of the terrain individual textures
       aka the size of a texture "square", 1x1 */
    constexpr int ImageHeight = 16;
    constexpr int ImageWidth = 16;

    class TerrainRenderer {
    private:
	
	std::vector<TerrainDataInfo> _tdata;

	familyline::logic::Terrain* _t;

	Camera* _cam  = nullptr;
        glm::mat4 _wmatrix = glm::mat4(1.0f);

        /**
	 * Create a terrain vertex data with the data from a specific terrain 
	 */
	TerrainVertexData GetTerrainVerticesFromSection(unsigned int section);

	GLuint CreateVAOFromTerrainData(TerrainVertexData& tvd);

	Texture* terrain_tex = nullptr;
	
	bool needs_update = true;

	std::array<unsigned int, ImageWidth*ImageHeight> GetTerrainTexture(unsigned int type_id);

	/**
	 * Generate terrain texture from a certain terrain data 
	 *
	 * \param data The said terrain data
	 */
	Texture* GenerateTerrainSlotTexture(familyline::logic::TerrainData* data);
	
    public:
        TerrainRenderer();

        void SetTerrain(familyline::logic::Terrain*);
        familyline::logic::Terrain* GetTerrain();

        void SetCamera(Camera*);
        Camera* GetCamera();

	/**
	 * Update the terrain VAO with the current rendering information 
	 */
        void Update();

	/**
	 * Render the terrain 
	 */
	void Render();

    };


    /**
     * Convert a terrain point from graphical to game space 
     */
    glm::vec3 GraphicalToGameSpace(glm::vec3 graphical);

    /**
     * Convert a terrain point from game to graphical space
     */
    glm::vec3 GameToGraphicalSpace(glm::vec3 game);

}



#endif /* end of include guard: TERRAINRENDERER_HPP */
