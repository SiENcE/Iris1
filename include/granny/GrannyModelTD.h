//
// File: GrannyModelTD.h
// Created by:   Alexander Oster - tensor@ultima-iris.de
//

// Third Dawn Granny Support

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
 
#ifndef __GRANNYMODELTD_H__
#define __GRANNYMODELTD_H__

#include <string>
#include <map>

#include "GrannyModel.h"
#include "grannyfile.h"

// #include "../Fluid/mmgr.h"

class cGrannyModelTD : public cGrannyModel
{
public:
        cGrannyModelTD (std::string modelfile, std::string basepath, std::string defaultanim, std::string prefix="");
        virtual ~cGrannyModelTD ();
       virtual void Render(int animid, int tick, float & curtime, GrnMatrix * left_matrix, GrnMatrix * right_matrix, cCharacterLight * character_light, float r, float g, float b, float alpha, bool is_corpse);
        virtual int Age(int tick);
        virtual void Free ();
        virtual void AddAnimation(int animid, std::string filename);
        virtual void SetHandBones(int left, int right);
        virtual void SetHand (int hand) { this->hand = hand; }
        virtual void SetAnimset(int anim_set){this->m_animset = anim_set;}
        virtual int animset(){return this->m_animset;}
private:
        int last_tick;
        int m_animset;
        std::string basepath;
        std::map <int, std::string> animation_names;
        std::map <int, cGrannyAnimation *> animations;
        cGrannyFile * model;
        cGrannyAnimation * default_animation;
        std::string defaultanimname;
        std::string m_filename;
        std::string m_prefix;
        int left_hand_bone, right_hand_bone;
        int hand;
};

#endif
