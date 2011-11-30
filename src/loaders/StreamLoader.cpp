//
// File: StreamLoader.cpp
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


#include "loaders/StreamLoader.h"
#include "Logger.h"
#include "Exception.h"
#include <string.h>
#include <iostream>




using namespace std;

cStreamLoader::cStreamLoader (std::ifstream * file, Uint32 size)
{
  if (!file)
    THROWEXCEPTION ("NULL file pointer");
  if (size == 0)
    THROWEXCEPTION ("zero stream size");

  /* Allocing Memory */
  try
  {
    m_data = new Uint8[size];
    m_eof = m_data + size;
    m_file_pos = m_data;
  }
  catch (...)
  {
    delete m_data;
    m_data = NULL;
    THROWEXCEPTION ("could not create stream loader");
  }

  /* Reading Stream */
  try
  {
    file->read ((char *) m_data, size);
  } catch (...)
  {
    delete m_data;
    m_data = NULL;
    THROWEXCEPTION ("could not load data into stream");
  }
}

cStreamLoader::~cStreamLoader ()
{
  delete m_data;
  m_data = NULL;
}

void cStreamLoader::seek (Uint32 pos)
{
  ASSERT (m_data);
  ASSERT (m_eof);
  if (m_data + pos >= m_eof)
    THROWEXCEPTION ("stream seek out of range");
  m_file_pos = m_data + pos;
}

Uint8 *cStreamLoader::read (Uint32 size)
{
  ASSERT (m_data);
  ASSERT (m_eof);
  ASSERT (m_file_pos);

  if (!size)
    THROWEXCEPTION ("stream read out of range");

  if (m_file_pos + size > m_eof)
    THROWEXCEPTION ("stream read out of range");

  Uint8 *result = m_file_pos;

  m_file_pos += size;

  return result;
}


Uint16 *cStreamLoader::read_uint16 (Uint32 count)
{
  return (Uint16 *) read (count * 2);
}

Sint16 *cStreamLoader::read_sint16 (Uint32 count)
{
  return (Sint16 *) read (count * 2);
}

Uint32 *cStreamLoader::read_uint32 (Uint32 count)
{
  return (Uint32 *) read (count * 4);
}

Sint32 *cStreamLoader::read_sint32 (Uint32 count)
{
  return (Sint32 *) read (count * 4);
}

Uint16 cStreamLoader::read_uint16 ()
{
  return *((Uint16 *) read (2));
}

Sint16 cStreamLoader::read_sint16 ()
{
  return *((Sint16 *) read (2));
}

Uint32 cStreamLoader::read_uint32 ()
{
  return *((Uint32 *) read (4));
}

Sint32 cStreamLoader::read_sint32 ()
{
  return *((Sint32 *) read (4));
}

Uint8 cStreamLoader::read_uint8 ()
{
  return *(read (1));
}

Uint32 cStreamLoader::size ()
{
  ASSERT (m_data);
  ASSERT (m_eof);
  return (m_eof - m_data);
}

Uint32 cStreamLoader::pos ()
{
  ASSERT (m_data);
  ASSERT (m_eof);
  ASSERT (m_file_pos);
  return (m_eof - m_file_pos);
}

bool cStreamLoader::eof ()
{
  return (m_file_pos == m_eof);
}
