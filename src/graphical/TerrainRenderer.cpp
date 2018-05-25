#include "TerrainRenderer.hpp"
#include "TextureOpener.hpp"
#include "../config.h"

#include "ShaderManager.hpp"

using namespace Familyline::Graphics;
using namespace Familyline::Logic;

/***
Renderer


1    2    3    4    5
+----+----+----+----+
|    |    |    |    |
|w+1 |w*2 |    |    |
+----+----+----+----+
|    |    |    |    |
|2w+1|    |    |    |
+----+----+----+----+
|    |    |    |    |
|    |    |    |    |
+----+----+----+----+

***/

TerrainRenderer::TerrainRenderer()
{

}

void TerrainRenderer::SetTerrain(Terrain* t)
{
    _t = t;
 
    Log::GetLog()->Write("terrain-renderer", "Added terrain with %d sections",
	 t->GetSectionCount());

    needs_update = true;
}

Terrain* TerrainRenderer::GetTerrain() { return _t; }

void TerrainRenderer::SetCamera(Camera* c) { _cam = c; }
Camera* TerrainRenderer::GetCamera() { return _cam; }


/* Create a terrain vertex data with the data from a specific terrain */
TerrainVertexData TerrainRenderer::GetTerrainVerticesFromSection(unsigned int section)
{
    TerrainData* tData = _t->GetSection(section);


    TerrainVertexData tvd;

    for (auto y = 0; y < SECTION_SIDE; y++) {
	for (auto x = 0; x < SECTION_SIDE; x++) {
	    const auto idx = (y * SECTION_SIDE + x);
	    const auto height = tData->data[idx].elevation * SEC_HEIGHT;

	    // Send the actual vertex
	    tvd.vertices.push_back(glm::vec3(x * SEC_SIZE, height, y * SEC_SIZE));
	    
	    // Draw the triangle
	    // We can only send the complete box of the terrain if we are not in the borders
	    // Note that the vertices will still be sent, and the border vertices will be referenced
	    // in the previous indices sent.
	    if (y+1 < SECTION_SIDE && x+1 < SECTION_SIDE) {
		tvd.indices.push_back(idx+1);
		tvd.indices.push_back(idx);
		tvd.indices.push_back(idx+SECTION_SIDE);

		tvd.indices.push_back(idx+SECTION_SIDE);
		tvd.indices.push_back(idx+SECTION_SIDE+1);
		tvd.indices.push_back(idx+1);
	    }

	    // Send a placeholder normal
	    tvd.normals.push_back(glm::vec3(0, 1, 0));

	    // Send the terrain type
	    tvd.terrain_ids.push_back(tData->data[idx].terrain_type);
	}
    }

    /* Calculate the normals

       Calculate the normal of every triangle that is part of a single vertex and sum them

          /|\           The vertex we need to calculate the normal is the 'O'.
         / | \          If possible, we need to get the normals of all four triangles there.
        /  |  \
       *---O---*        This might mean that no rough edges will be possible, but a "cliff" terrain
        \  |  /         type will exist (like in AoE2)
         \ | /
          \|/
    */
    for (auto y = 0; y < SECTION_SIDE; y++) {
	for (auto x = 0; x < SECTION_SIDE; x++) {
	    const auto idx = (y * SECTION_SIDE + x);

	    auto fnCalculateTriNormals = [](glm::vec3 e1, glm::vec3 e2, glm::vec3 e3) {
		auto u = e2 - e1;
		auto v = e3 - e1;
		
		return glm::cross(u, v);
	    };

	    const auto& verts = tvd.vertices;
	    glm::vec3 norms[4];
	    int q = 0;

	    if (y > 0) {
		const auto topidx = (y-1) * SECTION_SIDE + x;
		if (x > 0) {
		    const auto leftidx = y * SECTION_SIDE + x-1;
		    norms[q++] = fnCalculateTriNormals(verts[idx], verts[topidx],
						       verts[leftidx]);
		}

		if (x < SECTION_SIDE-1) {
		    const auto rightidx = y * SECTION_SIDE + x+1;
		    norms[q++] = fnCalculateTriNormals(verts[idx], verts[topidx],
						       verts[rightidx]);
		}
	    }

	    if (y < SECTION_SIDE-1) {
		const auto bottomidx = (y+1) * SECTION_SIDE + x;
		if (x > 0) {
		    const auto leftidx = y * SECTION_SIDE + x-1;
		    norms[q++] = fnCalculateTriNormals(verts[idx], verts[bottomidx],
						       verts[leftidx]);
		}

		if (x < SECTION_SIDE-1) {
		    const auto rightidx = y * SECTION_SIDE + x+1;
		    norms[q++] = fnCalculateTriNormals(verts[idx], verts[bottomidx],
						       verts[rightidx]);
		}
	    }

	    auto vnormal = norms[0];
	    for (auto i = 1; i < q; i++) { vnormal += norms[i]; }

	    tvd.normals[idx] = glm::normalize(vnormal);
	}
    }

    return tvd;
}

GLuint TerrainRenderer::CreateVAOFromTerrainData(TerrainVertexData& tvd)
{
    auto err = glGetError();
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint vboVertices, vboNormals, vboElement;
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, tvd.vertices.size() * sizeof(glm::vec3),
		 tvd.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, tvd.normals.size() * sizeof(glm::vec3),
		 tvd.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &vboElement);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElement);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tvd.indices.size() * sizeof(unsigned int),
		 tvd.indices.data(), GL_STATIC_DRAW);

    err = glGetError();
    if (err != GL_NO_ERROR) {
	Log::GetLog()->Warning("terrain-renderer", "GL error %#x", err);
    }
    
    return vao;
}

/* Update the terrain VAO with the current rendering information */
void TerrainRenderer::Update()
{
    if (!needs_update) {
	return;
    }
    needs_update = false;

    auto tvd = this->GetTerrainVerticesFromSection(0);
    auto vao = this->CreateVAOFromTerrainData(tvd);

    TerrainDataInfo tdi;
    tdi.vcount = tvd.indices.size();
    tdi.vao = vao;
    tdi.secidx = 0;

    this->_tdata.push_back(tdi);
}


void TerrainRenderer::Render()
{
    auto err = glGetError();

    glm::mat4 mvpmatrix = _cam->GetProjectionMatrix() * _cam->GetViewMatrix() * glm::mat4(1.0);
    
    ShaderManager::Get("forward")->SetUniform("mvp", mvpmatrix);
    ShaderManager::Get("forward")->SetUniform("model", glm::mat4(1.0));
    ShaderManager::Get("forward")->Use();
    for (const auto& tdi : this->_tdata) {
	glBindVertexArray(tdi.vao);
	glDrawElements(GL_TRIANGLES, tdi.vcount, GL_UNSIGNED_INT, 0);
    }
    
    err = glGetError();
    if (err != GL_NO_ERROR) {
	Log::GetLog()->Warning("terrain-renderer", "GL error %#x", err);
    }
}

/* Convert a terrain point from graphical to game space */
glm::vec3 Familyline::Graphics::GraphicalToGameSpace(glm::vec3 graphical)
{
    return glm::vec3(graphical.x / SEC_SIZE,
		     graphical.y / SEC_HEIGHT, graphical.z / SEC_SIZE);

}



/* Convert a terrain point from game to graphical space*/
glm::vec3 Familyline::Graphics::GameToGraphicalSpace(glm::vec3 game)
{
    return glm::vec3(game.x * SEC_SIZE,
		     game.y * SEC_HEIGHT, game.z * SEC_SIZE);
}
