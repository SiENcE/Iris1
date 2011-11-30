//
// File: ClilocLoader.h
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

#ifndef __CLILOC__LOADER__
#define __CLILOC__LOADER__

#include <iostream>

#include <fstream>

#include <cstring>
#include <vector>
#include <string>
#include <map>

#ifdef WIN32
#include <windows.h>
#endif

// #include "../Fluid/mmgr.h"

using namespace std;

class cClilocLoader
{
  public:
      cClilocLoader ();
      ~cClilocLoader();

      bool Init (std::string path);
      void DeInit ();
      
      std::string GetMessage(int id);
      std::string GetMessageWithArguments(int id, int args_num, vector<std::string> &args);

  private:
      std::map <int, std::string> cliloc_messages;
};

extern cClilocLoader pClilocLoader;

#endif
