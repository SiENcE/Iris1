//
// File: Buffer.cpp
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



#include "Logger.h"
#include "Exception.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "Buffer.h"




using namespace std;

BufferEntry::BufferEntry ()
{
}

BufferEntry::~BufferEntry ()
{
}

Buffer::Buffer ()
{
  size = 0;
  bufferarray = NULL;
}

Buffer::~Buffer ()
{
  if (bufferarray)
    Free ();
}

int Buffer::GetSize (void)
{
  return size;
}

void Buffer::Create (int size)
{
  if (bufferarray)
    Free ();
  this->size = size;
  bufferarray = (BufferEntry **) malloc (size * 4);
  memset (bufferarray, 0, size * 4);
}

void Buffer::Free (void)
{
  if (!bufferarray)
    return;
  Flush ();

  free (bufferarray);
  bufferarray = NULL;
  size = 0;
}

void Buffer::Flush (void)
{
  if (!bufferarray)
    return;

  for (int index = 0; index < size; index++)
    if (bufferarray[index])
      delete (bufferarray[index]);

  memset (bufferarray, 0, size * 4);
}

void Buffer::Add (int index, BufferEntry * entry)
{
  if (!bufferarray)
    THROWEXCEPTION ("NULL buffer array");

  if ((index < 0) || (index >= size))
    THROWEXCEPTION ("array bounds violation");


  if (bufferarray[index])
    delete bufferarray[index];

  bufferarray[index] = entry;
}

void Buffer::Delete (int index)
{
  Add (index, NULL);
}

BufferEntry *Buffer::Get (int index)
{
  if (!bufferarray)
    THROWEXCEPTION ("NULL buffer array");

  if ((index < 0) || (index >= size))
      {
        return NULL;
      }

  return bufferarray[index];
}
