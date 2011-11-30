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

#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <assert.h>
#include "Common.h"
#include "Exception.h"
#include "iris_endian.h"
#include "uotype.h"
#include "Font.h"

// #include "../Fluid/mmgr.h"

struct stChar
{
	unsigned char width;
	unsigned char height;
	unsigned char header; // Unknown
	unsigned char *redmask; // red mask pixel data (for hueing only)
	unsigned int *pixels; // Pixel Data
};

struct stFont
{
	unsigned char maxHeight;
	stChar chars[224]; // The first 32 characters are NOT included (control chars)
};


class FontManager
{
public:
	FontManager();
	~FontManager();
	static FontManager *GetInstance();

	void AddTTFFont( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue );
	void AddMulFont( std::string sFileName );

	void UnregisterTTFFont( int iId );

	TTF_Font *GetTTFFont( int iId );
	const stFont *GetMulFont( unsigned short iId ) const;

private:
	static FontManager *m_sgFontManager;
	std::vector<Font *> m_vFonts;
	std::map<unsigned short, stFont> m_mFonts;
};

#endif
