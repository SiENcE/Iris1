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

#include "granny/grntype.h"
#include "granny/grnobjects.h"
#include <iostream>




using namespace std;

Object::Object ()
{
  numKeys = 0;
}

Object::~Object ()
{
}

void Object::addValue (dword key, dword value)
{
  keys[numKeys] = key;
  values[numKeys] = value;
  numKeys++;
}

dword Object::getValue (dword key)
{
  for (unsigned int i = 0; i < numKeys; i++)
    if (keys[i] == key)
      return values[i];
  return 0;
}

bool Object::findValue (dword key, dword value)
{
  for (unsigned int i = 0; i < numKeys; i++)
    if (keys[i] == key && values[i] == value)
      return true;
  return false;
}

Objects::Objects ()
{
}

Objects::~Objects ()
{
	for_each(objects.begin(),objects.end(),my_delete<Object*>);
	objects.clear();
}

//static dword objNum=1;
void Objects::load (cGrannyStream * file, dword objOffset, dword baseOffset,
                    dword peers)
{
  dword oldOfs, val;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E0f00:   //object
              object = new Object ();
              objects.push_back (object);
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0f01:   //object key
              oldOfs = file->tellg ();
              file->seekg (offset + baseOffset);
              key = file->readDword ();
              file->seekg (oldOfs);
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0f02:   //object value
              oldOfs = file->tellg ();
              file->seekg (offset + baseOffset);
              file->readDword ();
              val = file->readDword ();
              object->addValue (key, val);
              file->seekg (oldOfs);
              break;
            case 0xCA5E0f05:   //object key list
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0f06:   //object value list
              load (file, offset, baseOffset, children);
              break;
            default:
              hex (cerr);
              cerr << "Unknown Objects Chunk: " << chunk << endl;
              exit (1);
            }
        i += children + 1;
      }
}

dword Objects::getValue (dword obj, dword key)
{
  return objects[obj]->getValue (key);
}

dword Objects::findValue (dword key, dword value)
{
  vector < Object * >::iterator iobj;
  dword obj = 1;
  for (iobj = objects.begin (); iobj != objects.end () &&
       (*iobj)->findValue (key, value) == false; iobj++, obj++);
  return obj;
}
