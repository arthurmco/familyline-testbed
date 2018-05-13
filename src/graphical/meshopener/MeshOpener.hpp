
/**
 * Familyline mesh opener class
 *
 * The mesh opener "Open" method returns a list of meshes, because a mesh file
 * can have more than one mesh
 * 
 * Before, you could use the mesh directly from the file opener.
 * You still can (using mesh_list[0]), but it's not recommended.
 * 
 * It's recommended to use the MeshManager and its Get method, or the 
 * AssetManager, using the path (asset_name/mesh_name)
 *
 * The mesh name is defined in the file itself. The definition of "mesh" can vary from file to
 * file.
 * For example, inside .obj files, it is the mesh group
 *
 * Copyright (C) 2016-2018 Arthur Mendes
 **/

#ifndef MESHOPENER_HPP
#define MESHOPENER_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "../Mesh.hpp"

namespace Familyline::Graphics {
    
// Call this in the constructor
#define OPENER_REGISTER(extension)		\
    this->RegisterExtension(extension, this)

    class MeshOpener {
    private:

	/* We maintain a list of mesh openers, so we can use this class itself to open it, 
	 * independent of extension
	 * 
	 * Since we don't need more than one instance of the mesh opener class, we enforce it
	 * by using a unique_ptr
	 */
	static std::unordered_map<std::string /* extension */,
				  std::unique_ptr<MeshOpener> /*opener*/> openers;

    protected:
	/* Register the extension into the main class.
	 * The extension is without the dot
	 */
	void RegisterExtension(const char* extension, MeshOpener* opener);

    public:
	/* Open any file */
	static std::vector<Mesh*> Open(const char* file);

	/* Open only the file for that extension
	 * (i.e, this method in a .obj file opener will only open .obj files
	 */
	virtual std::vector<Mesh*> OpenSpecialized(const char* file);

	virtual ~MeshOpener() {}

	/* Remove the other constructors. They are not needed, this class will be instanciated only once */
	MeshOpener& operator=(const MeshOpener &other) = delete;
	MeshOpener& operator=(MeshOpener &&other) = delete;

    };
}

#endif /* MESHOPENER_HPP */
