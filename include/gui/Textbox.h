//
// File: Textbox.h
// Created by:
//
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

#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <vector>
#include <map>

#include "SDL/SDL.h"
#include "Label.h"

// #include "../Fluid/mmgr.h"


class Textbox : public Control
{
public:
	Textbox();
	~Textbox();

	virtual void Draw( GumpHandler *gumps );
	void AddMessage(char * message, unsigned int timeout, unsigned short hue = 0);

private:
	std::vector< Label * > labels;

};


#endif
