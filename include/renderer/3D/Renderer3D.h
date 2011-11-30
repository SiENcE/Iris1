//
// File: Renderer.h
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

#ifndef _RENDERER3D_H_
#define _RENDERER3D_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "Game.h"
#include "renderer/Texture.h"
#include "renderer/Camera.h"

#include <string>
#include <vector>
#include <list>

#include "SDL/SDL.h"

#include "renderer/Fade.h"
#include "renderer/Renderer.h"

// #include "../Fluid/mmgr.h"

/*
//flo:
#ifdef WIN32
#define GetTime(store) store = GetTickCount()
#else
#include <sys/time.h>
#define GetTime(store) \
	{ \
		struct timeval tv; \
		gettimeofday(&tv, NULL); \
		store = (tv.tv_usec/1000) + (tv.tv_sec*1000); \
	}
#endif
//floend:
*/

class Renderer3D : public Renderer
{
protected:
	Game *m_kGame;
	void RenderTerrain (bool do_culling);
	void RenderWater (bool do_culling);
	void RenderSkybox ();
	void RenderTransparent ();
	void RenderDynamics (bool do_culling);
	void RenderDynamicsTransparent ();
	void RenderCharacters (bool do_culling);
	void RenderDragModel();
    void GenerateLight ();
    //void LoadSkyboxTextures ();
	
	Texture *skyboxtextures[5];
    Texture *tex_water;
    Texture *tex_char_shadow;
    

private:
	float fog_view_dist;
	float lightNormalize;
	float cam_x, cam_y, cam_z;
	int current_ticks;
    
	sColor m_ambient_light_color, m_sun_light_color;
	float m_light_direction[3];

//flo:	unsigned long lastTime;

public:
   Renderer3D ();
   virtual ~Renderer3D ();

   virtual int Init(void);
   virtual void DeInit(void);
   virtual void RenderScene(void);
   virtual void GrabDynamic(int x, int y, cDynamicObject ** r_object, cCharacter ** r_character);
   virtual void GrabMousePosition(int x, int y, int maxz, int cursor3d[3], int & artid);
   virtual void AddDynamic (cDynamicObject * object);
   virtual void DelDynamic (cDynamicObject * object);
   virtual void LoadSkyboxTextures(int map=0);
   
      
};

#endif //_Renderer3D_H_
