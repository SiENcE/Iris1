//
// File: Renderer.cpp
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


#include "renderer/Renderer.h"
#include "renderer/SDLScreen.h"
#include "renderer/MapBuffer.h"
#include "renderer/DynamicObjects.h"
#include "renderer/Characters.h"
#include "renderer/particles/ParticleEngine.h"

#include "granny/GrannyLoader.h"

#include "gui/GUIHandler.h"
#include "gui/Label.h"

#include "net/Client.h"

#include "include.h"
#include "Config.h"
#include "Logger.h"

#include "Geometry.h"

#include <math.h>
#include <iostream>




using namespace std;

//extern SDLScreen *SDLscreen;

Renderer::Renderer ()
{
	flags = 0;
	view_distance = Config::GetViewDistance();
	m_lightlevel = 1.0f;
	drag_model = 0;
}

Renderer::~Renderer ()
{
	ClearStaticFaders ();
}


void Renderer::SetRenderFlag (int flag, int value)
{
	switch (flag)
	{

	case 0:
		if (value)
			flags |= flag;        /* set flag */
		else
			flags &= !flag;       /* reset flag */
		break;

	default:
		Logger::WriteLine ("Unknown Flag in Game::SetRenderFlag(int, int)", __FILE__,
			__LINE__, LEVEL_ERROR);
	}
}

int Renderer::GetRenderFlag (int flag)
{
	return (flags & flag);
}


int Renderer::SetFader (int min_z, Uint8 alpha, cFader * fader, bool below)
{
	int blockx = pCamera.GetBlockX ();
	int blocky = pCamera.GetBlockY ();
	int count = 0;

	for (int y = -view_distance; y < view_distance; y++)
		for (int x = -view_distance; x < view_distance; x++)
		{
			cMapblock *block = pMapbufferHandler.buffer ()->CreateBlock (blockx + x, blocky + y);
			if (block)
				count += block->UpdateFader (min_z, alpha, fader, below);
		}

	count += pDynamicObjectList.UpdateFader (min_z, alpha, fader, below);

	return count;

}

void Renderer::ResetFader (cFader * fader)
{
	pMapbufferHandler.buffer ()->ResetFader (fader);

	pDynamicObjectList.ResetFader (fader);

}

void Renderer::FadeStatics (int to, Uint32 time)
{
}

int Renderer::GetRoofHeight ()
{

	Uint32 x, y;
	int z;
	pCamera.GetGamePosition (x, y, z);

	cMapblock *block = pMapbufferHandler.buffer ()->CreateBlock (x / 8, y / 8);
	if (block)
	{
		if (block->GetRoofHeight (x % 8, y % 8, z) != ROOF_NONE)
			return z + 15;
	}

	if (pDynamicObjectList.GetRoofHeight (x, y, z) != ROOF_NONE)
		return z + 15;

	return ROOF_NONE;
}


void Renderer::AdjustCameraZ ()
{

	float act_x, act_y, act_z;

	pCamera.GetGamePosition (act_x, act_y, act_z);

	unsigned int x1, y1, x2, y2, x3, y3, x4, y4;
	int z1, z2, z3, z4;

	x1 = (int) act_x;
	y1 = (int) act_y;
	x2 = (int) (act_x + 0.99f);
	y2 = (int) (act_y);
	x3 = (int) (act_x);
	y3 = (int) (act_y + 0.99f);
	x4 = (int) (act_x + 0.99f);
	y4 = (int) (act_y + 0.99f);
	z1 = z2 = z3 = z4 = (int) act_z;

	cMapblock *block;

	block = pMapbufferHandler.buffer ()->CreateBlock (x1 / 8, y1 / 8);
	z1 = block->GetGroundZ (x1 % 8, y1 % 8);

	block = pMapbufferHandler.buffer ()->CreateBlock (x2 / 8, y2 / 8);
	z2 = block->GetGroundZ (x2 % 8, y2 % 8);

	block = pMapbufferHandler.buffer ()->CreateBlock (x3 / 8, y3 / 8);
	z3 = block->GetGroundZ (x3 % 8, y3 % 8);

	block = pMapbufferHandler.buffer ()->CreateBlock (x4 / 8, y4 / 8);
	z4 = block->GetGroundZ (x4 % 8, y4 % 8);

	float fracx = act_x - (float) ((int) act_x);
	float fracy = act_y - (float) ((int) act_y);

	float d4 = fracx * fracy;
	float d3 = (1.0f - fracx) * fracy;
	float d2 = fracx * (1.0f - fracy);
	float d1 = (1.0f - fracx) * (1.0f - fracy);

	float smooth_z = (d1 * z1 + d2 * z2 + d3 * z3 + d4 * z4);


	pCamera.SetZ (-smooth_z);


	pCamera.CamHasChanged ();

}


void Renderer::ClearStaticFaders ()
{
	list < cFader * >::iterator iter;
	for (iter = static_faders.begin (); iter != static_faders.end (); iter++)
		delete *iter;
	static_faders.clear ();
}

void Renderer::HandleStaticFaders ()
{
	list < cFader * >::iterator iter;
	for (iter = static_faders.begin (); iter != static_faders.end (); iter++)
	{
		(*iter)->Handle ();
		if ((*iter)->finished ())
		{
			ResetFader ((*iter));
			delete (*iter);
			static_faders.erase (iter);
			iter = static_faders.begin ();
			// HARKON : fixed bug
			if (iter == static_faders.end()) break;
		}
	}
}

void Renderer::setDragModel (Uint16 drag_model, int x, int y, int z)
{
	this->drag_model = drag_model;
	dragposition[0] = x;
	dragposition[1] = y;
	dragposition[2] = z;
}

void Renderer::AddDynamic (cDynamicObject * object)
{
}

void Renderer::DelDynamic (cDynamicObject * object)
{
}

cWorldEnvironment & Renderer::world_environment ()
{
    return m_world_environment;
}

