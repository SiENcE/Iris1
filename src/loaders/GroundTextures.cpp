/*
 * File: GroundTextures.cpp
 * Created by Alexander Oster.
 * Last change: 10-04-06 (SiENcE)
 */

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
#include "loaders/GroundTextures.h"
#include "Logger.h"
#include "Exception.h"
#include "uotype.h"
#include "xml.h"
#include <string.h>
#include <iostream>




using namespace std;

cGroundTextureLoader pGroundTextureLoader;

cGroundTextureLoader::cGroundTextureLoader ()
{
    texmapsfile = NULL;
    texmapsindex = NULL;
}

cGroundTextureLoader::~cGroundTextureLoader ()
{
    DeInit ();
}

void cGroundTextureLoader::Init (std::string filename,
                                            std::string indexname)
{
  string errstr;

  DeInit ();

  texmapsfile = new ifstream (filename.c_str (), ios::in | ios::binary);
  texmapsindex = new ifstream (indexname.c_str (), ios::in | ios::binary);

  errstr = "Could not load ground texture file: ";
  if (!texmapsfile->is_open ())
      {
        errstr += filename;
        delete texmapsfile;
        delete texmapsindex;
        texmapsfile = NULL;
        texmapsindex = NULL;
        THROWEXCEPTION (errstr);
      }

  if (!texmapsindex->is_open ())
      {
        errstr += indexname;
        delete texmapsfile;
        delete texmapsindex;
        texmapsfile = NULL;
        texmapsindex = NULL;
        THROWEXCEPTION (errstr);
      }

  texmapsindex->seekg (0, ios::end);
  unsigned long idxE = texmapsindex->tellg ();

  groundtex_count = idxE / 12;
}

void cGroundTextureLoader::DeInit ()
{
  delete texmapsfile;
  delete texmapsindex;
    texmapsfile = NULL;
    texmapsindex = NULL;
}

Texture *cGroundTextureLoader::LoadTexture (int index)
{
  ASSERT (texmapsfile);
  ASSERT (texmapsindex);
  
  if ((index < 0) || ((unsigned int) index >= groundtex_count))
    return NULL;

  struct stIndexRecord idx;

  texmapsindex->seekg (index * 12, ios::beg);
  texmapsindex->read ((char *) &idx, sizeof (struct stIndexRecord));

  idx.offset = IRIS_SwapU32 (idx.offset);
  idx.length = IRIS_SwapU32 (idx.length);
  idx.extra = IRIS_SwapU32 (idx.extra);

  if (idx.offset == 0xffffffff)
    return NULL;

//SiENcE: support for custom size Textures added
//other Formula choosen. Now the size of the Textures is calculated
//  int size = (idx.extra == 1 ? 128 : 64);
  Uint32 size = (Uint32) sqrt( idx.length / 2.0 );

  if (idx.length != (unsigned int) (size * size * 2))
      {
        return NULL;
      }

  char *sprite = (char *) malloc (size * size * 4);
  Uint16 *temp = (Uint16 *) malloc (size * size * 2);

  texmapsfile->seekg (idx.offset, ios::beg);
  texmapsfile->read ((char *) temp, size * size * 2);

  Uint16 *src = temp;
  Uint32 *dst = (Uint32 *) sprite;

  for (int i = 0; i < (size * size); i++)
      {
        *dst = color15to32 (IRIS_SwapU16 (*src)) | 0xff000000;
        dst++;
        src++;
      }

  free (temp);

  Texture *texture = new Texture;
  if (texture)
      {
        texture->LoadFromData (sprite, size, size, 32, GL_LINEAR);
        free (sprite);
      }

  return texture;
}
