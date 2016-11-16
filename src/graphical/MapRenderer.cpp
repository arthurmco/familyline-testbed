#include "MapRenderer.hpp"

using namespace Tribalia::Graphics;
using namespace Tribalia::Logic;

MapRenderer::MapRenderer(Tribalia::Logic::Map* map)
{
    _map = map;
}

/*  Create terrain vertex data on the variable 'd'.
    Returns true if the chunk could be rendered */
bool MapRenderer::DrawChunk(int x, int y, VertexData* d)
{
    d->Positions.clear();
    d->Normals.clear();

    MaterialData md;
    md.diffuseColor = glm::vec3{0,0.8,0};
    md.ambientColor = glm::vec3{0,0.008,0};
    md.specularColor = glm::vec3{0,1,0};

    Material* m = new Material{0xfffe, "terrain", md};
    MaterialManager::GetInstance()->AddMaterial(m);


    for (int yPos = (y * CHUNK_SIZE); yPos < ((x+1) * CHUNK_SIZE); yPos++) {
        if (yPos >= _map->GetHeight()) break;

        int w = _map->GetWidth();
        for (int xPos = (x * CHUNK_SIZE); xPos < ((x+1) * CHUNK_SIZE); xPos++){

            if (xPos >= w) break;

            MapSlot* ml = _map->GetMapData();
            float h = ml[yPos*w + xPos].elevation;
            float hx = ((xPos+1) < w) ? ml[yPos*w + xPos+1].elevation : 0.0f;
            float hy = ((yPos+1) < _map->GetHeight()) ? ml[(yPos+1)*w + xPos].elevation : 0.0f;

            float hxy = 0.0f;
            if (((xPos+1) < w) && ((yPos+1) < _map->GetHeight())) {
                hxy = ml[(yPos+1)*w + xPos + 1].elevation;
            }

            for (int i = 0; i < 6; i++) {
                d->Normals.push_back(glm::vec3(0,1,0));
                d->MaterialIDs.push_back(m->GetID());
            }

            d->Positions.push_back(glm::vec3(xPos, h, yPos));
            d->Positions.push_back(glm::vec3(xPos + CHUNK_SIZE, hx, yPos));
            d->Positions.push_back(glm::vec3(xPos, hy, yPos + CHUNK_SIZE));

            d->Positions.push_back(glm::vec3(xPos + CHUNK_SIZE, hx, yPos));
            d->Positions.push_back(glm::vec3(xPos + CHUNK_SIZE, hxy, yPos  + CHUNK_SIZE));
            d->Positions.push_back(glm::vec3(xPos, hy, yPos + CHUNK_SIZE));
        }
    }

    /* Return true only if we have some vertices to draw */
    return (d->Positions.size() > 0);
}
