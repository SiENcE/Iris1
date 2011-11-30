//
// File: MultisLoader.h
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

#ifndef _MULTISLOADER_H_
#define _MULTISLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"

#include <list>
#include <string>

#include "SDL/SDL.h"
#include "uotype.h"

#include <iostream>
#include <fstream>

// #include "../Fluid/mmgr.h"


#define MAX_MULTIS 8196
#define MULTI_ID 0x4000

// Compiler directives
#if defined(WIN32) && !defined(__MINGW32__)
// Visual C pragma
#pragma pack(push, 1)
#define STRUCT_PACKED
#else
// GCC packed attribute
#define STRUCT_PACKED	__attribute__ ((packed))
#endif

struct stMultiPart
{
	unsigned short tile;
	signed short x;
	signed short y;
	signed short z;
	bool visible;
}STRUCT_PACKED;

struct stMultiList
{
	std::list<stMultiPart> multiParts;
	bool ishouse;
};

#if defined(WIN32) && !defined(__MINGW32__)
#pragma pack(pop)
#endif

typedef std::list<stMultiPart>::const_iterator MultiIter;

class cMultisLoader
{
private:
	int MultiCount;
	stMultiList MultiList[MAX_MULTIS];
    
    
public:
	cMultisLoader (std::string filename, std::string indexname);
	~cMultisLoader ();
	bool IsHouse(unsigned short id) 
	{ 
		if(id >= MULTI_ID) id-=MULTI_ID;
		if(id >= MAX_MULTIS)
			return false;
		return MultiList[id].ishouse;
	}
	bool MultiExists(unsigned short id)
	{
		if(id >= MULTI_ID) id-=MULTI_ID;
		if (id >= MAX_MULTIS)
			return false;
		return (MultiList[id].multiParts.size() ? true : false);
	}

	stMultiList* GetMulti(unsigned short id)
	{ 
		if(id >= MULTI_ID) id-=MULTI_ID;
		if (id >= MAX_MULTIS)
			return NULL;
		return &MultiList[id];
	}
   

protected:
};

extern	cMultisLoader * pMultisLoader;

#endif //_MULTISLOADER_H_
