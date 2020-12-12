#pragma once

#include <fmt/format.h>

#include <stdexcept>
#include <string_view>

namespace familyline::graphics
{
class graphical_exception : public std::runtime_error
{
protected:
    std::string_view _message;
    std::string msg_;

public:
    explicit graphical_exception(std::string_view message);

    virtual const char* what() const noexcept { return msg_.c_str(); }
};

class renderer_exception : public graphical_exception
{
private:
    int errorCode;
    std::string _msg;

public:
    explicit renderer_exception(std::string_view message, int code)
        : graphical_exception(message), errorCode(code)
    {
        _msg = fmt::format("Renderer error {}, code {}", this->_message.data(), this->errorCode);
    }

    virtual const char* what() const noexcept { return _msg.c_str(); }
};

class shader_exception : public graphical_exception
{
private:
    int errorCode;
    std::string _msg;

public:
    std::string file;

    explicit shader_exception(std::string_view message, int code, std::string file="")
        : graphical_exception(message), errorCode(code), file(file)
    {
        _msg = fmt::format("Shader error {}, code {}", this->_message.data(), this->errorCode);
    }

    virtual const char* what() const noexcept { return _msg.c_str(); }
};

/**
 * A list of possible asset errors
 */
enum class AssetError {
    AssetFileOpenError = 1025,
    AssetFileParserError,
    InvalidAssetType,
    AssetNotFound,
    AssetOpenError
};

class asset_exception : public graphical_exception
{
private:
    AssetError errorCode;
    std::string _msg;

public:
    explicit asset_exception(std::string_view message, AssetError code)
        : graphical_exception(message), errorCode(code)
    {
        _msg = fmt::format("Asset error {}, code {}", this->_message.data(), (int)this->errorCode);
    }

    virtual const char* what() const noexcept { return _msg.c_str(); }
};

}  // namespace familyline::graphics

namespace familyline::logic
{
class logic_exception : public std::runtime_error
{
protected:
    std::string_view _message;
    std::string msg_;

public:
    explicit logic_exception(std::string_view message) : std::runtime_error(message.data())
    {
        msg_ = fmt::format("Logic error {}", message.data());
    }

    virtual const char* what() const noexcept { return msg_.c_str(); }
};

}  // namespace familyline::logic
