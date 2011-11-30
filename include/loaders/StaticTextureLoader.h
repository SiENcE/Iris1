//
// File: StaticTextureLoader.h
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#ifndef _STATICTEXTURELOADER_H_
#define _STATICTEXTURELOADER_H_

#include "Common.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Exception.h"
#include "renderer/Texture.h"
#include "iris_endian.h"
#include "uotype.h"

// #include "../Fluid/mmgr.h"

#define TEXTUREFLAG_COLORKEY 1

class cStaticTexture
{
public:
	cStaticTexture( Uint32 stream_start, Uint32 stream_length );
	~cStaticTexture();

	void LoadTexture( std::ifstream *stream );
	Texture * texture() { return m_texture; }
	void SetColorKey( Uint32 colorkey );

private:
	Uint32 m_stream_start, m_stream_length;
	bool m_hascolorkey;
	Uint32 m_colorkey;
	Texture *m_texture;
};

class cStaticTextureLoader
{
public:
	cStaticTextureLoader( std::ifstream *stream, Uint32 length );
	~cStaticTextureLoader();

	Texture *GetTexture( Uint32 index );
	Texture *GetGroundTexture( Uint32 id ); // Get replaced ground texture

private:
	std::ifstream *modelstream;
	Uint32 texturestream_start;
	Uint32 texturestream_length;
	Uint32 texturestream_end;
	std::vector<cStaticTexture *> textures;

	std::map <Uint32, Uint32> ground_texture_map;
};

#endif //_STATICTEXTURELOADER_H_
