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
    unsigned char v[3];
    unsigned char normal;
};

/* MD2 triangle */
struct md2_triangle
{
  unsigned short vertex[3];   /* vertex indices of the triangle */
  unsigned short st[3];       /* tex. coord. indices */
};

/* Texture coords */
struct md2_texcoords
{
  short s;
  short t;
};

struct md2_frame {
    float scaleX, scaleY, scaleZ;
    float transX, transY, transZ;
    char name[16];
}; //__attribute__((packed));

#include "anorms.h"

Mesh* MD2Opener::Open(const char* file)
{
    FILE* fMD2 = fopen(file, "rb");
    rewind(fMD2);

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
    Log::GetLog()->Write("MD2Opener: skin (aka suggested texture) size is "
        "%d x %d", hdr.skinwidth, hdr.skinheight);

    /*  A MD2 file contains fields specifying the triangles and fields
        specifying the vertices.
        We must parse the vertices first, and then the triangles */

    auto verts = new std::vector<glm::vec3>{};
    auto normals = new std::vector<glm::vec3>{};    //each tris has 1 normal
    auto textures = new std::vector<glm::vec2>{};


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
    auto trisMD2 = new md2_triangle[hdr.num_tris];
    auto texcoordsMD2 = new md2_texcoords[hdr.num_st];

    fread(vertsMD2, sizeof(md2_vertex), hdr.num_vertices, fMD2);

    auto aVerts = new glm::vec3[hdr.num_vertices];
    auto aNorms = new glm::vec3[hdr.num_vertices];
    auto aTex = new glm::vec2[hdr.num_st];

    for (size_t i = 0; i < hdr.num_vertices; i++) {
        glm::vec3 vert = glm::vec3(vertsMD2[i].v[0], vertsMD2[i].v[1], vertsMD2[i].v[2]);
        aVerts[i] = (vert * scaleMult) + transMult;
        aNorms[i] = glm::vec3(anorms[vertsMD2[i].normal][0],
            anorms[vertsMD2[i].normal][1], anorms[vertsMD2[i].normal][2]);
    }

    delete vertsMD2;

    /* Read texcoords */
    fseek(fMD2, hdr.offset_st, SEEK_SET);
    fread(texcoordsMD2, sizeof(md2_texcoords), hdr.num_st, fMD2);

    for (size_t i = 0; i < hdr.num_st; i++) {
        float texS,texT;
        texS = texcoordsMD2[i].s / (float)hdr.skinwidth;
        texT = texcoordsMD2[i].t / (float)hdr.skinheight;

        aTex[i] = glm::vec2(texS, texT);
    }

    delete texcoordsMD2;


    /* Read triangle data */
    fseek(fMD2, hdr.offset_tris, SEEK_SET);
    fread(trisMD2, sizeof(md2_triangle), hdr.num_tris, fMD2);

    for (size_t i = 0; i < hdr.num_tris; i++) {
        /*printf("tri %d: (%d %d %d)\n", i,
            trisMD2[i].vertex[0], trisMD2[i].vertex[1], trisMD2[i].vertex[2]); */
        verts->push_back(aVerts[trisMD2[i].vertex[0]]);
        verts->push_back(aVerts[trisMD2[i].vertex[1]]);
        verts->push_back(aVerts[trisMD2[i].vertex[2]]);

        normals->push_back(aNorms[trisMD2[i].vertex[0]]);
        normals->push_back(aNorms[trisMD2[i].vertex[1]]);
        normals->push_back(aNorms[trisMD2[i].vertex[2]]);


        /*printf("\t\t [%f %f %f], [%f %f %f], [%f %f %f]\n",
            aVerts[trisMD2[i].vertex[0]].x, aVerts[trisMD2[i].vertex[0]].y, aVerts[trisMD2[i].vertex[0]].z,
            aVerts[trisMD2[i].vertex[1]].x, aVerts[trisMD2[i].vertex[1]].y, aVerts[trisMD2[i].vertex[1]].z,
            aVerts[trisMD2[i].vertex[2]].x, aVerts[trisMD2[i].vertex[2]].y, aVerts[trisMD2[i].vertex[2]].z);
            */

        textures->push_back(aTex[trisMD2[i].st[0]]);
        textures->push_back(aTex[trisMD2[i].st[1]]);
        textures->push_back(aTex[trisMD2[i].st[2]]);
    }

    int i = 0;
    for (auto it = normals->begin(); it != normals->end(); it++) {
        /*printf("\t%d: [%f %f %f]\n",
            i, it->x, it->y, it->z); */
        i++;
    }

    VertexData* vd = new VertexData;
    vd->Positions = *verts;
    vd->TexCoords = *textures;
    vd->Normals = *normals;

    Mesh* m = new Mesh{vd};
    fclose(fMD2);
    return m;
}
