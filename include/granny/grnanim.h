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

#ifndef __GRNANIM_H__
#define __GRNANIM_H__

#include <vector>
#include "GrannyStream.h"
#include "Common.h"

// #include "../Fluid/mmgr.h"

class BoneAnim
{
	public:
		BoneAnim();
		~BoneAnim();
		void load( cGrannyStream * file, dword offset, dword baseOffset, dword children );
		std::vector<float> translateTimeline;
		std::vector<float> quaternionTimeline;
		std::vector<Point> translates;
		std::vector<Point> quaternions;
		float length;
		dword id;
	private:
		dword numTranslates,numQuaternions,numUnknowns;
		std::vector< float > unknownTimeline;
		std::vector< Point > unknowns;
};

class Animations
{
public:
  Animations();
  ~Animations();
  void load( cGrannyStream * file,dword offset,dword baseOffset,dword children);
	BoneAnim &getBoneAnim(dword id);
	std::vector<BoneAnim *>bones;
  float length();
private:
};

#endif
