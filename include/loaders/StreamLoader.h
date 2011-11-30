//
// File: StreamLoader.h
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

#ifndef _STREAMLOADER_H_
#define _STREAMLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include "include.h"
#include "SDL/SDL.h"

// #include "../Fluid/mmgr.h"

class cStreamLoader
{
private:
    Uint8 * m_data;
    Uint8 * m_eof;
    Uint8 * m_file_pos;
 
public:
    cStreamLoader (std::ifstream * file, Uint32 size);
    ~cStreamLoader ();
    void seek (Uint32 pos);
    Uint8 * read (Uint32 size);
    Uint8 read_uint8 ();
    Uint16 read_uint16 ();
    Sint16 read_sint16 ();
    Uint32 read_uint32 ();
    Sint32 read_sint32 ();
    Uint16 * read_uint16 (Uint32 count);
    Sint16 * read_sint16 (Uint32 count);
    Uint32 * read_uint32 (Uint32 count);
    Sint32 * read_sint32 (Uint32 count);
    Uint32 size ();
    Uint32 pos ();
    bool eof ();
};

#endif //_STREAMLOADER_H_
