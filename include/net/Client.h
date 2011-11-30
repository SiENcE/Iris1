//
// File: Client.h
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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <cassert>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "Common.h"
#include "Packet.h"
#include "uo_huffman.h"
#include "renderer/Characters.h"
#include "Game.h"


#include "net/uo_huffman.h"
#include "net/netconfig.h"

#include "renderer/DynamicObjects.h"
#include "renderer/Characters.h"
#include "renderer/MapBuffer.h"

#include "gui/GUIHandler.h"
#include "gui/Container.h"
#include "gui/Label.h"
#include "gui/Border.h"
#include "gui/Button.h"
#include "gui/Image.h"
#include "gui/InputField.h"
#include "gui/ImageArt.h"
#include "gui/MultiLabel.h"
#include "gui/HTMLGumpParser.h"
#include "gui/Checkbox.h"
#include "gui/RadioButton.h"
#include "loaders/ClilocLoader.h"
#include "loaders/MapInfo.h"
#include "loaders/UOMap.h"
#include "string_utils.h"


#include "loaders/TileDataLoader.h"
#include "loaders/MultisLoader.h"
#include "loaders/SpeechLoader.h"
#include "renderer/3D/MapBlock3D.h"
#include "renderer/3D/MapBuffer3D.h"
#include "renderer/Renderer.h"

#include "Config.h"
#include "Logger.h"
#include "Exception.h"
#include "SynchronizedTime.h"

#include "net/unicode.h"

#include "sound/SoundMixer.h"


#include "csl/CSLHandler.h"

#include "renderer/particles/ParticleEngine.h"


// #include "../Fluid/mmgr.h"

#define WALK_LEFT    0
#define WALK_RIGHT   1
#define WALK_FORWARD 2

struct sNewCharSettings {
	char name[31];
	Uint8 sex;
	Uint8 stat[3];
	Uint8 skill[3];
	Uint8 skill_val[3];
	Uint16 skin_color;
	Uint16 hair;
	Uint16 hair_color;
	Uint16 facial_hair;
	Uint16 facial_hair_color;
	Uint16 starting_location;
	Uint16 slot;
	Uint16 pants_color;
	Uint16 shirt_color;
};

struct sWalkStackEntry {
	Uint8 requestid;
	int x, y, z;
	Uint8 direction;
};

class cLoginListEntry {
private:
	Uint32 m_id;
	std::string m_name;
public:
	cLoginListEntry (Uint32 id, std::string name) 
	{
		m_id = id; 
		m_name = name;
	}
	Uint32 id () { return m_id; }
	std::string name () { return m_name; }
};

class cClient
{
private:
	TCPsocket socket;
	SDLNet_SocketSet socketset;
	DecompressingCopier * copier;
    int m_popupx, m_popupy;

	int state;
	Uint32 player_char;
	Uint8 walk_sequence;
	Uint8 walk_direction;
	bool decompress;
	bool m_warmode;
    
    Uint32 enemy;  

	int player_position[3];

	bool connected;
	bool in_game;
    
    Uint32 last_footstep_sound; // time (in ticks) of last footstep sound
    
        Uint32 last_object;
        Uint32 last_target;
        Uint32 last_attack;
        
        int last_spell;
        int last_skill;
        std::string m_wait_for_target;

	std::vector <struct sWalkStackEntry> walk_stack;
	std::vector <cLoginListEntry *> login_char_list;
	std::vector <cLoginListEntry *> login_location_list;
	std::vector <cLoginListEntry *> login_server_list;
    
    std::vector <Uint16> popup_entries;
    std::vector <Uint16> popup_tags; 
        
	/* handlers for incoming packets */
	void Act_ServerList(cPacket * packet);
	void Act_BadLog(cPacket * packet);
	void Act_ClientState(cPacket * packet);
	void Act_Relay(cPacket * packet);
	void Act_CharList(cPacket * packet);
	void Act_Char(cPacket * packet);
	void Act_Start(cPacket * packet);
	void Act_Speak(cPacket * packet);
	void Act_SpeakUnicode(cPacket * packet);
	void Act_View(cPacket * packet);
	void Act_Put(cPacket * packet);
	void Act_WalkCancel(cPacket * packet);
	void Act_WalkAck(cPacket * packet);
	void Act_Light(cPacket * packet);
	void Act_War(cPacket * packet);
	void Act_Delete(cPacket * packet);
	void Act_Status(cPacket * packet);
	void Act_StatChange(cPacket * packet);
	void Act_Content(cPacket * packet);
	void Act_ContOpen(cPacket * packet);
	void Act_Skill(cPacket * packet);
	void Act_CharMove(cPacket * packet);
	void Act_ItemEquip(cPacket * packet);
	void Act_DragCancel(cPacket * packet);
	void Act_Target(cPacket * packet);
	void Act_MenuItems(cPacket * packet);
	void Act_GumpDialog(cPacket * packet);
	void Act_MapDisplay(cPacket * packet);
	void Act_Paperdoll(cPacket * packet);
	void Act_AttackOK(cPacket * packet);
	void Act_Dye(cPacket * packet);
    void Act_ContAdd(cPacket * packet);
    void Act_Sound(cPacket * packet);
    void Act_CharAction(cPacket * packet);
    void Act_SubCommands(cPacket * packet); 
	void Act_CorpEquip(cPacket * packet);
    void Act_SpeakTable(cPacket * packet);
    void Act_VendOpenBuy(cPacket * packet);
    void Act_VendOpenSell(cPacket * packet);
    void Act_VendorBuy(cPacket * packet);
    void Act_SecureTrade(cPacket * packet);
    void Act_PlayMusic(cPacket * packet);
    void Act_AOSTooltip(cPacket * packet);
    void Act_CharDeath(cPacket * packet);
    void Act_Time(cPacket * packet);
    void Act_ParticleEffect(cPacket * packet);
  
