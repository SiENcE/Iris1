//
// File: MapBlock.h
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

#ifndef _MAPBLOCK3D_H_
#define _MAPBLOCK3D_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"

#include <iostream>
#include <assert.h>
#include "renderer/StaticObjects.h"
#include "renderer/DynamicObjects.h"
#include "renderer/MapBlock.h"
#include "renderer/3D/SceneMaker.h"

#include "renderer/3D/SceneMaker.h"
#include "renderer/3D/Light3D.h"
#include "renderer/3D/LightNodes.h"
#include "renderer/3D/LightNodeEnvironment.h"
#include "renderer/3D/MotiveBasedLight.h"
#include "renderer/3D/MotiveBasedGroundLight.h"
#include "renderer/3D/ShaderMatrixBlock.h"

// #include "../Fluid/mmgr.h"

class cMapblock3D : public cMapblock
{
public:
   cMapblock3D (int blockx, int blocky);
   virtual ~cMapblock3D ();
   bool Generate(cLightNodeEnvironment & environment);
   void Render(int x, int y, bool do_culling, float move_x, float move_y);
   void RenderWater(int x, int y, bool do_culling);
   void RenderTransparent();
   void RenderSelection(int x, int y);
   
   sStaticObject* CheckRay(float vecOrigin[3], float vecDir[3], float &distance, float deltax, float deltay, int max_z = 1000);
   Uint8 CheckRayOnGround(float vecOrigin[3], float vecDir[3], float &distance, float deltax, float deltay, int &height);
   
   cLightNodeHandler * GetNodeHandler () { return &nodehandler; }
 
   void AddLight (cLight3D * light);
   void RemoveLight (cLight3D * light);
   
   cShaderMatrixBlock * GetShaderMatrix () { return &shader_matrix; }
   void SetRecalcAmbientLightFlag ();
   void AddMultiObject(Uint32 id, Uint16 tileid, Uint16 dye, int x, int y, int z);

   virtual void set_light_color (sColor ambient_color, sColor sun_color);
   virtual void set_light_direction (float light_direction[3]);

protected:
	//float groundtiles[8*8*4*3];
	float groundnormals[9][9][3];
	//float groundtexs[8*8*4*2];
	//unsigned short ground_indices[8*8*6];

	//float groundspheres[8][8][4];

	//char hash[64];

	std::vector<cMotiveBasedLight *> m_vLights;

	int watermap[8][8];
	int cullmode;
	float sphere[4];

	void CreateFrustum(float x, float y);

	//cNodeList nodes;
	//cNodeQuadList nodequads;
	cLightNodeHandler nodehandler;

	bool _transparent;
	bool m_recalc_ambient_light;

	vertex ground_vertieces[9][9];
	Uint8 ground_colors[9][9][2][3]; // [y][x][mode (0 = drawn light, 1 = static light)][rgb]

	cMotiveBasedGroundLight * light_handler;
	cShaderMatrixBlock shader_matrix;
	sColor m_ambient_light_color, m_sun_light_color;
	float m_light_direction[3];
   

};


#endif //_MAPBLOCK3D_H_
