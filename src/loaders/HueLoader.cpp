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


#include "iris_endian.h"
#include "loaders/HueLoader.h"
#include "Logger.h"
#include "Config.h"
#include "Exception.h"
#include "uotype.h"

#include <iostream>
#include <fstream>
#include <string>




cHueLoader::cHueLoader ()
{
}

cHueLoader::~cHueLoader ()
{
    DeInit ();
}

void cHueLoader::Init (std::string filename)
{
  std::ifstream huefile;

  DeInit ();
  
  huefile.open (filename.c_str (), std::ios::in | std::ios::binary);

  if (!huefile.is_open ())
    THROWEXCEPTION ("Couldn't open hues.mul");

  unsigned int i = 0;

      // Read as many hues as we can
      while (!huefile.eof ())
      {
        huefile.seekg (4, std::ios::cur);

        for (unsigned int j = 0; j < 8; ++j)
        {
            unsigned short hueId = i * 8 + j;
            
            stHue hue;

            // 32 Color Values
            for (unsigned int k = 0; k < 32; ++k)
            {
                unsigned short color15;
                huefile.read ((char *) &color15, 2);
                color15 = IRIS_SwapU16 (color15);
                
                // Swap Bytes to 32 bits
                hue.colors[k] = color15to32 (color15);
            }

            // Two other bytes, tablestart and tableend
            unsigned short tableStart, tableEnd;

            huefile.read ((char *) &tableStart, 2);
            huefile.read ((char *) &tableEnd, 2);
            tableStart = IRIS_SwapU16 (tableStart);
            tableEnd = IRIS_SwapU16 (tableEnd);

            hue.tableEnd = color15to32 (tableEnd);
            hue.tableStart = color15to32 (tableStart);

            hues.insert (std::make_pair (hueId, hue));

            // Skip the Name
            huefile.seekg (20, std::ios::cur);
        }
        i++;
      }
      huefile.close ();
      return;
}

void cHueLoader::DeInit ()
{
    hues.clear ();
}

const stHue *cHueLoader::getHue (unsigned short id)
{
  //this fixes the hue colors
  id--;

  std::map < unsigned short, stHue >::iterator it = hues.find (id);

  if (it == hues.end ())
    return 0;

  return &(it->second);
}

void cHueLoader::GetRGBHue (unsigned short id, float &r, float &g, float &b)
{
  const stHue *hue = getHue (id);
  if (id && hue)
      {
        Uint32 col32 = hue->colors[31];
        Uint8 *col8 = (Uint8 *) & col32;
        r = (float) col8[0] / 255.0f;
        g = (float) col8[1] / 255.0f;
        b = (float) col8[2] / 255.0f;
      }
  else
      {
        r = 1.0f;
        g = 1.0f;
        b = 1.0f;
      }
}


cHueLoader pHueLoader;
