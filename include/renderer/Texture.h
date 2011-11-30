//
// File: Texture.h
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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "Common.h"
#include "SDL/SDL.h"
#include "irisgl.h"

#include "../Buffer.h"
#include "Bitmask.h"

// #include "../Fluid/mmgr.h"

class Texture : public BufferEntry
{
private:

 GLuint gltex;
 bool assigned;
 int width;
 int height;
 int real_width;
 int real_height;
 
 cBitmask * bitmask;

public:
    Texture ();
   ~Texture ();

   int LoadFromData( void * data, int width, int height, int bits_per_pixel, int filtertype, bool do_bitmask = false);
   int LoadFromSurface(SDL_Surface * surface, int filter = GL_LINEAR);
   int LoadFromFile( const char * filename);
   int LoadFromSurfaceWithColorKey(SDL_Surface * surface, int colorkey );
   int LoadFromFileWithTransparency( const char * filename, unsigned char transparency);
   GLuint GetGLTex(void);
   bool GetAssigned(void);
   int GetHeight(void);
   int GetWidth(void);
   int GetRealHeight(void) { return real_height; }
   int GetRealWidth(void) { return real_width; }
   void SetRealSize(int width, int height) 
   {
   	this->real_width = width; 
	this->real_height = height;
   }
   
   bool CheckPixel(int x, int y, bool swapy = false);

protected:

};


#endif //_TEXTURE_H_
