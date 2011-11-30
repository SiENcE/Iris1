//
// File: ParticleHandler.h
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

#ifndef _PARTICLEHANDLER_H_
#define _PARTICLEHANDLER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <list>
#include "renderer/Texture.h"
#include "renderer/particles/ParticleLoader.h"
#include "Geometry.h"

// #include "../Fluid/mmgr.h"

namespace Particle
{

class cParticle
{
  private:
    float vec_pos[3];
    float vec_speed[3];
    float vec_acc[3];
    float m_lifetime;
    float texcoords[2][2];
    float m_time;
    float m_size;
    bool m_fadeout;
  public:
    cParticle (float x, float y, float z, float lifetime, float appeartime);
    virtual ~cParticle ();
    virtual void Handle (float time_fac);
    virtual void Draw(float vec1[3], float vec2[3]);
    virtual void Accelerate(cVector vector);
    virtual void SetAcceleration(cVector vector);
    void SetTexCoords(float x, float y, float w, float h);
    void setFadeOut (bool fadeout);
    void setSize (float size);
    float time ();
};


class cParticleHandler
{
  private:
    std::list<cParticle *> particles;
    Texture * m_texture;
  
    cVector m_position;
    bool m_active;
    Uint32 m_starttime;
    Uint32 m_nextemission;
    Uint32 m_last_time;
    cEffectDefinition * m_definition;
    
    float m_x, m_y, m_z;
    int m_texturewidth, m_textureheight;
  
    void AddParticle(cParticle * particle);
    void ClearParticles();
    
    bool m_moving;
    float dest_x, dest_y, dest_z;
  
  public:
    cParticleHandler (cEffectDefinition * definition, float x, float y, float z);
    ~cParticleHandler ();
    void Handle (Uint32 currenttick);
    
    void Start ();
    void Stop ();
    
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setMoving(bool mov);
    void setDestination(float d_x, float d_y, float d_z);
    
    void Render (int dx, int dy);
    
    bool active ();
};

}

#endif //_PARTICLEHANDLER_H_
