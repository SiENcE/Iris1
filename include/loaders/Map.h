//
// File: Map.h
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

#ifndef _MAP_H_
#define _MAP_H_

// #include "../Fluid/mmgr.h"

struct MulBlock;
struct stIndexRecord;
struct staticentry;

// Abstract BaseClass
class MapLoader
{
public:
	virtual ~MapLoader() { };
	virtual void LoadMapBlock( int x, int y, MulBlock * block ) = 0;
	virtual struct staticinfo *LoadStatics(int x, int y, int &len) = 0;
};

extern MapLoader *pMapLoader;

#endif //_MAP_H_
