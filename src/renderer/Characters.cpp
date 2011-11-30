//
// File: Characters.cpp
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


#include "renderer/Characters.h"
#include "include.h"
#include "Config.h"
#include "math.h"
#include "Geometry.h"
#include "renderer/Camera.h"
#include "irisgl.h"

#include <cassert>
#include "renderer/3D/Light3D.h"
#include "granny/GrannyLoader.h"
#include "loaders/HueLoader.h"

#include "renderer/particles/ParticleEngine.h"




using namespace std;

cCharacterList pCharacterList;

float Char_Outbox[12][3][3] = {
	{{0, 0, 0}, {1, 0, 0}, {1, 1, 0}},      // Bottom
	{{1, 1, 0}, {0, 1, 0}, {0, 0, 0}},
	{{0, 0, 1}, {1, 0, 1}, {1, 1, 1}},      // Top
	{{1, 1, 1}, {0, 1, 1}, {0, 0, 1}},
	{{0, 0, 0}, {1, 0, 0}, {1, 0, 1}},      // Front
	{{1, 0, 1}, {0, 0, 1}, {0, 0, 0}},
	{{0, 1, 0}, {1, 1, 0}, {1, 1, 1}},      // Back
	{{1, 1, 1}, {0, 1, 1}, {0, 1, 0}},
	{{0, 0, 0}, {0, 1, 0}, {0, 1, 1}},      // Left
	{{0, 1, 1}, {0, 0, 1}, {0, 0, 0}},
	{{1, 0, 0}, {1, 1, 0}, {1, 1, 1}},      // Right
	{{1, 1, 1}, {1, 0, 1}, {1, 0, 0}}
};

cCharacter::cCharacter ()
{
	m_id = 0;
	m_forceanim = -1;
	m_forcerepeat = -1;
	m_angle = 0.0f;
	m_body = 0;
	m_hue = 0;
	m_flag = 0;
	m_direction = 0;
	m_lastanim = 0;
	m_moving = false;

	m_name_change = false;
	m_sex = 0;
	m_gold = 0;
	m_armor = 0;
	m_weight = 0;

	m_strength = 0;
	m_dexterity = 0;
	m_intelligence = 0;

	m_hits_current = 0;
	m_hits_max = 0;
	m_stamina_current = 0;
	m_stamina_max = 0;
	m_mana_current = 0;
	m_mana_max = 0;

	m_statcap = 0;
	m_petscurrent = 0;
	m_petsmax = 0;

	m_fireresist = 0;
	m_coldresist = 0;
	m_poisonresist = 0;
	m_energyresist = 0;
	m_luck = 0;
	m_damagemax = 0;
	m_damagemin = 0;
	m_tithings = 0;

  particle_handler=0;

	m_corpse = false;

	setPosition (0, 0, 0);
	m_animtime = 0.0f;

	anim_smooth_time = 0;
	last_animtype = 0;

	m_warmode = false;


  m_light = new cCharacterLight (1, 1, 1);      // default character size

}

cCharacter::~cCharacter ()
{
	ClearEquip ();
	ClearSkills ();
	ClearText ();
	if (m_light)
		delete m_light;
}

void cCharacter::ClearEquip (void)
{
	equiplist_t::iterator iter;
	for (iter = equiplist.begin (); iter != equiplist.end (); iter++)
		delete iter->second;
	equiplist.clear ();
}

cCharacterEquip *cCharacter::AddEquip (Uint32 layer)
{
	equiplist_t::iterator iter;
	iter = equiplist.find (layer);

	if (iter != equiplist.end ())
		return iter->second;

	cCharacterEquip *result = new cCharacterEquip;
	equiplist.insert (std::make_pair (layer, result));
	result->setLayer (layer);
	return result;
}

void cCharacter::DelEquip (Uint32 layer)
{
	cCharacterEquip *equip = GetEquip (layer);
	if (equip)
	{
		delete equip;
		equiplist.erase (layer);
	}
}

cCharacterEquip *cCharacter::GetEquip (unsigned int layer)
{
	equiplist_t::iterator iter;
	iter = equiplist.find (layer);

	if (iter != equiplist.end ())
		return iter->second;
	return NULL;
}