    int GetZPositionForWalk(int destx, int desty, int srcz);
	
	void ClearLoginLists();
	
	char data_buffer[MAX_PACKET_LEN];
	int data_buffer_pos;
    
    
    std::vector <std::string> menu_entries;
    std::vector <int> menu_models;
    
    std::map <int, int> buylist;
    std::map <int, int> selllist;

public:
    cClient (void (*error_callback) (unsigned int error));
   ~cClient ();
   
   void OnData(void * data, unsigned int len);

   bool Connect (char * address, Uint16 port);
   void Disconnect ();
   Uint32 Get_Local_IP();

//SiENcE:new
   int ready();

   void Poll ();

   void Send (cPacket * packet);
   void Send (void * data, Uint32 len);
   
   void CastSpell(int spellid);
   void Send_SelectServer(Uint32 index);
   void Send_RequestChars(Uint32 account);
   void Send_SelectChar(Uint8 index);
   void Send_CreateChar(sNewCharSettings settings);
   void Send_DeleteChar(Uint32 id);
   bool Send_Walk(Uint8 direction);
   void Send_Speech(std::string text, Uint8 mode = 0);
   void Send_SpeechUNICODE(std::string text, Uint8 mode = 0);
   void Send_CharName(Uint32 id, std::string name);
   void Send_StatusRequest(Uint32 id, Uint8 mode = 0);
   void Send_Dye(Uint32 id, Uint16 model, Uint16 color);
   void Send_DoubleClick(Uint32 id);

   void Send_AoSCommand(Uint32 charid,Uint8 subcmd);
   void Send_Click(Uint32 id);
   void Send_Warmode(Uint32 mode);
   void Send_Action(int type, int mode = 0);
   void Send_AttackRequest(Uint32 id);
   void Send_SkillLock(Uint32 skill, Uint32 lock);
   void Send_PickupRequest(Uint32 id, Uint16 count = 1);
   void Send_DropRequest(Uint32 id, int x, int y, int z, Uint32 container = 0xffffffff);
   void Send_Target(Uint32 cursorid, int x, int y, int z, int artid);
   void Send_Target(Uint32 cursorid, Uint32 targetid);
   void Send_ItemEquipReq(Uint32 itemid, Uint32 charid, Uint16 model);
   void Send_MenuChoice(Uint32 dialogid, Uint16 menuid, Uint16 index, Uint16 model);
   void SendGumpDialogRet(int gumpID, int playerID, int ret_value);
   void SendSecureTradeCheck(Uint32 id, Uint32 check1, Uint32 check2);
   
   void Send_PopupRequest(Uint32 serial, int x, int y);
   void SendPopupChoice(Uint16 tag);
   Uint16 GetPopupEntry(int index);
   Uint16 GetPopupTag(int index);
   
   void toggleRunMode();

   bool Walk_Simple (Uint8 action);
   bool Walk(Uint8 direction);
   
   bool warmode () { return m_warmode; }

