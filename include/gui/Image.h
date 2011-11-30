//
// File: Image.h
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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"

// #include "../Fluid/mmgr.h"


class Image : public Control
{
 public:
  Image ();
  Image (int x, int y, int gump);
  Image (int x, int y, int gump, int flags);
  ~Image ();
  void SetGump (int gump);
  int GetGump(void);

  virtual void Draw(GumpHandler * gumps);
  virtual bool CheckPixel(int x, int y);
  void SetTiled(int width, int height);
 private:
  int gump;
  bool tiled;
  int tiled_w, tiled_h;
  Texture * tiled_tex;
};


#endif
