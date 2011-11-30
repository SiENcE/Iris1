// (c) Soulburner
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

#ifndef UOPROTO_H
#define UOPROTO_H

// Compiler directives
#if  	defined(WIN32) && !defined(__MINGW32__)
// Visual C pragma
#pragma pack(1)
#define STRUCT_PACKED
#else
// GCC packed attribute
#define STRUCT_PACKED	__attribute__ ((packed))
#endif

#include "include.h"
#include "SDL/SDL_types.h"
#include <stdlib.h>

// #include "../Fluid/mmgr.h"

// Type definitions

extern Uint16 bswap16 (Uint16 val);
extern Uint32 bswap32 (Uint32 val);
typedef	Uint8		NBYTE;

struct NWORD
{
        Uint16 m_val;

        operator Uint16 () const
        {
                
                
                //return( ntohs( m_val ));
                
                return bswap16 (m_val);
        }

        NWORD & operator = ( Uint16 val )
        {
                m_val = bswap16( val );
//                m_val = htons( val );
                return( * this );
        }

} STRUCT_PACKED;

struct NDWORD
{
        Uint32 m_val;

        operator Uint32 () const
        {
                //return( ntohl( m_val ));
                return bswap32(m_val);
        }

        NDWORD & operator = ( Uint32 val )
        {
                //m_val = htonl( val );
                m_val = bswap32(val);
                return( * this );
        }

} STRUCT_PACKED;

#define NCHAR	NWORD

