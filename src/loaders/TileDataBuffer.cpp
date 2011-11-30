//
// File: TileDataBuffer.cpp
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


#include "iris_endian.h"
#include "loaders/TileDataLoader.h"
#include "Logger.h"
#include "Config.h"
#include "loaders/TileDataBuffer.h"




using namespace std;

TileDataBuffer pTileDataBuffer;

TileDataBuffer::TileDataBuffer ()
{
  entries.setMaxSize (1000);
  entries.setCacheSize (0x4000);
}

TileDataBuffer::~TileDataBuffer ()
{
}

TileDataStaticEntry *TileDataBuffer::GetEntry (int index)
{
  if ((index < 0) || (index >= 0x4000))
    return NULL;

  TileDataStaticEntry *result = entries.findEntry (index);

  if (!result)
      {
        result = new TileDataStaticEntry;


        if (!pTileDataLoader.LoadEntry (index, result))
            {
              delete result;
              return NULL;
            }

        entries.addEntry (index, result);
      }

  return result;
}

void TileDataBuffer::Clear ()
{
     entries.Clear ();
}
