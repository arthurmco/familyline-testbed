/*
 * Image display control
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef GUIIMAGECONTROL_HPP
#define GUIIMAGECONTROL_HPP

#include "GUIControl.hpp"

namespace familyline::graphics::gui {

	/**
	 * This control reads a PNG file and draws it into the game
	 *
	 * Obviously it _can_ support other formats, but I only need to support png files now
	 */
	class GUIImageControl : public GUIControl {
	private:
		GUICanvas openImageFromFile(const char* filename);

		GUICanvas image;
		double imageW, imageH;
		const char* filename = nullptr;

	public:
		GUIImageControl(float xPos, float yPos, float width, float height, 
			const char* filename);

		virtual bool processSignal(GUISignal s);
		virtual GUICanvas doRender(int absw, int absh) const;

		virtual ~GUIImageControl() {}
	};
}

#endif // !GUIIMAGECONTROL_HPP
