#include "test_device.hpp"
#include "test_framebuffer.hpp"

int shader_nidx = 0;

TestShader::TestShader(const char* file, ShaderType type) : Shader(file, type)
{
    idx = ++shader_nidx;
}


familyline::graphics::Framebuffer* TestDevice::createFramebuffer(
    std::string name, int width, int height)
{
    return new TestFramebuffer{name, width, height};
}