cCharacterText::cCharacterText (std::string text, Uint32 timeout)
{
	m_text = text;
	m_timeout = timeout;
	m_done = false;
	m_starttime = SDL_GetTicks ();
	element = new cTextElement (text.c_str ());
	m_hue = Config::GetSpeechHue();
}

cCharacterText::~cCharacterText ()
{
	delete element;

}

void cCharacterText::Draw (int x, int y)
{
	int delta_time = SDL_GetTicks () - m_starttime;
	if (m_timeout < 1)
		m_timeout = 1;

	if ((int) delta_time > (int) m_timeout)
	{
		m_done = true;
		return;
	}

	float alpha = 1.0f;
	float r = 1.0f, g = 1.0f, b = 1.0f;
	if (m_hue != 100)
		pHueLoader.GetRGBHue (m_hue, r, g, b);

	if ((int) delta_time > (int) m_timeout / 2)
		alpha = 1.0f - (float) (delta_time * 2 - m_timeout) / (float) m_timeout;
	glColor4f (r, g, b, alpha);
	if (alpha < 1.0f)
		glColor4f (r, g, b, alpha);
	element->draw (x - element->width () / 2, y);
	if (alpha < 1.0f)
		glColor4f (r, g, b, 1.0f);
}

Uint32 cCharacterText::timeout ()
{
	return m_timeout;
}

bool cCharacterText::done ()
{
	return m_done;
}

cCharacterLight *cCharacter::light ()
{
	return m_light;
}

void cCharacter::RenderText ()
{
	int view_x = pCamera.GetBlockX () * 8;
	int view_y = pCamera.GetBlockY () * 8;

	int x, y;
	float position[3];
	position[0] = this->fx () - (float) view_x + 0.5f;
	position[1] = this->fy () - (float) view_y + 0.5f;
	position[2] = this->fz () * 0.1f + 2.0f;
	pCamera.GetRenderCoords (position, x, y);

	std::list < class cCharacterText * >::reverse_iterator rev_iter;
	for (rev_iter = text_list.rbegin (); rev_iter != text_list.rend ();
		rev_iter++)
	{
		y -= (*rev_iter)->height ();
		(*rev_iter)->Draw (x, y);
	}

	std::list < class cCharacterText * >::iterator iter;
	for (iter = text_list.begin (); iter != text_list.end (); iter++)
		if ((*iter)->done ())
		{
			delete *iter;
			text_list.erase (iter);
			iter = text_list.begin ();
			// HARKON : fixed bug
			if (iter == text_list.end()) break;
		}
}

void cCharacter::MoveTo (float x, float y, float z)
{
	m_destx = x;
	m_desty = y;
	m_destz = z;

//	m_moving = false;
// SiENcE: quick fix to get walking working again
    m_moving = -1;
}

