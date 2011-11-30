/*
 * File: Client.cpp
 * Created by Alexander Oster.
 * Changed: 10-01-04 (Florian Fischer^SiENcE)
 * Changed: 17-02-06 (Nuno Ramiro)
 * last Change: 13-04-06 (Florian Fischer^SiENcE)
 */

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

#include "net/Client.h"


cClient *pClient = NULL;
int spellbooktype = 2;

int actual_map = 0;

int direction_worldcoords[8][2] =
  { {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1} };

Uint32 popserial = 0;

Uint32 corpse_id = 0;
std::map < Uint32, Uint8 > corpse_equip;
bool buy_opening = false;
bool backpack_opened = false;
Uint32 vendor_id = 0;
Uint32 trade_1 = 0;
Uint32 trade_2 = 0;
int runflag=0;


int HandleGumpDialogEvent (Control * contr)
{
  Container *dlg = contr->GetParent ();
  Button *bt = dynamic_cast < Button * >(contr);
  if (bt->isCloseGump ())
      {
        int player_id = (Uint32) dlg->GetPlayerID ();
        int gump_id = (Uint32) dlg->GetGumpID ();
        int retval = (Uint32) bt->GetReturnMsg ();
        pClient->SendGumpDialogRet (gump_id, player_id, retval);
        pUOGUI.CloseWindow (dlg->GetID ());
        Logger::WriteLine ("CLOSE");
      }
  else if (bt->IsPageSelector ())
      {
        dlg->SetCurrentPage (bt->GetDestinationPage ());
        Logger::WriteLine ("PAGE");
        Logger::WriteLine ("UFF");
      }
  return -1;
}

// This shouldn't be hardcoded -> XML-File
Uint16 CheckIfBoat(Uint16 modelID)
{
 Uint16 new_modid;
 if((modelID >= 0x4000) && (modelID < 0x4022))
 {
  Logger::WriteLine("BOAT ModelID: " + modelID);
  switch(modelID)
  {
   //Small boat
   case 0x4000: new_modid = 16093; break;
   case 0x4001: new_modid = 15962; break;
   case 0x4002: new_modid = 16098; break;
   case 0x4003: new_modid = 15980; break;

   //Small Dragon Boat
   case 0x4004: new_modid = 16093; break;
   case 0x4005: new_modid = 15962; break;
   case 0x4006: new_modid = 16098; break;
   case 0x4007: new_modid = 15980; break;

   //Medium Boat
   case 0x4008: new_modid = 16093; break;
   case 0x4009: new_modid = 15962; break;
   case 0x400A: new_modid = 16098; break;
   case 0x400B: new_modid = 15980; break;

   //Median Dragon Dragon Boat
   case 0x400C: new_modid = 16093; break;
   case 0x400D: new_modid = 15962; break;
   case 0x400E: new_modid = 16098; break;
   case 0x400F: new_modid = 15980; break;

   //Large Boat
   case 0x4010: new_modid = 16093; break;
   case 0x4011: new_modid = 15962; break;
   case 0x4012: new_modid = 16098; break;
   case 0x4013: new_modid = 15980; break;

   //Large Dragon Boat
   case 0x4014: new_modid = 16093; break;
   case 0x4015: new_modid = 15962; break;
   case 0x4016: new_modid = 16098; break;
   case 0x4017: new_modid = 15980; break;
   default: new_modid = modelID;
  }
  return new_modid;
 }
 else
 {return modelID;}
}


bool IsInBoat(int posx, int posy)
{
 dynamiclist_t * dynamics = pDynamicObjectList.GetList();
  	dynamiclist_t::iterator iter;

  	for (iter = dynamics->begin(); iter != dynamics->end(); iter++)
    {
     cDynamicObject * object = iter->second;
      switch (object->model)
         {
          case 16093:
          case 16098:
             if(((posx >= object->x - 2) && (posx <= object->x + 2)) && ((posy >= object->y - 3) && (posy <= object->y + 3)) )
              {
               return true;
              }
                   break;
          case 15962:
          case 15980:
             if(((posx >= object->x - 3) && (posx <= object->x + 3)) && ((posy >= object->y - 2) && (posy <= object->y + 2)) )
              {
              return true;
              }
                   break;
         }
    }
    return false;
}

Sint8 CheckBoatZ(int posx, int posy)
{
 dynamiclist_t * dynamics = pDynamicObjectList.GetList();
  	dynamiclist_t::iterator iter;

  	for (iter = dynamics->begin(); iter != dynamics->end(); iter++)
    {
     cDynamicObject * object = iter->second;
      switch (object->model)
         {
          case 16093:
          case 16098:
             if(((posx >= object->x - 2) && (posx <= object->x + 2)) && ((posy >= object->y - 3) && (posy <= object->y + 3)) )
              {
               return object->z;
              }
                   break;
          case 15962:
          case 15980:
             if(((posx >= object->x - 3) && (posx <= object->x + 3)) && ((posy >= object->y - 2) && (posy <= object->y + 2)) )
              {
              return object->z;
              }
                   break;
         }
    }
    return 0;
}



Uint16 GetHighlightColor (Uint8 flag)
{
  Uint16 color;
  switch (flag)
      {
      case 1:
        color = 90;
        break;                  // blue
      case 2:
        color = 68;
        break;                  //green
      case 3:
        color = 905;
        break;                  //grey
      case 4:
        color = 44;
        break;                  //orange
      case 6:
        color = 38;
        break;                  // red
      default:
        color = 905;
        break;
      }
  return color;
}

void cClient::updateBuyList (int mode, int itemid, int amount)
{
  if (mode == 0)
      {
        buylist.clear ();
        return;
      }
  std::map < int, int >::iterator iter;
  if (mode == 1)
      {
        /*
           for(iter = buylist.begin(); iter != buylist.end(); iter++)//
           {
           if(iter->first == itemid)
           iter->second += amount; return;
           }
         */
        iter = buylist.find (itemid);
        if (iter != buylist.end ())
            {
              iter->second += amount;
              std::cout << "BUYLIST AMT: " << iter->second << endl;
              return;
            }
        buylist.insert (make_pair (itemid, amount));

      }
  else if (mode == 2)
      {
        iter = buylist.find (itemid);
        if (iter != buylist.end ())
            {
              if (iter->second > 1)
                  {
                    iter->second -= amount;
                    std::cout << "BUYLIST AMT: " << iter->second << endl;
                    return;
                  }
              else
                buylist.erase (iter);
            }                   //buylist.insert(make_pair(itemid, amount))
      }
}

//
void cClient::updateSellList (int mode, int itemid, int amount)
{
  if (mode == 0)
      {
        selllist.clear ();
        return;
      }
  std::map < int, int >::iterator iter;
  if (mode == 1)
      {
        /*
           for(iter = buylist.begin(); iter != buylist.end(); iter++)//
           {
           if(iter->first == itemid)
           iter->second += amount; return;
           }
         */
        iter = selllist.find (itemid);
        if (iter != selllist.end ())
            {
              iter->second += amount;
              std::cout << "SELLLIST AMT: " << iter->second << endl;
              return;
            }
        selllist.insert (make_pair (itemid, amount));

      }
  else if (mode == 2)
      {
        iter = selllist.find (itemid);
        if (iter != selllist.end ())
            {
              if (iter->second > 1)
                  {
                    iter->second -= amount;
                    std::cout << "SELLLIST AMT: " << iter->second << endl;
                    return;
                  }
              else
                selllist.erase (iter);
            }                   //buylist.insert(make_pair(itemid, amount))
      }
}

void cClient::buy ()
{
  cPacket packet;
  packet.AddByte (0x3B);
  int len = 8 + (buylist.size () * 7);
  packet.AddWord ((Uint16) len);
  packet.AddDword (vendor_id);
  packet.AddByte (2);
  std::map < int, int >::iterator iter;
  for (iter = buylist.begin (); iter != buylist.end (); iter++)
      {
        packet.AddByte (0x1A);
        packet.AddDword ((Uint32) iter->first);
        packet.AddWord ((Uint16) iter->second);
      }

  Send (&packet);
}

void cClient::sell (Uint32 id)
{
  cPacket packet;
  packet.AddByte (0x9F);
  int len = 9 + (selllist.size () * 6);
  packet.AddWord ((Uint16) len);
  packet.AddDword (id);
  packet.AddWord ((Uint16) selllist.size ());
  std::map < int, int >::iterator iter;
  for (iter = selllist.begin (); iter != selllist.end (); iter++)
      {

        packet.AddDword ((Uint32) iter->first);
        packet.AddWord ((Uint16) iter->second);
      }

  Send (&packet);
}


cClient::cClient( void (*error_callback)(unsigned int error) )
{
	copier = new DecompressingCopier();
	socket = NULL;
	decompress = false;
	connected = false;
	walk_sequence = 0;
	walk_direction = 0;
	m_warmode = false;
	data_buffer_pos = 0;
	in_game = false;
	m_popupx = 0;
	m_popupy = 0;
	enemy = 0;
	last_footstep_sound = 0;

	last_spell = 0;
	last_object = 0;
	/* TODO (ArTiX#1#): Add checks for UOX3 */

	last_target = 0;
	last_skill = 0;
	last_attack = 0;
	m_wait_for_target = "";
/* This is now handled by WorldEnvironment.cpp */
/*	cMapInfoEntry * actualmap = MapInfoLoader::GetInstance()->GetMapInfo(0);

	if ( actualmap )
	{
		GLfloat fogColor[4] = {(float) actualmap->fog_r() / 255.0f, (float) actualmap->fog_g() / 255.0f,(float) actualmap->fog_b() / 255.0f, 1.0 };
		glFogfv(GL_FOG_COLOR, fogColor);
	} */

	callback_OnGameStart = NULL;
	callback_OnTeleport = NULL;
	callback_OnServerList = NULL;
	callback_OnCharList = NULL;
	callback_OnNetError = error_callback;
	callback_OnLightLevel = NULL;
	callback_OnStatus = NULL;
	callback_OnSpeech = NULL;
	callback_OnStatChange = NULL;
	callback_OnContainerContent = NULL;
	callback_OnOpenContainer = NULL;
	callback_OnSkill = NULL;
	callback_OnDragCancel = NULL;
	callback_OnTarget = NULL;
	callback_OnPaperdoll = NULL;
	callback_OnAttackReply = NULL;
	callback_OnDyeWindow = NULL;
	callback_OnWarmode = NULL;
	callback_OnPopupDisplay = NULL;
	callback_OnMenuItems = NULL;
	callback_OnOpenSpellBook = NULL;
	callback_OnBuyWindowOpen = NULL;
	callback_OnBuyWinAdd = NULL;
	callback_OnSellWindowOpen = NULL;
	callback_OnSellWinAdd = NULL;
	callback_OnTradeStart = NULL;
	callback_OnTradeCheck = NULL;
	callback_OnTradeAdd = NULL;

	player_char = 0;
	for (int i = 0; i < 3; i++)
		player_position[i] = 0;


	printf( "Connecting to %s:%d\n", (char *)Config::GetServer().c_str(), Config::GetServerPort() );
	if ( !Connect( (char *)Config::GetServer().c_str(), Config::GetServerPort() ) )
	{
		Logger::WriteLine ("NET | Could not Connect");
		return;
	}

	unsigned int sign = Config::GetClientKey();
	Send( &sign, 4 );

	cPacket packet;
	packet.FillPacket( PCK_ServersReq );
	strncpy( packet.packet.LoginRequest.m_username, Config::GetLogin().c_str(), 29);
	strncpy( packet.packet.LoginRequest.m_password, Config::GetPassword().c_str(), 29);
	packet.packet.LoginRequest.m_unknown = 0x5D;    //client 3.0.8S T2A sends 0x5D
	Send( &packet );
}


cClient::~cClient()
{
	if ( connected )
	{
		Disconnect();
	}
	walk_stack.clear();
	ClearLoginLists();
	SAFE_DELETE( copier );
}


void cClient::ClearLoginLists()
{
	unsigned int i;
	for ( i = 0; i < login_char_list.size(); i++ )
	{
		SAFE_DELETE( login_char_list[i] );
	}

	for ( i = 0; i < login_server_list.size(); i++ )
	{
		SAFE_DELETE( login_server_list[i] );
	}

	login_char_list.clear();
	login_server_list.clear();
}


bool cClient::Connect( char *address, Uint16 port )
{
	IPaddress ip;

	if ( SDLNet_ResolveHost( &ip, address, port ) == -1 )
	{
		Logger::WriteLine( "SDLNet_ResolveHost: " + std::string( SDLNet_GetError() ) );
		if ( callback_OnNetError )
		{
			callback_OnNetError( NETERROR_UNKNOWNHOST );
		}
		return false;
	}

	socketset = SDLNet_AllocSocketSet( 1 );
	if ( !socketset )
	{
		Logger::WriteLine( "SDLNet_AllocSocketSet: " + std::string( SDLNet_GetError() ) );
		if ( callback_OnNetError )
		{
			callback_OnNetError( NETERROR_SOCKET );
		}
		return false;
	}

	socket = SDLNet_TCP_Open( &ip );
	if ( !socket )
	{
		if ( callback_OnNetError )
		{
			callback_OnNetError( NETERROR_NOCONNECTION );
		}
		Logger::WriteLine( "SDLNet_TCP_Open: " + std::string( SDLNet_GetError() ) );
		SDLNet_FreeSocketSet( socketset );
		return false;
	}

	int numused = SDLNet_TCP_AddSocket( socketset, socket );
	if ( numused == -1 )
	{
		if ( callback_OnNetError )
		{
			callback_OnNetError( NETERROR_SOCKET );
		}
		Logger::WriteLine( "SDLNet_AddSocket: " + std::string( SDLNet_GetError() ) );
		SDLNet_FreeSocketSet( socketset );
		SDLNet_TCP_Close( socket );
		return false;
	}
	connected = true;
	return true;
}


