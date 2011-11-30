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

#ifndef _INPUTFIELD_H_
#define _INPUTFIELD_H_

#include "Common.h"
#include <string>
#include "gui/TextManager.h"
#include "Control.h"

// #include "../Fluid/mmgr.h"

class cTextElement;

class InputField : public Control
{
public:
	InputField( int x, int y, unsigned int width, unsigned int height, const char *text = 0, unsigned short hue = 0, unsigned char font = 3, char passwordChar = 0 );
	~InputField();

	bool MouseIsOver(int x, int y);

	// Getter + Setter
	unsigned int width() const { return _width; }
	unsigned int height() const { return _height; }
	const char *text() const { return _text.c_str(); }
	unsigned short hue() const { return _hue; }
	unsigned short font() const { return _font; }
	char passwordChar() const { return _passwordChar; }

	void setWidth( unsigned int data ) { _width = data; generated = false; }
	void setText( char *text );
	void setHue( unsigned short data ) { _hue = data; generated = false; }
	void setFont( unsigned short data ) { _font = data; generated = false; }
	void setPasswordChar( char data ) { _passwordChar = data; }

	void regenerate();
	void Draw( GumpHandler *gumps );
	int HandleMessage( gui_message *msg );
	void OnKeyPress (int (*callback) (Control *sender, unsigned short key));
	void SetIgnoreCursorKeys(bool IgnoreCursorKeys);
	void DoOnKeyPress (int key);

private:
	int _width;
	int _height; // Automatically calculated (if height == 0)

	unsigned short _hue;
	unsigned short _font;
	unsigned int *data;
	bool generated;
	std::string _text;
	cTextElement *tElement;
	unsigned int xCropOffset;
	char _passwordChar;
	bool IgnoreCursorKeys;

	// Caret information and positioning information
	unsigned int _caretPos;
	Texture *caret;
	unsigned int caretX;
	void recalcXCrop();
	int (*callback_OnKeyPress) (Control *sender, unsigned short key);
};

#endif
