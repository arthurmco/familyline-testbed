/*
 * Label drawing code
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef GUILABEL_H
#define GUILABEL_H

#include <string>

#include "GUIControl.hpp"

namespace familyline::graphics::gui {

	class GUILabel : public GUIControl {
	private:
		std::string text;


		// Set the label size according to the text
		void autoResize(GUISignal s);

		int absw, absh;

	public:

		GUILabel(float x, float y, const char* text);

		bool processSignal(GUISignal s);
		const char* getText() const;
		void setText(const char* s);

		virtual GUICanvas doRender(int absw, int absh) const;

		virtual ~GUILabel() {}
	};



}


#endif /* GUILABEL_H */