void cClient::Disconnect()
{
	if ( !connected )
	{
		return;
	}
	SDLNet_TCP_Close( socket );
	SDLNet_FreeSocketSet( socketset );
	socket = NULL;

	connected = false;
}

/**
 * Check to see if bytes ready to be read.
 * @returns 0 = no bytes ready
 */
int cClient::ready()
{
    if (!socketset) return 0;

    SDLNet_CheckSockets(socketset,0);
    return SDLNet_SocketReady(socket);
}

void cClient::Poll()
{
    if (!connected) return;

    if ( ready() )
    {

//        if (network->read(act,1)!=-1)
//            serverHandler->handlePacket(act[0]);

        // This part should be rewritten sometime
        //-----------------------------------------------------------------------
        char packet[MAX_PACKET_LEN];

        bool do_poll = true;
        int poll_pos = 0;

        while ( do_poll )
        {
            do_poll = false;
            const int numready = SDLNet_CheckSockets( socketset, 0 );

            if ( numready == -1 )
            {
                Logger::WriteLine( "SDLNet_CheckSockets: " + std::string( SDLNet_GetError() ) );
                return;
            }

            if ( numready && SDLNet_SocketReady( socket ) )
            {
                int len = SDLNet_TCP_Recv( socket, packet + poll_pos, MAX_PACKET_LEN );
                if ( len <= 0 )
                {
                    if (len < 0) Logger::WriteLine( "SDLNet_TCP_Recv: " + std::string( SDLNet_GetError() ) );
                    break;
                }

                if ( len > 0 )
                {
                    do_poll = true;
                    poll_pos += len;
                }
            }
        }
        //-----------------------------------------------------------------------
        OnData( packet, poll_pos );
    }
}

void LogStream(void* data, int orig_len, bool dir)
{
	FILE *fp = fopen("packets.txt", "at");
	if(!fp) return;
	unsigned char * p = (unsigned char*) data;
	fprintf(fp, "Packet [%02x], Length: %i, Type: %s\n", *p, orig_len, (dir ? "Server" : "Client") );
	if(orig_len < 0) orig_len*=-1;
	for (int i = 0; i < orig_len; i++)
	{
		fprintf(fp, "%02x ", (Uint32)*p); p++;
		if (i % 16 == 15)
			fprintf(fp, "\n");
	}
	fprintf(fp, "\n\n");
	fclose(fp);
}

void cClient::OnData( void *data, unsigned int len )
{
	cPacket *packet = new cPacket();
	bool decompress_safe = decompress;
	void *uncompressed_data = data;

	//  int orig_len = len;
	int last_packet = 255;

	if (decompress)
	{
		//     int llen = len;
		uncompressed_data = malloc( MAX_PACKET_LEN );

		int dest_size = MAX_PACKET_LEN;
		int src_size = len;
		(*copier)( (char *)uncompressed_data, (char *)data, dest_size, src_size );
		len = dest_size;
	}

	if ( len + data_buffer_pos > MAX_PACKET_LEN )
	{
		Logger::WriteLine( "NET | Buffer overflow" );
		data_buffer_pos = 0;
	}


	memcpy( &data_buffer[data_buffer_pos], uncompressed_data, len );
	data_buffer_pos += len;

	len = data_buffer_pos;

	while ( data_buffer_pos > 0 )
	{
		char *p = (char *)data_buffer;
		int packet_len = packet->ParsePacket( p, len, PACKET_SERVER );

		// can be disabled
		LogStream( p, packet_len, true );

		if ( packet_len <= 0 )
		{
			SAFE_DELETE( packet );
			Logger::WriteLine( "NET | Warning: lost packet stream" );
			/*              printf
			("    Len: %d Needed Len: %d Cmd: %x (Last Packet: %x)\n",
			len, -packet_len, 0, last_packet);
			*/
			return;
		}

		switch ( packet->packet.Default.m_cmd )
		{
		case PCK_ServerList:
			Act_ServerList (packet);
			break;

		case PCK_LogBad:
			Act_BadLog( packet );
			break;

		case PCK_Relay:
			Act_Relay (packet);
			break;

		case PCK_CharList:
		case PCK_CharList2:
			Act_CharList (packet);
			break;

		case PCK_Char:
			Act_Char (packet);
			break;
		case PCK_Start:
			Act_Start (packet);
			break;

		case PCK_Speak:
			Act_Speak (packet);
			break;

		case PCK_SpeakUNICODE:
			Act_SpeakUnicode (packet);
			break;

		case PCK_Put:
			Act_Put (packet);
			break;

		case PCK_View:
			Act_View (packet);
			break;

		case PCK_WalkAck:
			Act_WalkAck (packet);
			break;

		case PCK_WalkCancel:
			Act_WalkCancel (packet);
			break;

		case PCK_Light:
			Act_Light (packet);
			break;

		case PCK_War:
			Act_War (packet);
			break;

		case PCK_Delete:
			Act_Delete (packet);
			break;

		case PCK_Status:
			Act_Status (packet);
			break;

		case PCK_Content:
			Act_Content (packet);
			break;

		case PCK_ContOpen:
			Act_ContOpen (packet);
			break;

		case PCK_Skill:
			Act_Skill( packet );
			break;

		case PCK_CharMove:
			Act_CharMove (packet);
			break;

		case PCK_ItemEquip:
			Act_ItemEquip (packet);
			break;

		case PCK_DragCancel:
			Act_DragCancel (packet);
			break;

		case PCK_Target:
			Act_Target (packet);
			break;

		case PCK_MenuItems:
			Act_MenuItems (packet);
			break;

		case PCK_GumpDialog:
			Act_GumpDialog (packet);
			break;

		case PCK_PaperDoll:
			Act_Paperdoll (packet);
			break;

		case PCK_DyeVat:
			Act_Dye (packet);
			break;

		case PCK_ContAdd:
			Act_ContAdd (packet);
			break;

		case PCK_Sound:
			Act_Sound (packet);
			break;

		case PCK_CharAction:
			Act_CharAction (packet);
			break;

		case PCK_ExtData:
			Act_SubCommands (packet);
			break;

		case PCK_AttackOK:
			Act_AttackOK (packet);
			break;

		case PCK_CharDeath:
			Act_CharDeath (packet);
			break;

		case PCK_StatChngStr:
		case PCK_StatChngDex:
		case PCK_StatChngInt:
			Act_StatChange (packet);
			break;

		case PCK_Pause:
			if (callback_OnGameStart && !in_game)
			{
				callback_OnGameStart ();
				in_game = true;
			}
			break;

		case PCK_CorpEquip:
			Act_CorpEquip (packet);
			break;

		case PCK_SpeakTable:
			Act_SpeakTable (packet);
			break;

		case PCK_SecureTrade:
			Act_SecureTrade (packet);
			break;

		case PCK_VendOpenBuy:
			Act_VendOpenBuy (packet);
			break;

		case PCK_VendOpenSell:
			Act_VendOpenSell (packet);
			break;

		case PCK_VendorBuy:
			Act_VendorBuy (packet);
			break;

		case PCK_PlayMusic:
			Act_PlayMusic (packet);
			break;

		case PCK_AOSTooltip:
			Act_AOSTooltip (packet);
			break;

		case PCK_ClientState:
			Act_ClientState (packet);
			break;

		case PCK_Ping:
			Send (packet);
			break;

		case PCK_ParticleEffect:
			Act_ParticleEffect(packet);
			break;

		case PCK_MapDisplay:
			Act_MapDisplay(packet);
			break;

		case PCK_Time:
			// This packet is sent at login
			Act_Time( packet );
			break;

		/*
		ToDo Packets.
		Ignore following packets
		*/
		case PCK_Effect:
		case PCK_EffectUpdate:
		case PCK_ZoneChange:
		case PCK_UnkUpdateRange:
		case PCK_TargetMulti:
		case PCK_Weather:
		case PCK_BookOpen:
		case PCK_Season:
		case PCK_AOSObjProp:
		case PCK_ReDrawAll:
		case PCK_ChatEnable:
		case PCK_Scroll:
		case PCK_LightPoint:
		case PCK_Options:
			break;

		default:
			printf( "Unknown Packet: %x (%i bytes), position %i (last packet: %x)\n",
				(int)packet->packet.Default.m_cmd, packet->len(), len, last_packet );
			break;
		}

		last_packet = (Uint8) packet->packet.Default.m_cmd;

		len -= packet_len;
		p += packet_len;
		data_buffer_pos -= packet_len;
		memcpy (data_buffer, p, MAX_PACKET_LEN - (p - data_buffer));
	  }

	  if ( decompress_safe )
	  {
		  free( uncompressed_data );
	  }
	  SAFE_DELETE( packet );
}

void cClient::Send (void *data, Uint32 len)
{
  if (!data || !connected)
    return;

  // can be disabled
  LogStream(data, len, false);

  int result = len;
  result = SDLNet_TCP_Send (socket, data, len);

  if (result != (int) len)
      {
        Logger::WriteLine ("SDLNet_TCP_Send: " + string (SDLNet_GetError ()));
        socket = NULL;
      }
}

void cClient::Send (cPacket * packet)
{
  if ((socket < 0) || (!packet))
    return;

  Send (&packet->packet, packet->len ());

}

void cClient::Act_SecureTrade (cPacket * packet)
{
  packet->SetPosition (3);
  Uint8 type = packet->GetByte ();
  if (type == 0)
      {

        Uint32 traderID = packet->GetDword ();
        Uint32 trade1 = packet->GetDword ();
        Uint32 trade2 = packet->GetDword ();
        Uint8 require_name = packet->GetByte ();

        char trader_name[30];

        packet->GetData (trader_name, 30);
        /*cCharacter * playerchar = player_character();
           std::string myname = playerchar->name(); */
        trade_1 = trade1;
        trade_2 = trade2;

        if (callback_OnTradeStart)
          callback_OnTradeStart (trade1, trade2, trader_name);
      }

  else if (type == 1)
      {

        Uint32 id2 = packet->GetDword ();
        if (callback_OnTradeCheck)
          callback_OnTradeCheck (0, 0, 1);
      }
  else if (type == 2)
      {

        Uint32 id = packet->GetDword ();
        Uint32 check1 = packet->GetDword ();
        Uint32 check2 = packet->GetDword ();
        if ( Config::GetIsPol() )
          Uint8 terminator = packet->GetByte ();
        if (callback_OnTradeCheck)
          callback_OnTradeCheck (check1, check2, 0);




      }
}

void cClient::Act_ServerList (cPacket * packet)
{
  if (!packet)
    return;

  sServerPacket *pack = packet->serverpacket ();

  packet->SetPosition (6);

  ServerList_DATA srv;
  char servername[33];

  ClearLoginLists ();

  for (int i = 0; i < pack->ServerList.m_count; i++)
      {
        packet->GetData (&srv, sizeof (ServerList_DATA));
        strncpy (servername, srv.m_servname, 32);
        login_server_list.
          push_back (new
                     cLoginListEntry ((Uint16) srv.m_index,

                                      string (servername)));
      }

  if (callback_OnServerList)
    callback_OnServerList (login_server_list.size ());
}

void cClient::Act_BadLog( cPacket *packet )
{
	if ( !packet )
	{
		return;
	}

	sServerPacket *pack = packet->serverpacket();

	char msg[128];
	sprintf( msg, "Login Denied: %i\n", pack->LogBad.m_code );
	Logger::WriteLine( msg );

	switch ( pack->LogBad.m_code )
	{
	case LOGINERR_NONE:
		callback_OnNetError( NETERROR_UNKNOWNUSER );
		break;

	case LOGINERR_ACCUSED:
		callback_OnNetError( NETERROR_ACCOUNTINUSE );
		break;

	case LOGINERR_BLOCKED:
		callback_OnNetError( NETERROR_ACCOUNTBLOCKED );
		break;

	case LOGINERR_BAD_PASS:
		callback_OnNetError( NETERROR_BADPASSWORD );
		break;

	default:
		callback_OnNetError( NETERROR_UNKNOWN );
	}

	Disconnect();
}

void cClient::Act_ClientState (cPacket * packet)
{
  if (!packet)

    return;

  sServerPacket *pack = packet->serverpacket ();

  char msg[128];
  sprintf (msg, "World entering Error: %i\n", pack->ClientState.m_type);
  Logger::WriteLine (msg);

/*  switch (pack->ClientState.m_type)
      {

      case NO_CHARACTER:
        callback_OnCharError (NO_CHARACTER);
        break;
      case CHAR_EXISTS:
        callback_OnCharError (CHAR_EXISTS);
        break;
      case CANT_CONNECT:
        callback_OnCharError (CANT_CONNECT);
        break;
      case CANT_CONNECT2:
        callback_OnCharError (CANT_CONNECT2);
        break;
      case CHAR_ALREAY_INUSE:
        callback_OnCharError (CHAR_ALREAY_INUSE);
        break;
      case LOGIN_PROBLEM:
        callback_OnCharError (LOGIN_PROBLEM);
        break;
      case IDLE:
        callback_OnCharError (IDLE);
        break;
      case CANT_CONNECT3:
        callback_OnCharError (CANT_CONNECT3);
        break;

      default:
        callback_OnNetError (NETERROR_UNKNOWN);
      }*/
  callback_OnNetError (NETERROR_UNKNOWN);
}

