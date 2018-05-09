#include "AssetManager.hpp"

#include "../config.h"
#include "meshopener/OBJOpener.hpp"
#include "meshopener/MD2Opener.hpp"
#include "materialopener/MTLOpener.hpp"

using namespace Tribalia::Graphics;


void AssetManager::Create() {
    this->af.LoadFile( ASSET_FILE_DIR "assets.yml");
}


AssetObject AssetManager::GetAsset(const char* name)
{
    Log::GetLog()->InfoWrite("asset-manager", "retrieving asset '%s'", name);

    std::string aname{name};
    auto ap = assetlist.find(aname);

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

    /* Load the dependencies first! */
    Texture* t = nullptr;
    for (auto d : ai->dependencies) {
	
	/* Check for child materials and textures */
	if (type == AMesh) {
	    auto mmaterial = ai->GetItemOr("mesh.material", "");
	    if (mmaterial != "") {
		auto mtl = this->GetAsset(mmaterial.c_str());
		if (!mtl.materialvec) {
		    /* TODO: Subchildren in meshes/materials 
		     *
		     * Now, the own mesh loader gets the material from the mesh
		     * This is bullshit. Load them here!
		     */
		
		    char* e;
		    asprintf(&e, "Could not load material %s for mesh %s (%s)", mmaterial.c_str(),
			     ai->name.c_str(), ai->path.c_str());
		    throw asset_exception(nullptr, e);
		}

		//auto material = mtl.materialvec->at(0);
		//assetobj.mesh->SetMaterial(material);
	    }
	
	    auto mtexture = ai->GetItemOr("mesh.texture", "");
	    if (mtexture != "") {
		auto tex = this->GetAsset(mtexture.c_str());
		if (!tex.texture) {
		    /* TODO: Subchildren in meshes/textures */ 

		    char* e;
		    asprintf(&e, "Could not load texture %s for mesh %s (%s)", mtexture.c_str(),
			     ai->name.c_str(), ai->path.c_str());
		    throw asset_exception(nullptr, e);
		}

		t = tex.texture;
		
	    }
	}

    }
    
    auto assetobj = this->LoadAsset(type, ai->path.c_str());

    AssetPointer aap;
    aap.item = ai;
    aap.type = type;

    if (assetobj.mesh)
	aap.object = make_optional(assetobj);

    if (t) {
	char* texname;
	asprintf(&texname, "tex_%s", ai->GetItemOr("mesh.texture", "nulltex").c_str());
	
	Material* mattex = new Material(texname, MaterialData(0.8, 1.0, 0.1));
	mattex->SetTexture(t);
	MaterialManager::GetInstance()->AddMaterial(mattex);
	assetobj.mesh->SetMaterial(mattex);
	
    }
    
    this->assetlist[ai->name] = aap;

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

	if (mesh) {

	    return create_asset_from_mesh(mesh);
	}
    } break;
    }

    return create_asset_from_null();
}

