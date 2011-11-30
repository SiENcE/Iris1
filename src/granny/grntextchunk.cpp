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
#include "granny/grntextchunk.h"
#include "include.h"




using namespace std;

TextChunk::TextChunk ()
{
}

TextChunk::~TextChunk ()
{
}

void TextChunk::load (cGrannyStream * file, dword offset, dword baseOffset,
                      dword peers)
{
  dword oldPos = file->tellg ();
  file->seekg (offset + baseOffset);
  numEntries = file->readDword ();
  file->readDword ();           //unknown

  for (unsigned int i = 0; i < numEntries; ++i)
      {
        char entry[512] = { 0 };
        unsigned short j = 0;

        do
            {
              file->get (entry[j]);
            }
        while (entry[j++] != 0);

        entries.push_back (std::string (entry));
      }

  file->seekg (oldPos);
}

// Find String ID by Name
dword TextChunk::findString (std::string name)
{
  dword i;
  for (i = 0; i < numEntries; ++i)
    if (!strcasecmp (name.c_str (), entries[i].c_str ()))
      return i;
//          if( name == entries[ i ] )
//                return i;

  return 0;
}

string TextChunk::findID (dword id)
{
  if (id >= entries.size ())
    return "";

  return entries[id];
}
