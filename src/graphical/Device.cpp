#include "Device.hpp"

#include <GL/glew.h>
#ifdef __linux__
#include <GL/glxew.h>
#endif

using Tribalia::Log;
using namespace Tribalia::Graphics;

DeviceInfo::DeviceInfo(int device_id, const char* name,
	const char* vendor, size_t vram)
	: device_id(device_id), device_name(name), vendor(vendor), video_ram(vram)
{}

/* Gets the actual device ID
   TODO: check support for OpenGL getting the vendor/device ID
   TODO: this is linux-only. Port this to Windows
*/

static int GetDeviceID() {
#ifdef __linux__
	if (GLXEW_MESA_query_renderer) {
		unsigned int vendor = 0, device = 0;
		glXQueryCurrentRendererIntegerMESA(GLX_RENDERER_VENDOR_ID_MESA, &vendor);
		glXQueryCurrentRendererIntegerMESA(GLX_RENDERER_DEVICE_ID_MESA, &device);

		return device | (vendor << 16);
	}
	else {
		Log::GetLog()->Fatal("gfx-device-manager",
			"You doesn't seem to support the MESA extensions.\n"
			"\t\tThey are the only way supported to get the"
			"device ID");

	}

#endif
	Log::GetLog()->Warning("gfx-device-manager",
		"Could not retrieve the ID for this device");
	return 0;
}

/* Gets the device RAM count */
static size_t GetDeviceVRAM() {

	if (GLEW_NVX_gpu_memory_info) {
		int vram = 0;

		//NVIDIA
		Log::GetLog()->InfoWrite("gfx-device-manager",
			"GL_NVX_gpu_memory_info is supported. Probably running in a Nvidia GPU");
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &vram);
		return vram / 1024;

	}
	else if (GLEW_ATI_meminfo) {
		// ATI/AMD
		Log::GetLog()->InfoWrite("gfx-device-manager",
			"GL_ATI_meminfo is supported. Probably running in a AMD GPU");
		Log::GetLog()->InfoWrite("gfx-device-manager",
			"GL_ATI_meminfo only allows getting avaliable memory, the vram count might be incorrect");

		int freevbo[4], freetex[4], freerb[4];
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, freevbo);
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, freetex);
		glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, freerb);

		return (freevbo[0] + freetex[0] + freerb[0]) / 1024;
	}
#ifdef __linux__
	else if (GLXEW_MESA_query_renderer) {
		// Generic: MESA
		Log::GetLog()->InfoWrite("gfx-device-manager",
			"GLX_MESA_query_renderer is supported");

		unsigned int uvram;
		glXQueryCurrentRendererIntegerMESA(GLX_RENDERER_VIDEO_MEMORY_MESA,
			&uvram);

		return (size_t)uvram;
	}
#endif

	else {

		Log::GetLog()->Warning("gfx-device-manager",
			"Could not retrieve the vram amount for this device");
		return 0;
	}


}

DeviceInfo DeviceManager::GetMainDevice()
{
	const char* vendor = (const char*)glGetString(GL_VENDOR);
	const char* renderer = (const char*)glGetString(GL_RENDERER);

	size_t vram = GetDeviceVRAM();
	int id = GetDeviceID();

	Log::GetLog()->Write("gfx-device-manager",
		"Using device %s from %s, %zu MB of vram (id %08x)",
		renderer, vendor, vram, id);

	return DeviceInfo(id, renderer, vendor, vram);
}
