/***
    MD2 model opener

    Copyright (C) 2016 Arthur Mendes.

***/

#include <cctype>
#include <cstdio>
#include <cstring>

#include "../material_manager.hpp"
#include "MeshOpener.hpp"

#ifndef MD2OPENER_HPP
#define MD2OPENER_HPP

namespace familyline::graphics
{
/**
 * \brief MD2 header
 */
struct md2_header_t {
    int ident;   /**< magic number: "IDP2" */
    int version; /**< version: must be 8 */

    int skinwidth;  /**< texture width */
    int skinheight; /**< texture height */

    int framesize; /**< size in bytes of a frame */

    int num_skins;    /**< number of skins */
    int num_vertices; /**< number of vertices per frame */
    int num_st;       /**< number of texture coordinates */
    int num_tris;     /**< number of triangles */
    int num_glcmds;   /**< number of opengl commands */
    int num_frames;   /**< number of frames */

    int offset_skins;  /**< offset skin data */
    int offset_st;     /**< offset texture coordinate data */
    int offset_tris;   /**< offset triangle data */
    int offset_frames; /**< offset frame data */
    int offset_glcmds; /**< offset OpenGL command data */
    int offset_end;    /**< offset end of file */
};

class MD2Opener : public MeshOpener
{
private:
    struct md2_header_t hdr;

public:
    MD2Opener() { OPENER_REGISTER("md2"); }

    virtual std::vector<Mesh*> OpenSpecialized(const char* file);

    virtual ~MD2Opener() { this->UnregisterExtension("md2"); };
};

}  // namespace familyline::graphics

#endif /* end of include guard: MD2OPENER_HPP */
