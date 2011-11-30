//
// File: MapBlock.cpp
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


#include <iostream>
#include "renderer/MapBlock.h"
#include "Logger.h"
#include "include.h"
#include "math.h"
#include "Config.h"
#include "uotype.h"
#include "loaders/Map.h"
#include "renderer/TextureBuffer.h"
#include "renderer/SDLScreen.h"
#include "Geometry.h"




using namespace std;

//extern SDLScreen *SDLscreen;


cMapblock::cMapblock (int blockx, int blocky)
{
  _in_use = false;
  _generated = false;
  m_blockx = blockx;
  m_blocky = blocky;
}

cMapblock::~cMapblock ()
{
}



int cMapblock::UpdateFader (int min_z, Uint8 alpha, cFader * fader,
                            bool below)
{
  int count = 0;
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
            {
              bool ok = (((object->z >= min_z) && (!below)) ||
                         ((object->z < min_z) && (below)));
              if (ok && (object->alpha == alpha))
                  {
                    object->fader = fader;
                    count++;
                  }
            }
      }
  return count;
}

void cMapblock::ResetFader (cFader * fader)
{
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
            {
              if (object->fader)
                object->alpha = (int) (object->fader->value () + 0.5f);
              if (object->fader == fader)
                object->fader = NULL;
            }
      }
}


void cMapblock::SetAlpha (int min_z, Uint8 alpha, bool below)
{
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
            {
              bool ok = (((object->z >= min_z) && (!below)) ||
                         ((object->z < min_z) && (below)));
              if (ok)
                object->alpha = alpha;
            }
      }
}

int cMapblock::GetRoofHeight (int x, int y, int z)
{
  int roof = ROOF_NONE;

  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
          if ((object->x == x) && (object->y == y) && (object->z > z))
              {
                if (object->z < roof)
                  roof = object->z;
              }
      }

  return roof;
}

int cMapblock::GetGroundZ (Uint32 x, Uint32 y)
{
  if ((x < 8) && (y < 8))
    return groundmap[y][x];
  return 0;
}



int cMapblock::GetWalkZ (Uint32 dstx, Uint32 dsty, int srcz)
{
  if ((dstx >= 8) || (dsty >= 8))
    return 255;
  int ground = groundmap[dsty][dstx];

  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if ((object->x == (int) dstx) && (object->y == (int) dsty)
            && (!object->dynamic_id))
            {
              if (object->
                  tiledata_flags & (TILEDATAFLAG_SURFACE | TILEDATAFLAG_BRIDGE
                                    | TILEDATAFLAG_STAIRBACK |
                                    TILEDATAFLAG_STAIRRIGHT))
                  {
                    int z = object->z + object->height;
                    if ((z <= srcz + 7) && (z > ground))
                      ground = z;
                  }
              if (object->
                  tiledata_flags & ( /*TILEDATAFLAG_WALL | */
                                    TILEDATAFLAG_IMPASSABLE))
                  {
                    int z = object->z + object->height;
                    if (!((srcz + 5 < object->z) || (srcz >= z)))
                      return 255;
                  }
            }
      }
  return ground;
}
