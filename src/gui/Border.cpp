//
// File: Border.cpp
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


#include "gui/Border.h"
#include "Logger.h"
#include "Config.h"
#include "loaders/GumpLoader.h"




using namespace std;

Border::Border ()
{
  Control::Control ();
  for (int i = 0; i < 9; i++)
    textures[i] = NULL;

  setGump (0);
  control_type = CONTROLTYPE_BORDER;
}

Border::Border (int x, int y, unsigned short gump)
{
  Control::Control ();

  for (int i = 0; i < 9; i++)
    textures[i] = 0;

  setGump (gump);
  SetPosition (x, y);
  control_type = CONTROLTYPE_BORDER;
}

Border::Border (int x, int y, unsigned short gump, int flags)
{
  Control::Control ();

  for (int i = 0; i < 9; i++)
    textures[i] = 0;

  setGump (gump);
  SetPosition (x, y);
  SetFlags (flags);
  control_type = CONTROLTYPE_BORDER;
}

Border::~Border ()
{
  delete textures[1];
  delete textures[3];
  delete textures[4];
  delete textures[5];
  delete textures[7];
}

void Border::generateTextures ()
{
  // Delete Cached Ones
  delete textures[1];
  delete textures[3];
  delete textures[4];
  delete textures[5];
  delete textures[7];

  // Create new Textures
  int x, y;
  int img_x[3], img_y[3];
  int img_width, img_height;

  GetPosition (&x, &y);

  img_x[0] = x;
  img_x[1] = x + pGumpLoader.GetGumpWidth (_gump);
  img_x[2] = x + GetWidth () - pGumpLoader.GetGumpWidth (_gump + 2);

  img_y[0] = y;
  img_y[1] = y + pGumpLoader.GetGumpHeight (_gump);
  img_y[2] = y + GetHeight () - pGumpLoader.GetGumpHeight (_gump + 6);

  img_width = img_x[2] - img_x[1];
  img_height = img_y[2] - img_y[1];

  // Borders
  textures[1] = pGumpLoader.LoadGumpTiled (_gump + 1, img_width, -1);
  textures[3] = pGumpLoader.LoadGumpTiled (_gump + 3, -1, img_height);
  textures[5] = pGumpLoader.LoadGumpTiled (_gump + 5, -1, img_height);
  textures[7] = pGumpLoader.LoadGumpTiled (_gump + 7, img_width, -1);

  // Content
  textures[4] = pGumpLoader.LoadGumpTiled (_gump + 4, img_width, img_height);

  generated = true;
}

void Border::Draw (GumpHandler *gumps)
{
  Control::Draw (gumps);

  if (!_gump)
    return;

  if (!generated)
    generateTextures ();


  int x, y;
  int img_x[3], img_y[3];
  int img_width, img_height;

  unsigned char alpha = GetAlpha ();

  GetPosition (&x, &y);

  img_x[0] = x;
  img_x[1] = x + pGumpLoader.GetGumpWidth (_gump);
  img_x[2] = x + GetWidth () - pGumpLoader.GetGumpWidth (_gump + 2);

  img_y[0] = y;
  img_y[1] = y + pGumpLoader.GetGumpHeight (_gump);
  img_y[2] = y + GetHeight () - pGumpLoader.GetGumpHeight (_gump + 6);

  img_width = img_x[2] - img_x[1];
  img_height = img_y[2] - img_y[1];

  // Load Corners
  textures[0] = LoadGump (_gump, gumps, false);
  textures[2] = LoadGump (_gump + 2, gumps, false);
  textures[6] = LoadGump (_gump + 6, gumps, false);
  textures[8] = LoadGump (_gump + 8, gumps, false);

  // Draw Content
  DrawRect (img_x[1], img_y[1], 0, 0, textures[4], alpha);

  // Draw Corners
  DrawRect (img_x[0], img_y[0], 0, 0, textures[0], alpha);
  DrawRect (img_x[2], img_y[0], 0, 0, textures[2], alpha);
  DrawRect (img_x[0], img_y[2], 0, 0, textures[6], alpha);
  DrawRect (img_x[2], img_y[2], 0, 0, textures[8], alpha);

  // Draw Borders
  DrawRect (img_x[1], img_y[0], 0, 0, textures[1], alpha);
  DrawRect (img_x[1], img_y[2], 0, 0, textures[7], alpha);

  DrawRect (img_x[0], img_y[1], 0, 0, textures[3], alpha);
  DrawRect (img_x[2], img_y[1], 0, 0, textures[5], alpha);
}
