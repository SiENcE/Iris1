//
// File: ParticleEngine.h
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

#ifndef _PARTICLEENGINE_H_
#define _PARTICLEENGINE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <map>

#include "loaders/StaticModels.h"
#include "renderer/particles/ParticleHandler.h"

// #include "../Fluid/mmgr.h"

class cParticleEngine 
{
  private:
    Uint32 current_handle;
    std::map <Uint32, Particle::cParticleHandler *> particle_handlers;
  public:
    cParticleEngine ();
    ~cParticleEngine ();

    void Reset ();
    
    void Handle ();
    void Render();
    
    Uint32 AddEffect (cStaticModelParticleEffectInfo * effect_info, float x, float y, float z);
    Uint32 AddEffect (Uint32 effect_id, float x, float y, float z);
    Uint32 AddMovingEffect (Uint32 effect_id, float x, float y, float z, float x2, float y2, float z2);
    
    void RemoveEffect (Uint32 id);
    
    void UpdateParticlePosition(Uint32 id, float newx, float newy, float newz);
};

extern cParticleEngine pParticleEngine;


#endif //_PARTICLEENGINE_H_