void cCharacter::Handle (float time_factor)
{

	int delay = 100;
	bool two_handed = false;
	bool armed = true;

    if(particle_handler) {
        pParticleEngine.UpdateParticlePosition(particle_handler, (float)m_x * 1.0f, (float)m_y * 1.0f, (float)m_z / 10.0f);
    }

	int new_animtype = 4;         // stand
	bool mounted;
	cCharacterEquip *mount = GetEquip (25);
	cMount *themount = NULL;
	if (mount == NULL)
		mounted = false;
	//std::cout << "NO MOUNT" << endl;
	else
	{                         //std::cout << "MOUNTED" << endl;
		mounted = true;
		themount = new cMount (mount->model ());
	}



	if (m_warmode)
	{

		if (!two_handed)
			new_animtype = 5;     // standforonehandedattack;
		else
			new_animtype = 7;     // standfortwohandedattack;
	}

	if (mounted)
	{
		//std::cout << "MOUNTED" << endl;
		new_animtype = themount->GetMountAnimation (24);
	}



	if (m_moving)
	{
		int sound = 300;
		if (!m_warmode)
		{
			if ((m_direction & 0x80))
			{             // if running
				//sound = 300;
				if (!armed)
					new_animtype = 2;   // rununarmed
				else
					new_animtype = 3;   // runarmed

			}
			else
			{
				if (!armed)
					new_animtype = 0;   // walkunarmed
				else
					new_animtype = 1;   // walkarmed
			}

		}
		else
		{
			if (!two_handed)
				new_animtype = 6;   //walkinattackpositiononehanded
			else
				new_animtype = 8;   //walkinattackpositiontwohanded
		}
		if (mounted)
		{
			//std::cout << "MOUNTED" << endl;
			sound = 298;
			new_animtype = themount->GetMountAnimation (25);
		}
		//std::cout << "Anim: " << new_animtype << endl;
		//    pSoundMix->PlaySound(sound, 200, 0, 0, 0, 0);
		delay = 0;
	}


	if (m_forceanim >= 0)
		new_animtype = m_forceanim;
	/*
	if(mounted){
	//std::cout << "MOUNTED" << endl;
	new_animtype = 25;
	}
	*/
	if (m_animtype != new_animtype)
	{
		last_animtype = m_animtype;
		m_animtype = new_animtype;
		anim_smooth_time = SDL_GetTicks () + delay;
		//    m_animtype = 25;
	}

	time_factor *= 0.75f;
	float dest_angle = (m_direction & 7) * 45.0f;
	if (m_angle != dest_angle)
	{
		float delta_angle = m_angle - dest_angle;
		if (delta_angle >= 180.0f)
			delta_angle -= 360.0f;
		if (delta_angle < -180.0f)
			delta_angle += 360.0f;
		if (fabs (delta_angle) < time_factor * 500.0f)
		{
			m_angle = dest_angle;
		}
		else
		{
			if (delta_angle > 0.0f)
				m_angle -= time_factor * 500.0f;
			else
				m_angle += time_factor * 500.0f;
			if (m_angle >= 360.0f)
				m_angle -= 360.0f;
			if (m_angle < 0.0f)
				m_angle += 360.0f;
		}
	}


	if (!time_factor)
		time_factor = 0.01f;

	//time_factor *= 1.5f;

	if (!m_moving)
		return;

	float dx = m_destx - m_x;
	float dy = m_desty - m_y;
	float dz = (m_destz - m_z) * 0.1f;

	float distance = sqrt (dx * dx + dy * dy + dz * dz);

	if ((distance < time_factor * 5.0f) || (distance > 5.0f))
	{
		setPosition (m_destx, m_desty, m_destz);
		m_moving = 0;
	}
	else
	{
		dx /= distance;
		dy /= distance;
		dz /= distance;

		m_x += dx * time_factor * 5.0;
		m_y += dy * time_factor * 5.0;
		m_z += dz * time_factor * 50.0;
	}
	/* TODO (#1#): Temporary fix for a bad memory leak...need to find a
	better solution in the future */
	if(themount)
		delete themount;
}

sSkillEntry *cCharacter::skill (Uint16 id)
{
	skilllist_t::iterator iter;
	iter = m_skills.find (id);
	if (iter != m_skills.end ())
		return iter->second;

	sSkillEntry *result = new sSkillEntry;
	memset (result, 0, sizeof (struct sSkillEntry));
	m_skills.insert (make_pair (id, result));
	return result;

}

void cCharacter::ClearSkills ()
{
	skilllist_t::iterator iter;
	for (iter = m_skills.begin (); iter != m_skills.end (); iter++)
		delete iter->second;
	m_skills.clear ();

}

void cCharacter::ClearText (void)
{
	std::list < class cCharacterText * >::iterator iter;
	for (iter = text_list.begin (); iter != text_list.end (); iter++)
		delete *iter;
	text_list.clear ();
}

void cCharacter::AddText (std::string text, int timeout, int hue)
{
	cCharacterText *entry = new cCharacterText (text, timeout);
	entry->setHue (hue);

	text_list.push_back (entry);
}

cCharacterList::cCharacterList ()
{
	callback_OnAdd = NULL;
	callback_OnDelete = NULL;
}


cCharacterList::~cCharacterList ()
{
	Clear ();
}


void cCharacterList::Clear ()
{
	characterlist_t::iterator iter;
	for (iter = characterlist.begin (); iter != characterlist.end (); iter++)
		delete iter->second;
	characterlist.clear ();
}


cCharacter *cCharacterList::Add (Uint32 id, int x, int y, int z, int model)
{
	characterlist_t::iterator iter;
	iter = characterlist.find (id);

	if (iter != characterlist.end ())
		return iter->second;

	cCharacter *result = new cCharacter;
	characterlist.insert (std::make_pair (id, result));
	result->setID (id);
	result->setX (x);
	result->setY (y);
	result->setZ (z);
	result->setBody (model);

	if (callback_OnAdd)
		callback_OnAdd (result);

	return result;
}


