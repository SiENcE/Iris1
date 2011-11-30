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

#ifndef _RENDERER_H_
#define _RENDERER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
#include <vector>
#include <list>

#include "Texture.h"
#include "Camera.h"
#include "DynamicObjects.h"
#include "Characters.h"

#include "SDL/SDL.h"

#include "Fade.h"

// #include "../Fluid/mmgr.h"
#include "renderer/3D/WorldEnvironment.h"

class Renderer
{
protected:
	int SetFader( int min_z, Uint8 alpha, cFader *fader, bool below );
	void ResetFader( cFader *fader );

	std::list<cFader *> static_faders;
	void ClearStaticFaders();
	void HandleStaticFaders();
	
	Uint16 drag_model;
	int dragposition[3];

	int view_distance;
	float m_lightlevel;
	int flags;

    cWorldEnvironment m_world_environment;    

public:
   Renderer();
   virtual ~Renderer();

   virtual int Init( void ) = 0;
   virtual void DeInit( void ) = 0;
   virtual void RenderScene( void ) = 0;
   virtual void GrabDynamic( int x, int y, cDynamicObject **r_object, cCharacter **r_character ) = 0;
   virtual void GrabMousePosition( int x, int y, int maxz, int cursor3d[3], int & artid ) = 0;
   virtual void AddDynamic( cDynamicObject *object );
   virtual void DelDynamic( cDynamicObject *object );
   virtual void AddCharacter( cCharacter *character ) { return; }
   virtual void DelCharacter( cCharacter *character ) { return; }
   virtual void LoadSkyboxTextures( int map = 0 ) { return; }


   void SetRenderFlag( int flag, int value );
   int GetRenderFlag( int flag );
   void FadeStatics( int to, Uint32 time );
   
   int GetRoofHeight();
   void AdjustCameraZ();
   
   int GetViewDistance() { return view_distance; }
   
   void setLightLevel( float lightlevel ) { m_lightlevel = lightlevel; }
   float lightlevel() { return m_lightlevel; }
   
   void setDragModel( Uint16 drag_model, int x = 0, int y = 0, int z = 0 );
   
   cWorldEnvironment & world_environment ();
};

#endif //_Renderer_H_
