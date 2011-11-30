//
// File: GrannyStream.cpp
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
#include "granny/GrannyStream.h"
#include "Logger.h"
#include <cassert>




using namespace std;

cGrannyStream::cGrannyStream (std::fstream * file, std::string filename)
{
  assert (file);
  file->seekg (0, ios::end);
  m_size = file->tellg ();
  if (m_size)
      {
        m_data = (char *) malloc (m_size);
        assert (m_data);
        file->seekg (0, ios::beg);
        file->read (m_data, m_size);
      }
  else
    m_data = NULL;
  m_pos = 0;
  m_filename = filename;
}


cGrannyStream::~cGrannyStream ()
{
  if (m_data)
    free (m_data);
  m_data = NULL;
}

bool cGrannyStream::seekg (Uint32 pos)
{
  if (pos < m_size)
      {
        m_pos = pos;
        return true;
      }
  else
      {
        Logger::WriteLine ("GrannyStream::seekg: file corrupt? (" + m_filename +
                    ")");
        return false;
      }
}

Uint32 cGrannyStream::tellg ()
{
  return m_pos;
}

bool cGrannyStream::read (char *data, Uint32 count)
{
  assert (data);
  if (m_pos + count <= m_size)
      {
        memcpy (data, m_data + m_pos, count);
        m_pos += count;
        return true;
      }
  else
      {
        memset (data, 0, count);
        Logger::WriteLine ("GrannyStream::read: file corrupt? (" + m_filename + ")");
        return false;
      }
}


Uint32 cGrannyStream::readDword ()
{

  if (m_pos + 4 <= m_size)
      {
        Uint8 *p = (Uint8 *) m_data + m_pos;
        m_pos += 4;
#if SDL_BYTEORDER==SDL_BIG_ENDIAN
        return (*p | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
        //return (*p << 8 | *(p+1) | *(p+2) << 24 | *(p+3) << 16);
        //return (*p << 24 | *(p+1) << 16 | *(p+2) << 8 | *(p+3));
#else
        return (*p | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
#endif

      }
  Logger::WriteLine ("GrannyStream::readDword: file corrupt? (" + m_filename + ")");
  return 0;
}

void cGrannyStream::get (char &value)
{
  if (m_pos < m_size)
      {
        value = *(m_data + m_pos);
        m_pos++;
      }
  else
      {
        Logger::WriteLine ("GrannyStream::get: file corrupt? (" + m_filename + ")");
        value = 0;
      }
}