cCharacter *cCharacterList::Get (unsigned int id)
{
	characterlist_t::iterator iter;
	iter = characterlist.find (id);

	if (iter != characterlist.end ())
		return iter->second;
	else
		return NULL;
}

void cCharacterList::Handle (float time_factor)
{
	characterlist_t::iterator iter;
	for (iter = characterlist.begin (); iter != characterlist.end (); iter++)
		iter->second->Handle (time_factor);
}

void cCharacterList::Delete (unsigned int id)
{
	cCharacter *character = Get (id);
	if (character)
	{
		if (callback_OnDelete)
			callback_OnDelete (character);
		delete character;
		characterlist.erase (id);
	}
}


bool cCharacter::CheckRay (float vecOrigin[3], float vecDir[3], float deltax,
						   float deltay, float deltaz, float &lambda)
{

	float vecs[3][3];
	float act_lambda;
	bool result = false;
	lambda = 100000000.0f;
	for (int index = 0; index < 12; index++)
	{
		for (int j = 0; j < 3; j++)
		{
			vecs[j][0] =
				Char_Outbox[index][j][0] * 0.33f + 0.33f + (float) m_x +
				deltax;
			vecs[j][1] =
				Char_Outbox[index][j][1] * 0.33f + 0.33f + (float) m_y +
				deltay;
			vecs[j][2] =
				(Char_Outbox[index][j][2] * 15.0f + (float) m_z +
				deltaz) * 0.1f;
		}
		if (IntersectTriangleWithRay
			(vecOrigin, vecDir, vecs[0], vecs[1], vecs[2], act_lambda))
		{
			result = true;
			if (act_lambda < lambda)
				lambda = act_lambda;
		}
	}

	return result;

}


cCharacter *cCharacterList::CheckRay (float vecOrigin[3], float vecDir[3],
									  float deltax, float deltay,
									  float deltaz, float &lambda)
{
	float act_lambda;
	cCharacter *result = NULL;
	lambda = 100000000.0f;
	characterlist_t::iterator iter;
	for (iter = characterlist.begin (); iter != characterlist.end (); iter++)
	{
		if (iter->second->
			CheckRay (vecOrigin, vecDir, deltax, deltay, deltaz, act_lambda))
		{
			if (act_lambda < lambda)
			{
				result = iter->second;
				lambda = act_lambda;
			}
		}
	}
	return result;
}


void cCharacterList::RenderText ()
{
	characterlist_t::iterator iter;
	for (iter = characterlist.begin (); iter != characterlist.end (); iter++)
		iter->second->RenderText ();
}

