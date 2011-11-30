//
// File: StaticModelRaster.cpp
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
  

#include "loaders/StaticModelRaster.h"
#include "Exception.h"
#include "iris_endian.h"




using namespace std;


cStaticModelRaster::cStaticModelRaster () 
{
	m_data = NULL;
	m_size_x = 0;
	m_size_y = 0;
	m_size_z = 0;
	m_min_x = 0;
	m_min_y = 0;
	m_min_z = 0;

}


void cStaticModelRaster::LoadFromStream (std::ifstream * stream) 
{
	stream->read ((char *) &m_size_x, 4);
	stream->read ((char *) &m_size_y, 4);
	stream->read ((char *) &m_size_z, 4);
	stream->read ((char *) &m_min_x, 4);
	stream->read ((char *) &m_min_y, 4);
	stream->read ((char *) &m_min_z, 4);

	m_size_x = IRIS_SwapI32(m_size_x);
	m_size_y = IRIS_SwapI32(m_size_y);
	m_size_z = IRIS_SwapI32(m_size_z);
	m_min_x  = IRIS_SwapI32(m_min_x);
	m_min_y  = IRIS_SwapI32(m_min_y);
	m_min_z  = IRIS_SwapI32(m_min_z);

	if ((m_size_x <= 0) || (m_size_y <= 0) || (m_size_z <= 0))
	 THROWEXCEPTION ("model raster load error: size < 0");

	if ((m_size_x >= 128) || (m_size_y >= 128) || (m_size_z >= 128))
	 THROWEXCEPTION ("model raster load error: size >= 128");

	m_data = (Uint8 *) malloc (m_size_x * m_size_y * m_size_z);

	stream->read ((char *) m_data, m_size_x * m_size_y * m_size_z);
} 


cStaticModelRaster::~cStaticModelRaster () 
{
	if (m_data)
	 free (m_data);

	m_data = NULL;
}


Uint8 * cStaticModelRaster::data () 
{
	return m_data;
}


Sint32 cStaticModelRaster::size_x () 
{
	return m_size_x;
}


Sint32 cStaticModelRaster::size_y () 
{
	return m_size_y;
}


Sint32 cStaticModelRaster::size_z () 
{
	return m_size_z;
}


Sint32 cStaticModelRaster::min_x () 
{
	return m_min_x;
}


Sint32 cStaticModelRaster::min_y () 
{
	return m_min_y;
}


Sint32 cStaticModelRaster::min_z () 
{
	return m_min_z;
}