   /* Callbacks */
   void OnGameStart(void (*callback) (void));
   void OnTeleport(void (*callback) (int x, int y, int z));
   void OnServerList(void (*callback) (int count));
   void OnCharList(void (*callback) (int count));
   void OnNetError(void (*callback) (unsigned int error));
   void OnLightLevel(void (*callback) (unsigned int lightlevel));
   void OnSpeech(void (*callback) (const char * text, const char * speaker, unsigned int speakerid,unsigned short color));
   void OnStatus(void (*callback) (unsigned int charid, Uint8 flag));
   void OnStatChange(void (*callback) (unsigned int charid, unsigned int type, unsigned int value, unsigned int maxvalue));
   void OnContainerContent(void (*callback) (unsigned int containerid));
   void OnOpenContainer(void (*callback) (unsigned int containerid, unsigned int gumpid));
   void OnSkill(void (*callback) (unsigned int skillid, unsigned int value));
   void OnDragCancel(void (*callback) (void));
   void OnTarget(void (*callback) (unsigned int cursorid, unsigned int type));
   void OnPaperdoll(void (*callback) (unsigned int charid, const char * text, Uint8 flag));
   void OnAttackReply(void (*callback) (unsigned int charid));
   void OnDyeWindow(void (*callback) (unsigned int charid, unsigned int model));
   void OnWarmode(void (*callback) (unsigned int mode));
   void OnMenuItems(void (*callback) (unsigned int id, unsigned int context, unsigned int count, char * name));
   void OnPopupDisplay(void (*callback) (int count, int x, int y)); 
   void OnOpenSpellBook(void (*callback) (Uint32 serial, Uint16 model, Uint8 * circles));
   void OnBuyWindowOpen(void(*callback) (Uint32 id));
   void OnBuyWindowAdd(void(*callback) (Uint32 id, int model, int count, Uint16 hue, Uint32 price, char * name));
   void OnSellWindowOpen(void(*callback) (Uint32 id));
   void OnSellWindowAdd(void(*callback) (Uint32 id, Uint16 model, Uint16 count, Uint16 hue, Uint16 price, char * name));
   void OnTradeStart(void (*callabck) (Uint32 id1, Uint32 id2, char * name));
   void OnTradeCheck(void (*callback) (Uint32 check1, Uint32 check2, int closewin));
   void OnTradeAdd (void(*callback) (Uint16 model, Uint32 tradeid, Uint16 count, Uint16 hue, Uint16 x, Uint16 y));
   
   
   void GetPlayerPosition (int &x, int &y, int &z) {
		x = player_position[0];
		y = player_position[1];
		z = player_position[2];
   }

   cCharacter * player_character();
   Uint32 player_charid() { return player_char; }
   
   cLoginListEntry * GetServerListEntry (Uint32 index);
   cLoginListEntry * GetLocationListEntry (Uint32 index);
   cLoginListEntry * GetCharListEntry (Uint32 index);
   
   void GetMenuItem (int index, std::string & text, int & model);

   void updateBuyList(int mode, int itemid = 0, int amount = 1);
   void updateSellList(int mode, int itemid = 0, int amount = 1);
   void buy();
   void sell(Uint32 id);

  Uint32 GetEnemy();
  
  Uint32 lastObject(){return last_object;}
  Uint32 lastTarget(){return last_target;}
  int lastSpell(){return last_spell;}
  int lastSkill(){return last_skill;}
  int lastAttack(){return last_attack;}
  void wait_for_target(std::string func){m_wait_for_target = func;} 

protected:

	/* callbacks */
	void (*callback_OnGameStart) (void);
	void (*callback_OnTeleport) (int x, int y, int z);
	void (*callback_OnServerList) (int count);
	void (*callback_OnCharList) (int count);
	void (*callback_OnNetError) (unsigned int error);
	void (*callback_OnLightLevel) (unsigned int lightlevel);
	void (*callback_OnSpeech) (const char * text, const char * speaker, unsigned int speakerid, unsigned short color);
	void (*callback_OnStatus) (unsigned int charid, Uint8 flag);
	void (*callback_OnStatChange) (unsigned int charid, unsigned int type, unsigned int value, unsigned int maxvalue);
	void (*callback_OnContainerContent) (unsigned int containerid);
	void (*callback_OnOpenContainer) (unsigned int containerid, unsigned int gumpid);
	void (*callback_OnSkill) (unsigned int skillid, unsigned int value);
	void (*callback_OnDragCancel) (void);
	void (*callback_OnPaperdoll) (unsigned int charid, const char * text, Uint8 flag);
	void (*callback_OnTarget) (unsigned int cursorid, unsigned int type);
	void (*callback_OnAttackReply) (unsigned int charid);
	void (*callback_OnDyeWindow) (unsigned int charid, unsigned int model);
	void (*callback_OnWarmode) (unsigned int mod);
	void (*callback_OnMenuItems) (unsigned int id, unsigned int context, unsigned int count, char * name);
    void (*callback_OnPopupDisplay) (int count, int x, int y); 
    void (*callback_OnOpenSpellBook) (Uint32 serial, Uint16 model, Uint8 * circles);
    void (*callback_OnBuyWindowOpen) (Uint32 id);
        void (*callback_OnBuyWinAdd) (Uint32 id, int model, int count, Uint16 hue, Uint32 price, char * name);
        void (*callback_OnSellWindowOpen) (Uint32 id);
        void (*callback_OnSellWinAdd) (Uint32 id, Uint16 model, Uint16 count, Uint16 hue, Uint16 price, char * name);
        void (*callback_OnTradeStart) (Uint32 id1, Uint32 id2, char * name);
        void (*callback_OnTradeCheck) (Uint32 check1, Uint32 check2, int closewin);
        void (*callback_OnTradeAdd) (Uint16 model, Uint32 tradeid, Uint16 count, Uint16 hue, Uint16 x, Uint16 y);
};

extern cClient * pClient;

#endif //_CLIENT_H_
