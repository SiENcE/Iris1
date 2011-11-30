//
// File: Characters.h
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#ifndef _CHARACTERS_H_
#define _CHARACTERS_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <map>
#include <list>
#include <string>

#include "gui/TextManager.h"


#include "renderer/3D/SceneMaker.h"
#include "renderer/3D/CharacterLight.h"
#include "uotype.h"

// #include "../Fluid/mmgr.h"

struct sSkillEntry {
	Uint16 value;
	Uint16 unmodified;
	Uint16 skillcap;
	Uint8 skillLock;
};

class cCharacterText
{
private: 
	std::string m_text;
	Uint32 m_timeout;
	Uint32 m_starttime;
	cTextElement * element;
	bool m_done;
	int m_hue;
public:
	cCharacterText (std::string text, Uint32 timeout);
	~cCharacterText ();
	void Draw (int x, int y);
	Uint32 timeout();
	int height () 
	{
		return element->height();
	}
	bool done ();
	void setHue(int hue){m_hue = hue;}
};


class cCharacterEquip 
{
private:
	Uint16 m_layer;
	Uint32 m_id;
	Uint16 m_hue;
	Uint16 m_anim;
	Uint16 m_model;
public:
	cCharacterEquip() {
		m_layer = 0;
		m_id = 0;
		m_model = 0;
		m_anim = 10;
		m_hue = 0;
	}
	void setLayer(Uint16 layer) { m_layer = layer; }
	void setID(Uint32 id) { m_id = id; }
	void setHue(Uint16 hue) { m_hue = hue; }
	void setModel(Uint16 model) { m_model = model; }
	void setAnim(Uint16 anim) { m_anim = anim; }
	Uint16 layer() { return m_layer; }
	Uint16 hue() { return m_hue; }
	Uint16 model() { return m_model; }
	Uint32 id () { return m_id; }
	Uint16 anim () { return m_anim; }
};

typedef std::map<Uint32, cCharacterEquip *>  equiplist_t;
typedef std::map<Uint16, sSkillEntry *>  skilllist_t;

class cMount 
{
private:
	Uint16 m_tileid;
	Uint16 m_mounttype;
	Uint8 m_mountcathegory;
public:
	cMount(Uint16 tileid);
	~cMount(){}
	Uint16 GetTileID(){return m_tileid;}
	Uint16 GetMountType(){return m_mounttype;}
	int GetMountAnimation(int animtype);
	int GetMountCathegory(){return m_mountcathegory;}
	//int GetAnimType();
};

class cCharacter
{
private:
	Uint32 m_id;
	float m_x, m_y, m_z;
	float m_destx, m_desty, m_destz;
	float m_angle;
	Uint16 m_body;
	Uint16 m_hue;
	Uint8 m_flag;
	Uint8 m_direction;
	float m_animtime;
	Uint32 m_lastanim;
	bool m_moving;
	Uint16 m_highlight;       

	bool m_corpse;

	std::string m_name;
	bool m_name_change;
	Uint8 m_sex;
	Uint32 m_gold;
	Uint16 m_armor;
	Uint16 m_weight;

	Uint16 m_strength;
	Uint16 m_dexterity;
	Uint16 m_intelligence;

	Uint16 m_hits_current;
	Uint16 m_hits_max;
	Uint16 m_stamina_current;
	Uint16 m_stamina_max;
	Uint16 m_mana_current;
	Uint16 m_mana_max;

	Uint16 m_statcap;
	Uint8 m_petscurrent;
	Uint8 m_petsmax;

	Uint16 m_fireresist;
	Uint16 m_coldresist;
	Uint16 m_poisonresist;
	Uint16 m_energyresist;
	Uint16 m_luck;
	Uint16 m_damagemax;
	Uint16 m_damagemin;
	Uint32 m_tithings;
	Uint32 particle_handler;
	
	
	int m_animtype;
	bool m_warmode;

	int m_forceanim;
	int m_forcerepeat;

	equiplist_t equiplist;

	skilllist_t m_skills;

	int last_animtype;
	Uint32 anim_smooth_time;

	std::list<class cCharacterText *> text_list;

	sColor light_points [8];

	cCharacterLight * m_light;


