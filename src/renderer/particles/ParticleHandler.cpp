//
// File: ParticleHandler.cpp
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
#include "renderer/particles/ParticleHandler.h"
#include "include.h"
#include "Config.h"
#include "Exception.h"
#include "Geometry.h"
#include "irisgl.h"

using namespace std;

namespace Particle
{
  cParticle::cParticle (float x, float y, float z, float lifetime,
                        float appeartime)
  {
    vec_pos[0] = x;
    vec_pos[1] = y;
    vec_pos[2] = z;
    m_lifetime = lifetime;
    m_time = lifetime + appeartime;
    for (int i = 0; i < 3; i++)
        {
          vec_speed[i] = 0.0f;
          vec_acc[i] = 0.0f;
        }
    vec_acc[2] = -98.1f;        // Gravity
    texcoords[0][0] = 0.0f;
    texcoords[0][1] = 0.0f;
    texcoords[1][0] = 1.0f;
    texcoords[1][1] = 1.0f;
    m_fadeout = true;
    m_size = 1.0f;
  }


  cParticle::~cParticle ()
  {
  }


  void cParticle::Handle (float time_fac)
  {
    if ((m_time > 0.0f) && (m_time < m_lifetime))
        {
          int i;
          for (i = 0; i < 3; i++)
            vec_pos[i] += vec_speed[i] * time_fac;
          for (i = 0; i < 3; i++)
            vec_speed[i] += vec_acc[i] * time_fac;
        }
    m_time -= time_fac;
  }


  void cParticle::Draw (float vec1[3], float vec2[3])
  {
    if ((m_time < 0.0f) || (m_time > m_lifetime))
      return;

    float vec[4][3];
    for (int i = 0; i < 3; i++)
        {
          vec[0][i] = vec_pos[i] - 0.5f * (vec1[i] + vec2[i]) * m_size;
          vec[1][i] = vec[0][i] + vec1[i] * m_size;
          vec[2][i] = vec[1][i] + vec2[i] * m_size;
          vec[3][i] = vec[0][i] + vec2[i] * m_size;
        }
    if (m_fadeout)
      glColor4f (1.0f, 1.0f, 1.0f, m_time * m_time / m_lifetime / m_lifetime);
    else
      glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
    glTexCoord2f (texcoords[0][0], texcoords[0][1]);
    glVertex3fv (vec[0]);
    glTexCoord2f (texcoords[1][0], texcoords[0][1]);
    glVertex3fv (vec[1]);
    glTexCoord2f (texcoords[1][0], texcoords[1][1]);
    glVertex3fv (vec[2]);
    glTexCoord2f (texcoords[0][0], texcoords[1][1]);
    glVertex3fv (vec[3]);
  }

  void cParticle::Accelerate (cVector vector)
  {
    vec_speed[0] += vector.x ();
    vec_speed[1] += vector.y ();
    vec_speed[2] += vector.z ();
  }


  void cParticle::SetTexCoords (float x, float y, float w, float h)
  {
    texcoords[0][0] = x;
    texcoords[0][1] = y;
    texcoords[1][0] = x + w;
    texcoords[1][1] = y + h;
  }

  void cParticle::SetAcceleration (cVector vector)
  {
    vec_acc[0] = vector.x ();
    vec_acc[1] = vector.y ();
    vec_acc[2] = vector.z ();
  }

  void cParticle::setFadeOut (bool fadeout)
  {
    m_fadeout = fadeout;
  }

  void cParticle::setSize (float size)
  {
    m_size = size;
  }

  float cParticle::time ()
  {
    return m_time;
  }



  cParticleHandler::cParticleHandler (cEffectDefinition * definition, float x,
                                      float y, float z)
  {
    m_moving = false;
    dest_x = 0.0f;
    dest_y = 0.0f;
    dest_z = 0.0f;

    m_texture = NULL;
    m_texturewidth = 256;
    m_textureheight = 256;

    assert (definition);
    m_definition = definition;
    m_x = x;
    m_y = y;
    m_z = z;

    m_texture = pParticleLoader.texture_manager ()->loadTexture (m_definition->
                                                                texture_name());
    ASSERT (m_texture);
    m_texturewidth = m_texture->GetWidth ();
    m_textureheight = m_texture->GetHeight ();

    if (m_texturewidth < 32)
        m_texturewidth = 32;
    if (m_textureheight < 32)
        m_textureheight = 32;

    Start ();
  }

  cParticleHandler::~cParticleHandler ()
  {
    ClearParticles ();
  }

  void cParticleHandler::Start ()
  {
    m_starttime = SDL_GetTicks ();
    m_nextemission = m_starttime + m_definition->time ();
    m_last_time = m_starttime;
    m_active = true;
  }

  void cParticleHandler::Stop ()
  {
    m_active = false;
  }

  bool cParticleHandler::active ()
  {
    return m_active;
  }

