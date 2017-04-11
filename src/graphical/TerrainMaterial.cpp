#include "TerrainRenderer.hpp"

using namespace Tribalia::Graphics;

TerrainMaterial::TerrainMaterial()
    : m(nullptr), pshader(nullptr), vshader(nullptr)
{}

TerrainMaterial::TerrainMaterial(Material* mat)
    : m(mat), pshader(nullptr), vshader(nullptr)
{}

TerrainMaterial::TerrainMaterial(Material* mat, Shader* ps, Shader* vs)
    : m(mat), pshader(ps), vshader(vs)
{}
      
