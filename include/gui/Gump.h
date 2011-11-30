//
// File: Gump.h
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

#ifndef _GUMP_H_
#define _GUMP_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "../renderer/Texture.h"
#include <map>
#include <list>

// #include "../Fluid/mmgr.h"


class GumpHandler
{
public:
  GumpHandler ();
  ~GumpHandler ();
  void ClearTextures (void);
  Texture * GetTexture (int index);
  void AddTexture(int index, Texture * texture);
  void DelTexture(int index);

private:
	typedef std::map<Uint32, Texture *>	GumpBuffer_t;
 GumpBuffer_t	tex_root;

};

extern GumpHandler pGumpHandler;

#endif
