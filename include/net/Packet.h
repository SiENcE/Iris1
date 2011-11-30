//
// File: Packet.h
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

#ifndef _PACKET_H_
#define _PACKET_H_


#ifdef WIN32
#include <windows.h>
#endif


#include "SDL/SDL.h"
#include "SDLUtils.h"
#include "uoproto.h"

// #include "../Fluid/mmgr.h"

#define PACKET_INVALID 0
#define PACKET_CLIENT 1
#define PACKET_SERVER 2
#define PACKET_BOTH PACKET_CLIENT | PACKET_SERVER


// Compiler directives 
#if   defined(WIN32) && !defined(__MINGW32__) 
// Visual C pragma 
#pragma pack(push,1) 
#define STRUCT_PACKED 
#else 
#define STRUCT_PACKED __attribute__ ((packed)) 
#endif

extern Uint16 packet_lens[256];

struct sDefault {
	NBYTE m_cmd;
	NWORD m_len;
} STRUCT_PACKED;

struct sLoginRequest {
	NBYTE m_cmd;
	char m_username[30];
	char m_password[30];
	NBYTE m_unknown;
} STRUCT_PACKED;

struct sServerSelect {
	NBYTE m_cmd;
	NWORD m_slot;
} STRUCT_PACKED;

struct sRelay {
	NBYTE	m_cmd;
	NDWORD	m_ip;
	NWORD	m_port;
	NDWORD	m_Account;
} STRUCT_PACKED;

struct sCharListReq {
	NBYTE	m_cmd;
	NDWORD	m_account;
	char	m_username[30];
	char	m_password[30];	
} STRUCT_PACKED;

struct sCharList {
	NBYTE	m_cmd;
	NWORD	m_len;
	NBYTE	m_count;
	struct	
		{
			char	m_name[30];
			char	m_pass[30];
		} m_names[5];
	NBYTE	m_startCount;
} STRUCT_PACKED;

struct sCharPlay
{
	NBYTE	m_cmd;
	NDWORD	m_edededed;
	char	m_username[30];
	char	m_password[30];
	NDWORD	m_slot;
	NDWORD	m_ip;
} STRUCT_PACKED;

struct sCharDelete
{
	NBYTE	m_cmd;
	char	m_password[30];
	NDWORD	m_id;
	NDWORD	m_ip;
} STRUCT_PACKED;

struct sView
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NWORD	m_bodyType;
	NBYTE	m_zero1;
	NWORD	m_skinHue;
	NBYTE	m_flag;
	NWORD	m_x;
	NWORD	m_y;
	NWORD	m_zero2; // 0
	NBYTE	m_dir;
	NBYTE	m_z;
} STRUCT_PACKED;

struct sStart
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NDWORD	m_unknown1;
	NWORD	m_bodyType;
	NWORD	m_x;
	NWORD	m_y;
	NWORD	m_z;
	NBYTE	m_dir;
	NWORD	m_unknown2;
	NDWORD	m_unknown3;
	NDWORD	m_unknown4;
	NBYTE	m_charMode;
	NBYTE	m_highlightColor;
	NBYTE	m_unknown[7];
} STRUCT_PACKED;

struct sSpeak
{
	NBYTE	m_cmd;
	NWORD	m_len;
	NDWORD	m_id;
	NWORD	m_model;
	NBYTE	m_type;
	NWORD	m_color;
	NWORD	m_font;
	char	name[30];
	char	message;
} STRUCT_PACKED;

struct sWalkRequest
{
	NBYTE	m_cmd;
	NBYTE	m_direction;
	NBYTE	m_sequence;
	NDWORD	m_fastwalk;
} STRUCT_PACKED;

struct sWalkAck
{
	NBYTE	m_cmd;
	NBYTE	m_seq;
	NBYTE	m_zero;
} STRUCT_PACKED;

struct sWalkCancel
{
	NBYTE	m_cmd;
	NBYTE	m_seq;
	NWORD	m_x;
	NWORD	m_y;
	NBYTE	m_dir;
	NBYTE	m_z;
} STRUCT_PACKED;

struct sStatus
{
	NBYTE	m_cmd;
	NWORD	m_len;
	NDWORD	m_id;
	char	m_name[30];
	NWORD	m_hitPoints;
	NWORD	m_maxPoints;
	NBYTE	m_changeName;
	NBYTE	m_flag;
	NBYTE	m_sex;
	NWORD	m_str;
	NWORD	m_dex;
	NWORD	m_int;
	NWORD	m_stamina;
	NWORD	m_maxStamina;
	NWORD	m_mana;
	NWORD	m_maxMana;
	NDWORD	m_gold;
	NWORD	m_armorClass;
	NWORD	m_Weight;
} STRUCT_PACKED;

struct sCharName
{
	NBYTE	m_cmd;
	NDWORD	m_charId;
	char	m_name[30];
} STRUCT_PACKED;

struct sStatChange
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NWORD	m_max;
	NWORD	m_val;
} STRUCT_PACKED;

struct sWar
{
	NBYTE	m_cmd;
	NBYTE	m_warmode;
	NBYTE	m_unk[3];		// 00 32 00
} STRUCT_PACKED;

struct sDoAction
{
	NBYTE	m_cmd;
	NWORD	m_len;
	NBYTE	m_type;
	char	m_name;
} STRUCT_PACKED;

struct sSkillLock
{
	NBYTE	m_cmd;
	NWORD	m_len;
	NWORD	m_skillId;
	NBYTE	m_skillLock;
} STRUCT_PACKED;