void cClient::Act_Relay (cPacket * packet)
{
  char ipstring[20];
  char *connip;
  Uint32 ip = bswap32 (packet->packet.Relay.m_ip);
  Uint16 port = packet->packet.Relay.m_port;
  Uint8 *byte_ip = (Uint8 *) & ip;
  sprintf (ipstring, "%d.%d.%d.%d", byte_ip[0], byte_ip[1], byte_ip[2],
           byte_ip[3]);
  printf ("Relay: %s:%d\n", ipstring, port);

  decompress = true;

  if ( !Config::GetIsRunUO() )
      {
        Disconnect ();


        if (ip == 0xffffffff )
          connip = (char *) Config::GetServer().c_str ();
        else
          connip = ipstring;
        if (!port)
          port = Config::GetServerPort();
        Connect (connip, port);
        if (!connected)
            {
              if (callback_OnNetError)
                callback_OnNetError (NETERROR_NOCONNECTION);
              Logger::WriteLine ("NET | Could not Reconnect");
              return;
            }

        unsigned int sign = Config::GetClientKey();
        Send (&sign, 4);
      }

  Send_RequestChars ((Uint32) packet->packet.Relay.m_Account);
}

void cClient::Act_CharList (cPacket * packet)
{
  if (!packet)
    return;

  char charname[31];
  char charpwd[31];
  Uint8 type;
  unsigned int i;
  charname[30] = 0;
  charpwd[30] = 0;

  ClearLoginLists ();


  packet->SetPosition (0);
  type = packet->GetByte ();
  packet->SetPosition (4);
  for (i = 0; i < 5; i++)
      {
        packet->GetData (charname, 30);
        packet->GetData (charpwd, 30);
        if (strlen (charname))
          login_char_list.
            push_back (new cLoginListEntry ((Uint16) i, string (charname)));
      }

  if (callback_OnCharList)
    callback_OnCharList (login_char_list.size ());

  if(type == PCK_CharList)
      {
	Uint8 id;
	char name[32];

    for ( i = 0; i < login_location_list.size(); i++ )
	  delete login_location_list[i];

	login_location_list.clear ();
	name[31] = 0;
	for (i = packet->GetByte (); i > 0; i--)
	    {
	      id = packet->GetByte ();
	      packet->GetData (name, 31);
	      login_location_list.
		push_back (new cLoginListEntry ((Uint16) id, string (name)));
	      packet->GetData (name, 31);
	    }
      }
}

void cClient::Act_Char (cPacket * packet)
{
  packet->SetPosition (3);
  Uint32 id = packet->GetDword ();
  Uint16 model = packet->GetWord ();
  Uint16 amount = 0;
  if (id & 0x80000000)
    amount = packet->GetWord ();
  id &= 0x7fffffff;
  Uint16 x = packet->GetWord ();
  Uint16 y = packet->GetWord ();
  Sint8 z = packet->GetByte ();
  Uint8 direction = packet->GetByte ();
  Uint16 skin = packet->GetWord () & 32767;
  Uint8 status = packet->GetByte ();
  Uint8 notoriety = packet->GetByte ();
  Uint32 itemID;

  if (id == player_char)
    walk_direction = direction;

        cCharacter *character = pCharacterList.Add (id, x, y, z, model);
        character->setDirection (direction);
        character->setHue (skin);
        character->setHighlightColor (GetHighlightColor (notoriety));
        character->setBody(model);

          while ((itemID = packet->GetDword ()))
              {
                Uint16 item_model = packet->GetWord ();
                Uint8 layer = packet->GetByte ();
                Uint16 item_hue = 0;
                if (item_model & 0x8000)
                  item_hue = packet->GetWord ();
                item_model &= 0x7fff;
                struct TileDataStaticEntry entry;
                if (pTileDataLoader.LoadEntry (item_model, &entry))
                    {
                      Uint16 anim = entry.animid;
                      cCharacterEquip *equip = character->AddEquip (layer);
                      equip->setID (itemID);
                      equip->setHue (item_hue);
                      equip->setModel (item_model);
                      equip->setAnim (entry.animid);

                      pDynamicObjectList.AddCharEquip (itemID, item_model,
                                                        item_hue, id);
                      //printf("   Equip (id:%x, model:%d, layer:%d, hue:%d, anim:%d, \"%s\")\n", equip->id(), equip->model(), equip->layer(), equip->hue(), equip->anim(), entry.name);
                    }


      }
}

void cClient::Act_Speak( cPacket *packet )
{
	char name[30];
	Uint16 len = (Uint16)packet->packet.Speak.m_len;
	char *msg = new char[len - 44 + 1];

	strncpy( name, packet->packet.Speak.name, 29 );
	strncpy( msg, &packet->packet.Speak.message, len - 44 );

	unsigned short color = packet->packet.Speak.m_color;

	if ( callback_OnSpeech )
	{
		callback_OnSpeech( msg, name, (Uint32)packet->packet.Speak.m_id, color );
	}

	// SAFE_DELETE( msg );
	SAFE_DELETE_ARRAY( msg );
}

void cClient::Act_SpeakUnicode (cPacket * packet)
{
  cUnicode uni ((NCHAR *) (&packet->packet.SpeakUnicode.m_msg));

  char name[31];
  name[30] = 0;
  memcpy (name, &packet->packet.SpeakUnicode.m_name, 30);

  if (callback_OnSpeech)
    callback_OnSpeech (uni.m_charBuf, name,
                       (Uint32) packet->packet.SpeakUnicode.m_id,
                       packet->packet.SpeakUnicode.m_color);
}

void cClient::Act_SpeakTable (cPacket * packet)
{
  if ( !Config::GetClilocs() )
    return;
  packet->SetPosition (1);
  Uint16 len = packet->GetWord ();

  Uint32 id = packet->GetDword ();
  Uint16 body = packet->GetWord ();
  Uint8 type = packet->GetByte ();
  Uint16 hue = packet->GetWord ();
  Uint16 font = packet->GetWord ();
  Uint32 msg = packet->GetDword ();

  char name[30];
  packet->GetData (name, 30);

  char *arguments;
  int arglen = len - 48;

  arguments = new char[arglen];

  packet->GetData (arguments, arglen);



  char *str2 = new char[arglen / 2];
  int idx2 = 0;
  for (int i = 0; i < arglen; i++)
      {
        if (arguments[i] != 0)
          str2[i / 2] = arguments[i];
      }
  str2[(arglen / 2) - 1] = 0;

  vector < std::string > arglist;
  std::string arg_string = std::string (str2);

  char tab[2];
  tab[0] = 9;
  tab[1] = 0;
  std::string delim = std::string (tab);

  stringSplit (arglist, arg_string, delim);

  std::string message =
    pClilocLoader.GetMessageWithArguments ((int) msg, (int) arglist.size (),
                                            arglist);

  Logger::WriteLine (message.c_str ());
  if (callback_OnSpeech)
    callback_OnSpeech (message.c_str (), name, (Uint32) id, hue);

  if (str2)
    delete[] str2;
  if (arguments)
    delete[] arguments;
}

void cClient::Act_Put (cPacket * packet)
{
  packet->SetPosition (3);
  Uint32 id = packet->GetDword ();
  Uint16 model = packet->GetWord ();
  model = CheckIfBoat(model);
  Uint16 itemcount = 0;
  if (id & 0x80000000)
    itemcount = packet->GetWord ();
  Uint8 incrcounter = 0;
  if (model & 0x8000)
    incrcounter = packet->GetByte ();
  Uint16 x = packet->GetWord ();
  Uint16 y = packet->GetWord ();
  Uint8 direction = 0;
  if (x & 0x8000)
    direction = packet->GetByte ();
  Sint8 z = packet->GetByte ();
  Uint16 dye = 0;
  if (y & 0x8000)
    dye = packet->GetWord ();
  Uint8 flag = 0;
  if (y & 0x4000)
    flag = packet->GetByte ();
  id &= 0x7fffffff;
  model &= 0x7fff;
  x &= 0x7fff;
  y &= 0x3fff;

  //Character died
  if (model == 0x2006)
      {
              cCharacter *character =  pCharacterList.Add (id, x, y, z, itemcount);
              character->setHue (dye & 32767);

            std::cout << "Corpse hue: " << dye << endl;

              character->setDirection (direction);
              character->setAsCorpse ();
      }

     if ((model>=MULTI_ID + 100) && pMultisLoader->MultiExists(model)
			 && !pDynamicObjectList.Get(id))
     {
        stMultiList* MultiList = pMultisLoader->GetMulti(model);
        MultiIter elem = MultiList->multiParts.begin();
        for(; elem!=MultiList->multiParts.end();elem++)
        {
          stMultiPart multi = *elem;
          int mx = x + multi.x;
          int my = y + multi.y;

          cMapblock3D *block = reinterpret_cast <cMapblock3D *>
               (pMapbufferHandler.buffer ()->CreateBlock (mx /8, my /8));
          if (block) block->AddMultiObject(id,multi.tile,dye,mx,my,z + multi.z);
        }
     }


    pDynamicObjectList.AddWorldItem (id, model, dye, x, y, z, itemcount,
                                      incrcounter, direction, flag);


}

void cClient::Act_View (cPacket * packet)
{
  Uint32 id = (Uint32) packet->packet.View.m_id;
        cCharacter *character =
          pCharacterList.Add (id, (Uint16) packet->packet.View.m_x,
                               (Uint16) packet->packet.View.m_y,
                               (Sint8) packet->packet.View.m_z,
                               (Uint16) packet->packet.View.m_bodyType);

        character->RotateTo((Uint8) packet->packet.View.m_dir);

        if(IsInBoat(packet->packet.View.m_x, packet->packet.View.m_y))
         character->setPosition ((Uint16) packet->packet.View.m_x,
                                (Uint16) packet->packet.View.m_y,
                                CheckBoatZ(packet->packet.View.m_x, packet->packet.View.m_y) + 10);
        else
         character->setPosition ((Uint16) packet->packet.View.m_x,
                                (Uint16) packet->packet.View.m_y,
                                (Sint8) packet->packet.View.m_z);

        walk_stack.clear ();
        walk_sequence = 0;

        if (callback_OnTeleport)
          callback_OnTeleport (character->x (), character->y (),
                               character->z ());

        player_char = id;

        player_position[0] = (Uint16) packet->packet.View.m_x;
        player_position[1] = (Uint16) packet->packet.View.m_y;
        player_position[2] = (Sint8) packet->packet.View.m_z;
        walk_direction = (Uint8) packet->packet.View.m_dir;
}

void cClient::Act_VendOpenBuy (cPacket * packet)
{
  packet->SetPosition (3);
  Uint32 inventory_id = packet->GetDword ();
  Uint8 item_count = packet->GetByte ();

  int incr = 1;
  buy_opening = true;

  if (callback_OnBuyWindowOpen)
    callback_OnBuyWindowOpen (inventory_id);

  dynamiclist_t *objlist = pDynamicObjectList.GetList ();
  dynamiclist_t::iterator iter;
  dynamiclist_t::iterator iter2;
  dynamiclist_t::iterator iter3;



  int model;

  Uint32 price;
  Uint8 namelen;
  int count;
  bool found = false;

  signed int i;
  if ( Config::GetIsPol() )
      {
        iter2 = objlist->end ();
        iter2--;
        iter3 = objlist->begin ();
        i = -1;
      }
  else
      {
        iter3 = objlist->end ();
        iter2 = objlist->begin ();
        i = 1;
      }


  for (iter = iter2; iter != iter3;)
      {

        cDynamicObject *object = iter->second;
        if (object)
            {
              if (object->parent == inventory_id)
                  {


                    model = object->model;
                    count = object->itemcount;

                    Uint16 hue = object->dye;

                    price = packet->GetDword ();
                    namelen = packet->GetByte ();

                    char name[257];


                    packet->GetData (name, namelen);
                    name[namelen] = 0;

                    if ((name[0] >= 0x30) && (name[0] <= 0x39))
                        {

                          int index = atoi (name);
                                std::string cliloc_name =
                                  pClilocLoader.GetMessage (index);

                                if (callback_OnBuyWinAdd)
                                  callback_OnBuyWinAdd (object->id, model,
                                                        count, hue, price,
                                                        (char *) cliloc_name.
                                                        c_str ());


                        }
                    else
                        {
                          if (callback_OnBuyWinAdd)
                            callback_OnBuyWinAdd (object->id, model, count,
                                                  hue, price, name);
                        }
                  }
            }
            if ( Config::GetIsPol() )
          iter--;
        else
          iter++;
      }

}

void cClient::Act_VendOpenSell (cPacket * packet)
{

  Logger::WriteLine ("Vendor SELL");
  packet->SetPosition (3);
  Uint32 inventory_id = packet->GetDword ();
  Uint16 item_count = packet->GetWord ();


  buy_opening = true;
  if (callback_OnSellWindowOpen)
    callback_OnSellWindowOpen (inventory_id);

  Uint32 id;
  Uint16 model;
  Uint16 count;
  Uint16 hue;
  Uint16 price;
  Uint16 namelen;
  char name[257];

  for (int i = 0; i < item_count; i++)
      {
        id = packet->GetDword ();
        model = packet->GetWord ();
        hue = packet->GetWord ();
        count = packet->GetWord ();
        price = packet->GetWord ();
        namelen = packet->GetWord ();
        if (namelen > 255)
          namelen = 255;

        packet->GetData (name, namelen);
        name[namelen] = 0;
        if ((name[0] >= 0x30) && (name[0] <= 0x39))
            {

              int index = atoi (name);
                    std::string cliloc_name =
                      pClilocLoader.GetMessage (index);
                    if (callback_OnSellWinAdd)
                      callback_OnSellWinAdd (id, model, count, hue, price,
                                             (char *) cliloc_name.c_str ());

            }
        else
            {
              if (callback_OnSellWinAdd)
                callback_OnSellWinAdd (id, model, count, hue, price, name);
            }
      }

}

