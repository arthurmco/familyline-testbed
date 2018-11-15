/*
 * The .obj file opener
 * Each mesh is delimited by a mesh group
 *
 * (You can create a mesh group by selecting the meshes you want in Blender and pressing ctrl+g, 
 * or something like that)
 *
 * If no mesh group is found, we create a mesh group composed of all meshes in the file
 *
 * Remember it also uses the meshes from the file.
 * Do not let the "New Mesh" and "New Mesh <2>" default names
 * NAME YOUR MESHES IN THE FILE
 *
 * Copyright (C) 2016-2018 Arthur Mendes
 */

#ifndef OBJOPENER_HPP
#define OBJOPENER_HPP

#include "MeshOpener.hpp"
#include "../Mesh.hpp"

namespace familyline::graphics {
    class OBJOpener : public MeshOpener {
    
    public:

	OBJOpener() { OPENER_REGISTER("obj"); }
    
	/* Open only the file for that extension
	 * (i.e, this method in a .obj file opener will only open .obj files
	 */
	virtual std::vector<Mesh*> OpenSpecialized(const char* file);

	virtual ~OBJOpener() { this->UnregisterExtension("obj"); };

    };
}

#endif /* OBJOPENER_HPP */
