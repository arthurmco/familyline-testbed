/*
   Graphical device representation class

   Copyright (C) 2017 Arthur M
*/

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <cstddef>
#include <list>

#include "Log.hpp"

namespace Familyline::Graphics {

/* Graphical device information

   "Graphical device" is just a nice name for "video card"
*/
    struct DeviceInfo {
	/* The device ID. 
	   If the device is a PCI device, it's the PCI vendor:device.

	   I don't know what might be if I port this engine where non-PCI
	   graphics card is common (like ARM...)
	*/
	const int device_id;
	
	// Name and vendor
	const char* const device_name;
	const char* const vendor;

	// VRAM size in MB
	const size_t video_ram;

	// Might as well add an array of supported features

	DeviceInfo() = delete;
	DeviceInfo(int device_id, const char* name,
		   const char* vendor, size_t vram);
    };

/* Class that manages graphical devices */
    class DeviceManager {
    public:
	/* Get all devices in this computer 
	   (TODO: See if OpenGL supports getting data from multiple devices
	          Last time I looked, it didn't )
	 */
	static std::list<DeviceInfo> GetDeviceList();

	/* Get the actual device used by the engine to render  */
	static DeviceInfo GetMainDevice();
    };
    
}

#endif /* DEVICE_HPP */
