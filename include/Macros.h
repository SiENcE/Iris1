//
// File: Macros.h
// Created by: ArT-iX ArT-iX@libero.it
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

#ifndef _MACROS_
#define _MACROS_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <string>
#include <map>
#include <vector>

// #include "../Fluid/mmgr.h"


#define PARAMETERTYPE_STRING 1;
#define PARAMETERTYPE_INTEGER 2;

struct m_Parameter
{
 int type;
 std::string str_value;
 int int_value;    
};


struct MacroEntry
{

 int id;
 //std::string name;
 SDLKey key;
 SDLMod keymod;
 std::string script_function;
 std::vector<m_Parameter*> parameters;
};

class MacroLoader
{
 private:
     //std::map<int, MacroEntry*> macros;
     std::multimap<int, MacroEntry*> macros;
     SDLKey getkey_byname(std::string keyname);
 public:
  MacroLoader();
  ~MacroLoader();     
  MacroEntry * GetMacro(int id);
  int GetEntriesCount(int id);
  MacroEntry * GetMultiMacro(int id, int index); 
};

extern MacroLoader * pMacroLoader;

#endif
