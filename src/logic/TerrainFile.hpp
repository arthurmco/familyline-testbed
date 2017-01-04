/* 	Terrain file parsing and opening
 	
	Copyright (C) 2017 Arthur M
*/
 
#include <cstdio>
#include <string>
#include <errno.h>
#include <stdexcept> //exceptions

#include <cstdint> //for base data types

#include "Terrain.hpp"

#ifndef TERRAINFILE_HPP
#define TERRAINFILE_HPP

namespace Tribalia {
namespace Logic {

/* Magic word for the terrain
 	Meant to be 'TRTB' in ascii */
#define TERRAIN_MAGIC_WORD 0x42545254

/*** Some data structures from the terrain file ***/
struct TerrainFileHeader {
	uint32_t magic; // Magic word
	uint32_t version; // Terrain version. Default is 1
	uint32_t game;	// Game type. Default is 0
	uint32_t thdr_offset; // Terrain data header file offset	
};

struct TerrainDataHeader {
	uint32_t width, height;	// Terrain size
	uint32_t name_offset;
	uint32_t auth_offset;
	uint32_t next_thdr_offset;
	uint32_t data_offset;
};

/* The terrain file class itself */
class TerrainFile {
private:
	FILE* fTerrain = nullptr;
	
	const char* fPath;
public:
	
	/* 	Opens the terrain file. 
	 	Throws terrain_exception if fail
	 */
	TerrainFile(const char* path);

	/* Retrieve the terrain.
	 	index is the terrain index you want to get. Defaults to 0.
	  */
	Terrain* GetTerrain(int index = 0);	

	~TerrainFile();
};

class terrain_file_exception : public std::runtime_error
    {
    public:
        explicit terrain_file_exception(std::string msg, 
						std::string file, int code);

		std::string file;
        int code;
    };



}
}



#endif
