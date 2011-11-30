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

#ifndef __GRNITEMLIST_H__
#define __GRNITEMLIST_H__
#include <string>
#include "grntype.h"

#include "grntextchunk.h"
#include "grnobjects.h"
#include "grnmeshes.h"
#include "grnbones.h"
#include "grnboneties.h"
#include "grntextureinfos.h"
#include "grntextures.h"
#include "grnanim.h"
#include "grnunhandled.h"

#include "GrannyStream.h"

// #include "../Fluid/mmgr.h"


class ItemList
{
public:
  ItemList();
  ~ItemList();
  void load( cGrannyStream * file );
   std::string getTextureName();
	Meshes &getMeshes();
	Bones &getBones();
	BoneTies &getTies();
	Textures &getTextures();
	BoneAnim &getBoneAnim(dword id);
	Animations &getAnimations();
	dword findString( std::string str );
	dword getValue( dword obj, dword key );
	dword findValue( dword key, dword value );
	std::string findID( dword id );
        
        float getAnimLength();
private:
	void loadList( dword peers, cGrannyStream  * file );
	TextChunk textChunk;
  Objects objects;
  Meshes meshes;
  Bones *bones;
	BoneTies boneTies;
  TextureInfos textureInfos;
  Textures textures;
  Animations animations;
	Unhandled unhandled;
	dword listOffset;
};

#endif
