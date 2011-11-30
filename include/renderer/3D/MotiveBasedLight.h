//
// File: MotiveBasedLight.h
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

/*
   This Module handles the Motive Based Lighting of Static Models
   for more information, see engine docs
*/
 
#ifndef _MOTIVEBASEDLIGHT_H_
#define _MOTIVEBASEDLIGHT_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <list>

#include "irisgl.h"
#include "SDL/SDL.h"

#include "loaders/StaticModels.h"
#include "renderer/3D/Light3D.h"
#include "renderer/3D/LightNodeEnvironment.h"
#include "uotype.h"

// #include "../Fluid/mmgr.h"

// A light motive contains information in which way a light source influences an object
class cLightMotive {
    private:
        float * m_factor_array; // each point has a factor for the intensity
        cLight3D * m_light; // each light has a corresponding "motive" class on an object
        int m_nodecount;
    public:
        cLightMotive (float x, float y, float z, cStaticModel * model, cLight3D * light);
        ~cLightMotive ();
        
        // Calculates the light intensity dependent from the model and the model position
        void ApplyLight (float x, float y, float z, cStaticModel * model);
        
        cLight3D * light ();
        float * factor_array ();
};

// This list of light motives handles the link between an object and the lights which are reaching the object
class cMotiveBasedLight
{
protected: // child classes may access
	std::list <cLightMotive *> motives;  // list of motives
	cStaticModel *m_model; // model of the object
	sColor *ambient_color_array; // each point has a specific ambient color
	float m_x, m_y, m_z; // position of the object

public:
	cMotiveBasedLight( float x, float y, float z, int blockx, int blocky,  cStaticModel *model );
	virtual ~cMotiveBasedLight();

	// Calculates the ambient light of the object
	virtual void CalcAmbientLight( sColor ambient_color, sColor sun_color, float direction [3] ) = 0;

	// Adds a new light source to the object
	void AddLight( cLight3D *light );

	// Removes an existing light source from the object
	void RemoveLight( cLight3D *light );

	// Sums the different light sources (motives) and calculates a color for each point
	// Call this immediately before rendering
	void PrepareModelForRendering();
};


/*  There are two different types of object lighting which only differ by the kind of ambient lighting */

// Entity lighting is the general form of ambient lighting
class cMotiveBasedLight_Entity : public cMotiveBasedLight
{
public:
	cMotiveBasedLight_Entity (float x, float y, float z, int blockx, int blocky, cStaticModel * model);
	~cMotiveBasedLight_Entity() { }

	virtual void CalcAmbientLight (sColor ambient_color, sColor sun_color, float direction [3]);
};

// Tile lighting needs more memory and costs more speed, but it looks alot better vor tileable items 
// (like walls / stairs / roofs and so on)
class cMotiveBasedLight_Tile : public cMotiveBasedLight
{
protected:
	sLightNode * * light_node_list;
public:
	cMotiveBasedLight_Tile (float x, float y, float z, int blockx, int blocky, cStaticModel * model, cLightNodeEnvironment & node_environment);
	~cMotiveBasedLight_Tile ();

	virtual void CalcAmbientLight (sColor ambient_color, sColor sun_color, float direction [3]);
};

 
#endif //_MOTIVEBASEDLIGHT_H_
