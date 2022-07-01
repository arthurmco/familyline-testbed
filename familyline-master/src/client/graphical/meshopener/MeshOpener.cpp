#include <client/graphical/meshopener/MeshOpener.hpp>
#include <common/logger.hpp>
#include <stdexcept>

using namespace familyline::graphics;

#include <cstring>

std::unordered_map<std::string /* extension */, MeshOpenerRef /*opener*/> MeshOpener::openers = {};

/* Register the extension into the main class */
void MeshOpener::RegisterExtension(const char* extension, MeshOpener* opener)
{
    auto& log = LoggerService::getLogger();
    if (MeshOpener::openers[std::string{extension}].ref == 0) {
        MeshOpener::openers[std::string{extension}].m = opener;
    }

    MeshOpener::openers[std::string{extension}].ref++;
    log->write(
        "meshopener", LogType::Debug, "registered mesh opener for the .{} extension", extension);
}

/* Unregister the extension into the main class */
void MeshOpener::UnregisterExtension(const char* extension)
{
    auto& log = LoggerService::getLogger();

    if (MeshOpener::openers.empty())
        return;
    
    auto ext = std::string{extension};
    auto opext = MeshOpener::openers.find(ext);
    if (opext == MeshOpener::openers.end())
        return;

    opext->second.ref--;

    if (opext->second.ref == 0)
        MeshOpener::openers.erase(ext);

    log->write(
        "meshopener", LogType::Debug, "unregistered mesh opener for the .{} extension", extension);
}

/* Open any file */
std::vector<Mesh*> MeshOpener::Open(const char* file)
{
    // WARNING: This function does not handle double extensions (such as .tar.gz )

    char* cfile = strdup(file);
    char* dot   = strrchr(cfile, '.');

    // TODO: Check if the file exists before opening
    // check out the filesystem header!

    std::string edot{++dot};
    auto meshit = MeshOpener::openers.find(edot);

    if (meshit == MeshOpener::openers.end()) {
        char* s = new char[strlen(file) + 76];
        sprintf(s, "error: cannot open %s (format %s), format unknown!", file, edot.c_str());
        throw std::runtime_error{s};
        delete[] s;
    }

    free(cfile);

    return meshit->second.m->OpenSpecialized(file);
}

/* Open only the file for that extension
 * (i.e, this method in a .obj file opener will only open .obj files
 */
std::vector<Mesh*> MeshOpener::OpenSpecialized(const char* file)
{
    char* s = new char[strlen(file) + 64];
    sprintf(s, "error: cannot open %s, format unknown!", file);
    throw std::runtime_error{s};
    delete[] s;
}
