//
// File: Buffer.h
// Created by:
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"


class BufferEntry
{
 public:
  BufferEntry();
  virtual ~BufferEntry();
};

class Buffer
{
private:
   BufferEntry * * bufferarray;
   int size;
public:
    Buffer ();
   virtual ~Buffer ();

   int GetSize(void);

   virtual void Create(int size);
   virtual void Free(void);
   virtual void Flush(void);
   virtual void Add(int index, BufferEntry * entry);
   virtual void Delete(int index);

   BufferEntry * Get(int index);

protected:
};


#endif //_BUFFER_H_
