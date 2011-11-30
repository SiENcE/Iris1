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

#ifndef __GRNTEXTUREINFOS_H__
#define __GRNTEXTUREINFOS_H__
#include <vector>
#include "GrannyStream.h"

// #include "../Fluid/mmgr.h"

class TextureInfo
{
	public:
		TextureInfo();
		~TextureInfo();
		void load( cGrannyStream *file, dword offset, dword baseOffset, dword children );
		dword getID();
	private:
		dword width,height,depth,id;
};

class TextureInfos
{
public:
  TextureInfos();
  ~TextureInfos();
  void load( cGrannyStream *file, dword offset, dword baseOffset, dword children );
	dword getTextureID();
private:
	std::vector<TextureInfo> textures;
};

#endif
