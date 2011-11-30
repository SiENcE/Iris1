//
// File: GrannyModelTD.cpp
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



#include "granny/GrannyModelTD.h"
#include "Logger.h"
#include <cassert>




using namespace std;

string ParseForPrefix (string s, string prefix)
{
  const char *p = s.c_str ();   // Find any $
  while ((*p) && (*p != '$'))
    p++;

  if (*p)
      {
        string result = "";

        for (unsigned int i = 0; i < s.size (); i++)
          if (s[i] == '$')
              {
                result += prefix;
              }
          else
            result += s[i];


        return result;

      }
  else
    return s;
}

cGrannyModelTD::cGrannyModelTD (std::string modelfile, std::string basepath,
                                std::string defaultanim, std::string prefix)
{
  last_tick = 0;
  left_hand_bone = -1;
  right_hand_bone = -1;
  hand = HAND_NONE;
  this->basepath = basepath;
  m_prefix = prefix;
  m_filename = ParseForPrefix (modelfile, prefix);
  defaultanimname = ParseForPrefix (defaultanim, prefix);
  model = NULL;
  m_animset = 0;
}

cGrannyModelTD::~cGrannyModelTD ()
{
  Free ();
}

void cGrannyModelTD::Render (int animid, int tick, float &curtime,
                             GrnMatrix * left_matrix,
                             GrnMatrix * right_matrix,
                             cCharacterLight * character_light, float r,
                             float g, float b, float alpha, bool is_corpse)
{
  if (!model)
      {
        model = new cGrannyFile ();
        model->load (m_filename, basepath);
        //grannyfile::initbone() do search hand bone.
        //model->left_hand_bone = left_hand_bone;
        //model->right_hand_bone = right_hand_bone;

//SiENcE
//printf ("[lefthandbone_in %d]\n", left_hand_bone);
//printf ("[righthandbone_in %d]\n", right_hand_bone);

        default_animation = new cGrannyAnimation ();
        default_animation->load (defaultanimname, basepath);
        default_animation->Assign (model);
      }

  assert (model);

  map < int, cGrannyAnimation * >::iterator anim_iter =animations.find (animid);

  cGrannyAnimation *animation = default_animation;

  if (anim_iter == animations.end ())
      {
        map < int, string >::iterator name_iter =
          animation_names.find (animid);
        if (name_iter != animation_names.end ())
            {
              animation = new cGrannyAnimation ();
              animation->load (name_iter->second, basepath);
              animation->Assign (model);
              animations.insert (make_pair (animid, animation));
            }

      }
  else
      {
        animation = anim_iter->second;
      }

  assert (model);

  last_tick = tick;

  if (hand != HAND_NONE)
    glPushMatrix ();

  if ((hand == HAND_LEFT) && left_matrix)
  {
     glLoadMatrixf (left_matrix->matrix);
//printf ("[hand_left %d]\n", hand);
  }
  if ((hand == HAND_RIGHT) && right_matrix)
  {
    glLoadMatrixf (right_matrix->matrix);
//printf ("[hand_right %d]\n", hand);
  }

  model->Render (animation, curtime, character_light, r, g, b, alpha,
                 is_corpse);

  if (hand != HAND_NONE)
    glPopMatrix ();

  if (hand == HAND_OWNER)
      {
        if (right_matrix)
          *right_matrix = animation->matrix_right_hand;
        if (left_matrix)
          *left_matrix = animation->matrix_left_hand;

//printf ("[hand_in %d]\n", hand);
      }
}

int cGrannyModelTD::Age (int tick)
{
  return tick - last_tick;
}

void cGrannyModelTD::Free()
{
	std::map<int, cGrannyAnimation *>::iterator iter;
	for ( iter = animations.begin(); iter != animations.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	animations.clear();

	SAFE_DELETE( model );
}

void cGrannyModelTD::AddAnimation (int animid, std::string filename)
{
  animation_names.
    insert (make_pair (animid, ParseForPrefix (filename, m_prefix)));
}

void cGrannyModelTD::SetHandBones (int left, int right)
{
  left_hand_bone = left;
  right_hand_bone = right;
}
