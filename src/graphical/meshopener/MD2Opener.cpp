#include "MD2Opener.hpp"
#include "../DeformAnimator.hpp"

using namespace Tribalia::Graphics;

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

#define FREAD_OR_THROW(ptr, size, nmemb, f) \
    if (fread(ptr, size, nmemb, f) < (size_t)nmemb) {			\
	throw mesh_exception("Unexpected EOF while reading mesh", errno, file); \
    }


#include "anorms.h"

Mesh* MD2Opener::Open(const char* file)
{
    FILE* fMD2 = fopen(file, "rb");
    rewind(fMD2);

    if (!fMD2) {
        throw mesh_exception("Failure to open mesh", errno, file);
    }

    FREAD_OR_THROW(&hdr, sizeof(md2_header_t), 1, fMD2);

    if (hdr.ident != 0x32504449) {
        throw mesh_exception("Invalid MD2 mesh header", errno, file);
    }

    if (hdr.version != 8) {
        throw mesh_exception("Invalid MD2 mesh version", errno, file);
    }

    /*  A MD2 file contains fields specifying the triangles and fields
        specifying the vertices.
        We must parse the vertices first, and then the triangles */

    auto verts = new std::vector<glm::vec3>{};
    auto normals = new std::vector<glm::vec3>{};    //each tris has 1 normal
    auto textures = new std::vector<glm::vec2>{};
    auto matids = new std::vector<int>{};

    /*  Read the frame information. The vertices are stored right after.
        Until we support animation, only one frame will be read
     */
    struct md2_frame frame;
    fseek(fMD2, hdr.offset_frames, SEEK_SET);
    FREAD_OR_THROW(&frame, sizeof(struct md2_frame), 1, fMD2);

    auto scaleMult = glm::vec3(frame.scaleX, frame.scaleY, frame.scaleZ);
    auto transMult = glm::vec3(frame.transX, frame.transY, frame.transZ);
    frame.name[15] = 0;

    auto vertsMD2 = new md2_vertex[hdr.num_vertices];
    auto trisMD2 = new md2_triangle[hdr.num_tris];
    auto texcoordsMD2 = new md2_texcoords[hdr.num_st];
    
    
    FREAD_OR_THROW(vertsMD2, sizeof(md2_vertex), hdr.num_vertices, fMD2);

    auto aVerts = new glm::vec3[hdr.num_vertices];
    auto aNorms = new glm::vec3[hdr.num_vertices];
    auto aTex = new glm::vec2[hdr.num_st];

    for (size_t i = 0; i < (size_t)hdr.num_vertices; i++) {
        glm::vec3 vert = glm::vec3(vertsMD2[i].v[0], vertsMD2[i].v[1], vertsMD2[i].v[2]);
        aVerts[i] = (vert * scaleMult) + transMult;
        aNorms[i] = glm::vec3(anorms[vertsMD2[i].normal][0],
            anorms[vertsMD2[i].normal][1], anorms[vertsMD2[i].normal][2]);
    }

    /* Save the end of first frame position */
    int frame2coords = (int)ftell(fMD2);

    delete[] vertsMD2;

    /* Read texcoords */
    fseek(fMD2, hdr.offset_st, SEEK_SET);
    FREAD_OR_THROW(texcoordsMD2, sizeof(md2_texcoords), hdr.num_st, fMD2);

    for (size_t i = 0; i < (size_t)hdr.num_st; i++) {
        float texS,texT;
        texS = texcoordsMD2[i].s / (float)hdr.skinwidth;
        texT = texcoordsMD2[i].t / (float)hdr.skinheight;

        aTex[i] = glm::vec2(texS, texT);
    }

    delete[] texcoordsMD2;


    /* Read triangle data */
    fseek(fMD2, hdr.offset_tris, SEEK_SET);
    FREAD_OR_THROW(trisMD2, sizeof(md2_triangle), hdr.num_tris, fMD2);

    for (size_t i = 0; i < (size_t)hdr.num_tris; i++) {
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
;
        textures->push_back(aTex[trisMD2[i].st[0]]);
        textures->push_back(aTex[trisMD2[i].st[1]]);
        textures->push_back(aTex[trisMD2[i].st[2]]);

	matids->push_back(0);
	matids->push_back(0);
	matids->push_back(0);
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
    vd->MaterialIDs = *matids;

    if (hdr.num_frames > 1) {

        /* Read the remaining of frame information */
        printf("Reading frame data - %d frames detected\n", hdr.num_frames);
		DeformAnimator* ad = new DeformAnimator(vd, hdr.num_frames);
        ad->AddFrame(0, vd->Positions);

        // Go to the second frame
        fseek(fMD2, frame2coords, SEEK_SET);

        glm::vec3* vlist = new glm::vec3[vd->Positions.size()];
        glm::vec3* avlist = new glm::vec3[hdr.num_vertices];

        for (size_t f = 1; f < (size_t)hdr.num_frames; f++) {

            struct md2_frame fframe;
            FREAD_OR_THROW(&fframe, sizeof(struct md2_frame), 1, fMD2);
 /*           printf("\tframe #%d: scalefactor: (%.3f,%.3f,%.3f), transfactor: (%.3f,%.3f,%.3f) \n",
                f, fframe.scaleX, fframe.scaleY, fframe.scaleZ, fframe.transX,
                fframe.transY, fframe.transZ);
*/
            scaleMult = glm::vec3(fframe.scaleX, fframe.scaleY, fframe.scaleZ);
            transMult = glm::vec3(fframe.transX, fframe.transY, fframe.transZ);

            struct md2_vertex* fverts = new md2_vertex[hdr.num_vertices];
            FREAD_OR_THROW(fverts, sizeof(struct md2_vertex), hdr.num_vertices, fMD2);

            // Mount the triangles. Get all vertices, scale and transform them
            for (size_t i = 0; i < (size_t)hdr.num_vertices; i++) {
                glm::vec3 v =  glm::vec3(fverts[i].v[0], fverts[i].v[1], fverts[i].v[2]);
                v = (v * scaleMult) + transMult;
                avlist[i] = v;
            }

            // Assemble
            for (   size_t i = 0, v = 0;
                    (i < (size_t)hdr.num_tris), (v < vd->Positions.size());
                    i++, v+=3) {
                /*printf("tri %d: (%d %d %d)\n", i,
                    trisMD2[i].vfvertsertex[0], trisMD2[i].vertex[1], trisMD2[i].vertex[2]); */
                vlist[v] = (avlist[trisMD2[i].vertex[0]]);
                vlist[v+1] = (avlist[trisMD2[i].vertex[1]]);
                vlist[v+2] = (avlist[trisMD2[i].vertex[2]]);

            }

			std::vector<glm::vec3> veclist;
			veclist.assign(vlist, vlist + vd->Positions.size());
            ad->AddFrame(f, veclist);

        }

        /* The list get copied, so we can delete it */
        delete[] vlist;
        delete[] avlist;
		vd->animator = std::unique_ptr<BaseAnimator>(ad);

    }
    Mesh* m = new Mesh{vd};
    fclose(fMD2);
    return m;
}