void cClient::Act_VendorBuy (cPacket * packet)
{

  packet->SetPosition (3);
  Uint32 id = packet->GetDword ();
  Uint8 nonames = packet->GetByte ();
  if (id != vendor_id)
      {
        std::map < int, int >::iterator iter;

        for (iter = selllist.begin (); iter != selllist.end (); iter++)
            {
              cDynamicObject *object = pDynamicObjectList.Get (iter->first);
              if (object)
                  {
                    if (object->itemcount > iter->second)
                      object->itemcount -= iter->second;
                    else
                        {
                          pDynamicObjectList.Delete (iter->first);
                          pUOGUI.Rewind ();
                        }
                  }
            }
      }
  else
      {
        vendor_id = 0;
      }
}

void cClient::Act_CharDeath (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  cCharacter *character = pCharacterList.Get (id);
  character->displayDeath ();
}


void cClient::Act_Start (cPacket * packet)
{
	in_game = true;
	Uint32 id = (Uint32) packet->packet.Start.m_id;
	cCharacter *character =
		pCharacterList.Add (id, (Uint16) packet->packet.Start.m_x,
		(Uint16) packet->packet.Start.m_y,
		(Sint8) packet->packet.Start.m_z,
		(Uint16) packet->packet.Start.m_bodyType);
	character->setDirection ((Uint8) packet->packet.Start.m_dir);

	if (callback_OnGameStart)
		callback_OnGameStart ();

	player_char = id;

	player_position[0] = (Uint16) packet->packet.Start.m_x;
	player_position[1] = (Uint16) packet->packet.Start.m_y;
	player_position[2] = (Sint8) packet->packet.Start.m_z;
	walk_direction = (Uint8) packet->packet.Start.m_dir;

	if ( Config::GetClientVersion() != "none" )
	{
		std::string vers = Config::GetClientVersion();
		cPacket pckt;
		pckt.AddByte( 0xBD );
		pckt.AddWord( 4 + vers.size() );
		pckt.AddData( (void *)vers.c_str(), vers.size() );
		pckt.AddByte( 0 );
		Send( &pckt );
		std::string message = "Client identified as: " + vers;
		Logger::WriteLine( (char *)message.c_str() );
	}
}


void cClient::Act_WalkAck (cPacket * packet)
{
  if (walk_stack.size () == 0)
    return;

  packet->SetPosition (1);
  Uint8 sequence = packet->GetByte ();
  struct sWalkStackEntry entry = walk_stack[0];

  if (entry.requestid == sequence)
      {
        walk_stack.erase (walk_stack.begin (), walk_stack.begin () + 1);
              cCharacter *character = pCharacterList.Get (player_char);
              if (character)
                  {
                    character->MoveTo (entry.x, entry.y, entry.z);
                    character->RotateTo (entry.direction);
                    player_position[0] = entry.x;
                    player_position[1] = entry.y;
                    player_position[2] = entry.z;
                  }

        if ( Config::GetFootSteps() )
            {
              int footsteps_snd = 300;

              Uint32 current_ticks = SDL_GetTicks ();
              if (current_ticks - last_footstep_sound > 500)
                  {
                    last_footstep_sound = current_ticks;

                    cCharacter *mychar = player_character ();
                    cCharacterEquip *mount = mychar->GetEquip (25);
                    if (mount != NULL)
                      footsteps_snd = 298;
					if ( SoundMix::GetInstance() )
						SoundMix::GetInstance()->PlaySound (footsteps_snd, Config::GetSoundVolume(), (char) 0,
                                            (int) mychar->x (),
                                            (int) mychar->y (), mychar->z ());
                  }
            }
      }
}


void cClient::Act_WalkCancel (cPacket * packet)
{

      {
        cCharacter *character = pCharacterList.Get (player_char);

        if (character)
            {

              walk_stack.clear ();
              walk_sequence = 0;

              character->
                setPosition ((int) ((Uint16) packet->packet.WalkCancel.m_x),
                             (int) ((Uint16) packet->packet.WalkCancel.m_y),
                             (int) ((Sint8) packet->packet.WalkCancel.m_z));
              character->setDirection ((Uint8) packet->packet.WalkCancel.
                                       m_dir);

              if (callback_OnTeleport)
                callback_OnTeleport (character->x (), character->y (),
                                     character->z ());
              player_position[0] = character->x ();
              player_position[1] = character->y ();
              player_position[2] = character->z ();
              walk_direction = character->direction ();

              cPacket npacket;
              npacket.AddByte (PCK_WalkAck);
              npacket.AddByte (packet->packet.WalkCancel.m_seq);
              npacket.AddByte (0);
              Send (&npacket);

            }
      }
}

void cClient::Act_Light (cPacket * packet)
{
  packet->SetPosition (1);
  Uint8 lightlevel = packet->GetByte ();
  if (lightlevel > 31)
    lightlevel = 31;
  if (callback_OnLightLevel)
    callback_OnLightLevel (lightlevel);
}


void cClient::Act_War (cPacket * packet)
{
  packet->SetPosition (1);
  int mode = packet->GetByte ();
  m_warmode = mode != 0x00;

      {
        cCharacter *character = pCharacterList.Get (player_char);

        if (character)
          character->setWarmode (m_warmode);
      }
  if (!m_warmode)
    enemy = 0;

  if (callback_OnWarmode)
    callback_OnWarmode (mode);

}

void cClient::Act_Delete (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  pDynamicObjectList.Delete (id);

        pCharacterList.Delete (id);
        characterlist_t *characters = pCharacterList.GetList ();
        characterlist_t::iterator iter;

        for (iter = characters->begin (); iter != characters->end (); iter++)
            {
              cCharacter *pChar = iter->second;
              for (Uint32 l = 0; l <= 25; l++)
                  {
                    cCharacterEquip *equip = pChar->GetEquip (l);
                    if (equip)
                        {
                          if (equip->id () == id)
                            pChar->DelEquip (l);
                        }
                  }
            }

}

void cClient::Act_Status (cPacket * packet)
{

  packet->SetPosition (3);
  Uint32 id = packet->GetDword ();
  cCharacter *character = pCharacterList.Get (id);
  Uint8 flag = 0;
  if (character)
      {
        char name[31];
        name[30] = 0;
        packet->GetData (name, 30);
        character->setName (name);

        character->setCurrentHits (packet->GetWord ());
        character->setMaxHits (packet->GetWord ());
        character->setNameChange (packet->GetByte () == 0xff);

        flag = packet->GetByte ();

        if (flag > 0)
            {

              character->setSex (packet->GetByte ());
              character->setStr (packet->GetWord ());
              character->setDex (packet->GetWord ());
              character->setInt (packet->GetWord ());
              character->setCurrentStamina (packet->GetWord ());
              character->setMaxStamina (packet->GetWord ());
              character->setCurrentMana (packet->GetWord ());
              character->setMaxMana (packet->GetWord ());
              character->setGold (packet->GetDword ());
              character->setArmor (packet->GetWord ());
              character->setWeight (packet->GetWord ());
              if (flag >= 3)
                  {
                    character->setStatCap (packet->GetWord ());
                    character->setPetsCurrent (packet->GetByte ());
                    character->setPetsMax (packet->GetByte ());
                  }
              if (flag == 4)
                  {
                    character->setFireResist (packet->GetWord ());
                    character->setColdResist (packet->GetWord ());
                    character->setPoisonResist (packet->GetWord ());
                    character->setEnergyResist (packet->GetWord ());
                    character->setLuck (packet->GetWord ());
                    character->setMinDamage (packet->GetWord ());
                    character->setMaxDamage (packet->GetWord ());
                    character->setTithingPoints (packet->GetDword ());
                  }
            }

        if (callback_OnStatus)
          callback_OnStatus (character->id (), flag);

      }

}

void cClient::Act_Content (cPacket * packet)
{

  vector < Uint32 > containers;
  unsigned int j;

  packet->SetPosition (3);
  Uint16 count = packet->GetWord ();

  Uint8 circles[8] = {0,0,0,0,0,0,0,0};
  bool is_spellbook = false;
  Uint32 spellbookserial = 0;

  for (int i = 0; i < count; i++)
      {
        Uint32 id = packet->GetDword ();
        Uint16 model = packet->GetWord ();
        packet->GetByte ();     /* unknown */
        Uint16 stackcount = packet->GetWord ();
        Uint16 x = packet->GetWord ();
        Uint16 y = packet->GetWord ();
        Uint32 contid = packet->GetDword ();

        Uint16 hue = packet->GetWord ();

        cDynamicObject * cont_obj = pDynamicObjectList.Get(contid);


        if (contid == corpse_id)
            {
              std::map < Uint32, Uint8 >::iterator iter;
              iter = corpse_equip.find (id);
              if (iter != corpse_equip.end ())
                  {
                    struct TileDataStaticEntry entry;
                    if (pTileDataLoader.LoadEntry (model, &entry))
                        {
                          cCharacter *character =
                            pCharacterList.Get (contid);
                          if (character)
                              {
                                cCharacterEquip *equip =
                                  character->AddEquip ((Uint32) iter->second);
                                equip->setID (id);
                                equip->setHue (hue);
                                equip->setModel (model);
                                equip->setAnim (entry.animid);
                              }
                        }

                  }
            }
           else if(cont_obj && cont_obj->model == 0xEFA )
            {
              Uint8 circle = 0;

              if(model >= 0x1F2D && model < 0x1F35)
               circle = 0;
              else if(model >= 0x1F35 && model < 0x1F3D)
               circle = 1;
              else if(model >= 0x1F3D && model < 0x1F45)
               circle = 2;
              else if(model >= 0x1F45 && model < 0x1F4D)
               circle = 3;
              else if(model >= 0x1F4D && model < 0x1F55)
               circle = 4;
              else if(model >= 0x1F55 && model < 0x1F5D)
               circle = 5;
              else if(model >= 0x1F5D && model < 0x1F65)
               circle = 6;
              else if(model >= 0x1F65 && model < 0x1F6D)
               circle = 7;

              int spellpos = model - (0x1F2D + (circle * 8));

              circles[circle] |= 1 << spellpos;
              is_spellbook = true;
              spellbookserial = contid;

            }

       if(!is_spellbook)
        pDynamicObjectList.AddContainerContent (id, model, hue, contid, x, y,
                                                 stackcount);
        bool found = false;
        for (j = 0; j < containers.size (); j++)
          if (containers[j] == contid)
              {
                found = true;
                break;
              }
        if (!found)
          containers.push_back (contid);

      }
     if(is_spellbook)
       {
        spellbooktype = 1;
        if (callback_OnOpenSpellBook)
            callback_OnOpenSpellBook (spellbookserial, 0xEFA,
                                      circles);
       return;
       }
  if (callback_OnContainerContent)
    for (j = 0; j < containers.size (); j++)
        {
                gui_message msg;
                msg.type = MESSAGE_REBUILDITEMCONTAINER;
                msg.rebuilditemcontainer.containerid = containers[j];
                pUOGUI.AddMessageToStack (&msg);
          callback_OnContainerContent (containers[j]);
        }

  containers.clear ();
  corpse_equip.clear ();
}

void cClient::Act_StatChange (cPacket * packet)
{

  Uint32 id = packet->packet.StatChange.m_id;
  Uint16 max = packet->packet.StatChange.m_max;
  Uint16 current = packet->packet.StatChange.m_val;
  Uint32 type = 0;

  cCharacter *character = pCharacterList.Get (id);
  if (character)
      {
        switch (packet->packet.StatChange.m_cmd)
            {
            case PCK_StatChngStr:
              character->setMaxHits (max);
              character->setCurrentHits (current);
              type = 1;
              break;
            case PCK_StatChngDex:
              character->setMaxStamina (max);
              character->setCurrentStamina (current);
              type = 2;
              break;
            case PCK_StatChngInt:
              character->setMaxMana (max);
              character->setCurrentMana (current);
              type = 3;
              break;
            }
        if (callback_OnStatChange && type)
          callback_OnStatChange (character->id (), type, current, max);
      }

}

void cClient::Act_ContOpen (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  Uint16 gump = packet->GetWord ();
  if (buy_opening && gump == 0x30)
  {
     buy_opening = true;
     vendor_id = id;
     return;
  }
  else if (gump == 0x003c)   // 0x003c = backpack
  {
     backpack_opened = true;
     std::cout << "Backpackopend == true" << std::endl;
  }

  if (callback_OnOpenContainer)
    callback_OnOpenContainer (id, gump);
}


void cClient::Act_Skill( cPacket *packet )
{
	packet->SetPosition( 3 );
	Uint16 id, skill = 0, unmodified, skillcap;
	Uint8 skilllock;
	Uint8 type = packet->GetByte();
	int count = 0;
	Uint16 skill_id = 0;
	cCharacter *character = player_character();
	if ( character )
	{
		switch ( type )
		{
		case 0x00:
		case 0x02:
			while ( ( id = packet->GetWord() ) && ( count < 100 ) )
			{
				count++;
				skill = packet->GetWord();
				unmodified = packet->GetWord();
				skilllock = packet->GetByte();
				if ( type == 0x02 )
				{
					skillcap = packet->GetWord();
				}
				else
				{
					skillcap = 0;
				}
				sSkillEntry *entry = character->skill( id );
				entry->value = skill;
				entry->unmodified = unmodified;
				entry->skillcap = skillcap;
				entry->skillLock = skilllock;
			}
			break;

		case 0xff:
		case 0xDF:
			id = packet->GetWord();
			skill = packet->GetWord();
			unmodified = packet->GetWord();
			skilllock = packet->GetByte();
			if ( type == 0xDF )
			{
				skillcap = packet->GetWord();
			}
			else
			{
				skillcap = 0;
			}
			sSkillEntry *entry = character->skill( id );
			entry->value = skill;
			entry->unmodified = unmodified;
			entry->skillcap = skillcap;
			entry->skillLock = skilllock;
			skill_id = skill;
			break;
		}
	}

	if ( !skill_id )
	{
		skill = 0;
	}

	if ( callback_OnSkill )
	{
		callback_OnSkill( skill_id, skill );
	}
}

