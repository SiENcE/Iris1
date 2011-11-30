//
// File: PaperdollBuilder.cpp
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


#include "loaders/PaperdollBuilder.h"
#include "loaders/GumpLoader.h"
#include "loaders/HueLoader.h"
#include "Logger.h"
#include "uotype.h"
#include <string.h>
#include <iostream>
#include <cassert>




using namespace std;

cPaperdollBuilder::cPaperdollBuilder ()
{
	pixels = new Uint32[65536];	// 256 * 256
	assert( pixels );
	memset (pixels, 0, 256 * 256 * 4);
}


cPaperdollBuilder::~cPaperdollBuilder ()
{
	// NOTE: Memory not beeing de-alocated because our pointer is beeing changed somewhere (other than memset)
	// SAFE_DELETE( pixels );
}

void cPaperdollBuilder::AddGump (int id, char layer, char *layermap, int hue)
{
  assert (layermap);
  assert (pixels);
  int tex_x, tex_y, real_x, real_y;
  Uint32 *gump = pGumpLoader.LoadGumpRaw (id, tex_x, tex_y, real_x, real_y);

  if (gump)
      {
        if (hue)
            {

              const struct stHue *sthue = pHueLoader.getHue (hue);

              if (sthue)
                  {
                    Uint32 *src = gump;
                    Uint8 *src8 = ((Uint8 *) gump) + 1;
                    for (int y = 0; y < min (tex_y, 256); y++)
                        {
                          Uint32 *dst = pixels + (y << 8);
                          Uint32 *dst_target = dst + min (tex_x, 256);

                          char *layer_dst = layermap + (y << 8);

                          Uint32 *src_p = src;
                          Uint8 *src8_p = src8;
                          while (dst < dst_target)
                              {
                                if (*src_p)
                                    {
                                      *dst = sthue->colors[*src8_p >> 3];
                                      *layer_dst = layer;
                                    }
                                dst++;
                                src_p++;
                                layer_dst++;
                                src8_p += 4;
                              }
                          src += tex_x;
                          src8 += tex_x * 4;
                        }
                  }

            }
        else
            {
              Uint32 *src = gump;
              for (int y = 0; y < min (tex_y, 256); y++)
                  {
                    Uint32 *dst = pixels + (y << 8);
                    Uint32 *dst_target = dst + min (tex_x, 256);
                    char *layer_dst = layermap + (y << 8);
                    Uint32 *src_p = src;
                    while (dst < dst_target)
                        {
                          if (*src_p)
                              {
                                *dst = *src_p;
                                *layer_dst = layer;
                              }
                          dst++;
                          src_p++;
                          layer_dst++;
                        }
                    src += tex_x;
                  }
            }
      }
  free( gump );
}


Texture *cPaperdollBuilder::CreateTexture ()
{
  assert (pixels);
  Texture *texture = new Texture;
  assert (texture);
  texture->LoadFromData (pixels, 256, 256, 32, GL_NEAREST);
  texture->SetRealSize (256, 256);
  return texture;
}
