#include <client/graphical/device.hpp>
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
		auto code = SDL_GetVideoDriver(i);
		auto is_default = !strcmp(code, SDL_GetCurrentVideoDriver());

		devs.push_back(new GLDevice(code, is_default, i));
	}

	return devs;
}

GLDevice::GLDevice(const char* code, bool is_default, int index)
{
	_code = std::string_view(code);
	_default = is_default;
	_data = std::any(index);
}

/// Get the device name
std::string_view GLDevice::getCode()
{
	return _code;
}

/// Get the device name
std::string_view GLDevice::getName()
{
    const unsigned char* s = glGetString(GL_RENDERER);

    if (s)
        return std::string_view((const char*)s);
    else
        return std::string_view("unknown");
}

/// Get the device name
std::string_view GLDevice::getVendor()
{
    const unsigned char* s = glGetString(GL_VENDOR);

    if (s)
        return std::string_view((const char*)s);
    else
        return std::string_view("you need to call this after initialize opengl");
}

bool GLDevice::isDefault() {
	return _default;
}

std::any GLDevice::getCustomData() {
	return _data;
}