void cClient::Act_CharMove (cPacket * packet)
{

  Uint32 id = packet->packet.CharMove.m_id;
  cCharacter *character = pCharacterList.Get (id);
  if (character)
      {
        Uint16 model = packet->packet.CharMove.m_model;
        Uint16 x = packet->packet.CharMove.m_x;
        Uint16 y = packet->packet.CharMove.m_y;
        Sint8 z = packet->packet.CharMove.m_z;
        Uint8 dir = packet->packet.CharMove.m_dir;
        Uint16 hue = ((Uint16) packet->packet.CharMove.m_hue) & 32767;
        Uint8 flag = packet->packet.CharMove.m_flag;
        Uint8 hcolor = packet->packet.CharMove.m_hColor;
        character->MoveTo (x, y, z);
        character->setBody (model);
        character->setHue (hue);
        character->setFlag (flag);
        character->RotateTo (dir);
        character->setHighlightColor (GetHighlightColor (hcolor));
      }
}

void cClient::Act_DragCancel (cPacket * packet)
{
  packet->SetPosition (1);
  Uint8 type = packet->GetByte ();
  if (callback_OnDragCancel)
    callback_OnDragCancel ();
}

void cClient::Act_ItemEquip (cPacket * packet)
{
  Uint32 id = packet->packet.ItemEquip.m_itemID;
  Uint32 charid = packet->packet.ItemEquip.m_playerId;
  Uint16 model = packet->packet.ItemEquip.m_model;
  Uint8 layer = packet->packet.ItemEquip.m_layer;
  Uint16 hue = packet->packet.ItemEquip.m_hue;

  cCharacter *character = pCharacterList.Get (charid);

  struct TileDataStaticEntry entry;
  if (pTileDataLoader.LoadEntry (model, &entry))
    if (character)
        {
          cCharacterEquip *equip = character->AddEquip (layer);
          equip->setID (id);
          equip->setHue (hue);
          equip->setModel (model);
          equip->setAnim (entry.animid);

          cDynamicObject *object = pDynamicObjectList.Get (id);

          if (object)
            if (object->type == DYNAMICTYPE_CONTAINER)
                {
                  gui_message msg;
                  msg.type = MESSAGE_REBUILDITEMCONTAINER;
                  msg.rebuilditemcontainer.containerid = object->parent;
                  pUOGUI.AddMessageToStack (&msg);
                }

          pDynamicObjectList.AddCharEquip (id, model, hue, charid);

                gui_message msg;
                msg.type = MESSAGE_UPDATEPAPERDOLL;
                msg.updatepaperdoll.id = charid;
                pUOGUI.AddMessageToStack (&msg);
        }
}


void cClient::Act_MenuItems (cPacket * packet)
{
  packet->SetPosition (3);
/*				NDWORD	m_id;
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
			} m_item; */
  Uint32 id = packet->GetDword ();
  Uint16 context = packet->GetWord ();
  Uint8 lenname = packet->GetByte ();
  char name[257];
  packet->GetData (name, lenname);
  name[(int) lenname] = 0;
  Uint8 count = packet->GetByte ();

  menu_entries.clear ();
  menu_models.clear ();

  for (int i = 0; i < count; i++)
      {
        Uint16 model = packet->GetWord ();
        Uint16 checkl = packet->GetWord ();
        Uint8 lentext = packet->GetByte ();
        char text[257];
        packet->GetData (text, lentext);
        text[(int) lentext] = 0;

        menu_entries.push_back (string (text));
        menu_models.push_back (model);
      }

  if (callback_OnMenuItems)
    callback_OnMenuItems (id, context, count, name);

  menu_entries.clear ();
  menu_models.clear ();
}

void cClient::Act_Target (cPacket * packet)
{
  Uint32 id = packet->packet.Target.m_cursorID;
  Uint32 type = packet->packet.Target.m_type;
  if (callback_OnTarget)
    callback_OnTarget (id, type);
  if(!m_wait_for_target.empty())
	 pCSLHandler.ExecuteFunction((char*)m_wait_for_target.c_str());
	m_wait_for_target.clear();
}

void cClient::Act_Paperdoll (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  char text[61];
  text[60] = 0;
  packet->GetData (text, 60);
  Uint8 flag = packet->GetByte ();
  if (callback_OnPaperdoll)
    callback_OnPaperdoll (id, text, flag);
}


void cClient::Act_Time( cPacket *kPacket )
{
	kPacket->SetPosition( 1 );

	tm z;
	z.tm_hour = kPacket->GetByte();
	z.tm_min = kPacket->GetByte();
	z.tm_sec = kPacket->GetByte();
	SynchronizedTime::Synchronize( z.tm_hour, z.tm_min, z.tm_sec );
}


void cClient::Act_CorpEquip (cPacket * packet)
{
  packet->SetPosition (1);
  int packetlen = packet->GetWord ();
  int items = (packetlen - 8) / 5;
  Uint32 id = packet->GetDword ();
  corpse_id = id;
  Uint8 layer;
  Uint32 itemid;
  for (int i = 0; i < items; i++)
      {
        layer = packet->GetByte ();
        itemid = packet->GetDword ();
        corpse_equip.insert (make_pair (itemid, layer));
      }
  Uint8 terminator = packet->GetByte ();
}

void cClient::Act_AttackOK (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  enemy = id;

  if (callback_OnAttackReply)
    callback_OnAttackReply (id);
}

void cClient::Act_Dye (cPacket * packet)
{
  packet->SetPosition (1);
  Uint32 id = packet->GetDword ();
  packet->SetPosition (7);
  Uint16 model = packet->GetWord ();
  if (callback_OnDyeWindow)
    callback_OnDyeWindow (id, model);
}

void cClient::Act_AOSTooltip (cPacket * packet)
{

  if (!Config::GetAOSToolTips())
    return;
  if ( !Config::GetClilocs() )
    return;
  packet->SetPosition (5);
  Uint32 id = packet->GetDword ();
  Uint16 unkn = packet->GetWord ();
  Uint32 listID = packet->GetDword ();
  cCharacter *character = NULL;
  cDynamicObject *obj = NULL;

  character = pCharacterList.Get (id);
  obj = pDynamicObjectList.Get (id);

  if (character)
      {
        character->ClearAOSTooltip ();
        character->SetAOSTooltipID (listID);
      }
  if (obj)
      {
        obj->ClearAOSTooltip ();
        obj->SetAOSTooltipID (listID);
      }

  Uint32 clilocID;
  do
      {
        clilocID = packet->GetDword ();
        if (!clilocID)
          break;

        Uint16 textlen = packet->GetWord ();

        char *arguments;
        arguments = new char[textlen];
        packet->GetData (arguments, textlen);

        char *str2 = new char[(textlen / 2) + 1];
        int idx2 = 0;
        for (int i = 0; i < textlen; i++)
            {
              if (arguments[i] != 0)
                str2[i / 2] = arguments[i];
            }
        str2[textlen / 2] = 0;


        vector < std::string > arglist;
        std::string arg_string = std::string (str2);
        char tab[2];
        tab[0] = 9;
        tab[1] = 0;
        std::string delim = std::string (tab);


        stringSplit (arglist, arg_string, delim);


        std::string message =
          pClilocLoader.GetMessageWithArguments ((int) clilocID,
                                                  (int) arglist.size (),
                                                  arglist);

        if (character)
          character->AddAOSTooltip (message);

        if (obj)
          obj->AddAOSTooltip (message);

          delete[] str2;
          delete[] arguments;
      }
  while (clilocID != 0);

}


void cClient::Act_GumpDialog( cPacket *packet )
{
/*
 * 0xB0 Packet
 * Last Modified on Tuesday, 20-Apr-1999
 * Send Gump Menu Dialog (Variable # of bytes)
 *	BYTE cmd
 *	BYTE[2] blockSize
 *	BYTE[4] id
 *	BYTE[4] gumpid
 *	BYTE[4] x
 *	BYTE[4] y
 *	BYTE[2] command section length
 *	BYTE[?] commands (zero terminated)
 *	BYTE[2] numTextLines
 *	BYTE[2] text length (in unicode (2 byte) characters.)
 *	BYTE[?] text (in unicode)
 */
	packet->SetPosition( 1 );
	Uint16 size = packet->GetWord();
	Uint32 id = packet->GetDword();
	Uint32 gumpid = packet->GetDword();
	Uint32 px = packet->GetDword();
	Uint32 py = packet->GetDword();

//	printf( "[GumpDialog (Size: %d, ID: %d, GumpID: %d, Position(%d, %d))]\n", size, id, gumpid, px, py );

	// parse gump commando-lines
	Uint16 cmds_len = packet->GetWord();

//	printf( "Command Length: %d\n", cmds_len ); // 676 -> DEBUG <-

	char *cmds = new char[cmds_len + 1];
	memset( cmds, 0, cmds_len + 1 );
	packet->GetData( cmds, cmds_len );

	//converts cmds to lowerspace
	char *conv = cmds;
//	printf( "Conv: %s\n", conv ); -> DEBUG <-

	while ( *conv )
	{
		if ( ( *conv >= 65 ) && ( *conv <= 90 ) )
		{
			*conv += 32;
		}
		conv++;
	}

	std::list<std::string> cmds_list;
	char *token = strtok( cmds, "{}" );

	while ( token != NULL )
	{
		cmds_list.push_back( std::string( token ) );   //add command line to list
		token = strtok( NULL, "{}" );
	}
	SAFE_DELETE_ARRAY( cmds );

	// parse gump text-lines
	Uint16 txt_lines = packet->GetWord();
	std::vector<std::string> texts_vector( txt_lines );

	Uint16 line_len = 0;
	for ( int i = 0; i < txt_lines; i++ )
	{
		line_len = packet->GetWord();    //get length of line

		if ( line_len > 0 )
		{
			char *line = new char[line_len * 2];
			packet->GetData( line, line_len * 2 );
			cUnicode unistring( (NCHAR*)line, line_len );

			if ( strlen( unistring.m_charBuf ) > 0 )
			{
				texts_vector[i] = std::string( unistring.m_charBuf );
			}
			else
			{
				texts_vector[i] = std::string(" ");
			}
			SAFE_DELETE_ARRAY( line );
		}
		else
		{
			texts_vector[i] = std::string( " " );
		}
	}

	Container *dialog = new Container();

	dialog->SetSize( 100, 100 );
	dialog->SetPosition( px, py );
	dialog->SetFlags( GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE );
	dialog->SetGumpID( (int)gumpid );
	dialog->SetPlayerID( (int)id );

	int target_page = 0;
	int cur_radio_group = 0;
	std::list<std::string>::iterator it;

	for ( it = cmds_list.begin(); it != cmds_list.end(); it++ )
	{
		//SiENcE: params has false value in it, don't know why
		std::stringstream stream;
		std::string command;
		stream << (*it);
		std::vector<int> params;
		stream >> command;

		// Crash bug (gumppic) 3rd
		while ( !stream.eof() )
		{
			int tmp;
			stream >> tmp;
			params.push_back( tmp );
		}

		Control *control = NULL;

		if ( command == "page" )
		{
			target_page = params[0];

			/*
			if ( target_page == 0 )
			{
                 target_page = 1;
			}
			*/
		}
		else if ( command == "group" )
		{
			cur_radio_group = params[0];
		}
		else if ( command == "resizepic" )
		{
			Border *border = new Border( params[0], params[1], params[2], 0 );
			border->SetSize( params[3], params[4] );
			control = border;
		}
		else if ( command == "text" )
		{
			Label *label = new Label( params[0], params[1], texts_vector[params[3]].c_str(), params[2] );
			label->setHue( params[2] );	// maybe cause some errors

			control = label;

			printf( "label: %s 0x%x", texts_vector[params[3]].c_str(), params[2] );
		}
		else if ( command == "croppedtext" )
		{
			int hue = params[4];
			if ( hue == 0 )
			{
				hue = 1;
			}
			Label *label = new Label( params[0], params[1], texts_vector[params[5]].c_str(), hue );
			label->Crop( params[2], params[3] );
			control = label;

			printf( "croppedtext:%s(%d)\n", texts_vector[params[5]].c_str(), hue );
		}
		else if ( command == "gumppic" )
		{
			Image *image = new Image( params[0], params[1], params[2] );
			control = image;
			/* TILEPIC X Y TILENR */
			/* TODO: loading art textures */
		}
		else if (command == "gumppictiled")
		{
			Image *image = new Image( params[0], params[1], params[4] );
			image->SetTiled( params[2], params[3] );
			control = image;
		}
		/* TODO (ArTiX#1#): temporarily disabled to fix some ugly bugs */
		//Artix fix removed -- startline
        else if ( command == "htmlgump" )
		{
			cMultiLabel *label = new cMultiLabel( params[0], params[1], params[2], params[3], params[6] );
			cHTMLGumpParser parser;
			parser.Parse( texts_vector[params[4]], label );
			label->Create();
			control = label;
		}
		else if ( command == "xmfhtmlgump" )
		{
			if ( !Config::GetClilocs() )
			{
				continue;
			}
			cMultiLabel *label = new cMultiLabel( params[0], params[1], params[2], params[3], params[6] );
			cHTMLGumpParser parser;
			parser.Parse( pClilocLoader.GetMessage( params[4] ), label );
			label->Create ();
			control = label;
		}
		else if ( command == "xmfhtmlgumpcolor" )
		{
			if ( !Config::GetClilocs() )
			{
				continue;
			}
			cMultiLabel *label = new cMultiLabel( params[0], params[1], params[2], params[3], params[6] );
			cHTMLGumpParser parser;
			parser.setDefaultColor( params[7] );
			parser.Parse( pClilocLoader.GetMessage( params[4] ), label );
			label->Create();
			control = label;
		}
		//Artix fix removed -- endline
		else if ( command == "tilepic" )
		{
			ImageArt *image = new ImageArt( params[0], params[1], params[2] );
			// image->SetPage (target_page);
			// dialog->AddControl (image);
			control = image;

			/* TEXTENTRY X Y Width Height Color storageID Default_text */
			/* TODO: storage id */
		}
		else if ( command == "textentry" )
		{
			InputField *inputfield = new InputField( params[0], params[1], params[2],
				params[3], texts_vector.at( params[6] ).c_str(), params[4] );
			// inputfield->setText ("AAA");
			control = inputfield;
		}
		else if ( command == "checkertrans" )
		{
			int alphax, alphay, alphaw, alphah;

			alphax = params[0];
			alphay = params[1];
			alphaw = params[2];
			alphah = params[3];

			ControlList_t *the_list = dialog->GetControlList();

			ControlList_t::iterator iter;
			for ( iter = the_list->begin(); iter != the_list->end(); iter++ )
			{
				//if
				iter->second->SetAlpha( 120 );
			}

			//dialog->SetAlpha(120);

			/* BUTTON X Y DOWNID UPID PAGESEL? DESTPAGENR BUTTONID */
			/* TODO: buttonid */
		}
		else if ( command == "button" )
		{
			Button *button = new Button( params[0], params[1] );
			button->SetButton( BUTTONGUMP_NORMAL, params[2] );
			button->SetButton( BUTTONGUMP_MOUSEOVER, params[2] );
			button->SetButton( BUTTONGUMP_PRESSED, params[3] );
			if ( params[4] != 0 )   //is it a page selector button?
			{
				gui_message *gmess;
				gmess = new gui_message();
				gmess->type = MESSAGE_CLOSEGUMP;
				button->SetClickMessage( gmess );
				button->HandleMessage( gmess );
				button->SetReturnMsg( params[6] );
				button->SetCloseGump( true );
			}
			if ( params[5] != 0 )   //is it a page selector button?
			{
				button->SetPageSelector( true );
				button->SetDestinationPage( params[5] );
			}
			button->OnClick( HandleGumpDialogEvent );
			control = button;

			/* CHECKBOX X Y SELECTEDPIC UNSELECTEDPIC STARTSTATE IDNR */
			/* TODO: idnr */
		}
		else if ( command == "checkbox" )
		{
			Logger::WriteDebug( "Trying to add a check box (DISABLED): " + command );
/*
			Checkbox *checkbox = new Checkbox( params[0], params[1], params[2], params[3], params[4] );
			control = checkbox;
*/
		}
		else if ( command == "radio" )
		{
			// TODO: Fix first time, it shows the wrong option checked but on click fixes (needs debugging).

			Logger::WriteDebug("DEBUG - params: " + params[0] + params[1] + params[2] + params[3] + params[4] );
/*
            RadioButton *radio = new RadioButton( params[0], params[1], params[2], params[3], params[4] );

            radio->SetPage( target_page );
            control = radio;
*/
		}
		else if ( command == "noresize" )
		{
			// dialog->RemoveFlag( GUMPFLAG_MOVABLE );
		}
		else if ( command == "noclose" )
		{
			dialog->RemoveFlag( GUMPFLAG_CLOSABLE );
		}
		else
		{
			Logger::WriteDebug( "-> Unhandled gump dialog statement " + command );
		}

		//add the constructed control to the dialog:
		if ( control != NULL )
		{
			control->SetPage( target_page );

			control->SetFlags( 0 );
			dialog->AddControl( control );
		}
	}
	dialog->SetCurrentPage( 1 );
	pUOGUI.AddControl( dialog );
}


