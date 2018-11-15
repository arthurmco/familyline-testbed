#include "TextureManager.hpp"

using namespace familyline::graphics;

TextureManager* TextureManager::_tm = nullptr;

/* Add texture, return its ID */
int TextureManager::AddTexture(const char* name, Texture* t)
{
    Log::GetLog()->Write("texture-manager", "Added texture %s (id %d)\n",
        name, t->GetHandle());

    t->SetName(name);
    
    if (_textures.find(std::string{name}) == _textures.end()) {
        _textures[name] = t;
    } else {
        Log::GetLog()->Warning("texture-manager", "Texture %s already exists",
			       name);
    }

    return t->GetHandle();
}

Texture* TextureManager::GetTexture(int ID)
{
    for (auto it = _textures.begin(); it != _textures.end(); ++it) {
        if (it->second->GetHandle() == ID) {
            return &(*it->second);
        }
    }

    return nullptr;
}

Texture* TextureManager::GetTexture(const char* name)
{
    std::string sname{name};
    for (auto it = _textures.begin(); it != _textures.end(); ++it) {
        if (sname == it->first) {
            return &(*it->second);
        }
    }

    return nullptr;
}
