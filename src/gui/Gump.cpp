//
// File: Gump.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
//
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

#include "gui/Gump.h"
#include "Logger.h"
#include "Config.h"




using namespace std;

GumpHandler pGumpHandler;

GumpHandler::GumpHandler()
{
}

GumpHandler::~GumpHandler()
{
	ClearTextures();
}

void GumpHandler::ClearTextures( void )
{
	GumpBuffer_t::iterator iter;

	for ( iter = tex_root.begin(); iter != tex_root.end(); iter++ )
	{
		SAFE_DELETE( (*iter).second );
	}

	tex_root.clear();
}

Texture *GumpHandler::GetTexture (int index)
{
  GumpBuffer_t::iterator iter;

  iter = tex_root.find ((Uint32) index);
  if (iter == tex_root.end ())
    return NULL;
  else
    return (*iter).second;
}

void GumpHandler::AddTexture (int index, Texture * texture)
{
  tex_root.erase ((Uint32) index);
  tex_root.insert (make_pair ((Uint32) index, texture));
}

void GumpHandler::DelTexture (int index)
{
  Texture *texture = GetTexture (index);
  if (texture)
    delete texture;
  tex_root.erase ((Uint32) index);
}

