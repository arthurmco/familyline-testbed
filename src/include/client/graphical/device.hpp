#pragma once

/**
 * Represents a video device
 *
 * Almost unused on OpenGL, might be useful on Vulkan
 *
 * Copyright (C) 2019 Arthur Mendes
 */

#include <SDL2/SDL.h>
#include <vector>
#include <string_view>
#include <any>

namespace familyline::graphics {
    
    class Device {
    public:
	
        /// Get the device code, name and vendor
        virtual std::string_view getCode() = 0;
        virtual std::string_view getName() = 0;
        virtual std::string_view getVendor() = 0;
	
        virtual bool isDefault() = 0;

        /// Get data that wil only make sense to the API, or to the respective 
        /// window (ex: a GLDevice data will only make sense to a GLWindow, 
        /// a VulkanDevice to a Vulkan Window...)
        virtual std::any getCustomData() = 0;
	
    };

    class GLDevice : public Device {
    private:
        std::string_view _code;
        bool _default;
        std::any _data;

    public:
        GLDevice(const char* code, bool is_default, int index);

        /// Get the device code, name and vendor
        virtual std::string_view getCode();
        virtual std::string_view getName();
        virtual std::string_view getVendor();

        
        virtual bool isDefault();

        virtual std::any getCustomData();
    };


    /**
     * Get a list of devices in the machine
     */
    std::vector<Device*> getDeviceList();
}
