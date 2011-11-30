//
// File: Light3D.cpp
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


#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include "renderer/3D/SceneMaker.h"
#include "Geometry.h"
#include "Logger.h"
#include "Exception.h"
#include "include.h"
#include "Config.h"
#include "renderer/3D/MapBuffer3D.h"
#include "renderer/DynamicObjects.h"
#include "renderer/3D/Light3D.h"




using namespace std;

cLightManager pLightManager;

/****************************************************************************************************
   cLight3D - Base Class for Light effects
****************************************************************************************************/

cLight3D::cLight3D (float x, float y, float z, float radius, sColor color,
                    bool white_light)
{
  m_radius = radius;
  m_color = color;
  m_white_light = white_light;
  SetPosition (x, y, z);
//   printf("Creating light (%.2f/%.2f/%.2f) radius: %.2f\n", x, y, z, radius);
}

cLight3D::~cLight3D ()
{
  //printf("Removing light (%.2f/%.2f/%.2f) radius: %.2f\n", m_x, m_y, m_z, m_radius);
}

// property getters      
float cLight3D::x ()
{
  return m_x;
}

float cLight3D::y ()
{
  return m_y;
}

float cLight3D::z ()
{
  return m_z;
}

float cLight3D::radius ()
{
  return m_radius;
}

int cLight3D::minx ()
{
  return m_minx;
}

int cLight3D::maxx ()
{
  return m_maxx;
}

int cLight3D::miny ()
{
  return m_miny;
}

int cLight3D::maxy ()
{
  return m_maxy;
}

bool cLight3D::white_light ()
{
  return m_white_light;
}

sColor cLight3D::color ()
{
  return m_color;
}


void cLight3D::get_bounds ()
{
  m_minx = (int) (m_x - m_radius - 1) / 8;
  m_maxx = (int) (m_x + m_radius + 1) / 8;
  m_miny = (int) (m_y - m_radius - 1) / 8;
  m_maxy = (int) (m_y + m_radius + 1) / 8;
}


void cLight3D::SetPosition (float x, float y, float z)
{
  m_x = x;
  m_y = y;
  m_z = z;
  get_bounds ();
}

bool cLight3D::light_in_block (int blockx, int blocky)
{
  return ((blockx >= m_minx) && (blocky >= m_miny) && (blockx <= m_maxx)
          && (blocky <= m_maxy));
}

void cLight3D::Handle (int aTick)
{
  return;                       // just for child classes, do nothing
}





/*****************************************************************************************************
   cFlickeringLight3D - class for Flickering Lights (a very nice effect)
*****************************************************************************************************/

cFlickeringLight3D::cFlickeringLight3D (float x, float y, float z, float radius, sColor color, bool white_light, int flicker_min_delay, int flicker_max_delay, int flicker_amount):cLight3D (x, y, z, radius, color,
          white_light)
{
  m_flicker_min_delay = flicker_min_delay;
  m_flicker_max_delay = flicker_max_delay;

  if (m_flicker_min_delay < 10)
    m_flicker_min_delay = 10;

  if (m_flicker_max_delay < m_flicker_min_delay + 10)
    m_flicker_max_delay = m_flicker_min_delay + 10;

  m_last_change_tick = 0;
  m_flicker_amount = flicker_amount;

  if (m_flicker_amount < 0)
    m_flicker_amount = 0;
  if (m_flicker_amount > 100)
    m_flicker_amount = 100;

  m_base_color = color;
  m_current_factor = 1.0f;
  m_target_factor = 1.0f;
  m_delta_factor = 1.0f;
}

void cFlickeringLight3D::Handle (int aTick)
{

  float factor =
    m_current_factor + m_delta_factor * (aTick - m_last_change_tick);
  if (((factor >= m_target_factor) && (m_delta_factor >= 0.0f)) ||      // check if target has been reached
      ((factor <= m_target_factor) && (m_delta_factor <= 0.0f)))
      {
        factor = m_target_factor;
        m_target_factor = 1.0f - ((float) (rand () % 1000) * 0.00001f) * (float) m_flicker_amount;    // "reflicker"

        // calc a new random delay
        float delay =
          ((float) m_flicker_min_delay +
           ((float) (rand () % 1000) * 0.001f) *
           (float) (m_flicker_max_delay - m_flicker_min_delay));
        m_delta_factor = (m_target_factor - factor) / (float) delay;    // summand is difference by tick
      }

  assert (factor <= 1.0f);

  for (int i = 0; i < 3; i++)
    m_color.color_array[i] =
      (int) (m_base_color.color_array[i] * factor * factor);

  m_current_factor = factor;
  m_last_change_tick = aTick;

}


/*****************************************************************************************************
   cLightManager - class for managing light effects
*****************************************************************************************************/

cLightManager::cLightManager ()
{
}


cLightManager::~cLightManager ()
{
  Clear ();                     // clean up
}


void cLightManager::Clear ()
{

  ClearStatic ();
  ClearDynamic ();
}

void cLightManager::ClearStatic ()
{
  list < cLight3D * >::iterator iter;
  for (iter = static_lights.begin (); iter != static_lights.end (); iter++)
    delete *iter;
  static_lights.clear ();
}

void cLightManager::ClearDynamic ()
{
  list < cLight3D * >::iterator iter;
  for (iter = dynamic_lights.begin (); iter != dynamic_lights.end (); iter++)
    delete *iter;
  dynamic_lights.clear ();
}

void cLightManager::AddStaticLight (cLight3D * light)
{
  static_lights.push_back (light);
}

void cLightManager::AddDynamicLight (cLight3D * light)
{
  dynamic_lights.push_back (light);
}


void cLightManager::Handle (int aTick)
{
  list < cLight3D * >::iterator iter;
  for (iter = static_lights.begin (); iter != static_lights.end (); iter++)
    (*iter)->Handle (aTick);

  for (iter = dynamic_lights.begin (); iter != dynamic_lights.end (); iter++)
    (*iter)->Handle (aTick);
}

cLight3D *cLightManager::AddDefinedStaticLightSource (int x, int y, int z,
                                                      cStaticModelLightSourceInfo
                                                      * light_info)
{
  if (!light_info)
    return NULL;

        float light_x = (float) x + light_info->x ();
        float light_y = (float) y + light_info->y ();
        float light_z = (float) z + light_info->z () * 10.0f;
        cLight3D *light;
        if (light_info->flickering ())
          light =
            new cFlickeringLight3D (light_x, light_y, light_z,
                                    light_info->radius (),
                                    light_info->color (), false,
                                    light_info->flicker_min_delay (),
                                    light_info->flicker_max_delay (),
                                    light_info->flicker_amount ());
        else
          light =
            new cLight3D (light_x, light_y, light_z, light_info->radius (),
                          light_info->color (), false);
        AddStaticLight (light);

        ((Mapbuffer3D *) pMapbufferHandler.buffer())->AddLight (light);
        pDynamicObjectList.AddLight (light);
        return light;
}


void cLightManager::UnRegisterLight (cLight3D * light)
{
  assert (light);
  ((Mapbuffer3D *) pMapbufferHandler.buffer())->RemoveLight (light);
    
  pDynamicObjectList.RemoveLight (light);

  list < cLight3D * >::iterator iter;
  for (iter = static_lights.begin (); iter != static_lights.end (); iter++)
    if (*iter == light)
        {
          static_lights.erase (iter);
          break;
        }
  delete light;
}
