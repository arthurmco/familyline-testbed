#pragma once

#include <stdexcept>
#include <string_view>

class graphical_exception : public std::runtime_error
{
public:
	explicit graphical_exception(std::string_view message);
};


class renderer_exception : public graphical_exception
{
private:
	int errorCode;

public:
	explicit renderer_exception(std::string_view message, int code)
		: graphical_exception(message), errorCode(code)
	{}
};

class shader_exception : public graphical_exception
{
private:
	int errorCode;

public:
	explicit shader_exception(std::string_view message, int code)
		: graphical_exception(message), errorCode(code)
	{}
};