void cCharacter::DoAnimation (int animid, int repeat)
{
	int animset = pGrannyLoader->GetAnimset ((Uint32) m_body);
	m_animtime = 0.0f;
	bool mounted;
	cCharacterEquip *mount = GetEquip (25);
	cMount *themount = NULL;
	if (mount == NULL)
		mounted = false;
	//std::cout << "NO MOUNT" << endl;
	else
	{                         //std::cout << "MOUNTED" << endl;
		mounted = true;
		themount = new cMount (mount->model ());
	}

	if ((animset == 0) || (animset == 1))
	{
		if (animid < 100)
			switch (animid)
		{
			case 0x00:
			case 0x01:
				m_forceanim = 0;
				break;
			case 0x02:
			case 0x03:
				m_forceanim = 2;
				break;

              case 0x04:
              case 0x05:
                m_forceanim = 4;
                break;
              case 0x06:
                m_forceanim = 11;
                break;
              case 0x07:
              case 0x08:
              case 0x09:
                m_forceanim = 9;
                break;

			case 0x0a:
			case 0x0b:
			case 0x0c:
			case 0x0d:
			case 0x0e:
				m_forceanim = animid + 2;

				break;
			case 0x10:
				m_forceanim = 18;
				break;
			case 0x11:
				m_forceanim = 19;
				break;
			case 0x12:
				m_forceanim = 20;
				break;
			case 0x13:
				m_forceanim = 21;
				break;
			case 0x14:
				m_forceanim = 10;
				break;
			case 0x15:
				m_forceanim = 22;
				break;
			case 0x16:
				m_forceanim = 23;
				break;
				//case 25: m_forceanim = 25; break;
			case 26:
				m_forceanim = themount->GetMountAnimation (27);
				break;
			case 27:
				m_forceanim = themount->GetMountAnimation (28);
				break;
			case 28:
				m_forceanim = themount->GetMountAnimation (29);
				break;
			case 32:
				m_forceanim = 31;
				break;
			case 33:
				m_forceanim = 32;
				break;
			case 34:
				m_forceanim = 33;
				break;
			case 35:
				m_forceanim = 76;
				break;


			default:
				m_forceanim = animid;
				printf ("Unknown Animation Type: %i\n", animid);
				break;
		}
		else
			m_forceanim = animid - 22;
	}
	else if (animset == 2)
	{
		switch (animid)
		{
		case 0:
			m_forceanim = 0;
			break;
		case 1:
			m_forceanim = 2;
			break;
		case 2:
			m_forceanim = 4;
			break;
		case 3:
			m_forceanim = 33;
			break;

		case 4:
		case 5:
		case 6:
			m_forceanim = 12;
			break;
		case 7:
			m_forceanim = 10;
			break;
		case 8:
			m_forceanim = 22;
			break;
		case 9:
			m_forceanim = 4;
			break;
		case 10:
			m_forceanim = 4;
			break;
		case 11:
			m_forceanim = 11;
			break;
		case 12:
			m_forceanim = 22;
			break;
		default:
			m_forceanim = animid;
			break;
		}
	}
	else if (animset == 3)
	{
		switch (animid)
		{
		case 0:
			m_forceanim = 0;
			break;
		case 1:
			m_forceanim = 4;
			break;
		case 2:
		case 3:
			m_forceanim = 22;
			break;
		case 4:
		case 5:
		case 6:
			m_forceanim = 12;
			break;
		case 7:
			m_forceanim = 20;
			break;
		case 8:
			m_forceanim = 21;
			break;
		case 9:
			m_forceanim = 12;
			break;
		case 10:
			m_forceanim = 10;
			break;
			//case 11: m_forceanim = 11; break;
			//case 12: m_forceanim = 22; break;
		default:
			m_forceanim = animid;
			break;
		}
	}
	else if (animset == 4)
	{
		switch (animid)
		{
		case 0:
			m_forceanim = 0;
			break;
		case 1:
			m_forceanim = 2;
			break;
		case 2:
		case 3:
			m_forceanim = 4;
			break;
		case 4:
			m_forceanim = 11;
			break;
		case 5:
		case 6:
			m_forceanim = 12;
			break;
		case 7:
			m_forceanim = 10;
			break;
		case 8:
			m_forceanim = 22;
			break;
		default:
			m_forceanim = animid;
			break;
		}
	}
	m_forcerepeat = repeat;
}

void cCharacter::setAnimtime (float animtime)
{
	if (!animtime)
	{
		if ((m_forcerepeat != 0) && (m_forcerepeat != 2))
		{
			m_forcerepeat = 0;
			m_forceanim = -1;
		}
		if (m_forcerepeat == 2)
			m_forcerepeat = 1;
	}
	m_animtime = animtime;
}

void cCharacterList::OnAdd (void (*callback) (cCharacter * character))
{
	callback_OnAdd = callback;
}

void cCharacterList::OnDelete (void (*callback) (cCharacter * character))
{
	callback_OnDelete = callback;
}

