//
// File: GumpLoader.cpp
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

#include "loaders/GumpLoader.h"




GumpLoader pGumpLoader;

GumpLoader::GumpLoader() : gumpfile( NULL ), gumpindex( NULL )
{
}

GumpLoader::~GumpLoader()
{
    DeInit();
}

void GumpLoader::Init (std::string filename, std::string indexname)
{
	std::string errstr;

  DeInit ();

  gumpfile = new std::ifstream (filename.c_str (), std::ios::in | std::ios::binary);
  gumpindex = new std::ifstream (indexname.c_str (), std::ios::in | std::ios::binary);

  errstr = "Could not load ground texture file: ";
  if (!gumpfile->is_open ())
      {
        errstr += filename;
        delete gumpfile;
        gumpfile = NULL;
        delete gumpindex;
        gumpindex = NULL;
        THROWEXCEPTION (errstr);
      }

  if (!gumpindex->is_open ())
      {
        errstr += indexname;
        delete gumpfile;
        gumpfile = NULL;
        delete gumpindex;
        gumpindex = NULL;
        THROWEXCEPTION (errstr);
      }

  gumpindex->seekg (0, std::ios::end);
  unsigned long idxE = gumpindex->tellg ();
  gump_count = idxE / 12;
}

void GumpLoader::DeInit ()
{
  delete gumpfile;
  gumpfile = NULL;
  delete gumpindex;
  gumpindex = NULL;
}

Uint32 *GumpLoader::LoadGumpRaw (int index, int &tex_width, int &tex_height,
                                  int &real_width, int &real_height)
{
  ASSERT (gumpfile);
  ASSERT (gumpindex);
  
  tex_width = 0;
  tex_height = 0;
  real_width = 0;
  real_height = 0;
  if ((index < 0) || ((unsigned int) index >= gump_count))
    return NULL;


  struct sPatchResult patch = pVerdataLoader.FindPatch (VERDATAPATCH_GUMP, index);

  struct stIndexRecord idx;

  if (patch.file)
      {
        idx = patch.index;
      }
  else
      {
        patch.file = gumpfile;
		gumpindex->seekg (index * 12, std::ios::beg);
        gumpindex->read ((char *) &idx, sizeof (struct stIndexRecord));
        idx.offset = IRIS_SwapU32 (idx.offset);
        idx.length = IRIS_SwapU32 (idx.length);
        idx.extra = IRIS_SwapU32 (idx.extra);
      }

  if (idx.offset == 0xffffffff)
    return NULL;

  Uint16 width = ((idx.extra >> 16) & 0xFFFF);
  Uint16 height = (idx.extra & 0xFFFF);

  if (width > 1024 || height > 1024)
    return NULL;

  Uint32 w = 1;
  while (w < width)
    w *= 2;
  Uint32 h = 1;
  while (h < height)
    h *= 2;


  Uint32 *data = (Uint32 *) malloc (w * h * 4);
  memset (data, 0, w * h * 4);
  Uint32 *heightTable = new Uint32[height];

  patch.file->seekg (idx.offset, std::ios::beg);
  patch.file->read ((char *) heightTable, height * 4);

  unsigned int rle = 0;

  for (unsigned short y = 0; y < height; y++)
      {
		  gumpfile->seekg (IRIS_SwapU32 (heightTable[y]) * 4 + idx.offset, std::ios::beg);

        unsigned short x = 0;

        // Start of this row (row per row)
        Uint32 *linedata = (Uint32 *) data + y * w;

        while (x < width)
            {
              patch.file->read ((char *) &rle, 4);    // Read a RLE key
              rle = IRIS_SwapU32 (rle);

              unsigned short length = (rle >> 16) & 0xFFFF; // First two byte
              unsigned short color = rle & 0xFFFF;    // Second two byte

              Uint32 col32 = color15to32 (color);

              for (unsigned int i = 0; i < length; i++)
                  {
                    *(linedata++) = col32;
                    x++;
                  }
            }
      }

  SAFE_DELETE_ARRAY( heightTable );

  tex_width = w;
  tex_height = h;
  real_width = width;
  real_height = height;

  return data;

}

Texture *GumpLoader::LoadGump (int index)
{
  int w, h, width, height;

  ASSERT (gumpfile);
  ASSERT (gumpindex);

  Uint32 *data = LoadGumpRaw (index, w, h, width, height);
  if (!data)
    return NULL;

  Texture *texture = new Texture;
  texture->LoadFromData (data, w, h, 32, GL_NEAREST, true);
  texture->SetRealSize (width, height);

  free (data);

  return texture;
}

