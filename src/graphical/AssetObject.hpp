/*
  Represents an asset object

  Truly, it's only an union with every known class instance who is an asset

  This is done this way because the asset objects doesn't have any specific
  function or property yet, so they are essentially just a void pointer

  Copyright (C) 2018 Arthur M
*/

#ifndef ASSETOBJECT_HPP
#define ASSETOBJECT_HPP

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include <vector>

namespace familyline::graphics {

    union AssetObject {
	Mesh* mesh;
	Texture* texture;
	std::vector<Material*>* materialvec;
    };

    AssetObject create_asset_from_mesh(Mesh* m);
    AssetObject create_asset_from_material(std::vector<Material*>* m);
    AssetObject create_asset_from_texture(Texture* t);

    AssetObject create_asset_from_null();

}


#endif /* ASSETOBJECT_HPP */
