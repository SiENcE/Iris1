//
// File: ParticleLoader.cpp
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


#include "renderer/particles/ParticleLoader.h"
#include "include.h"
#include "stdlib.h"
#include "Exception.h"
#include "Logger.h"
#include <cassert>
#include <math.h>




using namespace std;

cParticleLoader pParticleLoader;


namespace Particle
{

  float rnd (float value)
  {                             // Returns a random float between 0.0 and value
    return ((float) (rand () % 100000) / 100000.0f) * value;
  }

  cRandomValue::cRandomValue ()
  {
    m_variance = 0.0f;
    m_average = 0.0f;
  }

  void cRandomValue::LoadFromXML (XML::Node * node)
  {
    if (!node)
      return;
    m_average = node->findFloat ("average");
    m_variance = node->findFloat ("variance");
  }

  float cRandomValue::value ()  // returns a value between m_average - m_variance and m_average + m_variance
  {
    return m_average + ((float) (rand () % 100000) / 50000.0f -
                        1.0f) * m_variance;
  }


  cVectorCone::cVectorCone ()
  {
    m_angle1 = 0.0f;
    m_angle2 = 0.0f;
    m_coneangle = 0.0f;
  }


  void cVectorCone::LoadFromXML (XML::Node * node)
  {
    if (!node)
      return;
    m_angle1 = node->findFloat ("angle1");
    m_angle2 = node->findFloat ("angle2");
    m_coneangle = node->findFloat ("coneangle");
    m_amount.LoadFromXML (node->findNode ("amount"));
  }

  cVector cVectorCone::CreateVector ()    // Creates a random vector in the specific cone
  {
    float Phi = m_angle1 * 3.14159f / 180.0f;
    float Theta = m_angle2 * 3.14159f / 180.0f;

    float Psi = rnd (360.0f) * 3.14159f / 180.0f;
    float Zeta = (rnd (m_coneangle)) * 3.14159f / 180.0f;

    cVector TempDir (sin (Zeta) * cos (Psi), sin (Zeta) * sin (Psi), cos (Zeta));   // spherical coords

    if (Theta == 0.0f)
      return TempDir;

    cVector axis;
    axis.
      CrossProduct (cVector
                    (sin (Theta) * cos (Phi), sin (Theta) * sin (Phi),
                     cos (Theta)), cVector (0, 0, 1));

    cMatrix matrix;
    matrix.RotationMatrix (axis, Theta);

    cVector result = matrix.Apply (TempDir);
    result.Scale (m_amount.value ());

    return result;
  }


  cParticleTypeDefinition::cParticleTypeDefinition (XML::Node * node)
  {
    assert (node);
    m_tex_x = node->findInteger ("texcoord_x");
    m_tex_y = node->findInteger ("texcoord_y");
    m_width = node->findInteger ("texcoord_w");
    m_height = node->findInteger ("texcoord_h");
  }

  Sint32 cParticleTypeDefinition::tex_x ()
  {
    return m_tex_x;
  }

  Sint32 cParticleTypeDefinition::tex_y ()
  {
    return m_tex_y;
  }


  Sint32 cParticleTypeDefinition::width ()
  {
    return m_width;
  }


  Sint32 cParticleTypeDefinition::height ()
  {
    return m_height;
  }


  cEmission_Rotation::cEmission_Rotation (XML::Node * node)
  {
    assert (node);
    m_angle1 = node->findFloat ("angle1");
    m_angle2 = node->findFloat ("angle2");
  }

  cMatrix cEmission_Rotation::GetMatrix ()      // Returns a rotation matrix
  {
    float Phi = m_angle1 * 3.14159f / 180.0f;
    float Theta = m_angle2 * 3.14159f / 180.0f;

    cMatrix matrix;
    if (Theta == 0.0f)
      return matrix;

    cVector axis;
    axis.
      CrossProduct (cVector
                    (sin (Theta) * cos (Phi), sin (Theta) * sin (Phi),
                     cos (Theta)), cVector (0, 0, 1));

    matrix.RotationMatrix (axis, Theta);

    return matrix;
  }


  cEmission_Translation::cEmission_Translation (XML::Node * node)
  {
    assert (node);
    m_x.LoadFromXML (node);
    m_y.LoadFromXML (node);
    m_z.LoadFromXML (node);
  }

