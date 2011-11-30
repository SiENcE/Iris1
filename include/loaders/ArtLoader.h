/*! \file ArtLoader.h
* \brief Loads Art file
* 
* ArtLoader deals with all kind of Art Loads, puts in memory then removes them from memory.
*
* Copyright (©) Iris Team
*/

/*
* Created by Alexander Oster.
* Last change: 25-02-06 (Nuno Ramiro)
*/

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

#ifndef _ARTLOADER_H_
#define _ARTLOADER_H_

#include "Common.h"
#include <iostream>
#include <string.h>
#include "Logger.h"
#include "Exception.h"
#include "loaders/VerdataLoader.h"
#include "../renderer/Texture.h"

// #include "../Fluid/mmgr.h"

class ArtLoader
{
public:
	ArtLoader( std::string sFileName, std::string sIndexName );
	~ArtLoader();

	/// Returns ArtLoader Instance, NOTE that it MUST be instantiated before by using new.
	static ArtLoader *GetInstance();

	/// Load an Art Object (Static/Ground).
	Texture *LoadArt( int index, bool is2D = false, bool isCursor = false, Uint16 hue = 0 );

private:
	// Singleton
	static ArtLoader *m_sgArtLoader;

	std::ifstream *m_kArtFile;
	std::ifstream *m_kArtIndex;
	unsigned int m_uiArtCount;
	std::vector<Texture *> m_vTextures;	// < Keeps our textures, and deletes all of them on destructor.

private:
	Texture *LoadGroundArt( int index );
	Texture *LoadStaticArt( int index, bool is2D = false, bool isCursor = false );
};

#endif // _ARTLOADER_H_