void cClient::Act_MapDisplay (cPacket * packet)
{
/*
Map message [0x90] (19 bytes)
BYTE cmd
BYTE[4] key used
BYTE[2] gump art id (0x139D)
BYTE[2] upper left x location
BYTE[2] upper left y location
BYTE[2] lower right x location
BYTE[2] lower right y location
BYTE[2] gump width in pixels
BYTE[2] gump height in pixels
*/
  packet->SetPosition (1);
  Uint32 mapkey = packet->GetDword ();
  Uint16 gumpid = packet->GetWord ();

  Uint16 up_left_x = packet->GetWord ();
  Uint16 up_left_y = packet->GetWord ();

  Uint16 lo_right_x = packet->GetWord ();
  Uint16 lo_right_y = packet->GetWord ();

  Uint16 gump_x = packet->GetWord ();
  Uint16 gump_y = packet->GetWord ();

  printf ("Currently not supported - [MapDialog (Key: %d, GumpID: %d, Upper Left(%d, %d), Lower Right(%d, %d), Gumpwidth(%d, %d))]\n",
          mapkey, gumpid, up_left_x, up_left_y, lo_right_x, lo_right_y, gump_x, gump_y);
  /*
  Container *dialog = new Container ();

  dialog->SetSize (gump_x, gump_y);
  dialog->SetPosition (up_left_x, up_left_y);
  dialog->SetFlags (GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE);
  dialog->SetGumpID ((int) gumpid);
  //  dialog->SetPlayerID ((int) id);

  dialog->SetCurrentPage (1);
  pUOGUI.AddControl (dialog);
  */
}

void cClient::Act_PlayMusic (cPacket * packet)
{
  packet->SetPosition (1);
  Uint16 id = packet->GetWord ();
  if ( Config::GetMusic() )
      {
		  SoundMix::GetInstance()->PlayMusic ((int) id, Config::GetMP3(), Config::GetMusicVolume() );
      }
}

void cClient::Act_ContAdd (cPacket * packet)
{

  if ((Uint32) packet->packet.ContAdd.m_ContainerID == trade_1
      || (Uint32) packet->packet.ContAdd.m_ContainerID == trade_2)
      {
        if (callback_OnTradeAdd)
            {
              callback_OnTradeAdd (packet->packet.ContAdd.m_body,
                                   packet->packet.ContAdd.m_ContainerID,
                                   packet->packet.ContAdd.m_itemCount,
                                   packet->packet.ContAdd.m_hue,
                                   packet->packet.ContAdd.m_ContainerX,
                                   packet->packet.ContAdd.m_ContainerY);
            }
        return;
      }


  pDynamicObjectList.AddContainerContent (packet->packet.ContAdd.m_itemId,
                                           packet->packet.ContAdd.m_body,
                                           packet->packet.ContAdd.m_hue,
                                           packet->packet.ContAdd.
                                           m_ContainerID,
                                           packet->packet.ContAdd.
                                           m_ContainerX,
                                           packet->packet.ContAdd.
                                           m_ContainerY,
                                           packet->packet.ContAdd.
                                           m_itemCount);
        gui_message msg;
        msg.type = MESSAGE_ADDCONTAINERITEM;
        msg.addcontaineritem.id = packet->packet.ContAdd.m_itemId;
        msg.addcontaineritem.containerid =
          packet->packet.ContAdd.m_ContainerID;
        pUOGUI.AddMessageToStack (&msg);

}

void cClient::Act_Sound( cPacket *packet )
{
	if ( SoundMix::GetInstance() )
	{
		SoundMix::GetInstance()->PlaySound( (int)packet->packet.Sound.m_sound,
			(int)packet->packet.Sound.m_volume, (char)packet->packet.Sound.m_flags,
			(int)packet->packet.Sound.m_x, (int)packet->packet.Sound.m_y,
			(int)packet->packet.Sound.m_z );
	}
}

void cClient::Act_CharAction (cPacket * packet)
{
  assert (packet);
        cCharacter *character =
          pCharacterList.Get ((int) packet->packet.CharAction.m_id);

        if (character)
            {
              character->DoAnimation ((int) packet->packet.CharAction.
                                      m_movement,
                                      (int) packet->packet.CharAction.
                                      m_repeat);
            }
}


void cClient::Act_SubCommands (cPacket * packet)
{
  cCharacter *character = player_character ();
  packet->SetPosition (3);
  Uint16 subcmd = packet->GetWord ();

  switch (subcmd)
      {
	  //change Worldmap (maps.xml)
	  case 0x8:
		{
		  Uint8 mapID = packet->GetByte();

		  if(actual_map == mapID)
			  break;

		  Logger::WriteLine("MAP changed");

		  if(pMapLoader)
			delete pMapLoader;

		  pMapbufferHandler.buffer ()->Clear();

		  cMapInfoEntry * mapinfo_entry = MapInfoLoader::GetInstance()->GetMapInfo(mapID);
				if(!mapinfo_entry)
					THROWEXCEPTION ("Wanted to change to an unknown map!");

		  pMapLoader = new UOMapLoader((int)mapID);

		  actual_map = mapID;

		  Renderer * renderer = Game::GetInstance()->GetRenderer();

//SiENcE:new
		  renderer->LoadSkyboxTextures (actual_map);
		  //This is now handled by WorldEnvironment.cpp
          GLfloat fogColor[4] = {(float) mapinfo_entry->fog_r() / 255.0f, (float) mapinfo_entry->fog_g() / 255.0f,(float) mapinfo_entry->fog_b() / 255.0f, 1.0 };
		  glFogfv(GL_FOG_COLOR, fogColor);

		  break;
		}
	  // Display Gump-Popup
      case 0x14:
        {
          Logger::WriteLine ("DISPLAY Popup");

          popup_entries.clear ();
          popup_tags.clear ();
          Uint16 unkn = packet->GetWord ();
          popserial = packet->GetDword ();
          Uint8 popup_count = packet->GetByte ();
          Uint16 flags;
          Uint16 entrycolor;
          for (Uint8 i = 0; i < popup_count; i++)
              {
                popup_tags.push_back (packet->GetWord ());
                popup_entries.push_back (packet->GetWord ());
                flags = packet->GetWord ();
                if (flags & 0x20)
                  entrycolor = packet->GetWord ();
              }
          if (callback_OnPopupDisplay)
            callback_OnPopupDisplay (popup_entries.size (), m_popupx, m_popupy);
          break;
        }
	  // Displays AoS Spellbook
      case 0x1B:
        {                       // AoS spellbooks
          Logger::WriteLine ("AOS Spellbook");

          Uint16 unkn1 = packet->GetWord ();
          Uint32 SpellBookSerial = packet->GetDword ();
          Uint16 SpellBookModel = packet->GetWord ();
          Uint16 unkn2 = packet->GetWord ();
          Uint8 circles[8];
          for (int i = 0; i <= 7; i++)
              {
                circles[i] = packet->GetByte ();
              }
          if (callback_OnOpenSpellBook)
            callback_OnOpenSpellBook (SpellBookSerial, SpellBookModel,
                                      circles);
          break;

        }
	  // Display AOSTooltip
      case 0x10:
        {
          Logger::WriteLine ("AOS Tooltip");

          Uint32 objserial = packet->GetDword ();
          Uint32 listID = packet->GetDword ();
          cCharacter *mob = NULL;
          cDynamicObject *obj = NULL;
          mob = pCharacterList.Get (objserial);
          obj = pDynamicObjectList.Get (objserial);

          bool found = false;
          if(mob)
          {
           if(listID == mob->AOSTooltipID())
           return;
          }

          if(obj)
          {
           if(listID == obj->AOSTooltipID())
           return;
          }

          cPacket pck;
          pck.AddByte (0xBf);
          pck.AddWord (9);
          pck.AddWord (0x10);
          pck.AddDword (objserial);
          Send (&pck);
          break;
        }
        // Damage Packet
        case 0x22:
        {
          Logger::WriteLine ("Damage recv.");

          packet->GetByte();  // 1
          Uint32 id = packet->GetDword(); // id
          Uint8 damage = packet->GetByte();  // damage amount
          //printf("id:0x%x, damage: %d\n", id, damage);
          char buf[10];

          sprintf(buf, "%d", damage);
          callback_OnSpeech(buf, "", id, 0x35);
          break;
        }

      }
}

Uint16 cClient::GetPopupEntry (int index)
{
  return popup_entries[index];
}

Uint16 cClient::GetPopupTag (int index)
{
  return popup_tags[index];
}



void cClient::Send_SelectServer (Uint32 index)
{
  cPacket packet;
  packet.FillPacket (PCK_ServerSelect);
  packet.packet.ServerSelect.m_slot = (Uint16) index;
  Send (&packet);
  ClearLoginLists ();
}


void cClient::Send_RequestChars (Uint32 account)
{
  cPacket packet;
  packet.FillPacket (PCK_CharListReq);
  packet.packet.CharListReq.m_account = account;
  strncpy (packet.packet.CharListReq.m_username, Config::GetLogin().c_str (), 29);
  strncpy (packet.packet.CharListReq.m_password, Config::GetPassword().c_str (), 29);
  Send (&packet);
}

void cClient::SendSecureTradeCheck (Uint32 id, Uint32 check1, Uint32 check2)
{

  cPacket packet;
  packet.AddByte (0x6F);
  packet.AddWord (0x11);
  packet.AddByte (2);
  packet.AddDword (id);
  packet.AddDword (check1);
  packet.AddDword (check2);
  packet.AddByte (0);
  Send (&packet);

}

//new for cClient::Send_SelectChar (maybe put into network class
Uint32 cClient::Get_Local_IP()
{
#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif
      char buff[MAXHOSTNAMELEN]="\0";
      struct hostent *local;
      static unsigned long myAddr;

      // determine my name & address
      myAddr = 0;
      int hostnameres = gethostname(buff, MAXHOSTNAMELEN);

      if ( 0 == hostnameres )
      {
       local = gethostbyname(buff);
       if ( local )
       {
        myAddr = *reinterpret_cast<int *>(local->h_addr_list[0]);
       }
       else
       {
        Logger::WriteLine ("ANET_Init: Unable to determine IP adress.\n");
        return 0;
       }
      }
      else
      {
       Logger::WriteLine ("ANET_Init: Unable to determine hostname.\n");
       return 0;
      }

      if ( myAddr == 0 )
      {
       myAddr = inet_addr("127.0.0.1");
      }
      return myAddr;
}


