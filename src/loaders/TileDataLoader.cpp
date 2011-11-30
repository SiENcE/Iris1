//
// File: TileDataLoader.cpp
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
#include "Exception.h"
#include <string.h>
#include <iostream>




using namespace std;

cTileDataLoader pTileDataLoader;

cTileDataLoader::cTileDataLoader ()
{
   tiledatafile = NULL;
}
   
cTileDataLoader::~cTileDataLoader ()
{
  DeInit ();
}


void cTileDataLoader::Init (std::string filename)
{
  string errstr;

  DeInit ();

  tiledatafile = new ifstream (filename.c_str (), ios::in | ios::binary);


  errstr = "Could not load ground tiledata file: ";
  if (!tiledatafile->is_open ())
      {
        errstr += filename;
        delete tiledatafile;
        tiledatafile = NULL;
        THROWEXCEPTION (errstr);
      }

}

void cTileDataLoader::DeInit ()
{
  delete tiledatafile;
   tiledatafile = NULL;
}

bool cTileDataLoader::LoadEntry (Uint32 index,
                                 struct TileDataStaticEntry *entry)
{
  if ((index >= 49152) || !entry || !tiledatafile)
    return false;

  int group = index / 32;
  int groupidx = index % 32;

  tiledatafile->seekg (512 * 836 + 1188 * group + 4 + groupidx * 37,
                       ios::beg);
  tiledatafile->read ((char *) entry, 37);
  entry->flags = IRIS_SwapU32 (entry->flags);
  entry->unknown = IRIS_SwapU16 (entry->unknown);
  entry->animid = IRIS_SwapU16 (entry->animid);
  entry->unknown3 = IRIS_SwapU16 (entry->unknown3);

  return true;

}
