#pragma once

#include "asset_manager.hpp"
#include "shader_manager.hpp"

namespace familyline::graphics {

    
    class GFXService {
    private:
        static std::unique_ptr<ShaderManager> _shaderm;
        static std::unique_ptr<AssetManager> _assetm;
        
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

    };

}
