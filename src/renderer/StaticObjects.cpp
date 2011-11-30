//
// File: StaticObjects.cpp
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


#include "renderer/StaticObjects.h"
#include <assert.h>
#include "include.h"




using namespace std;

cStaticObjectList::cStaticObjectList ()
{
  m_count = 0;                  // Initialize count
  m_head = STATICOBJECTLIST_ARRAY_SIZE;   // fake a full array
  m_current_array = NULL;
}


cStaticObjectList::~cStaticObjectList ()
{
	free( m_current_array );
/*
  for (unsigned int index = 0; index < cache.size (); index++)    // Free all arrays
    free (cache[index]);*/
  cache.clear ();
}

struct sStaticObject *cStaticObjectList::Add (void)
{
  assert (m_head <= STATICOBJECTLIST_ARRAY_SIZE);

  if (m_head == STATICOBJECTLIST_ARRAY_SIZE)    // if current array is full
    AllocNewArray ();           // alloc a new array

  assert (m_current_array);
  sStaticObject *result = m_current_array + m_head;

  m_count++;
  m_head++;

  return result;
}


struct sStaticObject *cStaticObjectList::Get (unsigned int index)
{
  assert (index < m_count);
  struct sStaticObject *array = cache[index / STATICOBJECTLIST_ARRAY_SIZE];   // get array
  return (array + (index % STATICOBJECTLIST_ARRAY_SIZE));
}

Uint32 cStaticObjectList::count ()
{
  return m_count;
}

void cStaticObjectList::AllocNewArray ()  // alloc a new array
{
  m_head = 0;
  m_current_array =
    (struct sStaticObject *) malloc (STATICOBJECTLIST_ARRAY_SIZE *sizeof (struct sStaticObject));
  memset (m_current_array, 0,
          STATICOBJECTLIST_ARRAY_SIZE * sizeof (struct sStaticObject));
  cache.push_back (m_current_array);
}
