/*
 * Label drawing code
 *
 * Copyright (C) 2018 Arthur Mendes
 */

#ifndef GUILABEL_H
#define GUILABEL_H

#include <string>

#include "GUIControl.hpp"

namespace Familyline::Graphics::GUI {

    class GUILabel : public GUIControl {
    private:
	std::string text;
    
    public:

	GUILabel(float x, float y, const char* text);

	bool processSignal(GUISignal s);
	const char* getText() const;
	void setText(const char* s);
    
	virtual void doRender(int absw, int absh) const;

	~GUILabel();
    };

    
    
}


#endif /* GUILABEL_H */
