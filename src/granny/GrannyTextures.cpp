//
// File: GrannyTextures.cpp
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


#include "granny/GrannyTextures.h"
#include "Logger.h"
#include <cassert>




using namespace std;

cGrannyTextureLoader *pGrannyTextureLoader = NULL;

void LowerCase (std::string & s)
{
  for (unsigned int i = 0; i < s.size (); i++)
    if ((s[i] >= 'A') && (s[i] <= 'Z'))
      s[i] = s[i] + 32;
}

cGrannyTextureEntry::cGrannyTextureEntry (Texture * texture)
{
  assert (texture);
  m_texture = texture;
  m_ref_count = 0;
}


cGrannyTextureEntry::~cGrannyTextureEntry ()
{
  if (m_ref_count > 1)
      {
		  Logger::WriteLine("Freeing referenced Granny Texture (probably causes crash!!)",
               __FILE__, __LINE__, LEVEL_WARNING);
      }

  delete m_texture;
  m_texture = NULL;
}

int cGrannyTextureEntry::ref_count ()
{
  return m_ref_count;
}

void cGrannyTextureEntry::inc_ref_count ()
{
	//m_ref_count --;
	m_ref_count++;
}

void cGrannyTextureEntry::dec_ref_count ()
{
  //m_ref_count ++;
  m_ref_count--;
  if (m_ref_count < 0)
      {
        Logger::WriteLine ("Granny Texture RefCount < 0", __FILE__, __LINE__,
                    LEVEL_WARNING);
      }
}

Texture *cGrannyTextureEntry::texture ()
{
  return m_texture;
}

cGrannyTextureLoader::cGrannyTextureLoader (std::string texturepath)
{
  m_texturepath = texturepath;
}

cGrannyTextureLoader::~cGrannyTextureLoader ()
{
  std::map < std::string, cGrannyTextureEntry * >::iterator iter;
  for (iter = textures.begin (); iter != textures.end (); iter++)
    delete iter->second;
  textures.clear ();
}

Texture *cGrannyTextureLoader::LoadTexture (std::string filename)
{
  std::map < std::string, cGrannyTextureEntry * >::iterator iter;
  LowerCase (filename);
  iter = textures.find (filename);

  if (iter != textures.end ())
      {
        assert (iter->second);
        iter->second->inc_ref_count ();
        return iter->second->texture ();
      }
  else
      {
        string fn = m_texturepath + filename;

        // Try to load our Texture
        Texture *texture = new Texture;
        if (!texture->LoadFromFile (fn.c_str ()))
            {
              delete texture;
              return NULL;
            }

        cGrannyTextureEntry *entry = new cGrannyTextureEntry (texture);
        entry->inc_ref_count ();

        textures.insert (make_pair (filename, entry));

        return texture;
      }
}

void cGrannyTextureLoader::FreeTexture (std::string filename)
{
  std::map < std::string, cGrannyTextureEntry * >::iterator iter;
  LowerCase (filename);
  iter = textures.find (filename);

  if (iter != textures.end ())
      {
        assert (iter->second);
        iter->second->dec_ref_count ();
        if (!iter->second->ref_count ())
            {
              delete iter->second;
              textures.erase (filename);
            }
      }
  else
      {
/*     Logger::WriteLine(filename);
     Logger::WriteLine("Unknown Granny Texture Reference", __FILE__, __LINE__,
         LEVEL_WARNING); */
      }
}