  void cParticleHandler::Handle (Uint32 currenttick)
  {

    float time_fac = (float) (currenttick - m_last_time) * 0.001f;
    if (time_fac <= 0.0f)
      time_fac = 0.001f;
    if (time_fac >= 1.0f)
      time_fac = 1.0f;
    m_last_time = currenttick;

    //only for moving particles (artix)
    if(m_moving)
    {
     if(m_x < dest_x)
      m_x+=0.5f;
     if(m_x > dest_x)
      m_x-=0.5f;
     if(m_y < dest_y)
      m_y+=0.5f;
     if(m_y > dest_y)
      m_y-=0.5f;
     if(m_z < dest_z)
      m_z+=0.5f;
     if(m_z > dest_z)
      m_z-=0.5f;
    }

    if(m_x == dest_x && m_y==dest_y && m_z == dest_z)
     Stop();

    std::list < cParticle * >::iterator iter;   // Handle particles
	// There _IS_ a problem with this.
	// TODO: Fix this (Crash bug).
	for ( iter = particles.begin(); iter != particles.end(); iter++ )
	{
		(*iter)->Handle (time_fac);
		if ( (*iter)->time() < 0.0f )
		{
			std::list<cParticle *>::iterator nextiter = iter;
			nextiter++;
			if ( nextiter != particles.end() )
			{
				SAFE_DELETE( *iter );
				particles.erase( iter );
				iter = nextiter;	// Probably here
			}
		}
	}


    if ((m_nextemission < currenttick) && m_active)
        {                       // Do Emissions
          unsigned int particle_count = m_definition->particle_count ();
          for (unsigned int particle_index = 0;
               particle_index < particle_count; particle_index++)
              {
                cParticleDefinition *particle_definition =
                  m_definition->getParticle (particle_index);

                int amount = particle_definition->amount ();
                for (int index = 0; index < amount; index++)
                    {
                      cMatrix matrix =
                        particle_definition->GetTransformationMatrix ();

                      cVector position =
                        matrix.Apply (cVector (0.0f, 0.0f, 0.0f));

                      cParticle *particle =
                        new cParticle (position.x (), position.y (),
                                       position.z (),
                                       particle_definition->lifetime (),
                                       particle_definition->appeartime () *
                                       0.001f);

                      cVector speed = particle_definition->speed ();
                      if (particle_definition->transformspeed ())
                        speed = matrix.ApplyRotation (speed);
                      particle->Accelerate (speed);

                      cVector acceleration =
                        particle_definition->acceleration ();
                      if (particle_definition->transformacceleration ())
                        acceleration = matrix.ApplyRotation (acceleration);
                      particle->SetAcceleration (acceleration);

                      particle->setSize (particle_definition->size () *
                                         0.01f);

                      cParticleTypeDefinition *type_definition =
                        m_definition->getType (particle_definition->type ());
                      if (type_definition)
                        particle->SetTexCoords ((float) type_definition->
                                                tex_x () /
                                                (float) m_texturewidth,
                                                (float) type_definition->
                                                tex_y () /
                                                (float) m_textureheight,
                                                (float) type_definition->
                                                width () /
                                                (float) m_texturewidth,
                                                (float) type_definition->
                                                height () /
                                                (float) m_textureheight);

                      AddParticle (particle);
                    }

              }
          if (m_definition->appeartype () == 0) // If Effect is Once then stop
            Stop ();
          else
            m_nextemission += m_definition->time ();  // otherwise schedule next emission
        }
  }

  void cParticleHandler::ClearParticles ()
  {
    std::list < cParticle * >::iterator iter;
    for (iter = particles.begin (); iter != particles.end (); iter++)
      delete *iter;
    particles.clear ();
  }

  void cParticleHandler::AddParticle (cParticle * particle)
  {
    particles.push_back (particle);
  }

  void cParticleHandler::Render (int dx, int dy)
  {
    glPushMatrix ();

    glTranslatef (m_x - (float) dx, m_y - (float) dy, m_z);

    float matrix[16];
    glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
    InvertMatrix (matrix, matrix);


    float vec[3][3], vec2[3][3], dvec[2][3];
    vec[0][0] = 0.0f;
    vec[0][1] = 0.0f;
    vec[0][2] = 0.0f;
    vec[1][0] = 0.15f;
    vec[1][1] = 0.0f;
    vec[1][2] = 0.0f;
    vec[2][0] = 0.0f;
    vec[2][1] = 0.15f;
    vec[2][2] = 0.0f;

    for (int i = 0; i < 3; i++)
      Vec3TransformCoord (vec[i], matrix, vec2[i]);
    for (int i = 0; i < 3; i++)
        {
          dvec[0][i] = vec2[1][i] - vec2[0][i];
          dvec[1][i] = vec2[2][i] - vec2[0][i];
        }

    if (m_texture)
      glBindTexture (GL_TEXTURE_2D, m_texture->GetGLTex ());

    glBegin (GL_QUADS);
    std::list < cParticle * >::reverse_iterator rev_iter;
    for (rev_iter = particles.rbegin (); rev_iter != particles.rend ();
         rev_iter++)
      (*rev_iter)->Draw (dvec[0], dvec[1]);
    glEnd ();

    glPopMatrix ();
  }

void cParticleHandler::setX(float x)
{
  m_x = x;
}

void cParticleHandler::setY(float y)
{
  m_y = y;
}

void cParticleHandler::setZ(float z)
{
  m_z = z;
}

void cParticleHandler::setMoving(bool mov)
{
 m_moving = mov;
}

void cParticleHandler::setDestination(float d_x, float d_y, float d_z)
{
 dest_x = d_x;
 dest_y = d_y;
 dest_z = d_z;
}

}