// Protocol definition
#define PCK_Create				0x00
#define PCK_Disconnect			0x01
#define PCK_Walk				0x02
#define PCK_Talk				0x03
#define PCK_GodMode				0x04
#define PCK_Attack				0x05
#define PCK_DClick				0x06
#define	PCK_ItemPickupReq		0x07
#define PCK_ItemDropReq			0x08
#define	PCK_Click				0x09
#define PCK_EditItem			0x0A
#define PCK_EditArea			0x0B
#define	PCK_EditTileData		0x0C
#define PCK_EditNPC				0x0D
#define	PCK_EditTemplate		0x0E
#define PCK_EditPaperdoll		0x0F
#define PCK_EditHues			0x10
#define PCK_Status				0x11
#define PCK_DoAction				0x12
#define PCK_ItemEquipReq		0x13
#define PCK_GodLevitate			0x14
#define	PCK_Follow				0x15
#define PCK_ScriptsGet			0x16
#define PCK_ScriptsExe			0x17
#define PCK_ScriptsAdd			0x18
#define	PCK_UnkSpeakNPC			0x19
#define PCK_Put					0x1a
#define PCK_Start				0x1b
#define PCK_Speak				0x1c
#define PCK_Delete				0x1d
#define PCK_UnkAnimate			0x1e
#define PCK_UnkExplode			0x1f
#define	PCK_View				0x20
#define PCK_WalkCancel			0x21
#define	PCK_WalkAck				0x22
#define	PCK_DragAnim			0x23
#define PCK_ContOpen			0x24
#define PCK_ContAdd				0x25
#define PCK_Kick				0x26
#define PCK_DragCancel			0x27
#define PCK_ClearSquare			0x28
#define PCK_ObjectDropped		0x29
#define PCK_UnkBlood			0x2A
#define PCK_GodModeOK			0x2B
#define PCK_DeathMenu			0x2C
#define PCK_UnkHealth			0x2D
#define PCK_ItemEquip			0x2e
#define PCK_Fight				0x2f
#define PCK_UnkAttackOK			0x30
#define PCK_UnkPeace			0x31
#define PCK_UnkHackMove			0x32
#define PCK_Pause				0x33
#define PCK_CharStatReq			0x34
#define PCK_EditResType			0x35
#define PCK_EditResTiledata		0x36
#define PCK_UnkMoveObject		0x37
#define PCK_PathFind			0x38
#define PCK_ChangeGroup			0x39
#define	PCK_Skill				0x3a
#define PCK_VendorBuy			0x3b
#define PCK_Content				0x3c
#define PCK_UnkShip				0x3d
#define PCK_UnkVersions			0x3e
#define PCK_EditUpdateObj		0x3f
#define PCK_EditUpdateTerrain	0x40
#define PCK_EditUpdateTiledata	0x41
#define PCK_EditUpdateArt		0x42
#define PCK_EditUpdateAnim		0x43
#define PCK_EditUpdateHues		0x44
#define PCK_UnkVersionOK		0x45
#define PCK_EditNewArt			0x46
#define PCK_EditNewTerrain		0x47
#define PCK_EditNewAnim			0x48
#define PCK_EditNewHues			0x49
#define PCK_UnkDestroyArt		0x4a
#define PCK_UnkCheckVersion		0x4b
#define PCK__ScriptsNames		0x4c
#define PCK_ScriptsFile			0x4d
#define PCK_LightPoint			0x4e
#define PCK_Light				0x4f
#define PCK_UnkBBHeader			0x50
#define PCK_UnkBBMessage		0x51
#define PCK_UnkPostMsg			0x52
#define PCK_ClientState			0x53
#define PCK_Sound				0x54
#define PCK_ReDrawAll			0x55
#define PCK_MapEdit				0x56
#define PCK_UnkRegionsUpdate	0x57
#define PCK_UnkRegionsNew		0x58
#define PCK_UnkEffectNew		0x59
#define PCK_EffectUpdate		0x5a
#define PCK_Time				0x5b
#define PCK_UnkVersionRestart	0x5c
#define PCK_CharPlay			0x5d
#define PCK_UnkServerList		0x5e
#define PCK_UnkServerAdd		0x5f
#define PCK_UnkServerDel		0x60
#define PCK_UnkStaticDel		0x61
#define PCK_UnkStaticMove		0x62
#define PCK_UnkLoadArea			0x63
#define PCK_UnkLoadAreaTry		0x64
#define PCK_Weather				0x65
#define PCK_BookPage			0x66
#define PCK_UnkSimped			0x67
#define PCK_UnkAddLSScript		0x68
#define PCK_Options				0x69
#define PCK_UnkFriendNotify		0x6a
#define	PCK_UnkUseKey			0x6b
#define PCK_Target				0x6c
#define PCK_PlayMusic			0x6d
#define PCK_CharAction			0x6e
#define PCK_SecureTrade			0x6f
#define	PCK_Effect				0x70
#define PCK_BBoard				0x71
#define PCK_War					0x72
#define PCK_Ping				0x73
#define PCK_VendOpenBuy			0x74
#define PCK_CharName			0x75
#define PCK_ZoneChange			0x76
#define PCK_CharMove			0x77
#define PCK_Char				0x78
#define PCK_UnkResourceGet		0x79
#define PCK_UnkResourceData		0x7a
#define PCK_UnkSequence			0x7b
#define PCK_MenuItems			0x7c
#define PCK_MenuChoice			0x7d
#define PCK_GodGetView			0x7e
#define PCK_GodViewInfo			0x7f
#define PCK_ServersReq			0x80
#define PCK_CharList3			0x81
#define PCK_LogBad				0x82
#define PCK_CharDelete			0x83
#define PCK_UnkPasswordChange	0x84
#define PCK_DeleteBad			0x85
#define PCK_CharList2			0x86
#define PCK_UnkResourceSend		0x87
#define PCK_PaperDoll			0x88
#define PCK_CorpEquip			0x89
#define PCK_EditTrigger			0x8A
#define PCK_GumpTextDisp		0x8b
#define PCK_Relay				0x8c
#define PCK_Unused8d			0x8d
#define PCK_UnkCharMove			0x8e
#define PCK_Unused8f			0x8f
#define PCK_MapDisplay			0x90
#define PCK_CharListReq			0x91
#define PCK_EditMultiMul		0x92
#define PCK_BookOpen			0x93
#define PCK_EditSkillsMul		0x94
#define PCK_DyeVat				0x95
#define PCK_GodGameMon			0x96
#define PCK_WalkForce			0x97
#define PCK_UnkChangeName		0x98
#define PCK_TargetMulti			0x99
#define PCK_Prompt				0x9a
#define PCK_HelpPage			0x9b
#define PCK_GodAssist			0x9c
#define PCK_GodSingle			0x9d
#define PCK_VendOpenSell		0x9e
#define PCK_VendorSell			0x9f
#define PCK_ServerSelect		0xa0
#define	PCK_StatChngStr			0xa1
#define PCK_StatChngInt			0xa2
#define PCK_StatChngDex			0xa3
#define PCK_Spy					0xa4
#define PCK_Web					0xa5
#define PCK_Scroll				0xa6
#define PCK_TipReq				0xa7
#define PCK_ServerList			0xa8
#define PCK_CharList			0xa9
#define PCK_AttackOK			0xaa
#define PCK_GumpInpVal			0xab
#define PCK_GumpInpValRet		0xac
#define PCK_TalkUNICODE			0xad
#define PCK_SpeakUNICODE		0xae
#define PCK_CharDeath			0xaf
#define PCK_GumpDialog			0xb0
#define PCK_GumpDialogRet		0xb1
#define PCK_ChatReq				0xb2
#define PCK_ChatText			0xb3
#define PCK_TargetItems			0xb4
#define PCK_Chat				0xb5
#define PCK_ToolTipReq			0xb6
#define PCK_ToolTip				0xb7
#define PCK_CharProfile			0xb8
#define	PCK_ChatEnable			0xb9
#define PCK_Arrow				0xba
#define PCK_MailMsg				0xbb
#define PCK_Season				0xbc
#define PCK_ClientVersion		0xbd
#define PCK_UnkVersionAssist	0xbe
#define PCK_ExtData				0xbf
#define PCK_UnkHuedEffect		0xc0
#define PCK_SpeakTable			0xc1
#define PCK_UnkSpeakTableU		0xc2
#define PCK_UnkGQEffect			0xc3
#define PCK_UnkSemiVisible		0xc4
#define PCK_UnkInvalidMap		0xc5
#define PCK_UnkEnableInvalidMap	0xc6
#define PCK_ParticleEffect		0xc7
#define PCK_UnkUpdateRange		0xc8
#define PCK_UnkTripTime			0xc9
#define PCK_UnkUTripTime		0xca
#define PCK_UnkGQCount			0xcb
#define PCK_UnkTextIDandStr		0xcc
#define PCK_AOSTooltip			0xd6

