/*
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
 */

#ifndef _FONT_H_
#define _FONT_H_

#include <iostream>
#include <string>
#include "SDL/SDL_ttf.h"

// #include "../Fluid/mmgr.h"

/*
 * Our Font information structure.
 */
struct FontInfo
{
	int iId;
	std::string sFileName;
	std::string sFontName;
	int iSize;
	int iHue;
};


class Font
{
public:
	Font( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue );
	~Font();

	TTF_Font *GetFont() const;
	int GetId() const;

private:
	TTF_Font *m_kFont;
	int m_iId;
	std::string m_sFileName;
	std::string m_sFontName;
	int m_iSize;
	int m_iHue;
};

#endif
