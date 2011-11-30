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

#ifndef __GRNBONES_H__
#define __GRNBONES_H__

#include <iostream>
#include <vector>
#include "grnmatrix.h"
#include "GrannyStream.h"
#include "Common.h"

// #include "../Fluid/mmgr.h"

class Bone
{
public:
	Bone();
	~Bone();

	dword parent,id;
	Point translate;
	Point quaternion;
	GrnMatrix matrix,curMatrix;
	std::vector< class Bone * > children;
	std::string name;
};

class Bones
{
public:
	Bones();
	~Bones();

	void load( cGrannyStream * file,dword offset,dword baseOffset,dword children);
	std::vector<Bone *>bones;
private:
};

#endif
