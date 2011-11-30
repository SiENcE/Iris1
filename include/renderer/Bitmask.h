//
// File: Bitmask.h
// Created by: Alexander Oster - tensor@ultima-iris.de
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

#ifndef _BITMASK_H_
#define _BITMASK_H_

#include "SDL/SDL.h"
#include <iostream>
#include <math.h>
#include <assert.h>

#include "include.h"
#include "Config.h"

// #include "../Fluid/mmgr.h"

class cBitmask
{
public:
	cBitmask();
	~cBitmask();
	
	void Create( Uint32 *data, int width, int height );
	bool CheckPixel( int x, int y, bool swapy = false );

private:
	Uint8 *pixels;
	int m_width, m_height;
	int m_mem;
};

#endif //_BITMASK_H_
