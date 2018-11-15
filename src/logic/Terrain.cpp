#include "Terrain.hpp"

using namespace familyline::logic;


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
	    unsigned index = x + y * _section_width;
	    _data[index] = new TerrainData();

	    for (int iy = 0; iy < SECTION_SIDE; iy++) {
		for (int ix = 0; ix < SECTION_SIDE; ix++) {
		    int16_t v = (int16_t)(sqrt(ix*iy));
		    _data[index]->data[iy*SECTION_SIDE+ix].elevation = v;
//					printf(">> x:%d y%d sqrt %d\n", ix, iy, v);
		}
	    }
        }
    }

    float size = _section_width * _section_height * sizeof(TerrainData);
    const char* unit;
    int index = 0;
    const char* unit_index[] = {"bytes", "kB", "MB", "GB", "TB", "PB"};

    while (size >= 1024) {
	unit = unit_index[++index];
	size /= 1024.0;

	if (index >= 6)
	    break;
    }

    Log::GetLog()->Write("terrain", "Created terrain of %d x %d points, %d section (~ %.3f %s)", _width, _height, (_section_height * _section_width),
			 size, unit);


}

int Terrain::GetHeightFromPoint(unsigned x, unsigned y)
{
    unsigned sectionX = floor(float(x) / (SECTION_SIDE * 1.0));
    unsigned sectionY = floor(float(y) / (SECTION_SIDE * 1.0));
    unsigned index = sectionY * _section_width + sectionX;

    unsigned siX = x % SECTION_SIDE;
    unsigned siY = y % SECTION_SIDE;

    return _data[index]->data[siY*SECTION_SIDE+siX].elevation;
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
void Terrain::SetName(const char* n) { _name = std::string{n}; }
void Terrain::SetDescription(const char* d) { _description = std::string{d}; }


/* Get raw terrain data and split it into sections */
void Terrain::SetData(TerrainSlot* slot) {
    if (!slot) {
	Log::GetLog()->Warning("terrain",
			       "terrain slot data is a null pointer");
    }

    for (int sy = 0; sy < _section_height; sy++) {
	for (int sx = 0; sx < _section_width; sx++) {
	    int index = sy*_section_width+sx;
	    for (int y = 0; y < SECTION_SIDE; y++) {
		if ((sy * SECTION_SIDE + y) >= _height) {
		    /* No more terrain after this */
		    break;
		}

		for (int x = 0; x < SECTION_SIDE; x++) {
		    if ((sx * SECTION_SIDE + x) >= _width) {
			/* There's no more terrain after this */
			break;
		    }
			
		    size_t slotindex = size_t((sy * SECTION_SIDE + y) * _width + (sx * SECTION_SIDE + x));
		    _data[index]->data[y*SECTION_SIDE+x] = slot[slotindex];
		}
	    }
		
	}
	    
    }	

    delete[] slot;
}


