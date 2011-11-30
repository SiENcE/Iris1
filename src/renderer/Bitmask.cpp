//
// File: Bitmask.cpp
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


#include "renderer/Bitmask.h"




cBitmask::cBitmask() : pixels( NULL )
{

}

cBitmask::~cBitmask()
{
	SAFE_DELETE_ARRAY( pixels );
}


void cBitmask::Create( Uint32 *data, int width, int height )
{
	assert( data );

	m_width = width;
	m_height = height;

	int img_mem = width * height;
	int mask_mem = (img_mem + 7) / 8;

	pixels = new Uint8[mask_mem];
	m_mem = mask_mem;

	Uint8 *dst = pixels;
	Uint32 *src = data;
	Uint32 *enddata = data + img_mem;
	Uint8 b;

	for ( int i = 0; i < mask_mem; i++ )
	{
		b = 0;
		for ( int j = 0; j < 8; j++ )
		{
			if ( *src )
			{
                b |= 1 << j;
			}
			src++;
			if ( src >= enddata )
			{
                break;
			}
		}
		*dst = b;
		dst++;
	}
}


bool cBitmask::CheckPixel (int x, int y, bool swapy)
{
  if ((x >= m_width) || (x < 0) || (y >= m_height) || (y < 0))
    return false;

  if (swapy)
    y = m_height - 1 - y;

  int m_pos = x + y * m_width;

  if ((!pixels) || (m_pos < 0))
    return false;

  int maskbyte = m_pos >> 3;
  int maskbit = m_pos & 7;

  if (maskbyte >= m_mem)
    return false;

  return (pixels[maskbyte] & (1 << maskbit));
}
