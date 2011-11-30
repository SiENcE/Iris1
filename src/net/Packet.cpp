//
// File: Packet.cpp
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


#ifdef WIN32
#include <windows.h>
#endif

#include <SDL/SDL_endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "Config.h"
#include "net/Packet.h"




using namespace std;

Uint16 packet_lens[256];
bool table_initialised = false;

void InitTables ()
{
  memset (packet_lens, 0, 256);
  packet_lens[PCK_Create] = 0x68;
  packet_lens[PCK_Disconnect] = 0x05;
  if ( Config::GetIsSphere() )
    packet_lens[PCK_Walk] = 0x03;
  else
    packet_lens[PCK_Walk] = 0x07;
  packet_lens[PCK_Talk] = 0x00;
  packet_lens[PCK_GodMode] = 0x02;
  packet_lens[PCK_Attack] = 0x05;
  packet_lens[PCK_DClick] = 0x05;
  packet_lens[PCK_ItemPickupReq] = 0x07;
  packet_lens[PCK_ItemDropReq] = 0x0E;
  packet_lens[PCK_Click] = 0x05;
  packet_lens[PCK_EditItem] = 0x0B;
  packet_lens[PCK_EditArea] = 0x10A;
  packet_lens[PCK_EditTileData] = 0x00;
  packet_lens[PCK_EditNPC] = 0x03;
  packet_lens[PCK_EditTemplate] = 0x00;
  packet_lens[PCK_EditPaperdoll] = 0x3D;
  packet_lens[PCK_EditHues] = 0xD7;
  packet_lens[PCK_Status] = 0x00;
  packet_lens[PCK_DoAction] = 0x00;
  packet_lens[PCK_ItemEquipReq] = 0x0A;
  packet_lens[PCK_GodLevitate] = 0x06;
  packet_lens[PCK_Follow] = 0x09;
  packet_lens[PCK_ScriptsGet] = 0x01;
  packet_lens[PCK_ScriptsExe] = 0x00;
  packet_lens[PCK_ScriptsAdd] = 0x00;
  packet_lens[PCK_UnkSpeakNPC] = 0x00;
  packet_lens[PCK_Put] = 0x00;
  packet_lens[PCK_Start] = 0x25;
  packet_lens[PCK_Speak] = 0x00;
  packet_lens[PCK_Delete] = 0x05;
  packet_lens[PCK_UnkAnimate] = 0x04;
  packet_lens[PCK_UnkExplode] = 0x08;
  packet_lens[PCK_View] = 0x13;
  packet_lens[PCK_WalkCancel] = 0x08;
  packet_lens[PCK_WalkAck] = 0x03;
  packet_lens[PCK_DragAnim] = 0x1A;
  packet_lens[PCK_ContOpen] = 0x07;
  packet_lens[PCK_ContAdd] = 0x14;
  packet_lens[PCK_Kick] = 0x05;
  packet_lens[PCK_DragCancel] = 0x02;
  packet_lens[PCK_ClearSquare] = 0x05;
  packet_lens[PCK_ObjectDropped] = 0x01;
  packet_lens[PCK_UnkBlood] = 0x05;
  packet_lens[PCK_GodModeOK] = 0x02;
  packet_lens[PCK_DeathMenu] = 0x02;
  packet_lens[PCK_UnkHealth] = 0x11;
  packet_lens[PCK_ItemEquip] = 0x0F;
  packet_lens[PCK_Fight] = 0x0A;
  packet_lens[PCK_UnkAttackOK] = 0x05;
  packet_lens[PCK_UnkPeace] = 0x01;
  packet_lens[PCK_UnkHackMove] = 0x02;
  packet_lens[PCK_Pause] = 0x02;
  packet_lens[PCK_CharStatReq] = 0x0A;
  packet_lens[PCK_EditResType] = 0x28D;
  packet_lens[PCK_EditResTiledata] = 0x00;
  packet_lens[PCK_UnkMoveObject] = 0x08;
  packet_lens[PCK_PathFind] = 0x07;
  packet_lens[PCK_ChangeGroup] = 0x09;
  packet_lens[PCK_Skill] = 0x00;
  packet_lens[PCK_VendorBuy] = 0x00;
  packet_lens[PCK_Content] = 0x00;
  packet_lens[PCK_UnkShip] = 0x02;
  packet_lens[PCK_UnkVersions] = 0x25;
  packet_lens[PCK_EditUpdateObj] = 0x00;
  packet_lens[PCK_EditUpdateTerrain] = 0xC9;
  packet_lens[PCK_EditUpdateTiledata] = 0x00;
  packet_lens[PCK_EditUpdateArt] = 0x00;
  packet_lens[PCK_EditUpdateAnim] = 0x229;
  packet_lens[PCK_EditUpdateHues] = 0x2C9;
  packet_lens[PCK_UnkVersionOK] = 0x05;
  packet_lens[PCK_EditNewArt] = 0x00;
  packet_lens[PCK_EditNewTerrain] = 0x0B;
  packet_lens[PCK_EditNewAnim] = 0x49;
  packet_lens[PCK_EditNewHues] = 0x5D;
  packet_lens[PCK_UnkDestroyArt] = 0x05;
  packet_lens[PCK_UnkCheckVersion] = 0x09;
  packet_lens[PCK__ScriptsNames] = 0x00;
  packet_lens[PCK_ScriptsFile] = 0x00;
  packet_lens[PCK_LightPoint] = 0x06;
  packet_lens[PCK_Light] = 0x02;
  packet_lens[PCK_UnkBBHeader] = 0x00;
  packet_lens[PCK_UnkBBMessage] = 0x00;
  packet_lens[PCK_UnkPostMsg] = 0x00;
  packet_lens[PCK_ClientState] = 0x02;
  packet_lens[PCK_Sound] = 0x0C;
  packet_lens[PCK_ReDrawAll] = 0x01;
  packet_lens[PCK_MapEdit] = 0x0B;
  packet_lens[PCK_UnkRegionsUpdate] = 0x6E;
  packet_lens[PCK_UnkRegionsNew] = 0x6A;
  packet_lens[PCK_UnkEffectNew] = 0x00;
  packet_lens[PCK_EffectUpdate] = 0x1C;
  packet_lens[PCK_Time] = 0x04;
  packet_lens[PCK_UnkVersionRestart] = 0x02;
  packet_lens[PCK_CharPlay] = 0x49;
  packet_lens[PCK_UnkServerList] = 0x00;
  packet_lens[PCK_UnkServerAdd] = 0x31;
  packet_lens[PCK_UnkServerDel] = 0x05;
  packet_lens[PCK_UnkStaticDel] = 0x09;
  packet_lens[PCK_UnkStaticMove] = 0x0F;
  packet_lens[PCK_UnkLoadArea] = 0x0D;
  packet_lens[PCK_UnkLoadAreaTry] = 0x01;
  packet_lens[PCK_Weather] = 0x04;
  packet_lens[PCK_BookPage] = 0x00;
  packet_lens[PCK_UnkSimped] = 0x15;
  packet_lens[PCK_UnkAddLSScript] = 0x00;

  if ( Config::GetIsSphere() )
    packet_lens[PCK_Options] = 0x04;
  else
    packet_lens[PCK_Options] = 0x05;

  packet_lens[PCK_UnkFriendNotify] = 0x03;
  packet_lens[PCK_UnkUseKey] = 0x09;
  packet_lens[PCK_Target] = 0x13;
  packet_lens[PCK_PlayMusic] = 0x03;
  packet_lens[PCK_CharAction] = 0x0E;
  packet_lens[PCK_SecureTrade] = 0x00;
  packet_lens[PCK_Effect] = 0x1C;
  packet_lens[PCK_BBoard] = 0x00;
  packet_lens[PCK_War] = 0x05;
  packet_lens[PCK_Ping] = 0x02;
  packet_lens[PCK_VendOpenBuy] = 0x00;
  packet_lens[PCK_CharName] = 0x23;
  packet_lens[PCK_ZoneChange] = 0x10;
  packet_lens[PCK_CharMove] = 0x11;
  packet_lens[PCK_Char] = 0x00;
  packet_lens[PCK_UnkResourceGet] = 0x09;
  packet_lens[PCK_UnkResourceData] = 0x00;
  packet_lens[PCK_UnkSequence] = 0x02;
  packet_lens[PCK_MenuItems] = 0x00;
  packet_lens[PCK_MenuChoice] = 0x0D;
  packet_lens[PCK_GodGetView] = 0x02;
  packet_lens[PCK_GodViewInfo] = 0x00;
  packet_lens[PCK_ServersReq] = 0x3E;
  packet_lens[PCK_CharList3] = 0x00;
  packet_lens[PCK_LogBad] = 0x02;
  packet_lens[PCK_CharDelete] = 0x27;
  packet_lens[PCK_UnkPasswordChange] = 0x45;
  packet_lens[PCK_DeleteBad] = 0x02;
  packet_lens[PCK_CharList2] = 0x00;
  packet_lens[PCK_UnkResourceSend] = 0x00;
  packet_lens[PCK_PaperDoll] = 0x42;
  packet_lens[PCK_CorpEquip] = 0x00;
  packet_lens[PCK_EditTrigger] = 0x00;
  packet_lens[PCK_GumpTextDisp] = 0x00;
  packet_lens[PCK_Relay] = 0x0B;
  packet_lens[PCK_Unused8d] = 0x00;
  packet_lens[PCK_UnkCharMove] = 0x00;
  packet_lens[PCK_Unused8f] = 0x00;
  packet_lens[PCK_MapDisplay] = 0x13;
  packet_lens[PCK_CharListReq] = 0x41;
  packet_lens[PCK_EditMultiMul] = 0x00;
  packet_lens[PCK_BookOpen] = 0x63;
  packet_lens[PCK_EditSkillsMul] = 0x00;
  packet_lens[PCK_DyeVat] = 0x09;
  packet_lens[PCK_GodGameMon] = 0x00;
  packet_lens[PCK_WalkForce] = 0x02;
  packet_lens[PCK_UnkChangeName] = 0x00;
  packet_lens[PCK_TargetMulti] = 0x1A;
  packet_lens[PCK_Prompt] = 0x00;
  packet_lens[PCK_HelpPage] = 0x102;
  packet_lens[PCK_GodAssist] = 0x135;
  packet_lens[PCK_GodSingle] = 0x33;
  packet_lens[PCK_VendOpenSell] = 0x00;
  packet_lens[PCK_VendorSell] = 0x00;
  packet_lens[PCK_ServerSelect] = 0x03;
  packet_lens[PCK_StatChngStr] = 0x09;
  packet_lens[PCK_StatChngInt] = 0x09;
  packet_lens[PCK_StatChngDex] = 0x09;
  packet_lens[PCK_Spy] = 0x95;
  packet_lens[PCK_Web] = 0x00;
  packet_lens[PCK_Scroll] = 0x00;
  packet_lens[PCK_TipReq] = 0x04;
  packet_lens[PCK_ServerList] = 0x00;
  packet_lens[PCK_CharList] = 0x00;
  packet_lens[PCK_AttackOK] = 0x05;
  packet_lens[PCK_GumpInpVal] = 0x00;
  packet_lens[PCK_GumpInpValRet] = 0x00;
  packet_lens[PCK_TalkUNICODE] = 0x00;
  packet_lens[PCK_SpeakUNICODE] = 0x00;
  packet_lens[PCK_CharDeath] = 0x0D;
  packet_lens[PCK_GumpDialog] = 0x00;
  packet_lens[PCK_GumpDialogRet] = 0x00;
  packet_lens[PCK_ChatReq] = 0x00;
  packet_lens[PCK_ChatText] = 0x00;
  packet_lens[PCK_TargetItems] = 0x00;
  packet_lens[PCK_Chat] = 0x40;
  packet_lens[PCK_ToolTipReq] = 0x09;
  packet_lens[PCK_ToolTip] = 0x00;
  packet_lens[PCK_CharProfile] = 0x00;
  packet_lens[PCK_ChatEnable] = 0x03;
  packet_lens[PCK_Arrow] = 0x06;
  packet_lens[PCK_MailMsg] = 0x09;
  
  /*
  if ( Config::GetIsSphere55R() )
    packet_lens[PCK_Season] = 0x02;
  else
  */

	packet_lens[PCK_Season] = 0x03;
  packet_lens[PCK_ClientVersion] = 0x00;
  packet_lens[PCK_UnkVersionAssist] = 0x00;
  packet_lens[PCK_ExtData] = 0x00;
  packet_lens[PCK_UnkHuedEffect] = 0x24;
  packet_lens[PCK_SpeakTable] = 0x00;
  packet_lens[PCK_UnkSpeakTableU] = 0x00;
  packet_lens[PCK_UnkGQEffect] = 0x00;
  packet_lens[PCK_UnkSemiVisible] = 0x06;
  packet_lens[PCK_UnkInvalidMap] = 0xCB;
  packet_lens[PCK_UnkEnableInvalidMap] = 0x01;
  packet_lens[PCK_ParticleEffect] = 0x31;
  packet_lens[PCK_UnkUpdateRange] = 0x02;
  packet_lens[PCK_UnkTripTime] = 0x06;
  packet_lens[PCK_UnkUTripTime] = 0x06;
  packet_lens[PCK_UnkGQCount] = 0x07;
  packet_lens[PCK_UnkTextIDandStr] = 0x00;
  packet_lens[0xCD] = 0x01;
  packet_lens[0xCE] = 0x00;
  packet_lens[0xCF] = 0x4e;
  packet_lens[0xD0] = 0x00;
  packet_lens[0xD1] = 0x02;
  packet_lens[0xD2] = 0x19;
  packet_lens[0xD3] = 0x00;
  packet_lens[0xD4] = 0x00;
  packet_lens[0xD6] = 0x00;
  packet_lens[0x6a] = 0x00;

  // introduced with Age of Shadows Expansion
  packet_lens[PCK_AOSObjProp] = 0x09;
}