void cClient::Send_SelectChar (Uint8 index)
{
  cPacket packet;
  packet.FillPacket (PCK_CharPlay);
  packet.packet.CharPlay.m_edededed = 0xedededed;
  strncpy (packet.packet.CharPlay.m_username, Config::GetLogin().c_str (), 29);
//  strncpy (packet.packet.CharPlay.m_password, "", 32);
  packet.packet.CharPlay.m_slot = index;
  packet.packet.CharPlay.m_ip = SDL_Swap32( Get_Local_IP() );  //send IP in reverse order
  Send (&packet);
  ClearLoginLists ();
}

void cClient::Send_CreateChar (sNewCharSettings settings)
{
  char password[30];
  Uint8 i;
  string location;
  cPacket packet;

  for(i = 0; i < 30; i++)
    password[i] = 0;
  packet.AddByte (0x00);
  packet.AddDword (0xedededed);
  packet.AddDword (0xffffffff);
  packet.AddByte (0x00);
  packet.AddData (settings.name, 30);
  packet.AddData (password, 30); // I don't know the meaning of this
  packet.AddByte (settings.sex);
  packet.AddByte (settings.stat[0]);
  packet.AddByte (settings.stat[1]);
  packet.AddByte (settings.stat[2]);
  packet.AddByte (settings.skill[0]);
  packet.AddByte (settings.skill_val[0]);
  packet.AddByte (settings.skill[1]);
  packet.AddByte (settings.skill_val[1]);
  packet.AddByte (settings.skill[2]);
  packet.AddByte (settings.skill_val[2]);
  packet.AddWord (settings.skin_color);
  packet.AddWord (settings.hair);
  packet.AddWord (settings.hair_color);
  packet.AddWord (settings.facial_hair);
  packet.AddWord (settings.facial_hair_color);
  packet.AddWord (settings.starting_location);
  packet.AddWord (0x00); // unknown
  packet.AddWord (settings.slot);
  packet.AddDword (0); // should be IP
  packet.AddWord (settings.shirt_color);
  packet.AddWord (settings.pants_color);
  Send (&packet);
}

void cClient::Send_DeleteChar (Uint32 id)
{
  unsigned char password[30], loop;
  cPacket packet;
  for(loop = 0; loop < 30; loop++)
    password[loop] = '\0';
  packet.FillPacket (PCK_CharDelete);
  memcpy (packet.packet.CharDelete.m_password, password, 30);
  packet.packet.CharDelete.m_id = id;
  packet.packet.CharDelete.m_ip = 0; // should be IP
  Send (&packet);
}

bool cClient::Send_Walk (Uint8 direction)
{
  if ((direction & 0x7f) >= 8)
    return false;

  if (walk_stack.size () >= 5)
  {
  //SiENcE: Ghoulsblade: workaround hack for "character stops moving bug"
    walk_stack.clear();
    return false;
  }

  static Uint32 last_walk = 0;
  Uint32 currenttime = SDL_GetTicks ();

  if (currenttime < last_walk)
    return false;

  cCharacter *player_char = player_character ();
  if (!player_char)
    return false;

  if (player_char->moving ())
    return false;

  int walktime = 300;
  if (direction & 0x80)         // running
    walktime = 150;
  last_walk = currenttime + walktime;

  struct sWalkStackEntry entry;
  entry.requestid = walk_sequence;
  entry.direction = direction;
  entry.x = player_position[0];
  entry.y = player_position[1];
  entry.z = player_position[2];

  if ((direction & 7) == (walk_direction & 7))
      {
        entry.x += direction_worldcoords[direction & 7][0];
        entry.y += direction_worldcoords[direction & 7][1];
      }

  int new_z = GetZPositionForWalk (entry.x, entry.y, entry.z);
  if(IsInBoat(entry.x, entry.y))
   new_z=CheckBoatZ(entry.x, entry.y) + 10;
  if (new_z == 255)
    return false;

  entry.z = new_z;

  walk_stack.push_back (entry);
  player_char->MoveTo (entry.x, entry.y, entry.z);
  player_char->RotateTo (entry.direction);
  player_position[0] = entry.x;
  player_position[1] = entry.y;
  player_position[2] = entry.z;

  cPacket packet;
  packet.FillPacket (PCK_Walk);
  packet.packet.WalkRequest.m_direction = direction;
  packet.packet.WalkRequest.m_sequence = walk_sequence;
  walk_sequence++;
  if (!walk_sequence)
    walk_sequence = 0;
  Send (&packet);

  walk_direction = direction;
  return true;

}

void cClient::Send_Speech (std::string text, Uint8 mode)
{
	std::vector<Uint16> keywords;
    Uint16 count = 0;

    if(Config::GetUseSpeech())
    {
      keywords = SpeechLoader::GetInstance()->GetIDs( text );
	  count = keywords.size();
    }

	cPacket packet;
    //Artix: use speech.mul only if config option is enabled
	if ( count > 0 && Config::GetUseSpeech())
	{
		int keyw_size = 2;
		for ( int k = 0; k < count; k++ )
		{
			if ( (k & 1) == 0 )
			{
				keyw_size+=1;
			}
			else
			{
				keyw_size+=2;
			}
		}

		packet.AddByte( 0xAD );
		packet.AddWord( 12 + keyw_size + text.size() + 1 );
		packet.AddByte( (count > 0) ? 0xC0 : 0 );
		packet.AddWord( Config::GetSpeechHue() );
		packet.AddWord( 0 );
		packet.AddDword( 0x49544100 );
		packet.AddWord( ( count << 4 ) | ( (keywords.at( 0 ) & 0xF000) >> 8 ) );

		for ( int i = 0; i < count; i++ )
		{
			if ( (i & 1) == 0 )
			{
				Uint8 word =  (Uint8)keywords.at( i );
				packet.AddByte( word );
			}
			else
			{
				Uint16 nextword;

				if ( i == count - 1 )
				{
					nextword=0;
				}
				else
				{
					nextword = keywords.at( i + 1 );
				}

				Uint16 word = keywords.at( i );
				word <<= 4;
				word |= (nextword  & 0xF000);
				packet.AddWord( word );
			}
		}

		packet.AddData( (void*)text.c_str(), text.size() );
		packet.AddByte( 0 );

		Send( &packet );
	}
	else
	{
		//cPacket packet;
		packet.Clear();
		packet.FillPacket( PCK_Talk );
		packet.SetLength( 1 );
		packet.AddWord( text.size() + 9 );
		packet.AddByte( mode );
		//packet.AddWord( 100 );
		packet.AddWord( Config::GetSpeechHue() );
		packet.AddWord( 3 );
		packet.AddData( (void *)text.c_str(), text.size() );
		Send( &packet.packet, text.size() + 9 );
	}
}

void cClient::Send_SpeechUNICODE(std::string text, Uint8 mode)
{
	cUnicode unicode((char*)text.c_str());

	cPacket packet;
	packet.Clear();
	packet.FillPacket(PCK_TalkUNICODE);					// ID 0xAD
	packet.SetLength(1);
	packet.AddWord(unicode.m_unicodeLen + 12);			// Size of Packet
	packet.AddByte(mode);								// Mode (0=say,2=emote,8=whipser,9=yell)
	packet.AddWord(100);								// Text Color
	packet.AddWord(3);									// Font
	packet.AddData((void*)Config::GetClilocLang().c_str(), 4);					// Language, "ENU", "DEA", "DEU", "KOR" ...
	packet.AddData((void *)unicode.m_unicodeBuf, unicode.m_unicodeLen); // Text
	Send(&packet.packet, unicode.m_unicodeLen + 12);
}

void cClient::Send_CharName (Uint32 id, std::string name)
{
  cPacket packet;
  packet.FillPacket (PCK_CharName);
  packet.packet.CharName.m_charId = id;
  strncpy (packet.packet.CharName.m_name, name.c_str (), 29);
  Send (&packet);

}

void cClient::Send_StatusRequest (Uint32 id, Uint8 mode)
{
  cPacket packet;
  packet.AddByte (PCK_CharStatReq);
  packet.AddDword (0xedededed);
  if (mode == REQUEST_SKILLS)
    packet.AddByte (5);
  else
    packet.AddByte (4);
  packet.AddDword (id);

  Send (&packet);
}

void cClient::Send_Dye (Uint32 id, Uint16 model, Uint16 color)
{
  cPacket packet;
  packet.AddByte (PCK_DyeVat);
  packet.AddDword (id);
  packet.AddWord (model);
  packet.AddWord (color);
  Send (&packet);
}

void cClient::Send_DoubleClick (Uint32 id)
{
  cPacket packet;
  packet.AddByte (PCK_DClick);
  packet.AddDword (id);
  Send (&packet);

  last_object = id;
}

void cClient::Send_AoSCommand (Uint32 charid, Uint8 subcmd)
{
  cPacket packet;
  packet.AddByte (PCK_AOSCommand);
  packet.AddWord (0x0a);  //Not const! But for now packetsize 0x0a
  packet.AddDword(charid); //PlayerID
  packet.AddWord(subcmd);  //Subcommand
  packet.AddByte (0x07);     // Unknown
  Send (&packet);
}

void cClient::Send_Click (Uint32 id)
{
  cPacket packet;
  packet.AddByte (PCK_Click);
  packet.AddDword (id);
  Send (&packet);
}

void cClient::Send_Warmode (Uint32 mode)
{
  cPacket packet;
  packet.FillPacket (PCK_War);
  packet.packet.War.m_unk[0] = 0x00;
  packet.packet.War.m_unk[1] = 0x32;
  packet.packet.War.m_unk[2] = 0x00;
  if (mode == 0)
    packet.packet.War.m_warmode = 0x00;
  else
    packet.packet.War.m_warmode = 0x01;
  Send (&packet);
}

void cClient::Send_Action(int type, int mode)
{
	cPacket packet;
	packet.FillPacket (PCK_DoAction);

  	char buf[20];

  	switch (type) {
	   case ACTIONTYPE_SKILL:{


        int buflen;

        if(mode>9)
         buflen = 4;
        else
         buflen = 5;

        char buf2[20];
         sprintf(buf, "%i 0", mode);
         buf2[buflen] = 0;

		sprintf(buf, "%i 0", mode);

		cPacket pck;
		pck.AddByte(0x12);
		pck.AddWord(4+buflen);
		pck.AddByte(0x24);
		pck.AddData(&buf, buflen-1);
		pck.AddByte(0);

		Send(&pck);
		last_skill = mode;
		break;}
	   case ACTIONTYPE_SPELL:{
		sprintf(buf, "%i", mode);
		packet.packet.DoAction.m_type = 0x56;
		packet.packet.DoAction.m_len = strlen(buf) + 4;
		strcpy(&packet.packet.DoAction.m_name, buf);
		break;}
	   case ACTIONTYPE_OPENDOOR:{


	        cPacket p;
	        p.AddByte(0x12);
	        p.AddWord(5);
	        p.AddByte(0x58);
	        p.AddByte(0);
	        Send(&p);

	        return;
		break;}
	   case ACTIONTYPE_BOW:{
		//packet.packet.DoAction.m_type = 0xc7;
		//packet.packet.DoAction.m_len = 3 + 5;
		//strcpy(&packet.packet.DoAction.m_name, "bow");

		char buf3[20];
         sprintf(buf3, "bow", mode);
         buf3[3] = 0;

	    cPacket p;
        p.AddByte(0x12);
        p.AddWord(8);
        p.AddByte(0xc7);
        p.AddData(&buf3, 4);

        Send(&p);
        return;

        break;}
	   case ACTIONTYPE_SALUTE:{
		//packet.packet.DoAction.m_type = 0xc7;
		//packet.packet.DoAction.m_len = 6 + 5;
		//strcpy(&packet.packet.DoAction.m_name, "salute");


		char buf3[20];
         sprintf(buf3, "salute", mode);
         buf3[6] = 0;

	    cPacket p;
        p.AddByte(0x12);
        p.AddWord(11);
        p.AddByte(0xc7);
        p.AddData(&buf3, 7);

        Send(&p);
        return;

		break;}
	   default:
		return;

        }

        Send(&packet);

}


void cClient::Send_AttackRequest (Uint32 id)
{
  last_attack = id;
  cPacket packet;
  packet.AddByte (PCK_Attack);
  packet.AddDword (id);
  Send (&packet);
}

void cClient::Send_SkillLock (Uint32 skill, Uint32 lock)
{
  cPacket packet;

  if (lock <= 2)
      {
        packet.packet.SkillLock.m_cmd = PCK_Skill;
        packet.packet.SkillLock.m_len = 6;
        packet.packet.SkillLock.m_skillId = skill;
        packet.packet.SkillLock.m_skillLock = lock;
        Send (&packet);
      }
}

void cClient::Send_PickupRequest (Uint32 id, Uint16 count)
{
  cPacket packet;
  packet.FillPacket (PCK_ItemPickupReq);
  packet.packet.ItemPickupReq.m_itemID = id;
  packet.packet.ItemPickupReq.m_num = count;
  Send (&packet);
}


void cClient::Send_DropRequest (Uint32 id, int x, int y, int z,
                                Uint32 container)
{
  cPacket packet;
  packet.FillPacket (PCK_ItemDropReq);
  packet.packet.ItemDropReq.m_id = id;
  packet.packet.ItemDropReq.m_x = (Uint16) x;
  packet.packet.ItemDropReq.m_y = (Uint16) y;
  packet.packet.ItemDropReq.m_z = (Sint8) z;
  packet.packet.ItemDropReq.m_container = container;
  Send (&packet);
}


