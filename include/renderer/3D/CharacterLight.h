//
// File: Character.h
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

/*
   This Module handles dynamich lighting of characters
*/
 
#ifndef _CHARACTERLIGHT_H_
#define _CHARACTERLIGHT_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <list>

#include "irisgl.h"
#include "SDL/SDL.h"

#include "loaders/StaticModels.h"
#include "renderer/3D/Light3D.h"
#include "uotype.h"

// #include "../Fluid/mmgr.h"

// contains information about character lighting, for more information see engine doc
class cCharacterLight {
    private:
       sColor * color_array;  // contains a raster of colors
       int m_size_x, m_size_y, m_size_z;
    public:
       cCharacterLight (int size_x, int size_y, int size_z);
       virtual ~cCharacterLight ();
       
       // Generates lookup table
       void Generate (float char_x, float char_y, float char_z, int blockx, int blocky, sColor ambient_color, sColor sun_color, float direction [3], float * invmatrix); 
       void CalcColor (int x, int y, int z, float * normal, float * result); // normal and result must be arrays
};


 
#endif //_CHARACTERLIGHT_H_
