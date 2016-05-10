#include "MTLOpener.hpp"

using namespace Tribalia::Graphics;

std::vector<Material*> MTLOpener::Open(const char* file)
{
    FILE* fMat = fopen(file, "r");

    if (!fMat) {
        throw material_exception("Failure to open material", errno, file);
    }

    std::vector<Material*> mats;
    char* matname = nullptr;
    glm::vec3 diffuse, ambient, specular;

    char line[256];
    while (!feof(fMat)) {
        char* fline = &line[0];
        memset(fline, 0, 255);
        fgets(fline, 255, fMat);

        int cind = 0;
        while (isspace(fline[cind])) {
            cind++;
        }

        if (cind > 255) {
            continue; //avoid buffer overflows.
        }

        fline = &fline[cind];

        if (fline[0] == '#')
            continue; //Discard comment

        if (!strncmp(fline, "newmtl", 6)) {
            /* New material declared */
            if (matname) {
                /* Add the last processed material and discard it */
                MaterialData md;
                md.diffuseColor = diffuse;
                md.specularColor = specular;
                md.ambientColor = diffuse * 0.1f;
                Material* m = new Material{mats.size(), matname, md};
                mats.push_back(m);
            }

            matname = new char[256];
            memset((void*)matname, 0, 255);

            int i = 7;
            while (fline[i] != '\n') {
                matname[i-7] = fline[i];
                i++;
                if (i > 256)
                    break;
            }

            Log::GetLog()->Write("New material recognized: %s", matname);
            continue;
        }

        if (!strncmp(fline, "Ka", 2)) {
            fline += 2;
            sscanf(fline, "%f %f %f", &ambient.x, &ambient.y, &ambient.z);
            Log::GetLog()->Write("\t ambient: %.2f %.2f %.2f",
                ambient.x, ambient.y, ambient.z);
            continue;
        }
        if (!strncmp(fline, "Kd", 2)) {
            fline += 2;
            sscanf(fline, "%f %f %f", &diffuse.x, &diffuse.y, &diffuse.z);
            Log::GetLog()->Write("\t diffuse: %.2f %.2f %.2f",
                diffuse.x, diffuse.y, diffuse.z);
            continue;

        }
        if (!strncmp(fline, "Ks", 2)) {
            fline += 2;
            sscanf(fline, "%f %f %f", &specular.x, &specular.y, &specular.z);
            Log::GetLog()->Write("\t specular: %.2f %.2f %.2f",
                specular.x, specular.y, specular.z);
            continue;
        }

    }

    /* Add the last material */
    MaterialData md;
    md.diffuseColor = diffuse;
    md.specularColor = specular;
    md.ambientColor = diffuse * 0.1f;
    Material* m = new Material{mats.size(), matname, md};
    mats.push_back(m);

    fclose(fMat);
    return mats;
}
