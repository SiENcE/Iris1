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
/*
	File: TextManager.h
	Created: DarkStorm
	Purpose: Manage "free" OnScreen Text
*/

#ifndef _TEXTMANAGER_H_
#define _TEXTMANAGER_H_

#include "Common.h"
#include "renderer/Texture.h"
#include "SDL/SDL_ttf.h"

// #include "../Fluid/mmgr.h"

/*
	Class for external Text
*/
class cTextElement
{
private:
	struct stTextItem *p;
	unsigned int _width;
	unsigned int _height;
	int _style;

public:
	void draw( int x, int y, unsigned int width = 0, unsigned int height = 0, int xOffset = 0 ); // Need to get into "2d mode" before calling this

	void setText( const char *text, unsigned short hue = 0, unsigned short font = 3 );
	void setTextTTF( const char *text, unsigned short hue, TTF_Font * font);
	unsigned int width() const { return _width; }
	unsigned int height() const { return _height; }

	cTextElement( const char *text, unsigned short hue = 0, unsigned short font = 3, int style = 0 );
	virtual ~cTextElement();
	
	Texture * texture ();
};

class cFixedTextElement : public cTextElement
{
private:
	int _x, _y;
public:
	cFixedTextElement( int x, int y, const char *text, unsigned short hue, unsigned short font ): cTextElement( text, hue, font ), _x( x ), _y( y )  {};
	~cFixedTextElement() { }

	void setX( int data ) { _x = data; }
	int x() { return _x; }

	void setY( int data ) { _y = data; }
	int y() { return _y; }

	void draw()
	{
		cTextElement::draw( _x, _y );
	}
};

class cTextManager
{
private:
	// Private Implementation
	struct TextManagerPrivate *p;

public:
	cTextManager();
	virtual ~cTextManager();

    void draw();
	unsigned char getTextHeight( const char *text, unsigned short font = 3 );
	unsigned int getTextWidth( const char *text, unsigned short font = 3 );

	unsigned char getTextHeightTTF( const char *text, TTF_Font * font);
	unsigned int getTextWidthTTF( const char *text, TTF_Font * font);

	// Quick Methods for adding fixed text to the manager 
	// which should be automatically drawn in the correct draw stage
	void addFixedText( cFixedTextElement *element );
	void removeFixedText( cFixedTextElement *element );
};

extern cTextManager * pTextManager;

#endif
