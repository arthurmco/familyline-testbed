/***
    Terrain rendering system

    Copyright (C) 2016 Arthur M.

***/

#ifndef TERRAINRENDERER_HPP
#define TERRAINRENDERER_HPP

#include "Terrain.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "VertexData.hpp"

#define SEC_SIZE 0.50

namespace Tribalia {
namespace Graphics {


    struct TerrainDataInfo {
        TerrainData* data;
        GLint vao;
    };

    class TerrainRenderer {
    private:
        TerrainDataInfo* _tdata = nullptr;
        Terrain* _t;
        Camera* _cam  = nullptr;
        Renderer* _rend;
        glm::mat4 _wmatrix = glm::mat4(1.0f);

    public:
        TerrainRenderer(Renderer*);

        void SetTerrain(Terrain*);
        Terrain* GetTerrain();

        void SetCamera(Camera*);
        Camera* GetCamera();

        /*  Check the terrains that needs to be rendered and
            send them to the renderer.
            Will also cache terrain textures too */
        void Update();

    };

}
}



#endif /* end of include guard: TERRAINRENDERER_HPP */
