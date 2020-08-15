#include <client/graphical/texture_manager.hpp>
#include <common/logger.hpp>

using namespace familyline::graphics;

/* Add texture, return its ID */
int TextureManager::AddTexture(const char* name, Texture* t)
{
    auto& log = LoggerService::getLogger();

    log->write(
        "texture-manager", LogType::Debug, "Added texture %s (id %d)\n", name, t->GetHandle());

    t->SetName(name);

    if (_textures.find(std::string{name}) == _textures.end()) {
        _textures[name] = t;
    } else {
        log->write("texture-manager", LogType::Warning, "Texture %s already exists", name);
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
