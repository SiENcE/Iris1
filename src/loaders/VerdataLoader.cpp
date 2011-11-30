//
// File: VerdataLoader.cpp
// Created by:  Alexander Oster - tensor@ultima-iris.de
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
#include "loaders/VerdataLoader.h"
#include "Logger.h"
#include "uotype.h"
#include <string.h>
#include <iostream>




using namespace std;

cVerdataLoader pVerdataLoader;

cVerdataLoader::cVerdataLoader ()
{
  patches = NULL;
  verdatafile = NULL;
}

cVerdataLoader::~cVerdataLoader ()
{
      DeInit ();
}

void cVerdataLoader::Init (std::string filename)
{
  string errstr;

  DeInit ();
  
  verdatafile = new ifstream (filename.c_str (), ios::in | ios::binary);

  errstr = "Could not load verdata file: ";
  if (!verdatafile->is_open ())
      {
        errstr += filename;
        Logger::WriteLine ((char *) errstr.c_str (), __FILE__, __LINE__,
                    LEVEL_ERROR);
        delete verdatafile;
        verdatafile = NULL;
        return;
      }


  verdatafile->seekg (0, ios::beg);
  verdatafile->read ((char *) &patch_count, 4);
  patch_count = IRIS_SwapU32 (patch_count);
  patches = (struct VerDataEntry *) malloc (patch_count *sizeof (struct VerDataEntry));
  verdatafile->read ((char *) patches,
                     patch_count * sizeof (struct VerDataEntry));
                     
#if SDL_BYTEORDER==SDL_BIG_ENDIAN                     
  for (unsigned int ii = 0; ii < patch_count; ii++)
      {
        patches[ii].fileid = IRIS_SwapU32 (patches[ii].fileid);
        patches[ii].blockid = IRIS_SwapU32 (patches[ii].blockid);
        patches[ii].start = IRIS_SwapU32 (patches[ii].start);
        patches[ii].len = IRIS_SwapU32 (patches[ii].len);
        patches[ii].extra = IRIS_SwapU32 (patches[ii].extra);
      }
#endif

}

void cVerdataLoader::DeInit ()
{
  delete verdatafile;
  verdatafile = NULL;
  if (patches)
    free (patches);
  patches = NULL;
}

struct sPatchResult cVerdataLoader::FindPatch (unsigned int fileid,
                                               unsigned int blockid)
{
  struct sPatchResult result;
  result.file = NULL;

  if (!patches)
      return result;
  if (!verdatafile)
      return result;

      
  for (Uint32 i = 0; i < patch_count; i++)
    if ((patches[i].fileid == fileid) && (patches[i].blockid == blockid))
        {
          result.index.offset = patches[i].start;
          result.index.length = patches[i].len;
          result.index.extra = patches[i].extra;
          result.file = verdatafile;
        }

  return result;
}
