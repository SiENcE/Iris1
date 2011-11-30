//
// File: TileDataBuffer.h
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

#ifndef _TILEDATABUFFER_H_
#define _TILEDATABUFFER_H_

#include "SDL/SDL.h"
#include "uotype.h"
#include "../Cache.h"
#include "../Buffer.h"

// #include "../Fluid/mmgr.h"

class TileDataBuffer  
{
public:
   TileDataBuffer ();
   virtual ~TileDataBuffer ();

   void Clear ();

   TileDataStaticEntry * GetEntry(int index);

protected:
	Cache< TileDataStaticEntry > entries;
};

extern	TileDataBuffer pTileDataBuffer;

#endif //_TILEDATABUFFER_H_
