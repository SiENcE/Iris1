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
#include "granny/grnmeshes.h"
#include <stdlib.h>
#include <iostream>




using namespace std;

void BoneWeight::addWeight (dword bone, float weight)
{
  bones[numWeights] = bone;
  weights[numWeights] = weight;
  numWeights++;
}

Mesh::Mesh ()
{
  pointOffset = normalOffset = textureOffset = polygonOffset = weightOffset = (dword)-1;
}

Mesh::~Mesh ()
{
}
void Mesh::load (cGrannyStream * file, dword meshOffset, dword baseOffset,
                 dword peers)
{
  dword oldPos;
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
        switch (chunk)
            {
            case 0xCA5E0603:   // Point container
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0604:   // Point block
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0702:   // Weights
              weightOffset = offset + baseOffset;
              break;
            case 0xCA5E0801:   // Points
              pointOffset = offset + baseOffset;
              break;
            case 0xCA5E0802:   // Normals
              normalOffset = offset + baseOffset;
              break;
            case 0xCA5E0803:   // Texture Points
              textureOffset = offset + baseOffset;
              break;
            case 0xCA5E0804:   // Texture Container
              load (file, offset, baseOffset, children);
              break;
            case 0xCA5E0901:   // Polygons
              polygonOffset = offset + baseOffset;
              break;
            case 0xCA5E0f04:   // Mesh ID
              meshIDOffset = offset + baseOffset;
              oldPos = file->tellg ();
              file->seekg (meshIDOffset);
              id = file->readDword ();
              file->seekg (oldPos);
              loadPoints (file);
              loadNormals (file);
              loadTextureMap (file);
              loadWeights (file);
              loadPolygons (file);
              break;
            default:
              hex (cerr);
              cerr << "Unknown Mesh Chunk: " << chunk << endl;
              exit (1);
            }
        i += children + 1;
      }
}

void Mesh::loadPoints (cGrannyStream * file)
{
  union
  {
    dword d;
    float f;
  } fd;
  if (pointOffset == (dword)-1 || normalOffset == (dword)-1)
    return;
  dword oldPos = file->tellg ();
  file->seekg (pointOffset);
  for (unsigned int i = 0; i < (normalOffset - pointOffset) / 12; i++)
      {
		Point point;
        for (int x = 0; x < 3; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            }
        points.push_back (point);
      }
  file->seekg (oldPos);
}

void Mesh::loadNormals (cGrannyStream * file)
{
  union
  {
    dword d;
    float f;
  } fd;
  if (textureOffset == (dword)-1 || normalOffset == (dword)-1)
    return;
  dword oldPos = file->tellg ();
  file->seekg (normalOffset);
  for (unsigned int i = 0; i < (textureOffset - normalOffset) / 12; i++)
      {
		Point point;
        for (int x = 0; x < 3; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            }
        normals.push_back (point);
      }
  file->seekg (oldPos);
}

void Mesh::loadTextureMap (cGrannyStream * file)
{
  union
  {
    dword d;
    float f;
  } fd;

  if (textureOffset == (dword)-1 || weightOffset == (dword)-1)
    return;

  dword oldPos = file->tellg ();
  file->seekg (textureOffset);
                                          /*dword num= */ file->readDword ();
                                          //unknown
  for (unsigned int i = 0; i < (weightOffset - (textureOffset + 4)) / 12; i++)
      {
		Point point;
        for (int x = 0; x < 2; x++)
            {
              fd.d = file->readDword ();
			point.points[x]=fd.f;
            }
        fd.d = file->readDword ();  //unknown
        textureMap.push_back (point);
      }
  file->seekg (oldPos);
}

void Mesh::loadWeights (cGrannyStream * file)
{
  union
  {
    dword d;
    float f;
  } fd;

  if (weightOffset == (dword)-1 || polygonOffset == (dword)-1)
    return;

  dword oldPos = file->tellg ();
  file->seekg (weightOffset);
  dword size = file->readDword ();
  file->readDword ();           //unknown
  file->readDword ();           //unknown
  for (unsigned int i = 0; i < size; i++)
      {
        dword numBones = file->readDword ();
		BoneWeight boneWeight;
        for (unsigned int x = 0; x < numBones; x++)
            {
              dword bone = file->readDword ();
              fd.d = file->readDword ();
			boneWeight.addWeight(bone,fd.f);
            }
		weights.push_back(boneWeight);
      }
  file->seekg (oldPos);
}

void Mesh::loadPolygons (cGrannyStream * file)
{
  if (meshIDOffset == (dword)-1 || polygonOffset == (dword)-1)
    return;
  dword oldPos = file->tellg ();
  file->seekg (polygonOffset);
  for (unsigned int i = 0; i < (meshIDOffset - polygonOffset) / 24; i++)
      {
		gPolygon polygon;
        for (int x = 0; x < 6; x++)
			polygon.nodes[x]=file->readDword();
		polygons.push_back(polygon);
      }
  file->seekg (oldPos);
}



Meshes::Meshes ()
{
}

Meshes::~Meshes ()
{
}

void Meshes::load (cGrannyStream * file, dword meshOffset, dword baseOffset,
                   dword peers)
{
  for (unsigned int i = 0; i < peers;)
      {
        dword chunk = file->readDword ();
        dword offset = file->readDword ();
        dword children = file->readDword ();
		Mesh mesh;
        switch (chunk)
            {
            case 0xCA5E0601:
				mesh.load(file,offset,baseOffset,children);
				meshes.push_back(mesh);
              break;
            default:
              hex (cerr);
              cerr << "Unknown Meshes Chunk: " << chunk << endl;
              exit (1);
            }
        i += children + 1;
      }
}
