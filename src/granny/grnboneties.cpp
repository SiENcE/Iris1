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
#include "granny/grnboneties.h"

#include <stdlib.h>
#include <iostream>
#include <cassert>




using namespace std;

BoneTie::BoneTie ()
{
}

BoneTie::~BoneTie ()
{
}

BoneTies::BoneTies ()
{
}

BoneTies::~BoneTies ()
{
	for_each(boneties.begin(),boneties.end(),my_delete<BoneTie*>);
}

void BoneTies::load (cGrannyStream * file, dword boneOffset, dword baseOffset,
                     dword peers)
{
  assert (file);
  int x;
  union
  {
    dword d;
    float f;
  } fd;
  dword oldPos;
	BoneTie *bonetie=NULL;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E0b00:   //boneobject
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c00:   // boneties container
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c02:   // bone objptrs
              objectOffset = offset + baseOffset;
              break;
            case 0xCA5E0c03:   // bonetie group
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c04:   // bonetie data
              break;
            case 0xCA5E0c05:   // end bone objptrs
              endObjOffset = offset + baseOffset;
              oldPos = file->tellg ();
              file->seekg (objectOffset);
              for (unsigned j = 0; j < (endObjOffset - objectOffset) / 4; j++)
                  {
                    dword tie = file->readDword ();
                    boneObjPtrs.push_back (tie);
                  }
              file->seekg (oldPos);
              break;
            case 0xCA5E0c06:   // bonetie container
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c07:   // bone objptrs container
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c08:   // bone objptr
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c09:   // bonetie list
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0c0a:   // bonetie
              bonetie = new BoneTie ();
              oldPos = file->tellg ();
              file->seekg (offset + baseOffset);
              bonetie->bone = file->readDword ();
              for (x = 0; x < 7; x++)
                  {
                    fd.d = file->readDword ();
                    bonetie->unknown[x] = fd.f;
                  }
              boneties.push_back (bonetie);
              file->seekg (oldPos);
              break;
            case 0xCA5E0f04:   // bone object
              oldPos = file->tellg ();
              file->seekg (offset + baseOffset);
              {
                dword obj = file->readDword ();
                boneObjects.push_back (obj);
              }
              file->seekg (oldPos);
              break;
            default:
              hex (cerr);
              cerr << "Unknown BoneTies chunk: " << chunk << endl;
              exit (1);
            }
        i += children + 1;
      }
}

dword BoneTies::findID (dword id)
{
  vector < dword >::iterator iobj;
  dword obj = 1;
  for (iobj = boneObjects.begin (); iobj != boneObjects.end () &&
       (*iobj) != id; iobj++, obj++);
  return obj;
}

dword BoneTies::findBone (dword bone)
{
  vector < dword >::iterator ibone;
  dword bn = 0;
  for (ibone = boneObjPtrs.begin (); ibone != boneObjPtrs.end () &&
       (*ibone) != bone; ibone++, bn++);
  return bn;
}
