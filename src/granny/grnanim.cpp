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
#include "granny/grnanim.h"

#include <stdlib.h>
#include <iostream>
#include <cassert>




BoneAnim::BoneAnim() : id(0), length(0), numTranslates(0), numQuaternions(0), numUnknowns(0)
{
}

BoneAnim::~BoneAnim ()
{
}

void BoneAnim::load (cGrannyStream * file, dword offset, dword baseOffset,
                     dword peers)
{
  assert (file);
  union
  {
    dword d;
    float f;
  } fd;
  dword oldPos = file->tellg ();
  file->seekg (offset + baseOffset);
  id = file->readDword ();
  unsigned int i;
  for (i = 0; i < 5; i++)
      {
        file->readDword ();     //unknown
      };
  numTranslates = file->readDword ();
  numQuaternions = file->readDword ();
  numUnknowns = file->readDword ();
  for (i = 0; i < 4; i++)
      {
        file->readDword ();     //unknown
      };

  for (i = 0; i < numTranslates; i++)
      {
        fd.d = file->readDword ();
        translateTimeline.push_back (fd.f);
      };
  length = fd.f;
  for (i = 0; i < numQuaternions; i++)
      {
        fd.d = file->readDword ();
        quaternionTimeline.push_back (fd.f);
      };
  for (i = 0; i < numUnknowns; i++)
      {
        fd.d = file->readDword ();
        unknownTimeline.push_back (fd.f);
      };

	Point point;

  for (i = 0; i < numTranslates; i++)
      {
        for (int x = 0; x < 3; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            };
		translates.push_back(point);
      };
  for (i = 0; i < numQuaternions; i++)
      {
        for (int x = 0; x < 4; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            };
		quaternions.push_back(point);
      };
  for (i = 0; i < numUnknowns; i++)
      {
        for (int x = 0; x < 3; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            };
		unknowns.push_back(point);
      };
  file->seekg (oldPos);
}


Animations::Animations ()
{
}

Animations::~Animations ()
{
	for ( unsigned int i = 0; i < bones.size(); i++ )
	{
		SAFE_DELETE( bones[i] );
	}

	bones.clear();
}

void Animations::load (cGrannyStream * file, dword aOffset, dword baseOffset,
                       dword peers)
{
  assert (file);
  BoneAnim *bone;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E1200:   //animation list
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E1201:   //animation data
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E1203:   //animation
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E1204:   //bone animation data
              bone = new BoneAnim ();
              bone->load (file, offset, baseOffset, children);
              bones.push_back (bone);
              break;
            default:
				std::hex (std::cerr);
				std::cerr << "Unknown Animations Chunk: " << chunk << std::endl;
              exit (1);
            }
        i += children + 1;
      }
}

BoneAnim & Animations::getBoneAnim (dword id)
{
  return *bones[id];
}

float Animations::length ()
{
  if (bones.size () == 0)
    return 0.0f;

  float result = 100000000.0f;
  for (unsigned int i = 0; i < bones.size (); i++)
    if (bones[i]->length < result)
      result = bones[i]->length;

  return result;
}
