/*****
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****/

#ifndef _LABEL_H_
#define _LABEL_H_

#include "Common.h"
#include <iostream>
#include <vector>
#include "Control.h"
#include "TextManager.h"
#include "gui/TextManager.h"

// #include "../Fluid/mmgr.h"

#define ALIGN_LEFT  0
#define ALIGN_CENTER  1
#define ALIGN_RIGHT  2

class Label : public Control
{
public:
	Label( int x, int y, const char *text, unsigned short hue = 0, unsigned char font = 3 );
	~Label();

	const char *text() const { return _text; }
	void setText( char *text );

	unsigned short hue() const { return _hue; }
	void setHue( unsigned short hue );

	unsigned char font() const { return _font; }
	void setFont( unsigned char font );

	void Draw( GumpHandler *gumps );
	cTextElement * GetElement(void);

	void setAlign(unsigned int align);
	unsigned int align() { return _align; }

	unsigned int GetTimeOut(void);
	void SetTimeOut(unsigned int timeout);
	bool OutTimed(void);
	int labelWidth();
	int labelHeight();
	void Crop(int width, int height);
	void SetRGBColor(int r, int g, int b);
	void SetStyle(int style);
	int Wrap(int width);
	virtual bool MouseIsOver( int x, int y);

private:
	char *_text;
	unsigned short _hue;
	unsigned char _font;
	unsigned int timeout;
	unsigned int creation_time;
	unsigned int _align;

	bool rgbcolor;
	float _r, _g, _b;
	int _style;
	bool _wrapped;

	cTextElement *element;
	std::vector<cTextElement*> wrapped_text;
	void refresh();
};

#endif	// _LABEL_H_