//House Command Tools and a lot more Subcommands
#define PCK_AOSCommand			0xd7

// introduced with Age of Shadows Expansion
#define PCK_AOSObjProp			0xdc
#define PCK_MYUO				0xfe
#define PCK_QTY					0xcd

#define LOGINERR_NONE		0
#define LOGINERR_ACCUSED	1
#define LOGINERR_BLOCKED	2
#define LOGINERR_BAD_PASS	3
#define	LOGINERR_TIMEOUT	4
#define LOGINERR_SUCCESS	255

// Packed definition
#define MAX_PACKET_LEN	65535

// Client->Server
struct	ClientPack_s
{
	int	m_length;

	union
	{
		// Raw packet
		NBYTE	m_raw[MAX_PACKET_LEN];

		// Generic typeless packet
		struct {
			NBYTE	m_cmd;
			NWORD	m_len;
		} Default;

		// PCK_Create(0x00)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_edededed;
			NDWORD	m_ffffffff;
			NBYTE	m_pattern3;
			char	m_name[30];
			char	m_password[30];
			NBYTE	m_sex;
			NBYTE	m_str;
			NBYTE	m_dex;
			NBYTE	m_int;
			NBYTE	m_skill1;
			NBYTE	m_skill1value;
			NBYTE	m_skill2;
			NBYTE	m_skill2value;
			NBYTE	m_skill3;
			NBYTE	m_skill3value;
			NWORD	m_skinColor;
			NWORD	m_hairStyle;
			NWORD	m_hairColor;
			NWORD	m_facialHair;
			NWORD	m_facialHairColor;
			NWORD	m_startingLocation;
			NWORD	m_unknown1;
			NWORD	m_slot;
			NDWORD	m_clientIP;
			NWORD	m_shirtColor;
			NWORD	m_pantsColor; 
		} Create;

		// PCK_Disconnect (0x01)
		struct 
		{
			NBYTE	m_cmd;				// PCK_DISCONNECT
			NDWORD	m_ffffffff; 
		} Disconnect;

		// PCK_Walk (0x02)
		struct
		{
			NBYTE	m_cmd;				// PCK_MOVE_REQUEST
			NBYTE	m_direction;
			NBYTE	m_sequence;
			NDWORD	m_fastwalk;			// Fastwalk fix
		} Walk;

