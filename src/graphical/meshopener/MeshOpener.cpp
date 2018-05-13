#include "MeshOpener.hpp"

using namespace Familyline::Graphics;

#include <cstring>

std::unordered_map<std::string /* extension */,
		   std::unique_ptr<MeshOpener> /*opener*/> MeshOpener::openers;


/* Register the extension into the main class */
void MeshOpener::RegisterExtension(const char* extension, MeshOpener* opener)
{
    MeshOpener::openers[std::string{extension}] = std::unique_ptr<MeshOpener>{opener};
    fprintf(stderr, "registered mesh opener for the .%s extension\n", extension);
}


/* Open any file */
std::vector<Mesh*> MeshOpener::Open(const char* file)
{
    // WARNING: This function does not handle double extensions (such as .tar.gz )

    char* cfile = strdup(file);
    char* dot = strrchr(cfile, '.');

    // TODO: Check if the file exists before opening
    // check out the filesystem header!
    
    std::string edot{++dot};
    auto meshit = MeshOpener::openers.find(edot);

    if (meshit == MeshOpener::openers.end()) {
	char* s = new char[strlen(file) + 64];
	sprintf(s, "error: cannot open %s, format unknown!", file);
	throw std::runtime_error{s};
	delete[] s;
    }

    return meshit->second->OpenSpecialized(file);
}

/* Open only the file for that extension
 * (i.e, this method in a .obj file opener will only open .obj files
 */
std::vector<Mesh*> MeshOpener::OpenSpecialized(const char* file) {
    char* s = new char[strlen(file) + 64];
    sprintf(s, "error: cannot open %s, format unknown!", file);
    throw std::runtime_error{s};
    delete[] s;
    
}
