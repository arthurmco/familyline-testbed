#pragma once

#include <string_view>

namespace familyline::graphics {

/**
 * Represents an memory area where you can draw 3D data
 * 
 * This memory area will have 2D data, an image, which you can
 * use as a texture for something, like an gui overlay, or as a
 * map for something like shadows
 */
class Framebuffer {
private:
	std::string_view _name;

	unsigned int _handle;

	// texture, for color data, read-write
	unsigned int _textureHandle;

	// renderbuffer, for depth data, write-only
	unsigned int _rboHandle;

	void setupTexture(int width, int height);
public:
	Framebuffer(std::string_view name, int width, int height);
	
	/// Call this before you start drawing data
	void startDraw();

	/// Call this after you end drawing data
	void endDraw();

	int getTextureHandle();

	~Framebuffer();
};
}
