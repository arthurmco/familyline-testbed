#include "TerrainRenderer.hpp"

using namespace Familyline::Graphics;

TerrainMaterial::TerrainMaterial()
    : m(nullptr), pshader(nullptr), vshader(nullptr)
{}

TerrainMaterial::TerrainMaterial(Material* mat)
    : m(mat), pshader(nullptr), vshader(nullptr)
{}

TerrainMaterial::TerrainMaterial(Material* mat, Shader* ps, Shader* vs)
    : m(mat), pshader(ps), vshader(vs)
{}
      