void cClient::Send_Target (Uint32 cursorid, int x, int y, int z, int artid)
{
  cPacket packet;
  packet.FillPacket (PCK_Target);
  packet.packet.Target.m_type = 0x01;
  packet.packet.Target.m_cursorID = cursorid;
  packet.packet.Target.m_clickX = x;
  packet.packet.Target.m_clickY = y;
  packet.packet.Target.m_clickZ = (Uint8) ((Sint8) z);
  packet.packet.Target.m_Model = artid;
  Send (&packet);
}

void cClient::Send_ItemEquipReq (Uint32 itemid, Uint32 charid, Uint16 model)
{
  struct TileDataStaticEntry entry;
  if (pTileDataLoader.LoadEntry (model, &entry))
      {
        cPacket packet;
        packet.FillPacket (PCK_ItemEquipReq);
        packet.packet.ItemEquipReq.m_id = itemid;
        packet.packet.ItemEquipReq.m_layer = entry.quality;
        packet.packet.ItemEquipReq.m_charid = charid;
        Send (&packet);
      }
}

void cClient::Send_Target (Uint32 cursorid, Uint32 targetid)
{

  cPacket packet;
  packet.FillPacket (PCK_Target);
  packet.packet.Target.m_type = 0x00;
  packet.packet.Target.m_cursorID = cursorid;
  packet.packet.Target.m_clickedID = targetid;
  Send (&packet);

  last_target = targetid;
}

void cClient::Send_MenuChoice (Uint32 dialogid, Uint16 menuid, Uint16 index,
                               Uint16 model)
{
  cPacket packet;
  packet.FillPacket (PCK_MenuChoice);
  packet.packet.MenuChoice.m_dialogId = dialogid;
  packet.packet.MenuChoice.m_menuId = menuid;
  packet.packet.MenuChoice.m_index = index;
  packet.packet.MenuChoice.m_modelNum = model;
  packet.packet.MenuChoice.m_unknown = 0;
  Send (&packet);
}


void cClient::Send_PopupRequest (Uint32 serial, int x, int y)
{
//  popupx = x;
//  popupy = y;
  m_popupx = x;
  m_popupy = y;
  cPacket packet;
  packet.AddByte (0xBF);
  packet.AddWord (0x09);        // packet size
  packet.AddWord (0x13);        // Popup Request Sub-Command
  packet.AddDword (serial);
  Send (&packet);
}


void cClient::SendPopupChoice (Uint16 tag)
{
  cPacket packet;
  packet.AddByte (0xBF);
  packet.AddWord (0xB);         // packet size
  packet.AddWord (0x15);        // Popup Request Sub-Command
  packet.AddDword (popserial);
  packet.AddWord (tag);
  Send (&packet);
}


void cClient::OnGameStart (void (*callback) (void))
{
  callback_OnGameStart = callback;
}


void cClient::OnTeleport (void (*callback) (int x, int y, int z))
{
  callback_OnTeleport = callback;
}

void cClient::OnServerList (void (*callback) (int count))
{
  callback_OnServerList = callback;
}

void cClient::OnCharList (void (*callback) (int count))
{
  callback_OnCharList = callback;
}

void cClient::OnNetError (void (*callback) (unsigned int error))
{
  callback_OnNetError = callback;
}

void cClient::OnLightLevel (void (*callback) (unsigned int lightlevel))
{
  callback_OnLightLevel = callback;
}

void cClient::OnStatus (void (*callback) (unsigned int charid, Uint8 flag))
{
  callback_OnStatus = callback;
}

void cClient::OnContainerContent (void (*callback) (unsigned int containerid))
{
  callback_OnContainerContent = callback;
}

void cClient::
OnOpenContainer (void (*callback)
                 (unsigned int containerid, unsigned int gumpid))
{
  callback_OnOpenContainer = callback;
}

void cClient::
OnSpeech (void (*callback)
          (const char *text, const char *speaker, unsigned int speakerid,
           unsigned short color))
{
  callback_OnSpeech = callback;
}

void cClient::
OnStatChange (void (*callback)
              (unsigned int charid, unsigned int type, unsigned int value,
               unsigned int maxvalue))
{
  callback_OnStatChange = callback;
}
void cClient::
OnSkill (void (*callback) (unsigned int skillid, unsigned int value))
{
  callback_OnSkill = callback;
}

void cClient::OnDragCancel (void (*callback) (void))
{
  callback_OnDragCancel = callback;
}

void cClient::OnBuyWindowOpen (void (*callback) (Uint32 id))
{
  callback_OnBuyWindowOpen = callback;
}

void cClient::
OnBuyWindowAdd (void (*callback)
                (Uint32 id, int model, int count, Uint16 hue, Uint32 price,
                 char *name))
{

  callback_OnBuyWinAdd = callback;

}

void cClient::OnSellWindowOpen (void (*callback) (Uint32 id))
{
  callback_OnSellWindowOpen = callback;
}

void cClient::
OnSellWindowAdd (void (*callback)
                 (Uint32 id, Uint16 model, Uint16 count, Uint16 hue,
                  Uint16 price, char *name))
{

  callback_OnSellWinAdd = callback;

}

void cClient::
OnTarget (void (*callback) (unsigned int cursorid, unsigned int type))
{
  callback_OnTarget = callback;
}

void cClient::
OnPaperdoll (void (*callback)
             (unsigned int charid, const char *text, Uint8 flag))
{
  callback_OnPaperdoll = callback;
}

void cClient::
OnTradeStart (void (*callback) (Uint32 id1, Uint32 id2, char *name))
{
  callback_OnTradeStart = callback;
}

void cClient::
OnTradeCheck (void (*callback) (Uint32 check1, Uint32 check2, int closewin))
{
  callback_OnTradeCheck = callback;
}

void cClient::
OnTradeAdd (void (*callback)
            (Uint16 model, Uint32 tradeid, Uint16 count, Uint16 hue, Uint16 x,
             Uint16 y))
{
  callback_OnTradeAdd = callback;
}

void cClient::OnAttackReply (void (*callback) (unsigned int charid))
{
  callback_OnAttackReply = callback;
}

void cClient::
OnDyeWindow (void (*callback) (unsigned int charid, unsigned int model))
{
  callback_OnDyeWindow = callback;
}

void cClient::OnWarmode (void (*callback) (unsigned int mode))
{
  callback_OnWarmode = callback;
}

void cClient::
OnMenuItems (void (*callback)
             (unsigned int id, unsigned int context, unsigned int count,
              char *name))
{
  callback_OnMenuItems = callback;
}

void cClient::OnPopupDisplay (void (*callback) (int count, int x, int y))
{
  callback_OnPopupDisplay = callback;
}

void cClient::
OnOpenSpellBook (void (*callback)
                 (Uint32 serial, Uint16 model, Uint8 * circles))
{
  callback_OnOpenSpellBook = callback;
}

cCharacter *cClient::player_character()
{
	return pCharacterList.Get( player_char );
}

bool cClient::Walk_Simple (Uint8 action)
{
  int direction;

  if(runflag==0x80)
   walk_direction |= 0x80;
  else
  if(walk_direction & 0x80)
   walk_direction ^= 0x80;

  switch (action)
      {
      case WALK_RIGHT:
        direction = (walk_direction & 7) + 1;
        if (direction >= 8)
          direction -= 8;
        return Send_Walk (direction + (walk_direction & 0x80));
      case WALK_LEFT:
        direction = ((Sint8) (walk_direction & 7)) - 1;
        if (direction < 0)
          direction += 8;
        return Send_Walk (direction + (walk_direction & 0x80));
      case WALK_FORWARD:
        return Send_Walk (walk_direction);
      }
  return false;
}

bool cClient::Walk (Uint8 direction)
{
  int delta_dir = (int) (direction & 7) - (int) (walk_direction & 7);

  if (delta_dir != 0)           // Rotate
    Send_Walk (direction);

  if ((delta_dir == 0) && (direction != walk_direction))    // Just run faster / slower
      {
        Send_Walk (direction);
      }

  return Walk_Simple (WALK_FORWARD);
}


int cClient::GetZPositionForWalk (int destx, int desty, int srcz)
{

  cMapblock *block = pMapbufferHandler.buffer ()->CreateBlock (destx / 8, desty / 8);
  if (!block)
    return 255;

  int groundz = block->GetWalkZ (destx % 8, desty % 8, srcz);
  if (groundz == 255)
    return 255;

  dynamiclist_t *dynamics = pDynamicObjectList.GetList ();
  dynamiclist_t::iterator iter;

  for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
  {
     cDynamicObject *object = iter->second;
    if ((object->x == destx) && (object->y == desty))
    {
      struct TileDataStaticEntry entry;
      if (pTileDataLoader.LoadEntry (object->model, &entry))
      {
        int z = object->z + entry.height;
        if ((z < srcz + 8) && (z > groundz))
          groundz = z;
      }
    }
  }

  if (groundz > srcz + 8)
    return 255;

  if (groundz < srcz - 8)
    return 255;

  return groundz;
}

cLoginListEntry *cClient::GetServerListEntry (Uint32 index)
{
  if (index >= login_server_list.size ())
    return NULL;
  return login_server_list[index];
}

cLoginListEntry *cClient::GetLocationListEntry (Uint32 index)
{
  if (index >= login_location_list.size ())
    return NULL;
  return login_location_list[index];
}


cLoginListEntry *cClient::GetCharListEntry (Uint32 index)
{
  if (index >= login_char_list.size ())
    return NULL;
  return login_char_list[index];
}


void cClient::GetMenuItem (int index, std::string & text, int &model)
{
  model = 0;
  if ((index >= 0) && (index < (int) menu_entries.size ()))
    text = menu_entries[index];
  if ((index >= 0) && (index < (int) menu_models.size ()))
    model = menu_models[index];
}


void cClient::SendGumpDialogRet (int gumpID, int playerID, int ret_value)
{
  cPacket packet;
  packet.AddByte (PCK_GumpDialogRet);
  packet.AddWord (0x17);
  packet.AddDword ((Uint32) playerID);
  packet.AddDword ((Uint32) gumpID);
  packet.AddDword ((Uint32) ret_value);
  packet.AddDword (0);
  packet.AddDword (0);
  Send (&packet);
}

void cClient::CastSpell (int spellid)
{


  cPacket packet;

  if(spellbooktype == 1)
  {
       int len;
       char buf[20];
       (spellid > 9)?len = 7:len=6;
       sprintf(buf, "%i", spellid);
       packet.AddByte (0x12);
       packet.AddWord (len);
       packet.AddByte (0x56);

       packet.AddData(&buf, len - 5);
       packet.AddByte (0);
       Send(&packet);
       return;
  }


  packet.AddByte (0xBF);
  packet.AddWord (0x09);
  packet.AddWord (0x1C);
  packet.AddWord (0x02);
  packet.AddWord (spellid);
  Send (&packet);

  last_spell = spellid;
}

Uint32 cClient::GetEnemy ()
{
  if ( Config::GetIsUox3() && (enemy == 0))
  {
       return 0xFFFFFFFF;
  }

  return enemy;
}

void cClient::toggleRunMode()
{
 (runflag==0)?runflag=0x80:runflag=0;
}

void cClient::Act_ParticleEffect(cPacket * packet)
{
 packet->SetPosition (1);

 //preamble
 Uint8 type = packet->GetByte();
 Uint32 source_serial = packet->GetDword();
 Uint32 dest_serial = packet->GetDword();
 Uint16 model = packet->GetWord();
 Uint16 x = packet->GetWord();
 Uint16 y = packet->GetWord();
 Uint8  z = packet->GetByte();
 Uint16 x2 = packet->GetWord();
 Uint16 y2 = packet->GetWord();
 Uint8  z2 = packet->GetByte();
 Uint8  speed = packet->GetByte();
 Uint8  duration = packet->GetByte();    //loop
 Uint16 unkn = packet->GetWord();        //hue
 Uint8  fixed_dir = packet->GetByte();   //oneDir
 Uint8  explode = packet->GetByte();
 Uint32  hue = packet->GetDword();       //?
 Uint32  mode = packet->GetDword();

 //particle effect
 Uint16 particle_id = packet->GetWord();
 Uint16 explode_id = packet->GetWord();
 Uint16 mov_id = packet->GetWord();
 Uint32 itemid = packet->GetDword();
 Uint8 layer = packet->GetByte();
 Uint16 unk = packet->GetWord();

 //char idstr[2];
 //idstr[1] = 0;
 //sprintf(idstr, "%i", (int) type);

 std::cout << "Particle: type=" << (int)type << ", ID=" << particle_id << endl;

 switch(type)
 {
  case 2:{ //xyz particle
     std::cout << "Stationary particle" << endl;
     std::cout << "Z: " << (int)z << endl;
     pParticleEngine.AddEffect(particle_id, (float)x * 1.0f, (float)y * 1.0f, (float)z / 10.0f);
     break;
     }
  case 3:{
     std::cout << "On player particle" << endl;
     cCharacter *character = NULL;
     character = pCharacterList.Get (source_serial);
     if(!character)
      break;
      std::cout << "Z: " << (int)character->z() << endl;
     Uint32 p_id = pParticleEngine.AddEffect(particle_id, (float)character->x() * 1.0f, (float)character->y() * 1.0f, (float)character->z() * 1.0f);
     character->setParticle(p_id);
     break;
   }
  case 0: {
       std::cout << "Moving FX: " << endl;
       pParticleEngine.AddMovingEffect(particle_id, (float) x * 1.0f, (float) y * 1.0f, (float)z/10.0f, (float) x2 * 1.0f, (float) y2 * 1.0f, (float)z2/10.0f);
       break;
       }
  case 1: break;
 }
}