cMount::cMount (Uint16 tileid)
{
  m_tileid = tileid;
  switch (tileid)
      {
      case 0x3ffe:
        m_mounttype = 213;
        m_mountcathegory = 1;
        break;
      case 0x3ffd:
        m_mounttype = 241;
        m_mountcathegory = 1;
        break;
      case 0x3ffc:
        m_mounttype = 243;
        m_mountcathegory = 1;
        break;
      case 0x3ea2:             //dark brown/horse
        m_mounttype = 0xcc;
        m_mountcathegory = 1;
        break;
      case 0x3e9f:             //lightbrown/horse2
        m_mounttype = 0xc8;
        m_mountcathegory = 1;
        break;
      case 0x3ea0:             //light grey/horse3
        m_mounttype = 0xe2;
        m_mountcathegory = 1;
        break;
      case 0x3ea1:             //greybrown/horse4
        m_mounttype = 0xe4;
        m_mountcathegory = 1;
        break;
      case 0x3ea6:             //Llama
        m_mounttype = 0xdc;
        m_mountcathegory = 2;
        break;
      case 0x3ea3:             //desert
        m_mounttype = 0xd2;
        m_mountcathegory = 3;
        break;
      case 0x3ea4:             //frenzied
        m_mounttype = 0xda;
        m_mountcathegory = 3;
        break;
      case 0x3ea5:             //forest
        m_mounttype = 0xdb;
        m_mountcathegory = 3;
        break;
/*
      case 0x3f14:		//drake
	m_mounttype = 0x3c;
	break;
*/
      case 0x3ffb:
        m_mounttype = 213;
        m_mountcathegory = 1;
        break;
      case 0x3ea7:
        m_mounttype = 177;
        m_mountcathegory = 1;
        break;                  // Nightmare
      case 0x3ea8:
        m_mounttype = 117;
        m_mountcathegory = 1;
        break;                  // Silver Steed
      case 0x3ea9:
        m_mounttype = 114;
        m_mountcathegory = 1;
        break;                  // Nightmare
      case 0x3eaa:
        m_mounttype = 115;
        m_mountcathegory = 1;
        break;
        // case 0x3e9f:  m_mounttype = 0xC8;  m_mountcathegory = 1; break;  // Light Brown horse
      case 0x3eaf:
        m_mounttype = 120;
        m_mountcathegory = 1;
        break;                  // War Horse (Blood Red)
      case 0x3eb0:
        m_mounttype = 121;
        m_mountcathegory = 1;
        break;                  // War Horse (Light Green)
      case 0x3eb1:
        m_mounttype = 119;
        m_mountcathegory = 1;
        break;                  // War Horse (Light Blue)
      case 0x3eb3:
        m_mounttype = 144;
        m_mountcathegory = 7;
        break;                  // Sea Horse (Medium Blue)
      case 0x3eb5:
        m_mounttype = 116;
        m_mountcathegory = 1;
        break;                  // Nightmare
      case 0x3eb6:
        m_mounttype = 178;
        m_mountcathegory = 1;
        break;                  // Nightmare 4
      case 0x3ead:
        m_mounttype = 132;
        m_mountcathegory = 4;
        break;                  // Kirin
      case 0x3eb2:
        m_mounttype = 118;
        m_mountcathegory = 1;
        break;                  // War Horse (Purple)
      case 0x3eb7:
        m_mounttype = 179;
        m_mountcathegory = 1;
        break;                  // Dark Steed
      case 0x3eb8:
        m_mounttype = 187;
        m_mountcathegory = 6;
        break;                  // Ridgeback
      case 0x3eba:
        m_mounttype = 188;
        m_mountcathegory = 6;
        break;                  // Ridgeback, Savage
      case 0x3ebb:
        m_mounttype = 793;
        m_mountcathegory = 1;
        break;                  // Skeletal Mount
      case 0x3ebc:
        m_mounttype = 791;
        m_mountcathegory = 5;
        break;                  // Beetle
      case 0x3ebd:
        m_mounttype = 794;
        m_mountcathegory = 5;
        break;                  // SwampDragon
      case 0x3ebe:
        m_mounttype = 799;
        m_mountcathegory = 5;
        break;
      case 0x3e9e:		// Armored Swamp Dragon
        m_mounttype = 190;
        m_mountcathegory = 1;
        break;
      case 0x3E92:		// MondainSteed01
        m_mounttype = 284;
        m_mountcathegory = 1;
        break;

      default:
        Logger::WriteLine("Mount TileID: "+ tileid);
      }
}

int cMount::GetMountAnimation (int animtype)
{
	if (m_mountcathegory == 1)
		return animtype;
	else
	{
		int animoffset = 7 * (m_mountcathegory - 2);
		return animtype + (10 + animoffset);
	}
	return 0;
}

void cCharacter::setParticle(Uint32 particle_id)
{
	if(particle_handler)
		pParticleEngine.RemoveEffect(particle_handler);

	particle_handler  =  particle_id;
}
