#pragma once

/**
 * Represents a video device
 *
 * Almost unused on OpenGL, might be useful on Vulkan
 *
 * Copyright (C) 2019 Arthur Mendes
 */

#include <SDL2/SDL.h>

#include <any>
#include <string_view>
#include <vector>

#include <client/graphical/renderer.hpp>

namespace familyline::graphics
{
class Window;
    class Shader;
    class ShaderProgram;
    
class Device
{
public:
    /// Get the device code, name and vendor
    virtual std::string_view getCode()   = 0;
    virtual std::string_view getName()   = 0;
    virtual std::string_view getVendor() = 0;

    virtual bool isDefault() = 0;

    /// Get data that wil only make sense to the API, or to the respective
    /// window (ex: a GLDevice data will only make sense to a GLWindow,
    /// a VulkanDevice to a Vulkan Window...)
    virtual std::any getCustomData() = 0;

    virtual Shader createShader(const char* file, ShaderType type) = 0;
    virtual ShaderProgram* createShaderProgram(std::string_view name,
                                               std::initializer_list<Shader> shaders) = 0;
    
    virtual Window* createWindow(size_t w, size_t h) = 0;
};

class GLDevice : public Device
{
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

    virtual Window* createWindow(size_t w, size_t h);

    virtual Shader createShader(const char* file, ShaderType type);
    virtual ShaderProgram* createShaderProgram(std::string_view name,
                                               std::initializer_list<Shader> shaders);
};

/**
 * Get a list of devices in the machine
 */
std::vector<Device*> getDeviceList();
}  // namespace familyline::graphics
