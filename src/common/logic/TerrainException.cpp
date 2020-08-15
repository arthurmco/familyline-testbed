#include <common/logic/TerrainFile.hpp>

using namespace familyline::logic;

terrain_file_exception::terrain_file_exception(std::string msg, std::string file, int code)
    : std::runtime_error(msg)
{
    this->file = file;
    this->code = code;
}
