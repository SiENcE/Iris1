//
// File: MapBuffer3D.cpp
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

#include "renderer/3D/MapBuffer3D.h"




cMapblock *Mapbuffer3D::CreateBlock( int x, int y )
{
	cMapblock3D *block;
	cLightNodeEnvironment environment;

	if ( (x < 0) || (y < 0) )
	{
		return NULL;
	}

	block = (cMapblock3D *)Get( x, y );

	if ( !block )
	{
		block = new cMapblock3D( x, y );
		Add( block );
	}

	if ( !block->generated() )
	{
		GetEnvironment( x, y, environment );
		//fprintf(stderr, "Generating %i %i\n", x, y);
		block->Generate( environment );

		if ( m_roof_z != ROOF_NONE )
		{
			block->SetAlpha( m_roof_z, Config::GetRoofFadeAlpha(), false );
		}
	}

	return block;
}


void Mapbuffer3D::GetEnvironment (int x, int y,
                                   cLightNodeEnvironment & environment)
{
  for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
        {
          cMapblock3D *block = (cMapblock3D *) Get (x + dx, y + dy);
          if (!block)
              {
                block = new cMapblock3D (x + dx, y + dy);
                Add (block);
              }
          environment.set (dx, dy, block->GetNodeHandler ());
        }
}


void Mapbuffer3D::AddLight (cLight3D * light)
{
  assert (light);
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
      {
        cMapblock3D *block = (cMapblock3D *) iter->second;
        if (light->light_in_block (block->getBlockX (), block->getBlockY ()))
          block->AddLight (light);
      }
}

void Mapbuffer3D::RemoveLight (cLight3D * light)
{
  assert (light);
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
      {
        cMapblock3D *block = (cMapblock3D *) iter->second;
        if (light->light_in_block (block->getBlockX (), block->getBlockY ()))
          block->RemoveLight (light);
      }
}

void Mapbuffer3D::SetRecalcAmbientLightFlag ()
{
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
      {
        cMapblock3D *block = (cMapblock3D *) iter->second;
        block->SetRecalcAmbientLightFlag ();
      }
}

void Mapbuffer3D::SetLightColorAndDirection (sColor ambient_color, sColor sun_color, float light_direction[3])
{
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
      {
        cMapblock3D *block = (cMapblock3D *) iter->second;
        block->set_light_color (ambient_color, sun_color);
        block->set_light_direction (light_direction);
      }
}
