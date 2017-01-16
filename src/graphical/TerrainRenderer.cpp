#include "TerrainRenderer.hpp"

using namespace Tribalia::Graphics;
using namespace Tribalia::Logic;

TerrainRenderer::TerrainRenderer(Renderer* r)
    : _rend(r)
{
    MaterialData matdata;
    matdata.diffuseColor = glm::vec3(0.4, 0.6, 0.0);
    matdata.ambientColor = glm::vec3(0.01, 0.01, 0.0);
    Material mat = Material("Terrain", matdata);
    MaterialManager::GetInstance()->AddMaterial(&mat);
}

void TerrainRenderer::SetTerrain(Terrain* t)
{
    if (_tdata)
        delete _tdata;

    _t = t;
    _tdata = new TerrainDataInfo[t->GetSectionCount()];

    Log::GetLog()->Write("Added terrain with %d sections",
        t->GetSectionCount());
        for (int i = 0; i < t->GetSectionCount(); i++) {
            _tdata[i].data = nullptr;
            _tdata[i].vao = -1;
        }

}
Terrain* TerrainRenderer::GetTerrain() { return _t; }

void TerrainRenderer::SetCamera(Camera* c) { _cam = c; }
Camera* TerrainRenderer::GetCamera() { return _cam; }

/*  Check the terrains that needs to be rendered and
    send them to the renderer.
    Will also cache terrain textures too */
void TerrainRenderer::Update()
{
    int matid = MaterialManager::GetInstance()->GetMaterial("Terrain")->GetID();

    int w = ceil(_t->GetWidth() / (SECTION_SIDE*1.0));
    int h = ceil(_t->GetHeight() / (SECTION_SIDE*1.0));
    float offsetX = 0, offsetY = 0;

    for (int y = 0; y < h; y++) {
        offsetY = (SECTION_SIDE * y * SEC_SIZE);
        offsetX = 0;

        int hh, hx, hy, hxy;

        for (int x = 0; x < w; x++) {

            int i = x+y*w;

            if (_tdata[i].data == nullptr && i == 0) {
                /* If empty, then build the vertices and send */
                _tdata[i].data = _t->GetSection(i);
                VertexData* vd = new VertexData();
                vd->Positions.reserve(SECTION_SIDE*SECTION_SIDE);
                vd->Normals.reserve(SECTION_SIDE*SECTION_SIDE);
				vd->TexCoords.reserve(SECTION_SIDE*SECTION_SIDE);
                vd->MaterialIDs.reserve(SECTION_SIDE*SECTION_SIDE);

                /* Compute maximum points */
                int exMax = SECTION_SIDE, eyMax = SECTION_SIDE;
                if (x == w-1) {
                    exMax = _t->GetWidth() - (x*SECTION_SIDE);
                }

                if (x == h-1) {
                    eyMax = _t->GetHeight() - (y*SECTION_SIDE);
                }

                float px = 0, py = 0;
                for (int ey = 0; ey < eyMax; ey++) {
                    for (int ex = 0; ex < exMax; ex++) {
                        hh = _tdata[i].data->data[ey*SECTION_SIDE + ex].elevation;

                        if (ex+1 < exMax)
                            hx = _tdata[i].data->data[ey*SECTION_SIDE + (ex+1)].elevation;

                        if (ey+1 < eyMax)
                            hy = _tdata[i].data->data[(ey+1)*SECTION_SIDE + (ex)].elevation;

                        if (ex+1 < exMax && ey+1 < eyMax)
                            hxy = _tdata[i].data->data[(ey+1)*SECTION_SIDE + (ex+1)].elevation;

			// Add the vertices
			glm::vec3 t1, t1x, t1y;
			t1 = glm::vec3(offsetX+px, hh * SEC_HEIGHT, offsetY+py);
			t1x = glm::vec3(offsetX+px, hy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			t1y = glm::vec3(offsetX+px+SEC_SIZE, hxy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			
                        vd->Positions.push_back(t1);
                        vd->Positions.push_back(t1x);
                        vd->Positions.push_back(t1y);

			glm::vec3 t2x, t2y;
			t2y = glm::vec3(offsetX+px+SEC_SIZE, hxy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			t2x = glm::vec3(offsetX+px+SEC_SIZE, hx * SEC_HEIGHT, offsetY+py);
			   
                        vd->Positions.push_back(t1);
                        vd->Positions.push_back(t2y);
                        vd->Positions.push_back(t2x);

			// Make the normals
			glm::vec3 t1a = glm::normalize(t1x - t1);
			glm::vec3 t1b = glm::normalize(t1y - t1);

			glm::vec3 t2a = glm::normalize(t2y - t1);
			glm::vec3 t2b = glm::normalize(t2x - t1);

			glm::vec3 tr1 = glm::max(glm::vec3(0,0,0), glm::cross(t1a, t1b));
			glm::vec3 tr2 = glm::max(glm::vec3(0,0,0), glm::cross(t2a, t2b));
			
                        vd->Normals.push_back(tr1);
                        vd->Normals.push_back(tr1);
                        vd->Normals.push_back(tr2);
                        vd->Normals.push_back(tr1);
                        vd->Normals.push_back(tr2);
                        vd->Normals.push_back(tr2);

			
			for (size_t i = 0; i < 6; i++) {
			    vd->TexCoords.push_back(glm::vec2(1,1));
			    vd->MaterialIDs.push_back(matid);
			}

                        px += SEC_SIZE;
                    }
                    py += SEC_SIZE;
                    px = 0;
                }
                //fclose(f);

                _tdata[i].vao = _rend->AddVertexData(vd, &_wmatrix);
                Log::GetLog()->Write("Generated terrain data for section %d, "
                    "(%d %d), vao is %d", i, x, y, _tdata[i].vao);
            }

            offsetX += SEC_SIZE * SECTION_SIDE;
        }
    }
}

/* Convert a terrain point from graphical to game space */
glm::vec3 TerrainRenderer::GraphicalToGameSpace(glm::vec3 graphical)
{
	return glm::vec3(graphical.x / SEC_SIZE, 
				graphical.y / SEC_HEIGHT, graphical.z / SEC_SIZE);

}

/* Convert a terrain point from game to graphical space*/
glm::vec3 TerrainRenderer::GameToGraphicalSpace(glm::vec3 game)
{
	return glm::vec3(game.x * SEC_SIZE, 
					game.y * SEC_HEIGHT, game.z * SEC_SIZE);
}