  cMatrix cEmission_Translation::GetMatrix ()   // Returns a rotation matrix
  {

    cMatrix matrix;
    matrix.
      TranslationMatrix (cVector (m_x.value (), m_y.value (), m_z.value ()));
    return matrix;
  }

  cEmission_Circle::cEmission_Circle (XML::Node * node)
  {
    assert (node);
    m_radius = node->findFloat ("radius");
  }

  cMatrix cEmission_Circle::GetMatrix ()  // Returns a rotation matrix
  {
    float R = rnd (m_radius);
    float Phi = rnd (360.0f) * 3.14159f / 180.0f;
    cMatrix matrix;
    matrix.TranslationMatrix (cVector (R * cos (Phi), R * sin (Phi), 0.0f));
    return matrix;
  }

  cEmission_Sphere::cEmission_Sphere (XML::Node * node)
  {
    assert (node);
    m_radius = node->findFloat ("radius");
  }

  cMatrix cEmission_Sphere::GetMatrix ()  // Returns a rotation matrix
  {
    float R = rnd (m_radius);
    float Phi = rnd (360.0f) * 3.14159f / 180.0f;
    float Theta = rnd (180.0f) * 3.14159f / 180.0f;
    cMatrix matrix;
    matrix.
      TranslationMatrix (cVector
                         (R * sin (Theta) * cos (Phi),
                          R * sin (Theta) * sin (Phi), cos (Theta)));
    return matrix;
  }

  cParticleDefinition::cParticleDefinition (XML::Node * node)
  {
    assert (node);
    m_speed.LoadFromXML (node->findNode ("speed"));
    m_acceleration.LoadFromXML (node->findNode ("acceleration"));
    m_lifetime.LoadFromXML (node->findNode ("lifetime"));
    m_appeartime.LoadFromXML (node->findNode ("appeartime"));
    m_size.LoadFromXML (node->findNode ("size"));
    m_type = node->findInteger ("particletype");
    m_amount = node->findInteger ("amount");
    m_fadein = node->findInteger ("fadein");
    m_fadeout = node->findInteger ("fadeout");
    m_transformspeed = node->findInteger ("transformspeed");
    m_transformacceleration = node->findInteger ("transformacceleration");

    XML::Node * subnode;
    int index = 0;
    while ((subnode = node->findNode ("emission", index)))
        {
          cEmissionDefinition *definition = NULL;
          std::string type = subnode->findString ("type");

          if (type == "rotation")
            definition = new cEmission_Rotation (subnode);
          if (type == "translation")
            definition = new cEmission_Translation (subnode);
          if (type == "circle")
            definition = new cEmission_Circle (subnode);
          if (type == "sphere")
            definition = new cEmission_Sphere (subnode);

          if (definition)
            emissions.push_back (definition);
          else
            Logger::WriteLine ("Warning: unknown emission type: " + type);
          index++;
        }
  }

  cParticleDefinition::~cParticleDefinition ()
  {
    for (unsigned int index = 0; index < emissions.size (); index++)
      delete emissions[index];
    emissions.clear ();
  }

  Sint32 cParticleDefinition::amount ()
  {
    return m_amount;
  }

  float cParticleDefinition::lifetime ()
  {
    return m_lifetime.value ();
  }

  float cParticleDefinition::appeartime ()
  {
    return m_appeartime.value ();
  }

  float cParticleDefinition::size ()
  {
    return m_size.value ();
  }

  cVector cParticleDefinition::speed ()
  {
    return m_speed.CreateVector ();
  }

  cVector cParticleDefinition::acceleration ()
  {
    return m_acceleration.CreateVector ();
  }


  Uint32 cParticleDefinition::type ()
  {
    return m_type;
  }

  bool cParticleDefinition::fadein ()
  {
    return m_fadein;
  }

  bool cParticleDefinition::fadeout ()
  {
    return m_fadeout;
  }

  bool cParticleDefinition::transformspeed ()
  {
    return m_transformspeed;
  }

  bool cParticleDefinition::transformacceleration ()
  {
    return m_transformacceleration;
  }

  cMatrix cParticleDefinition::GetTransformationMatrix ()
  {
    cMatrix matrix;
    for (unsigned int index = 0; index < emissions.size (); index++)
        {
          cMatrix tempmatrix = emissions[index]->GetMatrix ();
          matrix.Multiply (tempmatrix);
        }
    return matrix;
  }