		// PCK_Talk (0x03)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_SpeechType;
			NWORD	m_Color;
			NWORD	m_SpeechFont;
			char	m_msg; 
		} Talk;

		// PCK_GodMode (0x04)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_mode;
		} GodMode;

		// PCK_Attack (0x05)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemId;
		} Attack;

		// PCK_DClick (0x06)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemId;
		} DClick;

		// PCK_ItemPickupReq (0x07)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemID;
			NWORD	m_num; 
		} ItemPickupReq;

		// PCK_ItemDropReq (0x08)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_x;
			NWORD	m_y;
			NBYTE	m_z;
			NDWORD	m_container; 		
		} ItemDropReq;

		// PCK_Click (0x09)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemId;
		} Click;

		// PCK_ExtCmd (0x12)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_type;
			char	m_name;
		} ext_cmd_12;

		// PCK_ItemEquipReq (0x13)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemID;
			NBYTE	m_layer;
			NDWORD	m_playerID;
		} ItemEquipReq;

		// PCK_WalkAck (0x22)
		struct
		{
			NBYTE	m_cmd;
			NBYTE	m_seq;
			NBYTE	m_zero;
		} WalkAck;

		// PCK_DeathMenu (0x2C)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_action;
		} DeathMenu;

		// PCK_CharStatReq (0x34)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_edededed;
			NBYTE	m_getType;
			NDWORD	m_id;
		} CharStatReq;

		// PCK_Skill (0x3A)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_skillId;
			NBYTE	m_skillLock;
		} Skill;

		// PCK_VendorBuy (0x3B)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_vendorId;
			NBYTE	m_flag;

			struct 
			{
				NBYTE	m_Layer;	// 0x1A
				NDWORD	m_itemId;
				NWORD	m_numBought;
			} m_item;
		} VendorBuy;

		// PCK_MapEdit (0x56)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_serial;
			NBYTE	m_action;
			NBYTE	m_pinNumber;
			NWORD	m_PinX;
			NWORD	m_PinY;
		} MapEdit;

		// PCK_CharPlay (0x5d)
		struct 
		{
			NBYTE		m_cmd;
			NDWORD		m_edededed;
			char		m_name[30];
			char        m_pass[33];	//not_pass byte[33] Mostly 0's
			NBYTE		m_slot;
			NDWORD      m_ip;
		} CharPlay;

		#define MAX_BOOK_PAGES 64   // arbitrary max number of pages.
		#define MAX_BOOK_LINES 8    // max lines per page.
		// PCK_BookPage (0x66)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_bookId;
			NWORD	m_numPages;

			struct 
			{
				NWORD	m_pageNum;
				NWORD	m_lines;
				char	m_text;
			} m_pages;
		} BookPage;

		// PCK_Options (0x69)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_index;
		} Options;

		// PCK_Target (0x6C)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_type;
			NDWORD	m_cursorID;
			NBYTE	m_checkCrime;
			NDWORD	m_clickedID;
			NWORD	m_clickX;
			NWORD	m_clickY;
			NWORD	m_clickZ;
			NWORD	m_Model;
		} Target;

		// PCK_SecureTrade (0x6F)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_action;
			NDWORD	m_id1;
			NDWORD	m_id2;
			NDWORD	m_id3;
			NBYTE	m_nameFollowind;
			char	name;
		} SecureTrade;

		// PCK_BBoard (0x71)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_subcmd;
			NDWORD	m_boardId;
			NBYTE	data;
		} BBoard;

		// PCK_War (0x72)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_warmode;      // 0x00 - Normal | 0x01 - Fighting
			NBYTE	m_unk[3];		// (always 00 32 00 in testing)
		} War;

		// PCK_Ping (0x73)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_seq;
		} Ping;

		// PCK_CharName (0x75)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_charId;
			char	m_name[30];
		} CharName;

		// PCK_MenuChoice (0x7d)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_dialogId;
			NWORD	m_menuId;
			NWORD	m_index;
			NWORD	m_modelNum;
			NDWORD	m_unknown;		// 00 00
		} MenuChoice;

		// PCK_ServersReq (0x80)
		struct 
		{
			NBYTE	m_cmd;
			char	m_name[30];
			char	m_pass[30];
			NBYTE	m_unknown;
		} ServersReq;

		// PCK_CharDelete (0x83)
		struct 
		{
			NBYTE	m_cmd;
			char	m_pass[30];
			NDWORD	m_charIndex;
			NDWORD	m_Ip;
		} CharDelete;

		// PCK_CharListReq (0x91)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_fccount;
			char	m_name[30];
			char	m_pass[30];	
		} CharListReq;

		// PCK_BookOpen (0x93)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_bookId;
			NBYTE	m_writeFlag;
			NBYTE	m_newFlag;
			NWORD	m_pagesNum;
			char	m_title[60];
			char	m_author[30];
		} BookOpen;

		// PCK_DyeVat (0x95)
		struct  {
			NBYTE	m_cmd;
			NDWORD	m_itemId;
			NWORD	m_val1;
			NWORD	m_val2;
		} DyeVat;

		// PCK_Prompt (0x9A)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_objectId;
			NDWORD	m_promptNum;
			NDWORD	m_request;
			char	m_text;
		} Prompt;

		// PCK_HelpPage (0x9b)
		struct 
		{
			NBYTE	m_cmd;
			char	m_text[257];
		} HelpPage;

		// PCK_VendorSell (0x9F)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_vendorId;
			NWORD	m_itemCount;

			struct {
				NDWORD	m_itemId;
				NWORD	m_quantity;
			} m_item;
		} VendorSell;

		// PCK_ServerSelect (0xA0)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_select;
		} ServerSelect;

		// PCK_Spy (0xA4)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_data[148];
		} Spy;

		// PCK_Scroll (0xA6)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_flag;
			NWORD	m_unknown1;
			NWORD	m_tip;
			NWORD	m_msgLen;

			// char	msg;
		} Scroll;

		// PCK_TipReq (0xA7)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_lastTip;
			NBYTE	m_flag;
		} TipReq;

		// PCK_GumpInpValRet (0xAC)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NWORD	m_context;
			NBYTE	m_retcode;
			NWORD	m_textlen;
			char	m_text;
		} GumpInpValRet;

		// PCK_TalkUNICODE (0xAD)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_type;
			NWORD	m_color;
			NWORD	m_font;
			NDWORD	m_Language;
			
			// NCHAR	m_uMessage;
		} TalkUNICODE;

		// PCK_GumpDialogRet (0xB1)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NDWORD	m_gumpId;
			NDWORD	m_choiceId;

			NDWORD	m_checkQty;
/*			NDWORD	m_checkIds;
			NDWORD	m_textQty;

			// GumpDialogReg_DATA
//			struct	GumpDialogRet_DATA	m_retData[1];
*/
		} GumpDialogRet;

		// PCK_ChatReq (0xb2)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_uid1;
			NDWORD	m_unk2;
			NDWORD	m_unk3;
		} ChatReq;

		// PCK_ChatText (0xb3)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_lang;
			NCHAR	m_uText;
		} ChatText;

		// PCK_Chat (0xb5)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_unk1;
			NCHAR	m_uname[31];
		} Chat;

		// PCK_ToolTipReq (0xb6)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NBYTE	m_langId;
			char	m_lang[3];
		} ToolTipReq;

		// PCK_CharProfile (0xB8)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_WriteMode;
			NDWORD	m_id;
			NBYTE	m_unk1;
			NBYTE	m_retcode;
			NWORD	m_textLen;
			NCHAR	m_uText;
		} CharProfile;

		// PCK_MailMsg (0xBB)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id1;
			NDWORD	m_id2;
		} MailMsg;

		// PCK_ClientVersion (0xBD)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			char	m_text;
		} ClientVersion;

		// PCK_ExtData (0xBF)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_type;

