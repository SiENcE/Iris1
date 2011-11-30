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

#include "granny/grntype.h"
#include "granny/grntextures.h"

#include <stdlib.h>
#include <iostream>




using namespace std;

Textures::Textures ()
{
}

Textures::~Textures ()
{
}

void Textures::load (cGrannyStream * file, dword tOffset, dword baseOffset,
                     dword peers)
{
  unsigned int x;
  dword oldPos;
  dword numPolys;
  dword nextoffset;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E0e00:   //texture
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0e02:   //texture poly
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0e03:   //texture poly datalist
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0e04:   //texture poly data
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0e05:   //texture poly data
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0e06:   //texture polys
              oldPos = file->tellg ();
              file->readDword ();
              nextoffset = file->readDword ();
              file->seekg (offset + baseOffset);
              numPolys = file->readDword ();

              if ((nextoffset - offset - 4) / numPolys == 28)
                  {
                    for (x = 0; x < numPolys; x++)
                        {
                          gPolygon polygon;
						  polygon.nodes[0]=file->readDword();
                          for (int p = 1; p < 4; p++)
                              {
                                file->readDword ();
								polygon.nodes[p]=file->readDword();
                              }
                          polygons.push_back (polygon);
                        }

                  }
              else
                for (x = 0; x < numPolys; x++)
                    {
					gPolygon polygon;
                      for (int p = 0; p < 4; p++)
						polygon.nodes[p]=file->readDword();
					polygons.push_back(polygon);
                    }

              file->seekg (oldPos);
              break;
            case 0xCA5E0e07:   //texturelist
              load (file, offset, baseOffset, children);
              break;
            default:
              hex (cerr);
              cerr << "Unknown Textures Chunk: " << chunk << endl;
              exit (1);
            }
        i += children + 1;
      }
}
