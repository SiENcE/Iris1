//
// File: GrannyStream.h
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

#ifndef _GRANNYSTREAM_H_
#define _GRANNYSTREAM_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <stdlib.h>
#include <fstream>
#include <string>

// #include "../Fluid/mmgr.h"

class cGrannyStream 
{
   private:
      char * m_data;
      Uint32 m_size;
      Uint32 m_pos;
      std::string m_filename;
   public:
      cGrannyStream (std::fstream * file, std::string filename);
      ~cGrannyStream ();
      
      bool seekg (Uint32 pos);
      Uint32 tellg ();
      bool read (char * data, Uint32 count);
      
      Uint32 readDword ();
      void get(char & value);
      
};

#endif //_GRANNYSTREAM_H_