//			ExtraData	m_msg;
		} ExtData;
	};

} STRUCT_PACKED;

typedef	struct ClientPack_s	sClientPacket;

struct  ServerPacket_s
{
	union
	{
		NBYTE	m_raw[MAX_PACKET_LEN];

		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
		} Default;

		// PCK_Status (0x11)
		struct 
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
		} Status;	

		// PCK_Follow (0x15)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_master;
			NDWORD	m_slave;
		} Follow;

		// PCK_Put (0x1a)
		struct 
		{
			NBYTE	m_cmd;					//
			NWORD	m_len;					//
			NDWORD	m_itemId;				//
			NWORD	m_model;				// #
			NWORD	m_itemCount;			// if (itemID & 0x80000000) (or model # for corpses)
			NBYTE	m_incrCounter;			// if (model & 0x8000) (increment model by this #)
			NWORD	m_x;					// (only use lowest significant 15 bits)
			NWORD	m_y;					//
			NBYTE	m_dir;					// if (xLoc & 0x8000)
			NBYTE	m_z;					//
			NWORD	m_dye;					// if (yLoc & 0x8000)
			NBYTE	m_flag;					// if (yLoc & 0x4000)
		} Put;

		#define ITEMF_MOVABLE       0x20
	    #define ITEMF_INVIS         0x80
		// PCK_Start (0x1B)
		struct 
		{
			NBYTE	m_cmd;					// 0
			NDWORD	m_id;					// 1
			NDWORD	m_unknown1;				// 5
			NWORD	m_bodyType;				// 9
			NWORD	m_x;					// 11
			NWORD	m_y;					// 13
			NWORD	m_z;					// 15
			NBYTE	m_dir;					// 17
			NWORD	m_unknown2;				// 18
			NDWORD	m_unknown3;				// 20
			NDWORD	m_unknown4;				// 24
			NBYTE	m_charMode;				// 28
			NBYTE	m_highlightColor;		// 29
			NBYTE	m_unknown[7];			// 30
		} Start;

		// Char mode flags
		#define CHARMODE_POISON             0x04    // green status bar.
		#define CHARMODE_YELLOW             0x08    // ? yellow status bar.
		#define CHARMODE_WAR                0x40
		#define CHARMODE_INVIS              0x80

		// PCK_Speak (0x1C)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NWORD	m_model;
			NBYTE	m_type;
			NWORD	m_color;
			NWORD	m_font;
			char	name[30];
