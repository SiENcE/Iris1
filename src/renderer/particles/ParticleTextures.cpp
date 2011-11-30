//
// File: ParticleTextures.cpp
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


#include "renderer/particles/ParticleTextures.h"
#include "include.h"
#include "Exception.h"
#include "Logger.h"
#include <cassert>




using namespace std;

namespace Particle
{

  cParticleTextureManager::cParticleTextureManager ()
  {
  }

  cParticleTextureManager::~cParticleTextureManager ()
  {
    std::map < std::string, Texture * >::iterator iter;
    for (iter = textures.begin (); iter != textures.end (); iter++)
      delete iter->second;
    textures.clear ();
  }

  Texture *cParticleTextureManager::loadTexture (std::string filename)
  {
    std::map < std::string, Texture * >::iterator iter;
    iter = textures.find (filename);

    if (iter != textures.end ())    // If cached, return cached texture
      return iter->second;

    string fn = "textures/" + filename;

    Texture *result = new Texture;
    result->LoadFromFile (fn.c_str ());

    textures.insert (make_pair (filename, result));

    return result;
  }

}
