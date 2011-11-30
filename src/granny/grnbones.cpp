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

#include <stdlib.h>
#include <cassert>
#include "granny/grntype.h"
#include "granny/grnbones.h"




Bone::Bone ()
{
}

Bone::~Bone ()
{
}

Bones::Bones ()
{
}

Bones::~Bones ()
{
	for ( unsigned int i = 0; i < bones.size(); i++ )
	{
		SAFE_DELETE( bones[i] );
	}

	bones.clear();
}

void Bones::load( cGrannyStream * file, dword boneOffset, dword baseOffset, dword peers)
{
  assert (file);
  int x;
  union
  {
    dword d;
    float f;
  } fd;
  dword oldPos;
  Bone *bone;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E0505:   //skeleton
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0506:   //bone
              oldPos = file->tellg ();
              file->seekg (offset + baseOffset);
              bone = new Bone ();
              bone->parent = file->readDword ();
              bone->id = bones.size ();
              for (x = 0; x < 3; x++)
                  {
                    fd.d = file->readDword ();
                    bone->translate.points[x] = fd.f;
                  };
              for (x = 0; x < 4; x++)
                  {
                    fd.d = file->readDword ();
                    bone->quaternion.points[x] = fd.f;
                  };
              // there's 9 floats left.. we don't know what they are
              bones.push_back (bone);
              if (bone->parent != bone->id)     //endless loop otherwise
                bones[bone->parent]->children.push_back (bone);
              file->seekg (oldPos);
              break;
            case 0xCA5E0508:   //bonelist
              load (file, offset, baseOffset, children);
              break;

            default:
				std::hex( std::cerr );
			  std::cerr << "Unknown Bones chunk: " << chunk << std::endl;
              exit (1);
            }
        i += children + 1;
      }
}
