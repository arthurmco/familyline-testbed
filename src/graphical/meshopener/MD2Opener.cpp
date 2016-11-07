#include "MD2Opener.hpp"

using namespace Tribalia::Graphics;

/* MD2 header */
struct md2_header_t
{
  int ident;                  /* magic number: "IDP2" */
  int version;                /* version: must be 8 */

  int skinwidth;              /* texture width */
  int skinheight;             /* texture height */

  int framesize;              /* size in bytes of a frame */

  int num_skins;              /* number of skins */
  int num_vertices;           /* number of vertices per frame */
  int num_st;                 /* number of texture coordinates */
  int num_tris;               /* number of triangles */
  int num_glcmds;             /* number of opengl commands */
  int num_frames;             /* number of frames */

  int offset_skins;           /* offset skin data */
  int offset_st;              /* offset texture coordinate data */
  int offset_tris;            /* offset triangle data */
  int offset_frames;          /* offset frame data */
  int offset_glcmds;          /* offset OpenGL command data */
  int offset_end;             /* offset end of file */
};

/*  MD2 vertex
    Each vertex is stored in a one-byte form, together with a multiplication
    factor in the frame information */
struct md2_vertex {
    char v[3];
    char normal;
};


struct md2_frame {
    float scaleX, scaleY, scaleZ;
    float transX, transY, transZ;
    char name[16];
};


Mesh* MD2Opener::Open(const char* file)
{
    FILE* fMD2 = fopen(file, "rb");

    if (!fMD2) {
        throw mesh_exception("Failure to open mesh", errno, file);
    }

    struct md2_header_t hdr;
    fread(&hdr, sizeof(md2_header_t), 1, fMD2);

    if (hdr.ident != 0x32504449) {
        throw mesh_exception("Invalid MD2 mesh header", errno, file);
    }

    if (hdr.version != 8) {
        throw mesh_exception("Invalid MD2 mesh version", errno, file);
    }

    Log::GetLog()->Write("MD2Opener: %s has %d vertices, %d tris, %d "
        "texcoords and %d frames", file,
        hdr.num_vertices, hdr.num_tris, hdr.num_st, hdr.num_frames);

    /*  A MD2 file contains fields specifying the triangles and fields
        specifying the vertices.
        We must parse the triangles first, and then the vertices */

    auto verts = new std::vector<glm::vec3>{hdr.num_tris * 3};
    auto normals = new std::vector<glm::vec3>{hdr.num_tris};    //each tris has 1 normal
    auto textures = new std::vector<glm::vec2>{hdr.num_tris * 3};


    /*  Read the frame information. The vertices are stored right after.
        Until we support animation, only one frame will be read
     */
    struct md2_frame frame;
    fseek(fMD2, hdr.offset_frames, SEEK_SET);
    fread(&frame, sizeof(struct md2_frame), 1, fMD2);

    auto scaleMult = glm::vec3(frame.scaleX, frame.scaleY, frame.scaleZ);
    auto transMult = glm::vec3(frame.transX, frame.transY, frame.transZ);
    frame.name[15] = 0;

    Log::GetLog()->Write("MD2Opener: %s first frame is %s",
        file, frame.name);
    printf("Scale (%f, %f, %f), trans (%f %f %f)",
        scaleMult.x, scaleMult.y, scaleMult.z,
        transMult.x, transMult.y, transMult.z);

    auto vertsMD2 = new md2_vertex[hdr.num_vertices];
    fread(vertsMD2, sizeof(md2_vertex), hdr.num_vertices, fMD2);

    auto aVerts = new glm::vec3[hdr.num_vertices];
    auto aNorms = new glm::vec3[hdr.num_vertices];

    for (size_t i = 0; i < hdr.num_vertices; i++) {
        glm::vec3 vert = glm::vec3(vertsMD2[i].v[0], vertsMD2[i].v[1], vertsMD2[i].v[2]);
        aVerts[i] = (vert * scaleMult) + transMult;
        printf("%d: [%d %d %d] -> %f %f %f\n", i,
            vertsMD2[i].v[0], vertsMD2[i].v[1], vertsMD2[i].v[2],
            aVerts[i].x, aVerts[i].y, aVerts[i].z);
    }

    delete vertsMD2;


    return nullptr;
}
