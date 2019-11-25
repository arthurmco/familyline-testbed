#pragma once

#include "asset_manager.hpp"
#include "shader_manager.hpp"
#include "material_manager.hpp"
#include "texture_manager.hpp"

namespace familyline::graphics {

    
    class GFXService {
    private:
        static std::unique_ptr<ShaderManager> _shaderm;
        static std::unique_ptr<AssetManager> _assetm;
        static std::unique_ptr<MaterialManager> _materialm;
        static std::unique_ptr<TextureManager> _texturem;
        
    public:
        static std::unique_ptr<ShaderManager>& getShaderManager() {
            if (!_shaderm) {
                _shaderm = std::make_unique<ShaderManager>();
            }

            return _shaderm;
        }

        static std::unique_ptr<AssetManager>& getAssetManager() {
            if (!_assetm) {
                _assetm = std::make_unique<AssetManager>();
            }

            return _assetm;
        }

        static std::unique_ptr<MaterialManager>& getMaterialManager() {
            if (!_materialm) {
                _materialm = std::make_unique<MaterialManager>();
            }

            return _materialm;
        }

        static std::unique_ptr<TextureManager>& getTextureManager() {
            if (!_texturem) {
                _texturem = std::make_unique<TextureManager>();
            }

            return _texturem;
        }

    };

}