	Uint32 aostooltips_listID;
	std::vector<std::string> aostooltips_list;

public:
	cCharacter();
	virtual ~cCharacter();

	void setID (Uint32 id) { m_id = id; }
	void setX (float x) { m_x = x; m_destx = x; m_moving = 0; }
	void setY (float y) { m_y = y; m_desty = y; m_moving = 0; }
	void setZ (float z) { m_z = z; m_destz = z; m_moving = 0; }
	void setX (int x) { m_x = (float) x; m_destx = m_x; }
	void setY (int y) { m_y = (float) y; m_desty = m_y; }
	void setZ (int z) { m_z = (float) z; m_destz = m_z; }
	void setPosition (float x, float y, float z) { 
		m_x = x; m_destx = m_x;
		m_y = y; m_desty = m_y; 
		m_z = z; m_destz = m_z;
		m_moving = 0;
	}
	void setPosition (int x, int y, int z) { 
		m_x = (float) x; m_destx = m_x;
		m_y = (float) y; m_desty = m_y; 
		m_z = (float) z; m_destz = m_z; 
		m_moving = 0;
	}
	void getPosition (float & x, float & y, float & z) { x = m_x; y = m_y; z = m_z; }
	virtual void setBody (int body) { m_body = body; }
	virtual void setHue (int hue) { m_hue = hue; }
	void setFlag (int flag) { m_flag = flag; }
	void setAnimtime (float animtime);
	void setLastAnim (Uint32 lastanim) { m_lastanim = lastanim; }
	void setDirection(Uint8 direction) { m_direction = direction; m_angle = direction * 45.0f;}

	void setName (std::string name) { m_name = name; }
	void setNameChange (bool name_change) { m_name_change = name_change; }
	void setSex (Uint8 sex) { m_sex = sex; }
	void setGold (Uint32 gold) { m_gold = gold; }
	void setArmor (Uint16 armor) { m_armor = armor; }
	void setWeight (Uint16 weight) { m_weight = weight; }

	void setStr (Uint16 strength) { m_strength = strength; }
	void setDex (Uint16 dexterity) { m_dexterity = dexterity; }
	void setInt (Uint16 intelligence) { m_intelligence = intelligence; }

	void setCurrentHits (Uint16 hits_current) { m_hits_current = hits_current; }
	void setMaxHits (Uint16 hits_max) { m_hits_max = hits_max; }
	void setCurrentStamina (Uint16 stamina_current) { m_stamina_current = stamina_current; }
	void setMaxStamina (Uint16 stamina_max) { m_stamina_max = stamina_max; }
	void setCurrentMana (Uint16 mana_current) { m_mana_current = mana_current; }
	void setMaxMana (Uint16 mana_max) { m_mana_max = mana_max; }

	void setStatCap(Uint16 statcap){m_statcap = statcap;}
	void setPetsCurrent(Uint8 petscurr){m_petscurrent= petscurr;}
	void setPetsMax(Uint8 petsmax){m_petsmax = petsmax;}

	void setFireResist(Uint16 fireres) {m_fireresist = fireres;}
	void setColdResist(Uint16 coldres) {m_coldresist = coldres;}
	void setPoisonResist(Uint16 poisres) {m_poisonresist = poisres;}
	void setEnergyResist(Uint16 energyres){m_energyresist = energyres;}
	void setLuck(Uint16 luck){m_luck = luck;}
	void setMaxDamage(Uint16 maxdamage){m_damagemax = maxdamage;}
	void setMinDamage(Uint16 mindamage){m_damagemin = mindamage;}
	void setTithingPoints(Uint32 tith){m_tithings = tith;}

	void displayDeath(){m_forceanim = 22;}
	void setAsCorpse(){m_corpse = true;}
	bool isCorpse(){return m_corpse;}


	Uint32 id () { return m_id; }
	int x () { return (int) (m_x + 0.5f); }
	int y () { return (int) (m_y + 0.5f); }
	int z () { return (int) (m_z + 0.5f); }
	float fx () { return m_x; }
	float fy () { return m_y; }
	float fz () { return m_z; }
	int body () { return m_body; }
	int hue () { return m_hue; }
	int flag () { return m_flag; }
	int moving () { return m_moving; }
	float angle () { return m_angle; }
	float animtime () { return m_animtime; }
	Uint32 lastanim () { return m_lastanim; }
	Uint8 direction() { return m_direction; }

