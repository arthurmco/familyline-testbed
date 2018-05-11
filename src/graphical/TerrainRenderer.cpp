#include "TerrainRenderer.hpp"
#include "TextureOpener.hpp"

using namespace Familyline::Graphics;
using namespace Familyline::Logic;

static Material m = Material("terrain", MaterialData(0.8f, 0.95f, 0.2f));

/* Return a valid material, or the above material 'm' */
#define MATERIAL_GET(id) \
    ((_texturemap[id].m != nullptr) ? _texturemap[id].m->GetID() : m.GetID())
     
    
    
TerrainRenderer::TerrainRenderer(Renderer* r)
    : _rend(r)
{
    MaterialData matdata;
    matdata.diffuseColor = glm::vec3(0.4, 0.6, 0.0);
    matdata.ambientColor = glm::vec3(0.01, 0.01, 0.0);
    MaterialManager::GetInstance()->AddMaterial(&m);

    Material* t0 = new Material("grass",
	MaterialData(glm::vec3(0.6, 0.6, 0.6),
		     glm::vec3(0.4, 1.0, 0.4),
		     glm::vec3(0.02)));
    t0->SetTexture(TextureOpener::OpenTexture("textures/terrain/grass.png"));
    MaterialManager::GetInstance()->AddMaterial(t0);
    
    this->AddMaterial(0, t0);
						       
}

void TerrainRenderer::SetTerrain(Terrain* t)
{
    if (_tdata)
        delete _tdata;

    _t = t;
    _tdata = new TerrainDataInfo[t->GetSectionCount()];

    Log::GetLog()->Write("terrain-renderer", "Added terrain with %d sections",
	 t->GetSectionCount());

    for (int i = 0; i < t->GetSectionCount(); i++) {
	_tdata[i].data = nullptr;
	_tdata[i].vao = -1;
    }

}

Terrain* TerrainRenderer::GetTerrain() { return _t; }

void TerrainRenderer::SetCamera(Camera* c) { _cam = c; }
Camera* TerrainRenderer::GetCamera() { return _cam; }

bool needs_update = true;

/*  Check the terrains that needs to be rendered and
    send them to the renderer.
    Will also cache terrain textures too */
void TerrainRenderer::Update()
{
    if (!needs_update) {
	return;
    }

    needs_update = false;


    int w = ceil(_t->GetWidth() / (SECTION_SIDE*1.0));
    int h = ceil(_t->GetHeight() / (SECTION_SIDE*1.0));
    float offsetX = 0, offsetY = 0;

    for (int y = 0; y < h; y++) {
        offsetY = (SECTION_SIDE * y * SEC_SIZE);
        offsetX = 0;

        int hh = 0, hx = 0, hy = 0, hxy = 0;

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

		int mid = 0, midx = 0, midy = 0, midxy = 0;
		
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

			mid = _tdata[i].data->data[ey*SECTION_SIDE + ex].terrain_type;
                        if (ex+1 < exMax)
			    midx = _tdata[i].data->data[ey*SECTION_SIDE + (ex+1)].terrain_type;
			if (ey+1 < eyMax)
			    midy = _tdata[i].data->data[(ey+1)*SECTION_SIDE + (ex)].terrain_type;
			
                        if (ex+1 < exMax && ey+1 < eyMax)
			    midxy = _tdata[i].data->data[(ey+1)*SECTION_SIDE + (ex+1)].terrain_type;

			
			// Add the vertices
			glm::vec3 t1, t1x, t1y;
			t1 = glm::vec3(offsetX+px, hh * SEC_HEIGHT, offsetY+py);
			t1x = glm::vec3(offsetX+px, hy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			t1y = glm::vec3(offsetX+px+SEC_SIZE, hxy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			
                        vd->Positions.push_back(t1);
                        vd->Positions.push_back(t1x);
                        vd->Positions.push_back(t1y);
			
			vd->TexCoords.push_back(glm::vec2(0,0));
			vd->TexCoords.push_back(glm::vec2(0,1));
			vd->TexCoords.push_back(glm::vec2(1,0));

			vd->MaterialIDs.push_back(MATERIAL_GET(mid));
			vd->MaterialIDs.push_back(MATERIAL_GET(midx));
			vd->MaterialIDs.push_back(MATERIAL_GET(midy));

			glm::vec3 t2x, t2y;
			t2y = glm::vec3(offsetX+px+SEC_SIZE, hxy * SEC_HEIGHT, offsetY+py+SEC_SIZE);
			t2x = glm::vec3(offsetX+px+SEC_SIZE, hx * SEC_HEIGHT, offsetY+py);
			   
                        vd->Positions.push_back(t1);
                        vd->Positions.push_back(t2y);
                        vd->Positions.push_back(t2x);

			vd->TexCoords.push_back(glm::vec2(0,0));
			vd->TexCoords.push_back(glm::vec2(1,1));
			vd->TexCoords.push_back(glm::vec2(1,0));

			vd->MaterialIDs.push_back(MATERIAL_GET(mid));
			vd->MaterialIDs.push_back(MATERIAL_GET(midxy));
			vd->MaterialIDs.push_back(MATERIAL_GET(midx));

			
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
		        
                        px += SEC_SIZE;
                    }
                    py += SEC_SIZE;
                    px = 0;
                }

                _tdata[i].vao = _rend->AddVertexData(vd, &_wmatrix);
                Log::GetLog()->Write("terrain-renderer",
				     "Generated terrain data for section %d, (%d %d), vao is %d", i, x, y, _tdata[i].vao);
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


void TerrainRenderer::AddMaterial(unsigned int id, Material* mat)
{
    _texturemap[id] = TerrainMaterial(mat);
}

