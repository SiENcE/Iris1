//
// File: SkillLoader.h
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

#ifndef _SKILLLOADER_H_
#define _SKILLLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "include.h"

#include "SDL/SDL.h"

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

// #include "../Fluid/mmgr.h"

class cSkillEntry
{
    private:
		std::string m_name;
		unsigned char m_use_button;
	public:
		std::string name ();
		unsigned char use_button ();
       cSkillEntry (std::string name, unsigned char use_button);
};

class cSkillLoader
{
private:
	std::map <Uint16, cSkillEntry *> skilllist;
 
public:
    cSkillLoader ();
   ~cSkillLoader ();

   void Init (std::string filename, std::string indexname);
   void DeInit ();

   cSkillEntry * GetSkill(Uint16 id);

};

extern cSkillLoader pSkillLoader;

#endif //_SKILLLOADER_H_