//			char	message;
		} Speak;

		// PCK_Remove (0x1D)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
		} Remove;

		// PCK_View (0x20)
		struct 
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
		} View;

		// PCK_WalkCancel (0x21)
		struct
		{
			NBYTE	m_cmd;
			NBYTE	m_seq;
			NWORD	m_x;
			NWORD	m_y;
			NBYTE	m_dir;
			NBYTE	m_z;
		} WalkCancel;

		// PCK_WalkAck (0x22)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_count;
			NBYTE	m_zero;
		} WalkAck;

		// PCK_DrawAnim (0x23)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_model;
			NBYTE	m_unknown1[3];
			NWORD	m_stackCount;
			NDWORD	m_sourceId;
			NWORD	m_srcX;
			NWORD	m_srcY;
			NBYTE	m_srcZ;
			NDWORD	m_targetId;
			NWORD	m_targetX;
			NWORD	m_targetY;
			NBYTE	m_targetZ;
		} DrawAnim;

		// PCK_ContOpen (0x24)
		struct
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_gump;
		} ContOpen;

		// PCK_ContAdd (0x25)
		struct 
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
		} ContAdd;

		// PCK_Kick (0x26)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_zero2;
		} Kick;

		// PCK_DragCancel (0x27)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_type;
		} DragCancel;

		// PCK_ClearSquare (0x28)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_x;
			NWORD	m_y;
		} ClearSquare;

		// PCK_ObjectDropped (0x29)
		struct 
		{
			NBYTE	m_cmd;
		} ObjectDropped;

		// PCK_DeathMenu (0x2c)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_shift;
		} DeathMenu;

		// PCK_ItemEquip (0x2e)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_itemID;
			NWORD	m_model;
			NBYTE	m_zero1;
			NBYTE	m_layer;
			NDWORD	m_playerId;
			NWORD	m_hue;
		} ItemEquip;

		// PCK_Fight (0x2F)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_dir;
			NDWORD	m_AttackerId;
			NDWORD	m_VictimId;
		} Fight;

		// PCK_Pause (0x33)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_arg;
		} Pause;

		// PCK_PathFind (0x38)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_x;
			NWORD	m_y;
			NWORD	m_z;
		} PathFind;

		// PCK_ChangeGroup (0x39)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_OwnerId;
			NDWORD	m_charId;
		} ChangeGroup;

		// PCK_Skill (0x3A)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_single;

			// Here Skill_DATA
		} Skill;

		// PCK_VendorBuy (0x3b)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_vendorId;
			NBYTE	m_flag;
		} VendorBuy;

		// PCK_Content (0x3C)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_count;

			// Content_DATA
		} Content;

		// PCK_LightPoint (0x4e)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NBYTE	m_level;
		} LightPoint;

		// PCK_Light (0x4f)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_level;
		} Light;

		// PCK_ClientState (0x53)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_type;
		} ClientState;

		// PCK_Sound (0x54)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_flags;
			NWORD	m_sound;
			NWORD	m_volume;
			NWORD	m_x;
			NWORD	m_y;
			NWORD	m_z;
		} Sound;

		// PCK_Redraw (0x55)
		struct 
		{
			NBYTE	m_cmd;
		} Redraw;

		// PCK_MapEdit (0x56)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NBYTE	m_mode;
			NBYTE	m_req;
			NWORD	m_pin_x;
			NWORD	m_pin_y;
		} MapEdit;

		// PCK_Time (0x5b)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_hour;
			NBYTE	m_min;
			NBYTE	m_sec;
		} Time;

		// PCK_Weather (0x65)
		struct
		{
			NBYTE	m_cmd;
			NBYTE	m_type;
			NBYTE	m_num;
			NBYTE	m_temperature;
		} Weather;

		// PCK_BookPage (0x66)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_bookId;
			NWORD	m_pagesNum;

			struct  
			{
				NWORD	m_page;
				NWORD	m_lines;
				char	m_text;
			} m_pages;
		} BookPage;

		// PCK_Options (0x69)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_index;
		} Options;

		// PCK_Target (0x6C)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_type;
			NDWORD	m_cursorID;
			NBYTE	m_checkCrime;	// 0

			// Not used
			NDWORD	m_id;
			NWORD	m_x;
			NWORD	m_y;
			NBYTE	m_unk;
			NBYTE	m_z;
			NWORD	m_tileId;
		} Target;

		// PCK_PlayMusic (0x6d)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_musicId;
		} PlayMusic;

		// PCK_CharAction (0x6e)
		struct 
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
		} CharAction;

		// PCK_SecureTrade (0x6F)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_action;	// SECURE_TRADE_TYPE
			NDWORD	m_id;
			NDWORD	m_containerId1;
			NDWORD	m_containerId2;
			NBYTE	m_withName;
			char	m_charName;
		} SecureTrade;

		// PCK_Effect (0x70)
        //Graphical Effect (28 bytes)
        //� BYTE cmd
        //� BYTE direction type
        //� 00 = go from source to dest
        //� 01 = lightning strike at source
        //� 02 = stay at current x,y,z
        //� 03 = stay with current source character id 
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_dirType;
			NDWORD	m_srcId;
			NDWORD	m_targetId;
			NWORD	m_model;
			NWORD	m_x;
			NWORD	m_y;
			NBYTE	m_z;
			NWORD	m_targetX;
			NWORD	m_targetY;
			NBYTE	m_targetZ;
			NBYTE	m_animSpeed;
			NBYTE	m_duration;
			NWORD	m_hue;
			NBYTE	m_adjustDirection;
			NBYTE	m_explode;
		} Effect;

		// PCK_BBoard (0x71)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_flag;
			NDWORD	m_id;
			NBYTE	m_data;
		} BBoard;

		// PCK_War (0x72)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_warMode;
			NBYTE	m_unk[3];		// 00 32 00
		} War;

		// PCK_VendOpenBuy (0x74)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_vendorId;

			struct 
			{
				NDWORD		m_price;
				NBYTE		m_length;
				char		m_name;
			} m_items;
		} VendOpenBuy;

		// PCK_ZoneChange (0x76)
		// New Subserver (16 bytes)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_x;
			NWORD	m_y;
			NWORD   m_z;
			NBYTE	m_unk;
			NWORD	m_boundry_x;
			NWORD	m_boundry_y;
			NWORD	m_boundry_width;
			NWORD	m_boundry_height;
		} ZoneChange;

		// PCK_CharMove (0x77)
		struct 
		{
			NBYTE	m_cmd;			// 0
			NDWORD	m_id;			// 1
			NWORD	m_model;		// 5 
			NWORD	m_x;			// 7
			NWORD	m_y;			// 9
			NBYTE	m_z;			// 11
			NBYTE	m_dir;			// 12
			NWORD	m_hue;			// 13
			NBYTE	m_flag;			// 15
			NBYTE	m_hColor;		// 16
		} CharMove;

		// PCK_Char (0x78)
		struct 
		{
			NBYTE	m_cmd;			// 0
			NWORD	m_len;			// 1
			NDWORD	m_id;			// 3
			NWORD	m_bodyType;		// 7
			NWORD	m_x;			// 9
			NWORD	m_y;			// 11
			NBYTE	m_z;			// 13
			NBYTE	m_dir;			// 14
			NWORD	m_hue;			// 15
			NBYTE	m_flags;		// 17
			NBYTE	m_noto;			// 18

			// Char_Equip_DATA
		} Char;

		// PCK_MenuItems (0x7C)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NWORD	m_context;
			NBYTE	m_lenname;
			char	m_name;		// Variable length name
			NBYTE	m_count;
			struct 
			{
				NWORD	m_model;
				NWORD	m_check;
				NBYTE	m_lentext;
				char	name;
			} m_item;
		} MenuItems;

		// PCK_CharList3 (0x81)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_count;
			NBYTE	m_unk;

			struct 
			{
				char	m_name[30];
				char	m_pass[30];
			} m_names;
		} CharList3;

		// PCK_LogBad (0x82)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_code;
		} LogBad;

		// PCK_DeleteBad (0x85)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_code;
		} DeleteBad;

		// PCK_CharList2 (0x86)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_count;

			struct 
			{
				char	m_name[30];
				char	m_pass[30];
			} m_names;
		} CharList2;

		// PCK_PaperDoll (0x88)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			char	m_text[60];
			NBYTE	m_mode;
		} PaperDoll;

		// PCK_CorpEquip (0x89)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;

			struct 
			{
				NBYTE	m_layer;
				NDWORD	m_itemId;
			} m_item;
		} CorpEquip;

		// PCK_GumpTextDisp (0x8b)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NWORD	m_gump;
			NWORD	m_len_unktext;
			NWORD	m_unknown1;
			char	m_unknownText;
			char	m_text;
		} GumpTextDisp;

		// PCK_Relay (0x8c)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_ip;
			NWORD	m_port;
			NDWORD	m_Account;
		} Relay;

		// PCK_MapDisplay (0x90)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_Gump_Corner;
			NWORD	m_x_ul;
			NWORD	m_y_ul;
			NWORD	m_x_lr;
			NWORD	m_y_lr;
			NWORD	m_xsize;
			NWORD	m_ysize;
		} MapDisplay;

		// PCK_EditMultiMul (0x92)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NDWORD	m_tag;

			struct 
			{
				NWORD	m_id;
				NWORD	m_dx;
				NWORD	m_dy;
				NWORD	m_dz;
				NDWORD	m_flags;
			} m_item;
		} EditMultiMul;

		// PCK_BookOpen (0x93)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_bookId;
			NBYTE	m_writeFlag;
			NBYTE	m_newFlag;
			NWORD	m_pagesNum;
			char	m_title[60];
			char	m_author[30];
		} BookOpen;

		// PCK_EditSkillsMul (0x94)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_index;
			NDWORD	m_tag;
			NBYTE	m_button;
			char	m_skillName;
		} EditSkillsMul;

		// PCK_DyeVat (0x95)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_zero5;
			NWORD	m_sid;
		} DyeVat;

		// PCK_WalkForce (0x97)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_dir;
		} WalkForce;

		// PCK_TargetMulti (0x99)
		struct 
		{
			NBYTE	m_cmd;       // Bring up house/board placement View
			NBYTE	m_req;       // 0x01 from server, 0x00 from client
			NDWORD	m_deedId;    // of deed
			NDWORD	m_zero1;     // unknown (all 0)
			NDWORD	m_zero2;     // unknown (all 0)
			NDWORD	m_zero3;     // unknown (all 0)
			NWORD	m_model;     // item model - 0x4000
			NWORD	m_zero4;     // all 0
			NDWORD	m_zero5;     // all 0
		} TargetMulti;

		// PCK_Prompt (0x9a)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	unk3;
			char	m_text;
		} Prompt;

		// PCK_VendOpenSell (0x9e)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_vendorId;

			struct 
			{
				NDWORD	m_id;
				NWORD	m_model;
				NWORD	m_hue;
				NWORD	m_amount;
				NWORD	m_value;
				NWORD	m_nameLen;
				char	m_name;
			} m_item;

		} VendOpenSell;

		// PCK_StatChngStr (0xa1)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_max;
			NWORD	m_val;
		} StatChngStr;

		// PCK_StatChngInt (0xa2)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_max;
			NWORD	m_val;
		} StatChngInt;

		// PCK_StatChngDex (0xa3)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
			NWORD	m_max;
			NWORD	m_val;
		} StatChngDex;

		// PCK_Web (0xa5)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			char	m_page;
		} Web;

		// PCK_Scroll (0xA6)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_type;
			NDWORD	m_context;
			NWORD	m_lentext;
			char	m_text;
		} Scroll;


		// PCK_ServerList (0xA8)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_unk3;		// 0x14
			NWORD	m_count;

			// Additional server information (ServerList_DATA) here
		} ServerList;

		// PCK_CharList (0xa9)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_count;

			struct	
			{
				char	m_name[30];
				char	m_pass[30];
			} m_names[5];

			NBYTE	m_startCount;

			// CharList_Location_DATA
		} CharList;

		// PCK_AttackOK (0xaa)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_id;
		} AttackOK;

		// PCK_GumpInpVal (0xab)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NBYTE	m_parentId;
			NBYTE	m_buttonId;
			NBYTE	m_textLen;
			char	m_text;
			NBYTE	m_cancel;
			NBYTE	m_style;
			NDWORD	m_format;
			NBYTE	m_text2Len;
			char	m_text2;
		} GumpInpVal;

		// PCK_SpeakUNICODE (0xae)
		struct 
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

			// Here goes unicode message
