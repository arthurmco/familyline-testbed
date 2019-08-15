/*
  Represents an asset object

  It is a generic object with a method that returns its type.

  Copyright (C) 2018-2019 Arthur M
*/

#ifndef ASSETOBJECT_HPP
#define ASSETOBJECT_HPP

#include <vector>

namespace familyline::graphics {

    enum AssetType {
        MeshAsset,
        MaterialAsset,
        TextureAsset,
        UnknownAsset
    };

    /**
     * An opaque interface. Represents an asset
     *
     * Everything that is considered an asset, such as a mesh or a texture, should
     * inherit from this class
     */
    class AssetObject {
    public:
        virtual AssetType getAssetType() const = 0;
    };


}


#endif /* ASSETOBJECT_HPP */
