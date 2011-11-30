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
#include "granny/grnunhandled.h"




Unhandled::Unhandled ()
{
}

Unhandled::~Unhandled ()
{
}

void Unhandled::load (cGrannyStream * file, dword uoffset, dword baseOffset,
                      dword peers)
{
  for (unsigned int i = 0; i < peers;)
      {
        /* dword chunk=  */ file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        load (file, offset, baseOffset, children);
        i += children + 1;
      }
}
