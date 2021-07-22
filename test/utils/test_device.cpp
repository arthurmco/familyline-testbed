#include "test_device.hpp"

#include "test_framebuffer.hpp"
#include "test_texenv.hpp"

int shader_nidx = 0;

TestShader::TestShader(const char* file, ShaderType type) : Shader(file, type)
{
    idx = ++shader_nidx;
}

familyline::graphics::Framebuffer* TestDevice::createFramebuffer(
    std::string name, int width, int height)
{
    fblist_.push_back(std::make_unique<TestFramebuffer>(name, width, height));
    return fblist_.back().get();
}

std::unique_ptr<familyline::graphics::TextureEnvironment> TestDevice::createTextureEnv() {
    auto env = std::make_unique<TestTextureEnvironment>();
    env->initialize();

    return env;
}
