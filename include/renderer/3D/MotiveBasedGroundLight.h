//
// File: MotiveBasedGroundLight.h
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
   This Module handles the Motive Based Lighting of a ground block
   for more information, see engine docs
*/
 
#ifndef _MOTIVEBASEDGROUNDLIGHT_H_
#define _MOTIVEBASEDGROUNDLIGHT_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <list>

#include "irisgl.h"
#include "SDL/SDL.h"

#include "renderer/3D/Light3D.h"
#include "renderer/3D/SceneMaker.h"
#include "renderer/3D/ShaderMatrixBlock.h"
#include "uotype.h"

// #include "../Fluid/mmgr.h"

// A light motive contains information in which way a light source influences an object
class cGroundLightMotive {
    private:
        float m_factor_array[9][9]; // each point has a factor for the intensity
        cLight3D * m_light; // each light has a corresponding "motive" class on an object
    public:
        cGroundLightMotive (int blockx, int blocky, vertex * ground_vertieces, float * ground_normals, cLight3D * light); // ground_vertieces needs to be a 9x9 array of struct vertex
        ~cGroundLightMotive ();
        
        // Calculates the light intensity dependent from the model and the model position
        void ApplyLight (int blockx, int blocky, vertex * ground_vertieces, float * ground_normals); // ground_vertieces needs to be a 9x9 array of struct vertex
        
        cLight3D * light ();
        float * factor_array ();
};

// This list of light motives handles the link between a ground block and the lights which are reaching the object
class cMotiveBasedGroundLight {
    protected: // child classes may access
       std::list <cGroundLightMotive *> motives;  // list of motives
       vertex * m_ground_vertieces; // needs to be an array of 9x9
       sColor ambient_color_array[81]; // needs to be a array of 9x9 
       float * m_ground_normals; // needs to be an array of 9x9x3
       int m_blockx, m_blocky; // position of the object
    public:
       cMotiveBasedGroundLight (int blockx, int blocky, vertex * ground_vertieces, float * ground_normals); // ground_vertieces needs to be a 9x9 array of struct vertex
       virtual ~cMotiveBasedGroundLight ();
       
       // Calculates the ambient light of the ground
       void CalcAmbientLight (sColor ambient_color, sColor sun_color, float direction [3], cShaderMatrixBlock * shader_matrix);
       
       // Adds a new light source 
       void AddLight (cLight3D * light);
       
       // Removes an existing light source
       void RemoveLight (cLight3D * light);
       
       // Sums the different light sources (motives) and calculates a color for each point
       // Call this immediately before rendering
       void PrepareForRendering ();
};

 
#endif //_MOTIVEBASEDGROUNDLIGHT_H_