cPacket::cPacket ()
{
  if (!table_initialised)
      {
        table_initialised = true;
        InitTables ();
      }
  Reset ();
  memset (&packet, 0, sizeof packet);
}

Uint16 bswap16 (Uint16 val)
{
#if SDL_BYTEORDER==SDL_LIL_ENDIAN
  return SDL_Swap16 (val);
  /*
     Uint8 *src = (Uint8*) &val;
     Uint16 res;
     Uint8 * dst = (Uint8*) &res;
     *dst = *(src + 1);
     *(dst + 1) = *src;
     return res;
   */
#else
  return val;
#endif
}

Uint32 bswap32 (Uint32 val)
{
#if SDL_BYTEORDER==SDL_LIL_ENDIAN
  return SDL_Swap32 (val);
  /*
     Uint8 *src = (Uint8*) &val;
     Uint32 res;
     Uint8 * dst = (Uint8*) &res;
     for (int i=0; i < 4; i++) 
     *(dst + i) = *(src + 3 - i);
     return res;
   */
#else
  return val;
#endif
}


cPacket::~cPacket ()
{
}

void cPacket::Clear (void)
{
  memset (&packet, 0, sizeof packet);
}

void cPacket::Reset (void)
{
  m_length = 0;
  m_pos = 0;
}

