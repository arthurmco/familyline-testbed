#pragma once

#include <client/graphical/asset_manager.hpp>
#include <client/graphical/device.hpp>
#include <client/graphical/material_manager.hpp>
#include <client/graphical/shader_manager.hpp>
#include <client/graphical/texture_manager.hpp>

namespace familyline::graphics
{
class GFXService
{
private:
    static std::unique_ptr<ShaderManager> _shaderm;
    static std::unique_ptr<AssetManager> _assetm;
    static std::unique_ptr<MaterialManager> _materialm;
    static std::unique_ptr<TextureManager> _texturem;
    static std::unique_ptr<Device> _devicem;

public:
    static std::unique_ptr<ShaderManager>& getShaderManager()
    {
        if (!_shaderm) {
            _shaderm = std::make_unique<ShaderManager>();
        }

        return _shaderm;
    }

    static std::unique_ptr<AssetManager>& getAssetManager()
    {
        if (!_assetm) {
            _assetm = std::make_unique<AssetManager>();
        }

        return _assetm;
    }

    static std::unique_ptr<MaterialManager>& getMaterialManager()
    {
        if (!_materialm) {
            _materialm = std::make_unique<MaterialManager>();
        }

        return _materialm;
    }

    static void createTextureManager(std::unique_ptr<TextureManager> m)
    {
        _texturem = std::move(m);
    }

    static std::unique_ptr<TextureManager>& getTextureManager() { return _texturem; }

    static std::unique_ptr<Device>& getDevice() { return _devicem; }
    static void setDevice(std::unique_ptr<Device>&& d) { _devicem = std::move(d); }
};

}  // namespace familyline::graphics
