#include "AssetManager.hpp"

#include "meshopener/OBJOpener.hpp"
#include "meshopener/MD2Opener.hpp"
#include "materialopener/MTLOpener.hpp"

using namespace Tribalia::Graphics;


void AssetManager::Create() {
    this->af.LoadFile("assets.yml");
}


AssetObject AssetManager::GetAsset(const char* name)
{
    auto ap = assetlist.find(name);
    if (ap != assetlist.end()) {

	if (ap->second.object)
	    return ap->second.object.value_or(create_asset_from_null());
    }

    // If not found or no value, request the value again
    AssetItem* ai = af.GetAsset(name);
    if (!ai) {
	Log::GetLog()->Warning("asset-manager", "asset %s not found in the asset list!",
			       name);
	return create_asset_from_null();
    }

    AssetType type;
    if (ai->type == "mesh") {
	type = AssetType::AMesh;
    } else if (ai->type == "material") {
	type = AssetType::AMaterial;
    } else if (ai->type == "texture") {
	type = AssetType::ATexture;
    }

    auto assetobj = this->LoadAsset(type, ai->path.c_str());

    AssetPointer aap;
    aap.item = ai;
    aap.type = type;

    if (assetobj.mesh)
	aap.object = make_optional(assetobj);

    this->assetlist[ai->path] = aap;

    Log::GetLog()->InfoWrite("asset-manager", "found asset %s at '%s'",
			     name, ai->path.c_str());
    return aap.object.value_or(create_asset_from_null());
}

AssetObject AssetManager::LoadAsset(AssetType type, const char* path)
{

    switch (type) {
    case ATexture: {
	auto texture = TextureOpener::OpenTexture(path);
	
	if (texture) {
	    std::string fname{ path };
	    size_t i = fname.find_last_of('/');

	    if (i == std::string::npos)	i = -1;

	    std::string tname = fname.substr(i + 1);
	    TextureManager::GetInstance()->AddTexture(tname.c_str(), texture);
	    return create_asset_from_texture(texture);
	}
	
	} break;
    case AMaterial: {
	std::string ext{ path };
	ext = ext.substr(ext.find_last_of('.') + 1);
	
	if (ext == "mtl") {
	    MTLOpener o;
	    std::vector<Material*>* m = new std::vector<Material*>;
	    *m = o.Open(path);
	    
	    if (m) {
		MaterialManager::GetInstance()->AddMaterials(*m);
		return create_asset_from_material(m);
	    }
	    
	}
    } break;
    case AMesh: {
	
	std::string ext{ path };
	ext = ext.substr(ext.find_last_of('.') + 1);
	MeshOpener* o;
	
	if (ext == "obj") {
	    o = new OBJOpener{};
	} else if (ext == "md2") {
	    o = new MD2Opener{};
	} else {
	    Log::GetLog()->Warning("asset-manager",
				   "%s uses an unsupported extension", path);
	    break;
	}
	
	auto mesh = o->Open(path);

	if (mesh)
	    return create_asset_from_mesh(mesh);
	
    } break;
    }

    return create_asset_from_null();
}

