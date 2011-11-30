//
// File: ParticleEngine.cpp
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


#include <cassert>
#include <math.h>
#include "renderer/particles/ParticleEngine.h"
#include "renderer/Camera.h"
#include "include.h"
#include "Logger.h"

using namespace std;

cParticleEngine pParticleEngine;

cParticleEngine::cParticleEngine ()
{
  current_handle = 0;
}

cParticleEngine::~cParticleEngine ()
{
    Reset ();
}

void cParticleEngine::Reset ()
{
  std::map < Uint32, Particle::cParticleHandler * >::iterator iter;
  for (iter = particle_handlers.begin (); iter != particle_handlers.end ();
       iter++)
    delete iter->second;
  particle_handlers.clear ();
  current_handle = 0;
}

void cParticleEngine::Handle ()
{
  Uint32 currentticks = SDL_GetTicks ();
  std::map < Uint32, Particle::cParticleHandler * >::iterator iter;
  for (iter = particle_handlers.begin (); iter != particle_handlers.end ();
       iter++)
    iter->second->Handle (currentticks);
}

void cParticleEngine::Render ()
{
  glDepthMask (GL_FALSE);
  glDisable (GL_CULL_FACE);
  glDisable (GL_ALPHA_TEST);

  int dx = pCamera.GetBlockX () * 8;
  int dy = pCamera.GetBlockY () * 8;

  std::map < Uint32, Particle::cParticleHandler * >::iterator iter;
  for (iter = particle_handlers.begin (); iter != particle_handlers.end ();
       iter++)
    iter->second->Render (dx, dy);

  glEnable (GL_CULL_FACE);
  glEnable (GL_ALPHA_TEST);
  glDepthMask (GL_TRUE);
}

Uint32 cParticleEngine::AddEffect (cStaticModelParticleEffectInfo *
                                   effect_info, float x, float y, float z)
{
  assert (effect_info);
  current_handle++;

//  printf ("[Adding Effect \"%s\" (%d)]\n", effect_info->name ().c_str (), current_handle);


        Particle::cEffectDefinition * effect_definition =
          pParticleLoader.getEffect (effect_info->name ());
        if (effect_definition)
            {

              Particle::cParticleHandler * handler =
                new Particle::cParticleHandler (effect_definition,
                                                x + effect_info->x (),
                                                y + effect_info->y (),
                                                z + effect_info->z ());

              particle_handlers.insert (make_pair (current_handle, handler));

            }
        else
            {
              Logger::WriteLine ("Warning: Effect not found: " + effect_info->name ());
            }

  return current_handle;
}

Uint32 cParticleEngine::AddEffect (Uint32 effect_id, float x, float y, float z)
{
  assert (effect_id);
  current_handle++;

//  printf ("[Adding Effect \"%s\" (%d)]\n", effect_id, current_handle);

        Particle::cEffectDefinition * effect_definition =
          pParticleLoader.getEffect (effect_id);
        if (effect_definition)
            {

              Particle::cParticleHandler * handler =
                new Particle::cParticleHandler (effect_definition,
                                                x,
                                                y,
                                                z);

              particle_handlers.insert (make_pair (current_handle, handler));

            }
        else
            {
              Logger::WriteLine ("Warning: Effect-ID not found: " + effect_id);
            }

  return current_handle;
}

//Does this work correct??
Uint32 cParticleEngine::AddMovingEffect(Uint32 effect_id, float x, float y, float z, float x2, float y2, float z2){
  assert (effect_id);
  current_handle++;
//  printf ("[Adding Effect \"%s\" (%d)]\n", effect_id, current_handle);


        Particle::cEffectDefinition * effect_definition =
          pParticleLoader.getEffect (effect_id);
        if (effect_definition)
            {
                Particle::cParticleHandler * handler =
                new Particle::cParticleHandler (effect_definition,
                                                x,
                                                y,
                                                z);
                handler->setMoving(true);
                handler->setDestination(x2, y2, z2);
                particle_handlers.insert (make_pair (current_handle, handler));
            }
        else
            {
                Logger::WriteLine ("Warning: Effect-ID not found: " + effect_id);
            }

  return current_handle;
}


void cParticleEngine::RemoveEffect (Uint32 id)
{
  printf("[Removing Effect %d]\n", id);
  std::map < Uint32, Particle::cParticleHandler * >::iterator iter;
  iter = particle_handlers.find (id);
  if (iter != particle_handlers.end ())
    iter->second->Stop ();
}

void cParticleEngine::UpdateParticlePosition(Uint32 id, float newx, float newy, float newz)
{
  printf("[Moving Effect %d]\n", id);

  std::map < Uint32, Particle::cParticleHandler * >::iterator iter;
  iter = particle_handlers.find (id);
  if (iter != particle_handlers.end ()){
    iter->second->setX(newx);
    iter->second->setY(newy);
    iter->second->setZ(newz);
    }

}