void cPacket::AddByte (Uint8 item)
{
  if (m_length + 1 >= MAX_PACKET_LEN)
    return;
  packet.m_raw[m_length++] = item;
}

void cPacket::AddWord (Uint16 item)
{
  if (m_length + 2 >= MAX_PACKET_LEN)
    return;
  *((Uint16 *) & packet.m_raw[m_length]) = bswap16 (item);
  m_length += 2;
}

void cPacket::AddDword (Uint32 item)
{
  if (m_length + 4 >= MAX_PACKET_LEN)
    return;
  *((Uint32 *) & packet.m_raw[m_length]) = bswap32 (item);
  m_length += 4;
}

void cPacket::AddData (void *data, int len)
{
  if (m_length + len >= MAX_PACKET_LEN)
    return;
  memcpy (&packet.m_raw[m_length], data, len);
  m_length += len;
}

Uint8 cPacket::GetByte ()
{
  if (m_length + 1 >= MAX_PACKET_LEN)
    return 0;
  return packet.m_raw[m_pos++];
}

Uint16 cPacket::GetWord ()
{
  if (m_length + 2 >= MAX_PACKET_LEN)
    return 0;
  Uint16 v = *((Uint16 *) & packet.m_raw[m_pos]);
  //v = bswap16( v);
  m_pos += 2;
  return bswap16 (v);
}