Texture *GumpLoader::LoadGumpTiled( int index, int width, int height )
{
	int tex_width, tex_height, gump_width, gump_height;

	ASSERT( gumpfile );
	ASSERT( gumpindex );

	Uint32 *srcData = LoadGumpRaw( index, tex_width, tex_height, gump_width, gump_height );
	if ( !srcData )
	{
		return NULL;
	}

/*  Texture * texture = new Texture;
  texture->LoadFromData(srcData, tex_width, tex_height, 32, GL_NEAREST);
  texture->SetRealSize(tex_width, tex_height);
  
  free(srcData);
  
  return texture; */

	if ( width <= 0 )
	{
		width = gump_width;
	}

	if ( height <= 0 )
	{
		height = gump_height;
	}

	// Now determin the "realsize"
	unsigned short tWidth = 64;
	unsigned short tHeight = 64;

	while ( tWidth < width )
	{
		tWidth *= 2;
	}

	while ( tHeight < height )
	{
		tHeight *= 2;
	}

	unsigned int *data = new unsigned int[tHeight * tWidth];

	memset( data, 0, tHeight * tWidth * 4 );

	// Fill the image row-by-row
	for ( int y = 0; y < height; ++y )
	{
		Uint32 *p_dst = data + ( y * tWidth );
		Uint32 *line_src = srcData + ( ( y % gump_height ) * tex_width );
		Uint32 *p_src = line_src;
		for ( int x = 0; x < width; ++x )
		{
			if ( p_src - line_src >= gump_width )
			{
				p_src = line_src;
			}
			*p_dst = *p_src;
			p_dst++;
			p_src++;
		}
	}

	Texture *texture = new Texture();

	texture->LoadFromData( data, tWidth, tHeight, 32, GL_NEAREST );
	texture->SetRealSize( width, height );

	free( srcData );
	SAFE_DELETE_ARRAY( data	);

	return texture;
}


bool GumpLoader::CheckGump (int index)
{
  ASSERT (gumpfile);
  ASSERT (gumpindex);

  if ((index < 0) || ((unsigned int) index >= gump_count))
    return false;


  struct sPatchResult patch = pVerdataLoader.FindPatch (VERDATAPATCH_GUMP, index);

  struct stIndexRecord idx;

  if (patch.file)
      {
        idx = patch.index;
      }
  else
      {
        patch.file = gumpfile;
		gumpindex->seekg (index * 12, std::ios::beg);
        gumpindex->read ((char *) &idx, sizeof (struct stIndexRecord));
        idx.offset = IRIS_SwapU32 (idx.offset);
        idx.length = IRIS_SwapU32 (idx.length);
        idx.extra = IRIS_SwapU32 (idx.extra);
      }

  if (idx.offset == 0xffffffff)
    return false;
  return true;
}

void GumpLoader::GetGumpSize (int index, int &r_width, int &r_height)
{
  ASSERT (gumpfile);
  ASSERT (gumpindex);

  r_width = 0;

  r_height = 0;

  if ((index < 0) || ((unsigned int) index >= gump_count))
    return;


  struct sPatchResult patch = pVerdataLoader.FindPatch (VERDATAPATCH_GUMP, index);

  struct stIndexRecord idx;

  if (patch.file)
      {
        idx = patch.index;
      }
  else
      {
        patch.file = gumpfile;
		gumpindex->seekg (index * 12, std::ios::beg);
        gumpindex->read ((char *) &idx, sizeof (struct stIndexRecord));
        idx.offset = IRIS_SwapU32 (idx.offset);
        idx.length = IRIS_SwapU32 (idx.length);
        idx.extra = IRIS_SwapU32 (idx.extra);
      }

  if (idx.offset == 0xffffffff)
    return;

  Uint16 width = ((idx.extra >> 16) & 0xFFFF);
  Uint16 height = (idx.extra & 0xFFFF);

  r_width = width;
  r_height = height;

  return;

}

int GumpLoader::GetGumpWidth (int index)
{
  int width, height;

  GetGumpSize (index, width, height);
  return width;
}

int GumpLoader::GetGumpHeight (int index)
{
  int width, height;
  GetGumpSize (index, width, height);
  return height;
}