struct sCharMove
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NWORD	m_model;
	NWORD	m_x;
	NWORD	m_y;
	NBYTE	m_z;
	NBYTE	m_dir;
	NWORD	m_hue;
	NBYTE	m_flag;
	NBYTE	m_hColor;
} STRUCT_PACKED;

struct sItemEquip
{
	NBYTE	m_cmd;
	NDWORD	m_itemID;
	NWORD	m_model;
	NBYTE	m_zero1;
	NBYTE	m_layer;
	NDWORD	m_playerId;
	NWORD	m_hue;
} STRUCT_PACKED;


struct sItemPickupReq
{
	NBYTE	m_cmd;
	NDWORD	m_itemID;
	NWORD	m_num; 
} STRUCT_PACKED;

struct sItemDropReq
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NWORD	m_x;
	NWORD	m_y;
	NBYTE	m_z;
	NDWORD	m_container; 		
} STRUCT_PACKED;

struct sSpeakUnicode
{
	NBYTE	m_cmd;
	NWORD	m_len;
	NDWORD	m_id;
	NWORD	m_model;
	NBYTE	m_type;
	NWORD	m_color;
	NWORD	m_font;
	NDWORD	m_language;
	char	m_name[30];
	char 	m_msg;
} STRUCT_PACKED;

struct sTarget
{
	NBYTE	m_cmd;
	NBYTE	m_type;
	NDWORD	m_cursorID;
	NBYTE	m_checkCrime;
	NDWORD	m_clickedID;
	NWORD	m_clickX;
	NWORD	m_clickY;
	NBYTE   m_dummy;
	NBYTE	m_clickZ;
	NWORD	m_Model;
} STRUCT_PACKED;

struct sContAdd
{
	NBYTE	m_cmd;
	NDWORD	m_itemId;
	NWORD	m_body;
	NBYTE	m_zero1;
	NWORD	m_itemCount;
	NWORD	m_ContainerX;
	NWORD	m_ContainerY;
	NDWORD	m_ContainerID;
	NWORD	m_hue;
} STRUCT_PACKED;

struct sSound
{
	NBYTE	m_cmd;
	NBYTE	m_flags;
	NWORD	m_sound;
	NWORD	m_volume;
	NWORD	m_x;
	NWORD	m_y;
	NWORD	m_z;
} STRUCT_PACKED;

struct sItemEquipReq
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NBYTE	m_layer;
	NDWORD	m_charid;
} STRUCT_PACKED;

struct sCharAction
{
	NBYTE	m_cmd;
	NDWORD	m_id;
	NWORD	m_movement;
	NBYTE	m_zero1;
	NBYTE	m_direction;
	NWORD	m_repeat;
	NBYTE	m_animForward;
	NBYTE	m_repeatFlag;
	NBYTE	m_frameDelay;
} STRUCT_PACKED;

struct sMenuChoice
{
        NBYTE	m_cmd;
	NDWORD	m_dialogId;
	NWORD	m_menuId;
	NWORD	m_index;
	NWORD	m_modelNum;
	NDWORD	m_unknown;		// 00 00
} STRUCT_PACKED;

struct	sPacket
{
	union
	{
		Uint8 m_raw[MAX_PACKET_LEN];
		struct sDefault Default;
		struct sLoginRequest LoginRequest;
		struct sServerSelect ServerSelect;
		struct sRelay Relay;
		struct sCharListReq CharListReq;
		struct sCharList CharList;
		struct sStart Start;
		struct sCharPlay CharPlay;
		struct sCharDelete CharDelete;
		struct sSpeak Speak;
		struct sView View;
		struct sWalkRequest WalkRequest;
		struct sWalkAck WalkAck;
		struct sWalkCancel WalkCancel;
		struct sStatus Status;
		struct sCharName CharName;
		struct sStatChange StatChange;
		struct sWar War;
		struct sDoAction DoAction;
		struct sSkillLock SkillLock;
		struct sCharMove CharMove;
		struct sItemEquip ItemEquip;
		struct sItemPickupReq ItemPickupReq;
		struct sSpeakUnicode SpeakUnicode;
		struct sItemDropReq ItemDropReq;
		struct sTarget Target;
                struct sContAdd ContAdd;
                struct sSound Sound;
                struct sItemEquipReq ItemEquipReq;
                struct sCharAction CharAction;
                struct sMenuChoice MenuChoice;
	};
	
} STRUCT_PACKED;


class cPacket
{
private:
    int	 m_length;
    int	 m_pos;
public:
    
   struct sPacket packet;
   
   cPacket ();
   ~cPacket ();
   void Reset(void);
   void Clear(void);
   
      
   void AddByte( Uint8 item );
   void AddWord( Uint16 item );
   void AddDword( Uint32 item );
   void AddData( void * data, int len );
   
   Uint8  GetByte();
   Uint16 GetWord();
   Uint32 GetDword();
   void GetData( void * data, int len );

   int ParsePacket(void * data, unsigned int len, int direction = PACKET_BOTH);
   void FillPacket(Uint8 type, unsigned int len = 0);
   
   sClientPacket * clientpacket() { return (sClientPacket *) &packet; }
   sServerPacket * serverpacket() { return (sServerPacket *) &packet; }
   
   int len () { return m_length; }
   void SetPosition (int pos) { m_pos = pos; }
   void SetLength (int len) { m_length = len; }
   
protected:

  }; 
   
#if defined(WIN32) && !defined(__MINGW32__) 
#pragma pack(pop) 
#endif 
   
#endif //_PACKET_H_ 