Uint32 cPacket::GetDword ()
{
  if (m_length + 4 >= MAX_PACKET_LEN)
    return 0;

  Uint32 v = *((Uint32 *) & packet.m_raw[m_pos]);
  //v = bswap32( v);
  m_pos += 4;
  return bswap32 (v);
}

void cPacket::GetData (void *data, int len)
{
  if (m_length + len >= MAX_PACKET_LEN)
      {
        memset (data, 0, len);
        return;
      }
  memcpy (data, &packet.m_raw[m_pos], len);
  m_pos += len;
}

int cPacket::ParsePacket (void *data, unsigned int len, int direction)
{
  if ((!data) || (len < 1))
    return -1;

  Uint8 cmd = *((Uint8 *) data);

  unsigned int real_len = packet_lens[cmd];

  if (!real_len && (len < 3))
    return -1;

  if (!real_len)
      {
        Reset ();
        AddData (data, 3);
        m_pos = 1;
        real_len = GetWord ();
      }

  if ((real_len > len) || (real_len > MAX_PACKET_LEN))
    return -(int) real_len;

  Reset ();
  Clear ();

  AddData (data, real_len);
  Reset ();

  m_length = real_len;

  return real_len;
}

void cPacket::FillPacket (Uint8 type, unsigned int len)
{
  Reset ();
  Clear ();
  packet.Default.m_cmd = type;
  if (packet_lens[type])
    packet.Default.m_len = packet_lens[type];
  else
    packet.Default.m_len = len;
  m_pos = 3;
  m_length = packet.Default.m_len;
  if (!m_length)
    m_pos = 1;
}
