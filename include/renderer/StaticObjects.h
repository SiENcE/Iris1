//
// File: StaticObjects.h
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

 /*
    cStaticObjectList contains the static object of one map block.
    It is record-array-based due to speed reasons..
 */
 
#ifndef _STATICOBJECTS_H_
#define _STATICOBJECTS_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <vector>

#include "renderer/Fade.h"
#include "renderer/3D/MotiveBasedLight.h"

// #include "../Fluid/mmgr.h"

#define STATICOBJECTLIST_ARRAY_SIZE 256


/** A Static Object */
struct sStaticObject {
  int x, y, z;  // Position, x and y are values from 0-7
  int blockx, blocky; // block of this object
  int tileid;  // item type
  unsigned int flags; // object specific flags
  unsigned int tiledata_flags; // uo tiledata flags (see file format doc)
  int height;  // height in uo coords (= world coords * 10)
  bool clip;  // used for frustum culling
  Uint8 alpha;  // used for fading
  Uint16 hue;  // color
  float sphere[5];  // bounding sphere, used by frustum culling
  int dynamic_id;  
  cFader * fader;  // fader class
  cMotiveBasedLight * light;  // class for lighting
};

class cStaticObjectList
{
private:
    std::vector <sStaticObject *> cache; // a vector of arrays of the size STATICOBJECTLIST_ARRAY_SIZE
    sStaticObject * m_current_array; // the current array
    
    Uint32 m_count;
    Uint32 m_head;  // index for adding
    
    void AllocNewArray ();
       
public:
    cStaticObjectList ();
   ~cStaticObjectList ();
   
   struct sStaticObject * Add (void);    // Adds a new object (struct is zeroed)
   struct sStaticObject * Get (unsigned int index); // gets object #index - expects a valid index number
   Uint32 count ();  // returns list count

protected:

};



#endif //_STATICOBJECTS_H_
