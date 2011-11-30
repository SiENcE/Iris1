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

#ifndef __GRNMESHES_H__
#define __GRNMESHES_H__
#include <list>
#include <vector>
#include "GrannyStream.h"

// #include "../Fluid/mmgr.h"


class BoneWeight
{
	public:
		BoneWeight() {numWeights=0;}
		~BoneWeight() {}
		void addWeight(dword bone,float weight);
		dword bones[10];
		float weights[10];
		dword numWeights;
	private:
};

class Mesh
{
	public:
		Mesh();
		~Mesh();
		void load( cGrannyStream *file, dword offset, dword baseOffset, dword children );
		std::vector<Point>points;
		std::vector<Point>normals;
		std::vector<Point>textureMap;
		std::vector<BoneWeight>weights;
		std::vector<gPolygon>polygons;
	private:
		void loadPoints( cGrannyStream *file );
		void loadNormals( cGrannyStream *file );
		void loadTextureMap( cGrannyStream *file );
		void loadWeights( cGrannyStream *file );
		void loadPolygons( cGrannyStream *file );
		dword id;

		dword pointOffset;
		dword normalOffset;
		dword textureOffset;
		dword weightOffset;
		dword polygonOffset;
		dword meshIDOffset;
};

class Meshes
{
public:
  Meshes();
  ~Meshes();
  void load( cGrannyStream *file, dword offset, dword baseOffset, dword children );
  std::list<Mesh> meshes;
};

#endif