  cEffectDefinition::cEffectDefinition (XML::Node * node)
  {
    assert (node);
    m_time = node->findInteger ("time");
    m_appeartype = node->findInteger ("appeartype");
    m_texturename = node->findString ("texture");
    m_id = node->findInteger("effect_id");
    
    XML::Node * subnode;

    int index = 0;
    while ((subnode = node->findNode ("particletype_definition", index)))
        {
          Uint32 id = subnode->findInteger ("id");
          if ((id > 0) && (types.find (id) == types.end ()))
              {
                cParticleTypeDefinition *typedefinition =
                  new cParticleTypeDefinition (subnode);
                types.insert (make_pair (id, typedefinition));
              }
          else
            Logger::WriteLine ("Warning: duplicate definition of particletype");
          index++;
        }

    index = 0;
    while ((subnode = node->findNode ("particle_definition", index)))
        {
          cParticleDefinition *definition = new cParticleDefinition (subnode);
          particles.push_back (definition);
          index++;
        }
  }

  cEffectDefinition::~cEffectDefinition ()
  {
    for (unsigned int index = 0; index < particles.size (); index++)
      delete particles[index];

    std::map < Uint32, cParticleTypeDefinition * >::iterator type_iter;
    for (type_iter = types.begin (); type_iter != types.end (); type_iter++)
      delete type_iter->second;
  }

  Uint32 cEffectDefinition::time ()
  {
    return m_time;
  }

  Uint32 cEffectDefinition::appeartype ()
  {
    return m_appeartype;
  }

  Uint32 cEffectDefinition::particle_count ()
  {
    return particles.size ();
  }

  cParticleDefinition *cEffectDefinition::getParticle (Uint32 index)
  {
    assert (index < particles.size ());
    return particles[index];
  }

  cParticleTypeDefinition *cEffectDefinition::getType (Uint32 id)
  {
    std::map < Uint32, cParticleTypeDefinition * >::iterator iter;
    iter = types.find (id);
    if (iter != types.end ())
      return iter->second;
    else
      return NULL;
  }

  std::string cEffectDefinition::texture_name ()
  {
    return m_texturename;
  }

}


cParticleLoader::cParticleLoader ()
{
  m_texture_manager = NULL;
}

cParticleLoader::~cParticleLoader ()
{
    DeInit ();
}

void cParticleLoader::Init (std::string filename)
{
  DeInit ();
  m_texture_manager = NULL;

  XML::Parser parser;
  XML::Node * collection, *document, *effect_node;

  try
  {
    parser.loadData (filename);
    document = parser.parseDocument ();

    collection = document->findNode ("effect_collection");

    if (!collection)
        {
          //delete document;
          Logger::WriteLine ("could not find effect root node");
          return;
        }
  }


  catch (...)
  {
    Logger::WriteLine (string ("Couldn't parse ") + filename);
    return;
  }

  int index = 0;
  while ((effect_node = collection->findNode ("effect", index)))
      {
        std::string name = effect_node->findString ("name");
        if (effects.find (name) == effects.end ())
            {                   // don't load twice
              Particle::cEffectDefinition * effect =
                new Particle::cEffectDefinition (effect_node);
              effects.insert (make_pair (name, effect));
            }
        else
          Logger::WriteLine (string ("Warning: duplicate definition of effect") +
                      name);
        index++;
      }


  //delete document;

  m_texture_manager = new Particle::cParticleTextureManager;
}


void cParticleLoader::DeInit ()
{
  delete m_texture_manager;
  m_texture_manager = NULL;

  std::map < std::string, Particle::cEffectDefinition * >::iterator iter;
  for (iter = effects.begin (); iter != effects.end (); iter++)
    delete iter->second;
  effects.clear ();
}

Particle::cParticleTextureManager * cParticleLoader::texture_manager ()
{
  if (!m_texture_manager)
    THROWEXCEPTION ("invalid call of cParticleLoader::texture_manager ()");
  return m_texture_manager;
}


Particle::cEffectDefinition * cParticleLoader::getEffect (std::string name)
{
  std::map < std::string, Particle::cEffectDefinition * >::iterator iter;
  iter = effects.find (name);
  if (iter != effects.end ())
    return iter->second;
  else
    return NULL;
}

Particle::cEffectDefinition * cParticleLoader::getEffect(Uint32 effectid)
{
  std::map < std::string, Particle::cEffectDefinition * >::iterator iter;
  for(iter = effects.begin(); iter!=effects.end();iter++)
  if (iter->second->id() == effectid)
    return iter->second;
  //else
    return NULL;
}
