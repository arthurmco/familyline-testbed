#pragma once

/**
 * Binds a texture handle to a texture asset
 *
 * Copyright (C) 2021 Arthur Mendes
 */

#include <client/graphical/asset_object.hpp>
#include <client/graphical/texture_environment.hpp>
#include <client/graphical/texture_manager.hpp>
#include <optional>

namespace familyline::graphics
{
class TextureAsset : public AssetObject
{
public:
    TextureAsset(std::string name, std::string path)
        : name_(name), path_(path) {}

    tl::expected<TextureHandle, ImageError> getHandle(TextureManager& tm) { return tm.loadTexture(path_); }

    virtual AssetType getAssetType() const { return AssetType::TextureAsset; }    
    
private:
    std::string name_;
    std::string path_;

};

}  // namespace familyline::graphics
