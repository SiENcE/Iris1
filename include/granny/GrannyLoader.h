//
// File: GrannyLoader.h
// Created by:   Alexander Oster - tensor@ultima-iris.de
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

#ifndef _GRANNYLOADER_H_
#define _GRANNYLOADER_H_

#include "Common.h"
#include "SDL/SDL.h"
#include <string>
#include <map>

#include "GrannyModel.h"

// #include "../Fluid/mmgr.h"

class cGrannyLoader
{
private:
	std::string mulpath;
	std::string tex_basepath;
	std::map <Uint32, cGrannyModel *> models;
	int tick;

public:
    cGrannyLoader( std::string filename, std::string mulpath );
    ~cGrannyLoader();
 
    void Clear ();
    void Render(Uint32 id, Uint32 type, float & curtime, cCharacterLight * character_light, float r, float g, float b, float alpha, GrnMatrix * left_matrix = NULL, GrnMatrix * right_matrix = NULL, bool is_corpse = false);
    void Render(Uint32 id, Uint32 type, float & curtime, cCharacterLight * character_light, float r, float g, float b, float alpha, std::vector<int> bodyparts, GrnMatrix * left_matrix = NULL, GrnMatrix * right_matrix = NULL, bool is_corpse = false);
    int GetAnimset(Uint32 id);
    bool isFound(Uint32 id);
};

extern cGrannyLoader *pGrannyLoader;

#endif //_GRANNYLOADER_H_
