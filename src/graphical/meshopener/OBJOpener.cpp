#include "OBJOpener.hpp"

using namespace Tribalia::Graphics;

#define FGETS_OR_THROW(s, size, stream) \
    if (!fgets(s, size, stream)) {				      \
	if (!feof(stream)) {					      \
	    throw mesh_exception("Unexpected EOF while reading mesh", \
				 EIO, file);			      \
	}							      \
    }


Mesh* OBJOpener::Open(const char* file)
{
    FILE* fOBJ = fopen(file, "r");

    if (!fOBJ) {
        throw mesh_exception("Failure to open mesh", errno, file);
    }

    char line[256];

    std::vector<glm::vec3> verts, normals;
    std::vector<glm::vec2> texcoords;

    // Face indices
    std::vector<int> indVerts, indNormals, indTex;

    // Full vertices, temporary until we start using indexing
    std::vector<glm::vec3> realVerts, realNormals;
    std::vector<glm::vec2> realTex;
    std::vector<int> indMaterials;

    int materialID = 0;

    char mName[255] = "Unnamed";

    while (!feof(fOBJ)) {
        char* fline = &line[0];
        memset(fline, 0, 256);
        FGETS_OR_THROW(fline, 256, fOBJ);

        int cind = 0;
        while (isspace(fline[cind])) {
            cind++;
        }

        if (cind > 255) {
            continue; //avoid buffer overflows.
        }

        fline = &fline[cind];

        switch (fline[0]) {
        case '#': continue; //Comment
        case 'o': {
            // Mesh name.
            cind = 0;
            while (isspace(fline[cind])) {
                cind++;
            }

            fline = &fline[cind+2];
            strncpy(mName, fline, 255);

            /* Trim the mesh name */
            for (int i = strlen(mName)-1; i > 0; i--) {
                if (isspace(mName[i]) || mName[i] == '\n') {
                    mName[i] = 0;
                } else {
                    break;
                }
            }


            break;
        }
        case 'v': {
            //Vertex info
            if (isspace(fline[1])) {
                /* Position data (v) */
                fline = &fline[1];
                glm::vec3 vec = glm::vec3(0.0, 0.0, 1.0);
                sscanf(fline, "%f %f %f", &vec.x, &vec.y, &vec.z);
                verts.push_back(vec);
                continue;
            } else {
                switch (fline[1]) {
                    /* Texture data (vt) */
                    case 't': {
                        fline = &fline[2];
                        glm::vec2 vec = glm::vec2(0.0);
                        sscanf(fline, "%f %f", &vec.x, &vec.y);
                        texcoords.push_back(vec);
                        continue;
                    }
                    break;

                    /* Normal data (vn) */
                    case 'n': {
                        fline = &fline[2];
                        glm::vec3 vec = glm::vec3(0.0, 0.0, 1.0);
                        sscanf(fline, "%f %f %f", &vec.x, &vec.y, &vec.z);
                        vec = glm::normalize(vec);
                        normals.push_back(vec);
                        continue;
                    }
                    break;
                }
            }
        } // End of vertex info
        case 'f': {
            fline++;
            int vertIndex[3], normIndex[3], texIndex[3];

            // Face data
			if (sscanf(fline, "%d/%d/%d %d/%d/%d %d/%d/%d",
				&vertIndex[0], &texIndex[0], &normIndex[0],
				&vertIndex[1], &texIndex[1], &normIndex[1],
				&vertIndex[2], &texIndex[2], &normIndex[2]) > 7) {

				for (int i = 0; i < 3; i++) {
                    indVerts.push_back(vertIndex[i]);
                    indNormals.push_back(normIndex[i]);
					indTex.push_back(texIndex[i]);
                    indMaterials.push_back(materialID);
                }

			} else if (sscanf(fline, "%d//%d %d//%d %d//%d",
            //Normal + position
                &vertIndex[0], &normIndex[0],
                &vertIndex[1], &normIndex[1],
                &vertIndex[2], &normIndex[2]) > 2) {

                for (int i = 0; i < 3; i++) {
                    indVerts.push_back(vertIndex[i]);
                    indNormals.push_back(normIndex[i]);
                    indMaterials.push_back(materialID);
                }

            } else if (sscanf(fline, "%d %d %d",
                &vertIndex[0], &vertIndex[1], &vertIndex[2]) > 0) {
                //Position
                indVerts.push_back(vertIndex[0]);
                indVerts.push_back(vertIndex[1]);
                indVerts.push_back(vertIndex[2]);

            }
            continue;

        } // End of face
        case 'u':
        {
            // Use material
            char matname[256];
            if (sscanf(fline, "usemtl %s", matname) > 0) {
				Material* m = MaterialManager::GetInstance()->GetMaterial(matname);
                if (m) {
				    materialID = m->GetID();
                }
                continue;
            }

        } // End of material

        }

    }

    realVerts.reserve(verts.size() * 3);
    for (size_t i = 0; i < indVerts.size(); i++) {
        int index = indVerts[i]-1;

        //Treat negative indices
        if (index < 0) {
            index = indVerts.size() - index;
        }
        realVerts.push_back(verts[index]);
    }

    realNormals.reserve(normals.size() * 3);
    for (size_t i = 0; i < indNormals.size(); i++) {
        int index = indNormals[i]-1;

        //Treat negative indices
        if (index < 0) {
            index = indNormals.size() - index;
        }
        realNormals.push_back(normals[index]);
    }

	realTex.reserve(texcoords.size() * 2);
	if (indTex.size() > 0) {
		for (size_t i = 0; i < indTex.size(); i++) {
        	int index = indTex[i]-1;

        	//Treat negative indices
        	if (index < 0) {
        	    index = indTex.size() - index;
        	}

			realTex.push_back(texcoords[index]);
    	}

	} else {
		/* 	If no textures, fill a (1.0, 1.0) texture coordinate, so
			our shader doesn't break */
		for (size_t i = 0; i < indVerts.size(); i++) {
			realTex.push_back(glm::vec2(1.0, 1.0));
		}
	}

    Log::GetLog()->Write("Opened mesh \"%s\": (OBJ format) %d (%d) vertices, "
        "%d (%d) normals, %d texcoords, file is '%s'",
        mName, verts.size(), realVerts.size(),
        normals.size(), realNormals.size(), texcoords.size(), file);

    VertexData* vd = new VertexData;
    vd->Positions = realVerts;
    vd->Normals = realNormals;
	vd->TexCoords = realTex;
    vd->MaterialIDs = indMaterials;

    Mesh* m = new Mesh(vd);
    m->SetName(mName);

    return m;
}
