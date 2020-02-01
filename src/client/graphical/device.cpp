#include "device.hpp"
#include <SDL2/SDL_opengl.h>


using namespace familyline::graphics;

/**
 * Get a list of devices in the machine
  */
std::vector<Device*> familyline::graphics::getDeviceList()
{
	// TODO: make this a static method of each device?
	std::vector<Device*> devs;

	auto r = SDL_Init(SDL_INIT_EVERYTHING);
	if (r != 0) {

	}


	auto driverCount = SDL_GetNumVideoDrivers();
	for (auto i = 0; i < driverCount; i++) {
		auto name = SDL_GetVideoDriver(i);
		auto is_default = !strcmp(name, SDL_GetCurrentVideoDriver());

		devs.push_back(new GLDevice(name, is_default, i));
	}

	return devs;
}

GLDevice::GLDevice(const char* name, bool is_default, int index)
{
	_name = std::string_view(name);
	_default = is_default;
	_data = std::any(index);
}

/// Get the device name
std::string_view GLDevice::getName()
{
	return _name;
}

bool GLDevice::isDefault() {
	return _default;
}

std::any GLDevice::getCustomData() {
	return _data;
}
