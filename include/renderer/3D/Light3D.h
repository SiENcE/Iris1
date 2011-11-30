//
// File: Light3D.h
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
  This file contains the classes describing and handling lights in the world...
*/
 
#ifndef _LIGHT3D_H_
#define _LIGHT3D_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"
#include "SDL/SDL.h"

#include <iostream>
#include <list>

#include <uotype.h>

#include "loaders/StaticModels.h"

//// #include "../Fluid/mmgr.h"

// base class for light effects
class cLight3D 
{ 
    protected:  // child classes may change
      float m_x, m_y, m_z;
      float m_radius;
      int m_minx, m_miny, m_maxx, m_maxy;
      sColor m_color;
      bool m_white_light; // a white light is faster in rendering, the red value is used for intensity in this case
      void get_bounds (); // calc bounding box, called by the constructor
    public:
      // x, y, z: light position, white_light: flag for faster rendering white lights 
      cLight3D (float x, float y, float z, float radius, sColor color, bool white_light);
      virtual ~cLight3D ();
      
      // property getters
      float x ();
      float y ();
      float z ();
      float radius ();
      
      bool white_light ();
      sColor color ();
      
      int minx ();  // returns the bouding box around the light circle
      int maxx ();
      int miny ();
      int maxy ();
     
      void SetPosition (float x, float y, float z); // Moves the light
      bool light_in_block (int blockx, int blocky); // Checks, if a light touches a block
      
      // Basic function for child classes to implement flickering / moving / color changing
      // aTick: game tick of call
      virtual void Handle (int aTick);  
};


// class for flickering lights
class cFlickeringLight3D : public cLight3D
{
    private:
       int m_last_change_tick; // when did the last intensity change 
       int m_flicker_min_delay; // flickering speed
       int m_flicker_max_delay;
       int m_flicker_amount;  // how dark does it get maximum (in % of m_base_color)
       
       float m_current_factor;  // how dark is it currently
       float m_target_factor;   // how dark should it be
       float m_delta_factor;    // summand
       
       sColor m_base_color; // original color
    public:
       cFlickeringLight3D (float x, float y, float z, float radius, sColor color, bool white_light, int flicker_min_delay, int flicker_max_delay, int flicker_amount);
       virtual void Handle (int aTick);  
};
   

// class for managing light effects
class cLightManager 
{
    private:
      std::list <cLight3D *> dynamic_lights; // lights which can move freely (slow!!)
      std::list <cLight3D *> static_lights; // lights which are always on the same position
    public:
      cLightManager ();
      virtual ~cLightManager ();
      
      void Clear ();
      void ClearDynamic ();
      void ClearStatic ();
      
      void AddDynamicLight (cLight3D * light);
      void AddStaticLight (cLight3D * light);
      
      cLight3D *  AddDefinedStaticLightSource (int x, int y, int z, cStaticModelLightSourceInfo * light_info);
     
      std::list <cLight3D *> & static_light_list () { return static_lights; }
      std::list <cLight3D *> & dynamic_light_list () { return dynamic_lights; }
      
      void UnRegisterLight (cLight3D * light);
      
      void Handle (int aTick);
};


// global light manager instance
extern cLightManager pLightManager;
 
#endif //_LIGHT3D_H_
