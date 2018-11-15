/*
 * Button drawing code
 *
 * Copyright (C) 2018 Arthur Mendes
 *
 */

#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include <functional>

#include "GUIControl.hpp"
#include "GUILabel.hpp"

namespace familyline::graphics::gui {


	/* This is the default click handler prototype.
	 * It's a std::function because we can use closures with it, unlike function pointers
	 */
	typedef std::function<void(GUIControl*)> OnClickHandler;


	class GUIButton : public GUIControl {
	private:
		GUILabel* label = nullptr;

	public:
		OnClickHandler onClickHandler;

		GUIButton(float x, float y, float w, float h, const char* text);

		virtual bool processSignal(GUISignal s);
		virtual GUICanvas doRender(int absw, int absh) const;

		virtual ~GUIButton();
	};


}


#endif /* GUIBUTTON_H */