	std::string name () { return m_name; }
	bool name_change () { return m_name_change; }
	Uint8 sex () { return m_sex; }
	Uint32 gold () { return m_gold; }
	Uint16 armor () { return m_armor; }
	Uint16 weight () { return m_weight; }

	Uint16 strength () { return m_strength; }
	Uint16 dexterity () { return m_dexterity; }
	Uint16 intelligence () { return m_intelligence; }

	Uint16 hits_current () { return m_hits_current; }
	Uint16 hits_max () { return m_hits_max; }
	Uint16 stamina_current () { return m_stamina_current; }
	Uint16 stamina_max () { return m_stamina_max; }
	Uint16 mana_current () { return m_mana_current; }
	Uint16 mana_max () { return m_mana_max; } 

	Uint16 statcap() {return m_statcap;}
	Uint8 petscurrent() {return m_petscurrent;}
	Uint8 petsmax() {return m_petsmax;}

	Uint16 fireresist() {return m_fireresist;}
	Uint16 coldresist() {return m_coldresist;}
	Uint16 poisonresist() {return m_poisonresist;}
	Uint16 energyresist() {return m_energyresist;}
	Uint16 luck() {return m_luck;}
	Uint16 maxdamage() {return m_damagemax;}
	Uint16 mindamage() {return m_damagemin;}
	Uint32 tithings() {return m_tithings;}

	void setParticle(Uint32 particle_id);
	
    int animtype () { 
		if (SDL_GetTicks() > anim_smooth_time)
			return m_animtype;
		return last_animtype;

	}

	cCharacterLight * light ();

	void setHighlightColor(Uint16 h_color) {m_highlight = h_color;}             
	Uint16 getHighlightColor() {return m_highlight;}     

	void setWarmode(bool warmode) { m_warmode = warmode; }
	bool warmode() { return m_warmode; }

	void DoAnimation(int animid, int repeat);

	void ClearEquip (void);
	cCharacterEquip * AddEquip (Uint32 layer);
	cCharacterEquip * GetEquip (unsigned int layer);
	void DelEquip (Uint32 layer);

	virtual void Handle (float time_factor);
	void MoveTo (float x, float y, float z);
	virtual void RotateTo(Uint8 direction) { m_direction = direction; }

	sSkillEntry * skill (Uint16 id);
	void ClearSkills();

	bool CheckRay(float vecOrigin[3], float vecDir[3], float deltax, float deltay, float deltaz, float & lambda);

	void ClearText (void);
	void AddText(std::string text, int timeout, int hue = 0);
	void RenderText ();
	Uint32 AOSTooltipID(){return aostooltips_listID;}
	void SetAOSTooltipID(Uint32 id){aostooltips_listID = id;}
	void AddAOSTooltip(std::string tooltip){aostooltips_list.push_back(tooltip);}
	void ClearAOSTooltip(){aostooltips_list.clear();}
	int aostooltips_count(){return aostooltips_list.size();}
	std::string GetAOSTooltip(int id){return aostooltips_list.at(id);}
};


typedef std::map<Uint32, cCharacter *>  characterlist_t;

class cCharacterList
{
private:
	characterlist_t characterlist;
public:
	cCharacterList ();    
	~cCharacterList ();

	void Clear (void);

	cCharacter * Add (Uint32 id, int x, int y, int z, int model);
	cCharacter * Get (unsigned int id);
	void Delete(unsigned int id);

	characterlist_t * GetList(void) { return &characterlist; }

	int GetCount(void) { return characterlist.size(); }
	void Handle (float time_factor);
	void RenderText();

	cCharacter * CheckRay(float vecOrigin[3], float vecDir[3], float deltax, float deltay, float deltaz, float & lambda);

	void OnAdd(void (*callback) (cCharacter * character));
	void OnDelete(void (*callback) (cCharacter * character));

protected:
	void (*callback_OnAdd) (cCharacter * character);
	void (*callback_OnDelete) (cCharacter * character);

};

extern cCharacterList pCharacterList;

#endif //_CHARACTERS_H_
