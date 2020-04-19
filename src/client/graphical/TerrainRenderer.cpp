#include <client/graphical/TerrainRenderer.hpp>
#include <client/graphical/TextureOpener.hpp>
#include <client/graphical/gfx_service.hpp>

#include <client/graphical/shader_manager.hpp>
#include <common/logger.hpp>

using namespace familyline::graphics;
using namespace familyline::logic;

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
    auto& log = LoggerService::getLogger();

    _t = t;

    log->write("terrain-renderer", LogType::Info, "Added terrain with %d sections",
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


            /* Use x = 0 when x is even, x = 1 when x is odd
               This will make the texture repeat across the terrain
            */
            tvd.texcoords.push_back(glm::vec2(float(x) / SECTION_SIDE, float(y) / SECTION_SIDE));

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
            for (auto i = 0; i < q; i++) { vnormal += norms[i]; }

            tvd.normals[idx] = glm::normalize(vnormal);
        }
    }

    return tvd;
}


GLuint TerrainRenderer::CreateVAOFromTerrainData(TerrainVertexData& tvd)
{
    auto& sm = GFXService::getShaderManager();
    auto forward = sm->getShader("forward");

    auto err = glGetError();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    auto fnGetAttrib =
        [&](const char* name) {
            return glGetAttribLocation(forward->getHandle(), name);
        };

    printf("|| %d %d %d ||", fnGetAttrib("position"), fnGetAttrib("normal"), fnGetAttrib("texcoord"));
    
    GLuint vboVertices, vboNormals, vboTextures, vboElement;
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, tvd.vertices.size() * sizeof(glm::vec3),
                 tvd.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(fnGetAttrib("position"), 3, GL_FLOAT,
                          GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, tvd.normals.size() * sizeof(glm::vec3),
                 tvd.normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(fnGetAttrib("normal"), 3, GL_FLOAT,
                          GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboTextures);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
    glBufferData(GL_ARRAY_BUFFER, tvd.texcoords.size() * sizeof(glm::vec2),
                 tvd.texcoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(fnGetAttrib("texcoord"), 2, GL_FLOAT,
                          GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &vboElement);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboElement);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tvd.indices.size() * sizeof(unsigned int),
                 tvd.indices.data(), GL_STATIC_DRAW);

    auto& log = LoggerService::getLogger();
    err = glGetError();
    if (err != GL_NO_ERROR) {
        log->write("terrain-renderer", LogType::Warning,
                   "GL error %#x while creating VAO", err);
    }

    return vao;
}

/* Update the terrain VAO with the current rendering information */
void TerrainRenderer::Update()
{
    auto& log = LoggerService::getLogger();
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

    log->write("terrain-renderer", LogType::Debug,
               "Starting terrain texture generation for section 0");
    tdi.texture = this->GenerateTerrainSlotTexture(_t->GetSection(0));
    log->write("terrain-renderer", LogType::Debug,
               "Terrain texture for section 0 created");

    this->_tdata.push_back(tdi);
}


void TerrainRenderer::Render()
{
    auto& log = LoggerService::getLogger();
    auto err = glGetError();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

    auto& sm = GFXService::getShaderManager();
    auto forward = sm->getShader("forward");
    sm->use(*forward);

	forward->setUniform("mWorld", glm::mat4(1.0));
	forward->setUniform("mView",  _cam->GetViewMatrix());
    forward->setUniform("mProjection",  _cam->GetProjectionMatrix());
    forward->setUniform("diffuse_color", glm::vec3(0.5, 0.5, 0.5));
    forward->setUniform("ambient_color", glm::vec3(0.1, 0.1, 0.1));

    forward->setUniform("tex_amount", 1.0f);

    for (const auto& tdi : this->_tdata) {
        glBindTexture(GL_TEXTURE_2D, tdi.texture->GetHandle());
		glBindVertexArray(tdi.vao);
		glDrawElements(GL_TRIANGLES, tdi.vcount, GL_UNSIGNED_INT, 0);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        log->write("terrain-renderer", LogType::Warning, "GL error %#x", err);
    }
}

/* Convert a terrain point from graphical to game space */
glm::vec3 familyline::graphics::GraphicalToGameSpace(glm::vec3 graphical)
{
    return glm::vec3(graphical.x / SEC_SIZE,
                     graphical.y / SEC_HEIGHT, graphical.z / SEC_SIZE);

}



/* Convert a terrain point from game to graphical space*/
glm::vec3 familyline::graphics::GameToGraphicalSpace(glm::vec3 game)
{
    return glm::vec3(game.x * SEC_SIZE,
                     game.y * SEC_HEIGHT, game.z * SEC_SIZE);
}
