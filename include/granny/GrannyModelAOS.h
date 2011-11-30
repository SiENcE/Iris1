//
// File: GrannyModelAOS.h
// Created by:   Alexander Oster - tensor@ultima-iris.de
//

// Age of Shadows Granny Support

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
 
#ifndef __GRANNYMODELAOS_H__
#define __GRANNYMODELAOS_H__

#include <string>
#include <map>

#include "GrannyModel.h"
#include "GrannyModelTD.h"

// #include "../Fluid/mmgr.h"

class cGrannyModelAOS : public cGrannyModel
{
public:
        cGrannyModelAOS (std::string basepath, std::string defaultanim);
        ~cGrannyModelAOS();
           virtual void Render(int animid, int tick, float & curtime, GrnMatrix * left_matrix, GrnMatrix * right_matrix, cCharacterLight * character_light, float r, float g, float b, float alpha, bool is_corpse);
        virtual int Age(int tick);
        virtual void Free ();
        virtual void AddAnimation(int animid, std::string filename);
        virtual void SetHandBones(int left, int right);
        virtual void SetHand (int hand) { this->hand = hand; }
        void AddModel(int id, std::string filename);
        virtual void SetAnimset(int anim_set){this->m_animset = anim_set;}
        virtual int animset(){return this->m_animset;}
        void setBodyParts(std::vector<int> b_parts) {bodyparts = b_parts;}
private:
        int last_tick;
        int m_animset;
        std::string basepath;
        std::string defaultanimname;
        std::map <int, cGrannyModelTD *> models;
        std::vector<int> bodyparts;
        int left_hand_bone, right_hand_bone;
        int hand;
};

#endif
