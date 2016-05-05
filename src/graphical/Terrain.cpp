#include "Terrain.hpp"

using namespace Tribalia::Graphics;


Terrain::Terrain(int w, int h)
{
    _width = w;
    _height = h;

    _section_width = ceil(_width / (SECTION_SIDE * 1.0));
    _section_height = ceil(_height / (SECTION_SIDE * 1.0));

    _data = (TerrainData**) calloc(_section_height * _section_width, sizeof(void*));

    /* Create the sections */
    for (int y = 0; y < _section_height; y++) {
        for (int x = 0; x < _section_width; x++) {
            _data[x + y * _section_width] = new TerrainData();
        }
    }

    {
        float size = _section_width * _section_height * sizeof(TerrainData);
        char* unit;
        int index = 0;
        const char* unit_index[] = {"bytes", "kB", "MB", "GB", "TB", "PB", "EB"};

        while (size >= 1024) {
            unit = const_cast<char*>(unit_index[++index]);
            size /= 1024.0;

            if (index >= 7)
                break;
        }

        Log::GetLog()->Write("Created terrain of %d x %d points, %d sections"
            " (~ %.3f %s)", _width, _height, (_section_height * _section_width),
            size, unit);

    }


}

TerrainData* Terrain::GetSection(int index)
{
    return _data[index];
}
TerrainData* Terrain::GetSection(int x, int y)
{
    return _data[x + y * _section_width];
}
TerrainData** Terrain::GetAllSections()
{
    return _data;
}

int Terrain::GetWidth() const { return _width; }
int Terrain::GetHeight() const { return _height; }
int Terrain::GetSectionCount() const { return _section_height * _section_width; }
const char* Terrain::GetName() const { return _name.c_str(); }
const char* Terrain::GetDescription() const { return _description.c_str(); }
