//
// File: DynamicObjects.h
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#ifndef _DYNAMICOBJECTS_H_
#define _DYNAMICOBJECTS_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <map>
#include <cassert>

#include "Fade.h"
#include "renderer/3D/Light3D.h"
#include "renderer/3D/MotiveBasedLight.h"


// #include "../Fluid/mmgr.h"

#define DYNAMICTYPE_WORLD 0
#define DYNAMICTYPE_CONTAINER 1
#define DYNAMICTYPE_EQUIPMENT 2

class cDynamicObject {
private:
    Uint32 aostooltips_listID;
    std::vector<std::string> aostooltips_list;
    
    Uint32 m_particle_effect_handle;
public:
  Uint32 id; // That is ugly and has to be done better !!
  int x, y, z;
  int model;
  int itemcount;
  int incrcounter;
  Uint8 direction;
  Uint16 dye;
  Uint8 flag;
  bool clip;
  int type;
  Uint8 alpha;
  Uint32 parent;
  float sphere[5];
  cFader * fader;
  cMotiveBasedLight * m_motive;
  cLight3D * m_light_source;
  bool m_recalc_ambient_light;
  
  cDynamicObject ();
  ~cDynamicObject ();
  
  void setMotive (cMotiveBasedLight * motive);
  cMotiveBasedLight * motive ();
  
  void setLightSource (cLight3D * light_source);
  cLight3D * light_source ();
  
  void SetRecalcAmbientLightFlag ();
  bool RecalcAmbientLightFlag ();
  
  void SetParticleEffectHandle (Uint32 handle);
  Uint32 GetParticleEffectHandle ();
  
  Uint32 AOSTooltipID(){return aostooltips_listID;}
  void SetAOSTooltipID(Uint32 id){aostooltips_listID = id;}
  void AddAOSTooltip(std::string tooltip){aostooltips_list.push_back(tooltip);}
  void ClearAOSTooltip(){aostooltips_list.clear();}
  int aostooltips_count(){return aostooltips_list.size();}
  std::string GetAOSTooltip(int id){return aostooltips_list.at(id);}
};


typedef std::map<Uint32, cDynamicObject *>  dynamiclist_t;

class cDynamicObjectList
{
private:
    dynamiclist_t dynamiclist;
    int m_roof_z;
	std::vector<cMotiveBasedLight *> vLights;
    cDynamicObject * Add (Uint32 id);

public:
    cDynamicObjectList ();    
   ~cDynamicObjectList ();

   void Clear (void);
   
   cDynamicObject * AddContainerContent (Uint32 id, Uint16 model, Uint16 dye, Uint32 parent, int x, int y, int itemcount);
   cDynamicObject * AddCharEquip (Uint32 id, Uint16 model, Uint16 dye, Uint32 parent);
   cDynamicObject * AddWorldItem (Uint32 id, Uint16 model, Uint16 dye, int x, int y, int z, int itemcount, int incrcounter, int direction, int flag);
   cDynamicObject * Get (unsigned int id);
   void Delete (unsigned int id);
   
   dynamiclist_t * GetList(void) { return &dynamiclist; }

   int GetCount(void) { return dynamiclist.size(); }

   int UpdateFader(int min_z, Uint8 alpha, cFader * fader, bool below);
   void ResetFader(cFader * fader);
   
   void setRoofZ(int z) { m_roof_z = z; }
   int GetRoofHeight(int x, int y, int z);
   
   void OnAdd(void (*callback) (cDynamicObject * object));
   void OnDelete(void (*callback) (cDynamicObject * object));
   
   void CalcAmbientLight (sColor ambient_color, sColor sun_color, float direction [3]);
   void SetRecalcAmbientLightFlag ();
   void AddLight (cLight3D * light);
   void RemoveLight (cLight3D * light);
protected:
   void (*callback_OnAdd) (cDynamicObject * object);
   void (*callback_OnDelete) (cDynamicObject * object);
};

extern cDynamicObjectList pDynamicObjectList;

#endif //_DYNAMICOBJECTS_H_