//			NCHAR	m_msg;
		} SpeakUNICODE;

		// PCK_CharDeath (0xaf)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_playerId;
			NDWORD	m_corpseId;
			NDWORD	m_zero1;
		} CharDeath;

		// PCK_GumpDialog (0xb0)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD  m_id;
			NDWORD	m_gumpId;
			NDWORD	m_x;
			NDWORD	m_y;
			NWORD	m_cmdSelLen;

			/*char	command;

			NBYTE	m_zeroterm;
			NWORD	m_textlines;

			struct 
			{
				NWORD	m_len;
				NCHAR	m_uText;
			} m_texts; */
		} GumpDialog;

		// PCK_ChatReq (0xb2)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_subcmd;
			NDWORD	m_lang;
			NCHAR	m_uname;
			NCHAR	m_uname2;
		} ChatReq;

		// PCK_TargetItems (0xb4)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_allowGround;
			NDWORD	m_targetId;
			NWORD	m_x;
			NWORD	m_y;
			NWORD	m_z;
			NWORD	m_count;

			struct 
			{
				NWORD	m_id;
				NWORD	m_dx;
				NWORD	m_dy;
				NWORD	m_dz;
				NWORD	m_unk;
			} m_item;
		} TargetItems;

		// PCK_ToolTip (0xb5)
		struct
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NCHAR	m_uText;
		} ToolTip;

		// PCK_CharProfile (0xb7)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			char	title;
			NCHAR	m_uText1;
			NCHAR	m_uText2;
		} CharProfile;

		// PCK_ChatEnable (0xb9)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_enable;
		} ChatEnable;

		// PCK_Arrow (0xba)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_active;
			NWORD	m_x;
			NWORD	m_y;
		} Arrow;

		// PCK_Season (0xbc)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_season;
			NBYTE	m_cursor;
		} Season;

		// PCK_ExtData (0xbf)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NWORD	m_type;

