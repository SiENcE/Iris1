//
// File: MapBuffer.h
// Created by: Darkstorm
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

#ifndef _MAPBUFFER_H_
#define _MAPBUFFER_H_

#include <assert.h>
#include "MapBlock.h"
#include "Config.h"
#include "renderer/Camera.h"
#include "include.h"

// #include "../Fluid/mmgr.h"

class Mapbuffer 
{
public:
	Mapbuffer();
	virtual ~Mapbuffer();

	void Clear();
	int GetCount( void );

	cMapblock *Get( int x, int y );
	void Add(cMapblock * block);
	void FreeBuffer(int radius);
   
	virtual cMapblock *CreateBlock( int x, int y ) = 0;
   
	void setRoofZ( int z ) { m_roof_z = z; }
	void SetUsageFlag( bool value );
   
	void ResetFader( cFader *fader );
	void UpdateAlpha();

	void ResetLight();

protected:
	typedef std::map<Uint32, cMapblock *> MapBuffer_t;
	MapBuffer_t root;
	int m_roof_z;
};


class MapbufferHandler
{
public:
	MapbufferHandler();
	~MapbufferHandler();
	
	void Init( Mapbuffer *map_buffer );
	void DeInit();
	Mapbuffer *buffer();

private:
	Mapbuffer *map_buffer;
};


extern MapbufferHandler pMapbufferHandler;

#endif //_MAPBUFFER_H_
