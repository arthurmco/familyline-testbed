#include "AssetManager.hpp"

#include <sys/types.h>
#ifndef _MSC_VER
#include <sys/stat.h>
#include <unistd.h>
#endif
using namespace Tribalia::Graphics;


AssetGroup* AssetManager::AddAssetGroup(const char* path, const char* tag)
{
    /* Read all files and folders looking for valid assets */
    AssetGroup* grp = new AssetGroup{};
    strcpy(grp->tag, tag);
    strcpy(grp->folder, path);
    Log::GetLog()->Write("asset-manager",
			 "Added asset group tag '%s', path '%s'",
        grp->tag, grp->folder);
    _groups.push_back(grp);
    return grp;
}

/* Query group folder for valid assets */
void AssetManager::QueryAssetGroup(AssetGroup* grp)
{
    (void) grp;
}

Asset* AssetManager::GetAsset(const char* name)
{
    for (auto it : _assets) {
        if (!strcmp(name, it->name)) {
	    Asset* at = (Asset*)it;
	    if (!at->asset.mesh) {
		Log::GetLog()->Write("asset-manager",
				     "Loading %s by demand", at->name);
		fflush(stdout);
		this->LoadAsset(at);
		
		/* Check if we have a texture binded to this mesh in the file */
		Material* m;
		char* mname = new char[256];
		sprintf(mname, " ******  %s#texture", it->name);
		m = MaterialManager::GetInstance()->GetMaterial(mname);
		if (m != nullptr) {
		    if (at->asset_type == ASSET_MESH) {
			m->GetData()->diffuseColor.r = 0.0;
			at->asset.mesh->SetMaterial(m);
		    }
		}
	    }
	    return at;
        }
    }

    return nullptr;
}

void AssetManager::AddAsset(AssetGroup* grp, Asset* a)
{
    a->group = grp;
    _assets.push_back(a);

    static const char* aname[] =
        {"mesh", "material", "texture"};
    Log::GetLog()->Write("asset-manager",
			 "Added asset %s path %s type %s (%d) to group %s",
        a->name, a->path, aname[a->asset_type % 3], a->asset_type, grp->folder);
}


bool CheckIfExists(const char* path)
{
// todo: check for existance of stat(), not for a unix compiler
// (it seems that vc supports stat() 
#ifndef _MSC_VER
    struct stat statbuf;
    if (stat(path, &statbuf) < 0) {
	switch (errno) {
	case EBADF:
	case ENOENT:  return false;
	}
    }

    return (statbuf.st_size > 0);
#else
    #pragma message("Tribalia is unable to find a stat() version!")
    #pragma message(" Using less destructive return value")
    return true;
#endif

}

Asset* AssetManager::RetrieveAsset(AssetGroup* grp, AssetFileItem*& afi)
{
	std::string extension = afi->path.substr(afi->path.find_last_of('.')+1);
	Asset* a = new Asset();
	strcpy(a->path, afi->path.c_str());
	strcpy(a->name, afi->name.c_str());
	
	if (afi->type == "mesh") {
	    a->asset_type = ASSET_MESH;
	} else if (afi->type == "texture") {
	    a->asset_type = ASSET_TEXTURE;
	} else if (afi->type == "material") { 
	    a->asset_type = ASSET_MATERIAL;
	}

	std::vector<Material*>* child_mat = nullptr;
	Texture* child_t = nullptr;

	/** Treat dependencies */
	for (auto& dep : afi->depends) {

	    Asset* ac = nullptr;
	    
	    if (!dep->isProcessed) {
		ac = RetrieveAsset(grp, dep);
		AddAsset(grp, ac);
	    } else {
		ac = this->GetAsset(dep->name.c_str());
	    }

	    
	    fflush(stdout);
	    if (!ac) continue;
	    /* This is a workaround for supporting textured but material-less files. */
	    switch (ac->asset_type) {
	    case ASSET_MATERIAL:
		if (ac->asset.material) child_mat = ac->asset.material; break;
	    case ASSET_TEXTURE:
		if (ac->asset.texture)  child_t = ac->asset.texture;    break;	    
	    }
	
	
	}
	
	if (child_t) {
	    /* If no material but textured, then we create a ghost material for it
	       Note that materials defined in the model takes precedence */

	    char* mname = new char[256];
	    sprintf(mname, "%s#texture", a->name);
	    MaterialData md;
	    md.diffuseColor = glm::vec3(1,1,0);
	    
	    Material* mt = new Material{mname, md};
	    mt->SetTexture(TextureManager::GetInstance()->GetTexture(child_t->GetHandle()));
	    MaterialManager::GetInstance()->AddMaterial(mt);

	}
	
	afi->isProcessed = true;
	if (a->asset_type != ASSET_MESH) {
		if (!LoadAsset(a)) {
			throw asset_exception(afi, "Error loading asset");
		}

	}

	return a;
}


#include <functional>	// std::ptr_fun()
#include <cstdio>
/*  Read assets from a Tribalia Asset File (*.taif)
    Returns 'true' if read successfully, 'false' if not
*/
bool AssetManager::ReadFromFile(const char* file)
{
	AssetGroup* grp = this->AddAssetGroup(file, file);
	
	AssetFile* af = new AssetFile(file);
	af->BuildFileItemTree();

	for (auto& afi : *af->GetFileItemTree()) {
		if (!afi->isProcessed) {
			AddAsset(grp, RetrieveAsset(grp, afi));
		}
	}

	return true;
}



bool AssetManager::LoadAsset(Asset* at)
{

    switch (at->asset_type) {
    case ASSET_TEXTURE: {
	at->asset.texture = TextureOpener::OpenTexture(at->path);
	
	if (at->asset.texture) {
	    std::string fname{ at->path };
	    size_t i = fname.find_last_of('/');

	    if (i == std::string::npos)	i = -1;

	    std::string tname = fname.substr(i + 1);
	    TextureManager::GetInstance()->AddTexture(tname.c_str(), at->asset.texture);
	    return true;
	}
	
	} break;
    case ASSET_MATERIAL: {
	std::string ext{ at->path };
	ext = ext.substr(ext.find_last_of('.') + 1);
	
	if (ext == "mtl") {
	    MTLOpener o;
	    std::vector<Material*>* m = new std::vector<Material*>;
	    *m = o.Open(at->path);
	    
	    if (m) {
		at->asset.material = m;
		MaterialManager::GetInstance()->AddMaterials(*m);
		return true;
	    }
	    
	}
    } break;
    case ASSET_MESH: {
	
	std::string ext{ at->path };
	ext = ext.substr(ext.find_last_of('.') + 1);
	MeshOpener* o;
	
	if (ext == "obj") {
	    o = new OBJOpener{};
	} else if (ext == "md2") {
	    o = new MD2Opener{};
	} else {
	    Log::GetLog()->Write("asset-manager",
				 "%s uses an unsupported extension", at->path);
	    break;
	}
	
	at->asset.mesh = o->Open(at->path);
	if (at->asset.mesh) {
		return true;
	}
	
    } break;
    }

    return false;
}

