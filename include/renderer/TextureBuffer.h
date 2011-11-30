//
// File: TextureBuffer.h
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

#ifndef _TEXTUREBUFFER_H_
#define _TEXTUREBUFFER_H_

#include "SDL/SDL.h"
#include "loaders/GroundTextures.h"

#include "Texture.h"
#include "Cache.h"
#include "Buffer.h"

// #include "../Fluid/mmgr.h"

class TextureBuffer  
{
public:
	TextureBuffer();
	virtual ~TextureBuffer();

	static TextureBuffer *GetInstance();

	Texture *GetGroundTexture( int index );
	Texture *GetArtTexture( int index );

protected:
	static TextureBuffer *m_sgTextureBuffer;
	Cache<Texture> groundTiles;
	Cache<Texture> artTiles;
};

#endif //_TEXTUREBUFFER_H_
