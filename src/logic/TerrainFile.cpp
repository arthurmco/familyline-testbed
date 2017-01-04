#include "TerrainFile.hpp"

using namespace Tribalia::Logic;


/* 	Opens the terrain file. 
	Throws terrain_exception if fail
 */
TerrainFile::TerrainFile(const char* path)
{
	this->fPath = path;
	this->fTerrain = fopen(fPath, "rb");

	if (!this->fTerrain) {
		throw terrain_file_exception("File failed to open",
						path, errno);
	}		
}

/* Retrieve the terrain */
Terrain* TerrainFile::GetTerrain(int index)
{
	TerrainFileHeader tfhdr;
	rewind(fTerrain);

	/* Read the terrain file header */
	fread(&tfhdr, sizeof(TerrainFileHeader), 1, fTerrain);

	if (tfhdr.magic != TERRAIN_MAGIC_WORD) {
		throw terrain_file_exception("Invalid magic word",
						fPath, 0);
	}	

	if (tfhdr.version != 1 ) {
		throw terrain_file_exception("Invalid version",
						fPath, 0);
	}

	if (tfhdr.game != 0) {
		throw terrain_file_exception("Invalid game",
						fPath, 0);
	}

	if (tfhdr.thdr_offset < sizeof(TerrainFileHeader)) {
		throw terrain_file_exception("Invalid terrain header offset",
						fPath, 0);
	}

	/* Read the terrain data index */
	TerrainDataHeader tdh;
	fseek(fTerrain, tfhdr.thdr_offset, SEEK_SET);
	fread(&tdh, sizeof(TerrainDataHeader), 1, fTerrain);

	if (tdh.width == 0 || tdh.height == 0) {
		throw terrain_file_exception("Invalid size",
						fPath, 0);
	}

	Log::GetLog()->Write("TerrainFile: %s is a %ux%u terrain",
					fPath, tdh.width, tdh.height);

	return nullptr;	
}

TerrainFile::~TerrainFile()
{
	fclose(fTerrain);
}