//			ExtraData	m_extData;
		} ExtData;

		// PCK_SpeakTable (0xc1)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NDWORD	m_id;
			NWORD	m_body;
			NBYTE	m_mode;
			NWORD	m_hue;
			NWORD	m_font;
			NDWORD	m_index;
			char	m_charName[20];
		} SpeakTable;

		// PCK_ParticleEffect (0xc7)
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_motion;
			NDWORD	m_id;
			NDWORD	m_targetId;
			NWORD	m_model;
			NWORD	m_x;
			NWORD	m_y;
			NBYTE	m_z;
			NWORD	m_dstX;
			NWORD	m_dstY;
			NBYTE	m_dstZ;
			NBYTE	m_speed;
			NBYTE	m_loop;
			NWORD	m_hue;
			NBYTE	m_oneDir;
			NBYTE	m_explode;

			NBYTE	m_unknown1;

			NWORD	m_particleId;
			NBYTE	m_unknown2;
			NDWORD	m_charId;
			NBYTE	m_unknown3;
		} ParticleEffect;

		// PCK_UnkUpdateRange (0xc8)
        //Note: client+server packet.
        //Range: how far away client wants to see (=get send) items/npcs
        //Update: since client 3.0.8o it actually got activated.(in a useful way)
        //When increase/decrease macro send, client sends a 0xc8.
        //If and only if server "relays" the packet
        //(sending back the same data) range stuff gets activated client side.
        //"Greying" has no packet, purely client internal.
        //Minimal value:5, maximal: 18
		struct 
		{
			NBYTE	m_cmd;
			NBYTE	m_range;
		} UnkUpdateRange;

		// PCK_AOSObjProp (0xdc)
		struct 
		{
			NBYTE	m_cmd;
			NDWORD	m_serial;
			NDWORD	m_hash;
		} AOSObjProp;

		// PCK_MYUO (0xfe)
		struct 
		{
			NBYTE	m_cmd;
			NWORD	m_len;
			NBYTE	m_eCmd;
		} MyUo;
	};

	Uint32	m_pos;

} STRUCT_PACKED;

typedef	struct ServerPacket_s	sServerPacket;

//////////////////////////////////////////////////////////////////////
// Additional packet structures (for complex packets)
//////////////////////////////////////////////////////////////////////
struct	ServerList_DATA
{
				NWORD	m_index;
				char	m_servname[32];
				NBYTE	m_percFull;
				NBYTE	m_timeZone;
				NDWORD	m_pingIp;
} STRUCT_PACKED;

struct CharList_Location_DATA
{
NBYTE	m_index;
char	m_town[31];
char	m_name[31];
} STRUCT_PACKED;

struct Skill_DATA
{
		NWORD	m_index;
		NWORD	m_val;
		NWORD	m_valraw;
		NBYTE	m_lock;
} STRUCT_PACKED;

struct Char_Equip_DATA
{
		NDWORD	m_id;
		NWORD	m_model;
		NBYTE	m_layer;
//		NWORD	m_hue;
//		ONLY if m_id | 0x8000
} STRUCT_PACKED;

struct  Content_DATA
{
		NDWORD	m_id;
		NWORD	m_model;
		NBYTE	m_zero1;
		NWORD	m_amount;
		NWORD	m_x;
		NWORD	m_y;
		NDWORD	m_containerId;
		NWORD	m_Hue;
} STRUCT_PACKED;

struct	GumpDialogRet_DATA
{
		NWORD	m_id;
		NWORD	m_len;
//		NCHAR	m_utext;
} STRUCT_PACKED;

// Turn off structure packing.
#ifdef WIN32
#pragma pack()
#endif
    
#endif
