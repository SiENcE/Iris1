//
// File: ParticleLoader.h
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

#ifndef _PARTICLELOADER_H_
#define _PARTICLELOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <map>
#include <vector>
#include <string>

#include "renderer/particles/ParticleTextures.h"
#include "renderer/Texture.h"
#include "Geometry.h"
#include "xml.h"

// #include "../Fluid/mmgr.h"

namespace Particle
{

    // a linear random value
    class cRandomValue 
    {
        private:
            float m_average, m_variance;
        public:
            cRandomValue ();
            void LoadFromXML (XML::Node * node);
            float value ();  // returns a value between m_average - m_variance and m_average + m_variance
    };
    

    // Handles a "random" direction (a cone)
    class cVectorCone
    {
        private:
            float m_angle1, m_angle2, m_coneangle;
            cRandomValue m_amount;
        public:
            cVectorCone ();
            void LoadFromXML (XML::Node * node);
            cVector CreateVector (); // Creates a random vector in the specific cone
    };
    

    class cParticleTypeDefinition
    {
        private:
            Sint32 m_tex_x, m_tex_y, m_width, m_height; // texture coords
        public:
            cParticleTypeDefinition (XML::Node * node);
            Sint32 tex_x ();
            Sint32 tex_y ();
            Sint32 width ();
            Sint32 height ();
    };
    
    class cEmissionDefinition // Abstract base class for transformations
    {
        public:
             virtual cMatrix GetMatrix () = 0;  // Returns a 4x4 transformation matrix
    };
    
    class cEmission_Rotation : public cEmissionDefinition // a simple rotation
    {
        private:
            float m_angle1, m_angle2;
        public:
            cEmission_Rotation (XML::Node * node);
            virtual cMatrix GetMatrix ();  // Returns a rotation matrix
    };
   
    class cEmission_Translation : public cEmissionDefinition  // a simple translation
    {
        private:
            cRandomValue m_x, m_y, m_z;
        public:
            cEmission_Translation (XML::Node * node);
            virtual cMatrix GetMatrix ();  // Returns a translation matrix
    };
    
    class cEmission_Circle : public cEmissionDefinition  // translations, so that lots of particles do a "circle"
    {
        private:
            float m_radius;
        public:
            cEmission_Circle (XML::Node * node);
            virtual cMatrix GetMatrix ();  // Returns a translation matrix
    };
    
    class cEmission_Sphere : public cEmissionDefinition  // translations, so that lots of particles do a "sphere"
    {
        private:
            float m_radius;
        public:
            cEmission_Sphere (XML::Node * node);
            virtual cMatrix GetMatrix ();  // Returns a translation matrix
    };
    
    
    class cParticleDefinition
    {
        private:
            cVectorCone m_speed;
            cVectorCone m_acceleration;
            cRandomValue m_lifetime;
            cRandomValue m_appeartime;
            cRandomValue m_size;
            Uint32 m_type;
            Sint32 m_amount;
            bool m_fadein;
            bool m_fadeout;
            bool m_transformspeed;
            bool m_transformacceleration;
            std::vector <cEmissionDefinition *> emissions;            
        public:
            cParticleDefinition (XML::Node * node);
            ~cParticleDefinition ();
            
            Sint32 amount ();
            float lifetime ();
            float appeartime ();
            float size ();
            cVector speed ();
            cVector acceleration ();
            Uint32 type ();
            bool fadein ();
            bool fadeout ();
            bool transformspeed ();
            bool transformacceleration ();
            
            cMatrix GetTransformationMatrix ();
    };
    
    
    class cEffectDefinition
    {
        private:
            Uint32 m_time;
            Uint32 m_appeartype;
            std::vector <cParticleDefinition *> particles;
            std::map <Uint32, cParticleTypeDefinition *> types;
            std::string m_texturename;
            Uint32 m_id;
        public:
           cEffectDefinition (XML::Node * node); 
           ~cEffectDefinition ();
           
           Uint32 time ();
           Uint32 appeartype ();
           
           Uint32 particle_count ();
           cParticleDefinition * getParticle (Uint32 index);
           cParticleTypeDefinition * getType (Uint32 id);
           
           std::string texture_name ();
           Uint32 id(){return m_id;}
    };
}
    
class cParticleLoader
{
  private:
     std::map <std::string, Particle::cEffectDefinition *> effects;
     Particle::cParticleTextureManager * m_texture_manager;
  public:
    cParticleLoader ();
    ~cParticleLoader ();

    void Init (std::string filename);
    void DeInit ();
    
    Particle::cEffectDefinition * getEffect (std::string name);
    Particle::cEffectDefinition * getEffect (Uint32 effectid);
    Particle::cParticleTextureManager * texture_manager ();
};

extern cParticleLoader pParticleLoader;


#endif //_PARTICLELOADER_H_
