//
// File: CSLHandler.cpp
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

#include "csl/CSLHandler.h"

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cassert>

#include "Logger.h"

#include <csl/ZExcept.hpp>

#include "Game.h"
#include "SDLEvent.h"
#include "gui/Image.h"
#include "gui/Border.h"
#include "gui/ImageArt.h"
#include "gui/Button.h"
#include "gui/InputField.h"
#include "gui/Label.h"
#include "gui/Container.h"
#include "gui/ItemContainer.h"
#include "gui/GUIHandler.h"
#include "gui/Textbox.h"
#include "gui/Paperdoll.h"
#include "gui/TextManager.h"
#include "gui/MultiLabel.h"
#include "gui/HTMLGumpParser.h"

#include "renderer/DynamicObjects.h"
#include "renderer/SDLScreen.h"

#include "loaders/GumpLoader.h"
#include "loaders/SkillLoader.h"
#include "renderer/Camera.h"

#include "net/Client.h"

#include "Config.h"
#include "include.h"
#include "sound/SoundMixer.h"




int gamestate = GAMESTATE_MENU;

CSLHandler pCSLHandler;

extern SDLEvent *SDLevent;
//extern SDLScreen *SDLscreen;

static ZCsl *csl = 0;           // csl object ptr
static ZBoolean cslOk (zFalse); // ok flag
static ZString module ("iris"); // module name

static bool initialized = false;

int act_container_id = 0;

// External Declarations for CSLForm
extern ZString api_control_getid(ZCsl* aCsl);
extern ZString api_gui_addform (ZCsl* aCsl);


int on_closehandler (Control * control)
{
  char *func = control->GetScriptFunction (FUNC_ONCLOSE);
  char buf[15];
  sprintf (buf, "%d", control->GetID ());

  if (func)
    pCSLHandler.ExecuteFunction (func, buf);
  return -1;
}

int on_clickhandler (Control * control)
{
  char *func = control->GetScriptFunction (FUNC_ONCLICK);
  char buf[15];
  sprintf (buf, "%d", control->GetID ());

  if (func)
    pCSLHandler.ExecuteFunction (func, buf);
  return -1;
}

int on_mousedownhandler (Control * control)
{
  char *func = control->GetScriptFunction (FUNC_ONMOUSEDOWN);
  char buf[15];
  sprintf (buf, "%d", control->GetID ());

  if (func)
    pCSLHandler.ExecuteFunction (func, buf);
  return -1;
}

int on_mouseuphandler (Control * control)
{
  char *func = control->GetScriptFunction (FUNC_ONMOUSEUP);
  char buf[15];
  sprintf (buf, "%d", control->GetID ());

  if (func)
    pCSLHandler.ExecuteFunction (func, buf);
  return -1;
}

int on_keypressedhandler (Control * control, unsigned short key)
{
  char *func = control->GetScriptFunction (FUNC_ONKEYPRESSED);
  char buf[15], buf2[15];
  sprintf (buf, "%d", control->GetID ());
  sprintf (buf2, "%d", key);

  if (func)
    pCSLHandler.ExecuteFunction (func, buf, buf2);
  return -1;
}

void net_handler_serverlist (int count)
{
  char buf[15];
  sprintf (buf, "%d", count);
  if (pCSLHandler.ExecuteFunction ("net_onserverlist", buf))
    Logger::WriteLine ("No Server List Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);
}

void net_handler_charlist (int count)
{
  char buf[15];
  sprintf (buf, "%d", count);
  if (pCSLHandler.ExecuteFunction ("net_oncharlist", buf))
    Logger::WriteLine ("No Char List Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);
}

void net_handler_startgame (void)
{
  if (pCSLHandler.ExecuteFunction ("net_startgame"))
    Logger::WriteLine ("No Start Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  Game::GetInstance()->SetPause (false);

  return;
}

void net_handler_offline (void)
{
  if (pCSLHandler.ExecuteFunction ("net_offline"))
    Logger::WriteLine ("No Offline Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_handler_error (unsigned int error)
{
  char buf[15];
  sprintf (buf, "%d", error);

  if (pCSLHandler.ExecuteFunction ("net_error", buf))
    Logger::WriteLine ("No Neterror Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}


void net_handler_lightlevel (unsigned int lightlevel)
{
  char buf[15];
  sprintf (buf, "%d", lightlevel);

  if (pCSLHandler.ExecuteFunction ("net_onlightlevel", buf))
    Logger::WriteLine ("No Lightlevel Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

static ZString api_iris_playmusic (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int format = aCsl->get ("format").asInt ();
  int argCount = aCsl->get ("argCount").asInt ();
  int volume = 0;
  if (argCount > 2)
    volume = aCsl->get ("volume").asInt ();
  if ( SoundMix::GetInstance() )
	  SoundMix::GetInstance()->PlayMusic (id, format, volume);
  else
    return "-1";
  return "0";

}

static ZString api_iris_playsound (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int volume = aCsl->get ("volume").asInt ();

  if (SoundMix::GetInstance())
	  SoundMix::GetInstance()->PlaySoundWithVolume (id, volume);
  else
    return "-1";
  return "0";
}

void net_paperdoll (unsigned int charid, const char *text, Uint8 flag)
{
  char buf[15];
  char buf2[15];
  sprintf (buf, "%d", charid);
  sprintf (buf2, "%d", flag);

  if (pCSLHandler.ExecuteFunction ("net_paperdoll", buf, (char *) text, buf2))
    Logger::WriteLine ("No Paperdoll Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_status (unsigned int charid, Uint8 flag)
{
  char buf[15];
  char buf2[15];
  sprintf (buf, "%d", charid);
  sprintf (buf2, "%d", flag);

  if (pCSLHandler.ExecuteFunction ("net_status", buf, buf2))
    Logger::WriteLine ("No Status Info Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_trade (Uint32 id1, Uint32 id2, char *name)
{
  char buf1[15];
  char buf2[15];
  //char buf3[15];

  sprintf (buf1, "%d", id1);
  sprintf (buf2, "%d", id2);

  if (pCSLHandler.
      ExecuteFunction ("net_tradestart", buf1, buf2, (char *) name))
    Logger::WriteLine ("No Trade Window Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_tradecheck (Uint32 check1, Uint32 check2, int closewin)
{
  char buf1[15];
  char buf2[15];
  char buf3[15];

  sprintf (buf1, "%d", check1);
  sprintf (buf2, "%d", check2);
  sprintf (buf3, "%d", closewin);

  if (pCSLHandler.ExecuteFunction ("net_tradecheck", buf1, buf2, buf3))
    Logger::WriteLine ("No Trade Window Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_tradeadd (Uint16 model, Uint32 tradeid, Uint16 count, Uint16 hue,
                   Uint16 x, Uint16 y)
{
  char buf1[15];
  char buf2[15];
  char buf3[15];
  char buf4[15];
  char buf5[15];
  char buf6[15];

  sprintf (buf1, "%d", model);
  sprintf (buf2, "%d", tradeid);
  sprintf (buf3, "%d", count);
  sprintf (buf4, "%d", hue);
  sprintf (buf5, "%d", x);
  sprintf (buf6, "%d", y);

  if (pCSLHandler.
      ExecuteFunctionPlus ("net_tradeadd", buf1, buf2, buf3, buf4, buf5,
                           buf6))
    Logger::WriteLine ("No Trade Window Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}


void net_skill (unsigned int skillid, unsigned int skillvalue)
{
  char buf[15], buf2[15];
  sprintf (buf, "%d", skillid);
  sprintf (buf2, "%d", skillvalue);

  if (pCSLHandler.ExecuteFunction ("net_skills", buf, buf2))
    Logger::WriteLine ("No Skill Change Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_spellbook (Uint32 serial, Uint16 model, Uint8 * circles)
{



  char buf[15];
  char buf2[15];
  sprintf (buf, "%d", serial);
  sprintf (buf2, "%d", model);

  if (circles)
      {
        for (int i = 0; i < 8; i++)
            {
              csl->set ("circles[" + ZString (i) + "]",
                        ZString ((int) circles[i]));
            }

      }

  if (pCSLHandler.ExecuteFunction ("net_spellbookdisplay", buf, buf2))
    Logger::WriteLine ("No Spellbook Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

}


void net_statchange (unsigned int charid, unsigned int type,
                     unsigned int value, unsigned int maxvalue)
{
  char buf[15], buf2[15], buf3[15], buf4[15];
  sprintf (buf, "%d", charid);
  sprintf (buf2, "%d", type);
  sprintf (buf3, "%d", value);
  sprintf (buf4, "%d", maxvalue);

  if (pCSLHandler.ExecuteFunction ("net_statchange", buf, buf2, buf3, buf4))
    Logger::WriteLine ("No Stat Change Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_opencontainer (unsigned int contid, unsigned int gumpid)
{
  char buf[15], buf2[15];
  sprintf (buf, "%d", contid);
  sprintf (buf2, "%d", gumpid);

  if (pCSLHandler.ExecuteFunction ("net_opencontainer", buf, buf2))
    Logger::WriteLine ("No Container Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_containercontent (unsigned int contid)
{
  char buf[15];
  sprintf (buf, "%d", contid);

  if (pCSLHandler.ExecuteFunction ("net_containercontent", buf))
    Logger::WriteLine ("No Container Content Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}






void game_onstatusdrag (Uint32 charid, int mousex, int mousey)
{
  char buf[15], buf2[15], buf3[15];
  sprintf (buf, "%d", charid);
  sprintf (buf2, "%d", mousex);
  sprintf (buf3, "%d", mousey);

  if (pCSLHandler.ExecuteFunction ("net_onstatusdrag", buf, buf2, buf3))
    Logger::WriteLine ("No Status Drag Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

}

void game_onaostooltip (Uint32 id, int count, int x, int y)
{

  char buf[15];
  char buf2[15];

  char buf3[15];
  char buf4[15];

  sprintf (buf, "%d", id);
  sprintf (buf2, "%d", count);
  sprintf (buf3, "%d", x);
  sprintf (buf4, "%d", y);

  if (pCSLHandler.ExecuteFunction ("net_onaostooltip", buf, buf2, buf3, buf4))
    Logger::WriteLine ("No Status Drag Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);
}

void game_ondynamicdrag (Uint32 id, Uint16 model, int count, int x, int y,
                         Uint32 container)
{
  char buf[15], buf2[15], buf3[15], buf4[15], buf5[15], buf6[15];
  sprintf (buf, "%d", id);
  sprintf (buf2, "%d", model);
  sprintf (buf3, "%d", count);
  sprintf (buf4, "%d", x);
  sprintf (buf5, "%d", y);
  sprintf (buf6, "%d", container);
  if (pCSLHandler.
      ExecuteFunctionPlus ("net_ondynamicdrag", buf, buf2, buf3, buf4, buf5,
                           buf6))
    Logger::WriteLine ("No  Dynamic Drag Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

}

void net_warmodechange (unsigned int mode)
{
  char buf[15];
  sprintf (buf, "%d", mode);
  bool war_mode = mode != 0x00;
  pUOGUI.setwarmode (war_mode);
  if (pCSLHandler.ExecuteFunction ("net_warmodechange", buf))
    Logger::WriteLine ("No Warmode Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_attackreply (unsigned int id)
{
  char buf[15];
  sprintf (buf, "%d", id);

  if (pCSLHandler.ExecuteFunction ("net_attackreply", buf))
    Logger::WriteLine ("No Attack Reply Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_buyadd (Uint32 id, int model, int count, Uint16 hue, Uint32 price,
                 char *name)
{

  char buf1[15];
  char buf2[15];
  char buf3[15];
  char buf4[15];
  char buf5[15];

  sprintf (buf1, "%d", id);
  sprintf (buf2, "%d", model);
  sprintf (buf3, "%d", count);
  sprintf (buf4, "%d", hue);
  sprintf (buf5, "%d", price);

  if (pCSLHandler.

      ExecuteFunctionPlus ("net_buywinadd", buf1, buf2, buf3, buf4, buf5,
                           (char *) name))
      {
        Logger::WriteLine ("No Buy Window Add Handler - Please Check your Scripts",
                    __FILE__, __LINE__, LEVEL_WARNING);

        return;
      }

}
void net_buy (Uint32 id)
{

  char buf[15];
  sprintf (buf, "%d", id);

  if (pCSLHandler.ExecuteFunction ("net_buywindow", buf))
    Logger::WriteLine ("No Buy Window Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

static ZString api_net_sendpickup (ZCsl * aCsl)
{

  int id = aCsl->get ("id").asInt ();
  int model = aCsl->get ("model").asInt ();
  int count = aCsl->get ("count").asInt ();



  Game::GetInstance()->SendPickup (id, model, count);
  return "";
}


void net_selladd (Uint32 id, Uint16 model, Uint16 count, Uint16 hue,
                  Uint16 price, char *name)
{

  char buf1[15];
  char buf2[15];
  char buf3[15];
  char buf4[15];
  char buf5[15];

  sprintf (buf1, "%d", id);
  sprintf (buf2, "%d", model);
  sprintf (buf3, "%d", count);
  sprintf (buf4, "%d", hue);
  sprintf (buf5, "%d", price);

  if (pCSLHandler.
      ExecuteFunctionPlus ("net_sellwinadd", buf1, buf2, buf3, buf4, buf5,
                           (char *) name))
      {
        Logger::WriteLine ("No Sell Window Add Handler - Please Check your Scripts",
                    __FILE__, __LINE__, LEVEL_WARNING);

        return;
      }

}

void net_sell (Uint32 id)
{

  char buf[15];
  sprintf (buf, "%d", id);

  if (pCSLHandler.ExecuteFunction ("net_sellwindow", buf))
    Logger::WriteLine ("No Sell Window Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_WARNING);

  return;
}

void net_handler_speech (const char *text, const char *speaker,
                         unsigned int speakerid, unsigned short color)
{
  char buf[15];
  char buf2[15];
  sprintf (buf, "%d", speakerid);
  sprintf (buf2, "%d", color);
  if (pCSLHandler.
      ExecuteFunction ("net_speech", (char *) text, (char *) speaker, buf,
                       buf2))
    Logger::WriteLine ("No Speech Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_dyewindow (unsigned int id, unsigned int model)
{
  char buf[15], buf2[15];
  sprintf (buf, "%d", id);
  sprintf (buf2, "%d", model);

  if (pCSLHandler.ExecuteFunction ("net_dyewindow", buf, buf2))
    Logger::WriteLine ("No Dye Window Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}


void net_menuitems (unsigned int id, unsigned int context, unsigned int count,
                    char *name)
{
  char buf[15], buf2[15], buf3[15];
  sprintf (buf, "%d", id);
  sprintf (buf2, "%d", context);
  sprintf (buf3, "%d", count);

  if (pCSLHandler.ExecuteFunction ("net_menuitems", buf, buf2, buf3, name))
    Logger::WriteLine ("No Menu Item Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);

  return;
}

void net_displaypopup (int count, int x, int y)
{
  char buf[15];
  char buf2[15];
  char buf3[15];
  sprintf (buf, "%d", count);
  sprintf (buf2, "%d", x);
  sprintf (buf3, "%d", y);
  if (pCSLHandler.ExecuteFunction ("net_displaypopup", buf, buf2, buf3))
    Logger::WriteLine ("No Popup Menu Handler - Please Check your Scripts", __FILE__,
                __LINE__, LEVEL_WARNING);
}

static Control *api_findcontrol (unsigned int id)
{
        unsigned int c_id = id >> 16;
        if (c_id)
            {
              Container *container = NULL;
              Control *control = pUOGUI.GetControl (c_id);
              if (control)
                if (control->getType () == CONTROLTYPE_CONTAINER)
                  container = (Container *) control;
              if (container)
                return container->GetControl (id);
            }

        return pUOGUI.GetControl (id);
}

static Container *api_findcontainer (unsigned int id)
{
        unsigned int c_id = id >> 16;
        if (c_id)
            {
              Container *container = NULL;
              Control *control = pUOGUI.GetControl (c_id);
              if (control)
                if (control->getType () == CONTROLTYPE_CONTAINER)
                  container = (Container *) control;
              return container;
            }
        return NULL;
}

void edit_handler_position_reply (int x, int y, int z)
{
  char buf1[15];
  sprintf (buf1, "%d", x);

  char buf2[15];
  sprintf (buf1, "%d", y);

  char buf3[15];
  sprintf (buf1, "%d", z);

  if (pCSLHandler.ExecuteFunction ("net_error", buf1, buf2, buf3))
    Logger::WriteLine ("No Position Reply Handler - Please Check your Scripts",
                __FILE__, __LINE__, LEVEL_ERROR);

  return;
}

void api_addcontrol( Control *control )
{
	control->OnClose( on_closehandler );
	control->OnMouseDown( on_mousedownhandler );
	control->OnMouseUp( on_mouseuphandler );
	Container *container = NULL;

	if ( act_container_id )
	{
		Control *ccontrol = pUOGUI.GetControl( act_container_id );
		if ( ccontrol )
		{
			if ( ccontrol->getType() == CONTROLTYPE_CONTAINER )
			{
				container = (Container *)ccontrol;
			}
		}
	}

	if ( container )
	{
		container->AddControl( control );
	}
	else
	{
		pUOGUI.AddControl( control );
	}
}

static ZString api_iris_log (ZCsl * aCsl)
{
       Logger::WriteLine ("CSL | " + std::string (aCsl->get ("text").buffer ()));
       return "";
}

static ZString api_iris_disconnect (ZCsl * aCsl)
{
       Game::GetInstance()->Disconnect ();
       return "";
}

static ZString api_iris_deinit (ZCsl * aCsl)
{
       Logger::WriteDebug( "Game.DeInit() - This should never happen." );
       //Game::GetInstance()->DeInit ();
       return "";
}

static ZString api_iris_init (ZCsl * aCsl)
{
	// Needs recoding
	Logger::WriteDebug( "Game.Init() - This should never happen." );
	Game::GetInstance()->Init ();
	return "";
}

static ZString api_iris_connect (ZCsl * aCsl)
{
	Game::GetInstance()->Connect( net_handler_error );
	pCSLHandler.InitNet();
	return "";
}

static ZString api_iris_setlightlevel (ZCsl * aCsl)
{
  int level = aCsl->get ("level").asInt ();
  Renderer *renderer = Game::GetInstance()->GetRenderer ();

  if ((level < 0) || (level > 31) || !renderer)
    return "0";

  renderer->setLightLevel ((31 - level) / 31.0f);
  return "-1";

}

static ZString api_gui_clear (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  if (argCount == 1)
      {
        int id = aCsl->get ("id").asInt ();
              Control *control = api_findcontrol (id);
              if (control)
                if (control->getType () == CONTROLTYPE_CONTAINER)
                    {
                      ((Container *) control)->ClearControls ();
                      return "0";
                    }
      }
  else
      {
          pUOGUI.ClearControls ();
      }
  return "";
}

static ZString api_gui_setfocus (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
        unsigned int c_id = id >> 16;
        if (c_id)
            {
              Container *container = NULL;
              Control *control = pUOGUI.GetControl (c_id);
              if (control)
                if (control->getType () == CONTROLTYPE_CONTAINER)
                  container = (Container *) control;
              if (container)
                container->SetFocus (id);
            }

        pUOGUI.SetFocus (id);
  return "0";
}

static ZString api_gui_setdefaultfocus (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
        Control *control = pUOGUI.GetControl (id);
        pUOGUI.SetDefaultFocus (control);
  return "0";
}



static ZString api_net_lastobj(ZCsl * aCsl)
{

  if(pClient)
   return ZString(pClient->lastObject());
  return "-1";

}

static ZString api_net_lasttarget(ZCsl * aCsl)
{

  if(pClient)
   return ZString(pClient->lastTarget());
  return "-1";

}

static ZString api_net_lastskill(ZCsl * aCsl)
{

  if(pClient)
   return ZString(pClient->lastSkill());
  return "-1";

}

static ZString api_net_lastspell(ZCsl * aCsl)
{

  if(pClient)
   return ZString(pClient->lastSpell());
  return "-1";

}

static ZString api_net_lastattack(ZCsl * aCsl)
{

  if(pClient)
   pClient->Send_AttackRequest(pClient->lastAttack());
   return "0";

}

static ZString api_net_sendtarget(ZCsl * aCsl)
{
  int id = aCsl->get("id").asInt();

  if(pClient)
   pClient->Send_Target(Game::GetInstance()->cursorid(), (Uint32)id);
  Game::GetInstance()->click_mode(CLICK_NORMAL);
  return "0";

}

static ZString api_net_waittarget(ZCsl* aCsl)
{
	if(pClient){std::string fname = std::string(aCsl->get("funcname").buffer());
 std::cout << "fname: " << fname << std::endl;
 pClient->wait_for_target(fname);}


 return "0";
}

static ZString api_game_settimer(ZCsl* aCsl)
{

	std::string func = std::string(aCsl->get("func").buffer());
 Game::GetInstance()->SetTimerFunction(func, aCsl->get("time").asInt());

 return "0";
}

static ZString api_camera_move(ZCsl* aCsl)
{
int dir = aCsl->get("dir").asInt();
int amount = aCsl->get("amount").asInt();
//if(pCamera){
    switch(dir)
    {
    case 0: pCamera.Rotate(-(amount / 3.0f), 0.0f, -0 / 3.0f); break; // down
    case 1: pCamera.Rotate((amount / 3.0f), 0.0f, -0 / 3.0f);break; // up
    case 2: pCamera.Rotate(0.0f, 0.0f, -(amount / 3.0f));break;//left
    case 3: pCamera.Rotate(0.0f, 0.0f, (amount / 3.0f));break;
    default: break;
    }

//}


 return "0";
}

static ZString api_camera_zoom(ZCsl* aCsl)
{
int dir = aCsl->get("type").asInt();
int amount = aCsl->get("amount").asInt();
//if(pCamera){
    switch(dir){
    case 0:  pCamera.ChangeZoom(-amount * 0.05f); break; // in
    case 1: pCamera.ChangeZoom(amount * 0.05f);break; // out

    default: break;
    }

//}


 return "0";
}


static ZString api_net_togglerun(ZCsl* aCsl)
{

 if(pClient)
  pClient->toggleRunMode();
 return "0";
}

static ZString api_camera_firstperson(ZCsl* aCsl)
{
//if(!pCamera)
 //return "-1";
	int selfview = aCsl->get("selfview").asInt();
	float actualzoom = pCamera.GetZoom();
	float angle_x = pCamera.GetAngleX();
	pCamera.ChangeZoom(-actualzoom);
	if( !Config::GetHideSelf() )
		pCamera.Rotate(-45.0f, 0.0f, -0 / 3.0f);
	if(selfview)
		pCamera.ChangeZoom(-3.4f);

	return "0";
}

static ZString api_camera_reset(ZCsl * aCsl)
{
       pCamera.Reset();
       Config::SetHideSelf( 0 );
       return "0";
}

static ZString api_gui_translate (ZCsl * aCsl)
{
    pUOGUI.SetPosition (aCsl->get ("x").asInt (), aCsl->get ("y").asInt ());

  return "0";
}


static ZString api_gui_setcontainer (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int id = 0;
  if (argCount == 1)
    id = aCsl->get ("id").asInt ();

  act_container_id = id;

  return "0";
}

static ZString api_gui_addgump (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
  if (argCount == 4)
    flags = aCsl->get ("flags").asInt ();

        Image *image = new Image (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                     aCsl->get ("gump").asInt (), flags);
        api_addcontrol (image);
        return ZString (image->GetID ());
  return "0";
}

static ZString api_gui_additemcontainer (ZCsl * aCsl)
{

        ItemContainer *itemcontainer = new ItemContainer;
        itemcontainer->SetPosition (aCsl->get ("x").asInt (),
                                    aCsl->get ("y").asInt ());

        itemcontainer->setContainerID (aCsl->get ("containerid").asInt ());
        itemcontainer->Rebuild ();
        api_addcontrol (itemcontainer);
        return ZString (itemcontainer->GetID ());
  return "0";
}

static ZString api_gui_addart (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
  int hue = 0xFFFF;

  if (argCount == 4)
    flags = aCsl->get ("flags").asInt ();
  if (argCount == 5)
    hue = aCsl->get ("hue").asInt ();
        ImageArt *image =
          new ImageArt (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                        aCsl->get ("artid").asInt (), flags);
        if (hue != 0)
          image->SetHue (hue);
        api_addcontrol (image);
        return ZString (image->GetID ());

}



static ZString api_gui_addborder (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
  if (argCount == 4)
    flags = aCsl->get ("flags").asInt ();


        Border *border =
          new Border (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                      aCsl->get ("gump").asInt (), flags);
        api_addcontrol (border);
        return ZString (border->GetID ());
}

static ZString api_gui_addcontainer (ZCsl * aCsl)
{

        Container *container = new Container;
        container->SetPosition (aCsl->get ("x").asInt (),
                                aCsl->get ("y").asInt ());
        container->SetSize (aCsl->get ("width").asInt (),
                            aCsl->get ("height").asInt ());
        api_addcontrol (container);
        return ZString (container->GetID ());

}

static ZString api_gui_addbutton (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();


        Button *button =
          new Button (aCsl->get ("x").asInt (), aCsl->get ("y").asInt ());
        if (argCount >= 3)
          button->SetButton (BUTTONGUMP_NORMAL,
                             aCsl->get ("normal").asInt ());

        if (argCount >= 4)
          button->SetButton (BUTTONGUMP_MOUSEOVER,
                             aCsl->get ("mouseover").asInt ());

        if (argCount >= 5)
          button->SetButton (BUTTONGUMP_PRESSED,
                             aCsl->get ("pressed").asInt ());

        button->OnClick (on_clickhandler);
        api_addcontrol (button);
        return ZString (button->GetID ());

}

static ZString api_gui_addtextbox (ZCsl * aCsl)
{

        Textbox *textbox = new Textbox ();
        api_addcontrol (textbox);
        return ZString (textbox->GetID ());

}

static ZString api_gui_addlabel (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();

        Label *label =
          new Label (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                     aCsl->get ("text").buffer ());

        if (argCount >= 4)
          label->setFont (aCsl->get ("font").asInt ());

        if (argCount >= 5)
          label->setHue (aCsl->get ("hue").asInt ());

        api_addcontrol (label);
        return ZString (label->GetID ());

}


static ZString api_gui_rewind (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();


        Container *container = NULL;
        Control *control = NULL;
        if (argCount == 1)
            {
              control = api_findcontrol (aCsl->get ("id").asInt ());
              if (control->getType () == CONTROLTYPE_CONTAINER)
                container = (Container *) control;
            }
        if (container)
            {
              container->Rewind ();
            }
        else
            {
              pUOGUI.Rewind ();
            }
        return "0";

}

static ZString api_gui_findnext (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();


        Container *container = NULL;
        Control *control = NULL;
        if (argCount == 1)
            {
              control = api_findcontrol (aCsl->get ("id").asInt ());
              if (control->getType () == CONTROLTYPE_CONTAINER)
                container = (Container *) control;
            }
        if (container)
            {
              control = container->GetNext ();
            }
        else
            {
              control = pUOGUI.GetNext ();
            }

        if (control)
          return ZString (control->GetID ());
        return "0";

}

static ZString api_gui_registergump (ZCsl * aCsl)
{

        Texture *texture = new Texture;
        texture->LoadFromFile (aCsl->get ("filename").buffer ());
        return ZString ((int) pUOGUI.RegisterGump (texture));

}

static ZString api_gui_unregistergump (ZCsl * aCsl)
{

        pUOGUI.UnregisterGump (aCsl->get ("id").asInt ());

  return "0";
}


static ZString api_gui_addinputfield (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();


	int hue = 0xFFFF;
        int font = 3;
        int passwordchar = 0;

        if (argCount >= 5)
          hue = aCsl->get ("hue").asInt ();

        if (argCount >= 6)
          font = aCsl->get ("font").asInt ();

        if (argCount >= 7)
          passwordchar = aCsl->get ("passwordchar").asInt ();

        InputField *input =
          new InputField (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                          aCsl->get ("width").asInt (),
                          aCsl->get ("height").asInt (), NULL, hue, font,
                          passwordchar);
        input->OnKeyPress (on_keypressedhandler);


        api_addcontrol (input);
        return ZString (input->GetID ());

}


static ZString api_gui_addpaperdoll (ZCsl * aCsl)
{

        Paperdoll *paperdoll =
          new Paperdoll (aCsl->get ("x").asInt (), aCsl->get ("y").asInt (),
                         0);
        api_addcontrol (paperdoll);
        return ZString (paperdoll->GetID ());

}

static ZString api_gui_gettextwidth (ZCsl * aCsl)
{
  if (pTextManager)
    return ZString (pTextManager->
                    getTextWidth (aCsl->get ("text").buffer (),
                                  aCsl->get ("font").asInt ()));

  return "0";
}

static ZString api_gui_gettextheight (ZCsl * aCsl)
{
  if (pTextManager)
    return ZString (pTextManager->
                    getTextHeight (aCsl->get ("text").buffer (),
                                   aCsl->get ("font").asInt ()));

  return "0";
}

static ZString api_control_close (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->Close ();
        return "0";
      }
  return "-1";
}

static ZString api_control_getparent (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  return ZString (id >> 16);
}

static ZString api_control_setposition (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int x = aCsl->get ("x").asInt ();
  int y = aCsl->get ("y").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetPosition (x, y);
        return "0";
      }
  return "-1";
}

static ZString api_control_setsize (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int width = aCsl->get ("width").asInt ();
  int height = aCsl->get ("height").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetSize (width, height);
        return "0";
      }
  return "-1";
}

static ZString api_control_setx (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int x = aCsl->get ("x").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetX (x);
        return "0";
      }
  return "-1";
}

static ZString api_control_sety (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int y = aCsl->get ("y").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetY (y);
        return "0";
      }

  return "-1";
}

static ZString api_control_setflags (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int flags = aCsl->get ("flags").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetFlags (flags);
        return "0";
      }
  return "-1";
}

static ZString api_control_setflag (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int flag = aCsl->get ("flag").asInt ();
  int value = aCsl->get ("value").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetFlag (flag, value);
        return "0";
      }
  return "-1";
}

static ZString api_control_setalpha (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int alpha = aCsl->get ("alpha").asInt ();
  Control *control = api_findcontrol (id);

  if (control)
      {
        control->SetAlpha (alpha);
        return "0";
      }
  return "-1";
}

static ZString api_control_getx (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetX ());
  return "0";
}

static ZString api_control_gety (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetY ());
  return "0";
}


static ZString api_control_getfocus (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetFocus ());
  return "0";
}

static ZString api_control_getwidth (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetWidth ());
  return "0";
}

static ZString api_control_getheight (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetHeight ());
  return "0";
}

static ZString api_control_getflags (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetFlags ());
  return "0";
}

static ZString api_control_getflag (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int flag = aCsl->get ("flag").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetFlag (flag));
  return "0";
}


static ZString api_control_getalpha (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetAlpha ());
  return "0";
}


static ZString api_control_sethandler (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int id = aCsl->get ("id").asInt ();
  int type = aCsl->get ("type").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (argCount == 2)
          control->SetScriptFunction (type, NULL);
        else
          control->SetScriptFunction (type, aCsl->get ("funcname").buffer ());
        return "0";
      }
  return "-1";
}

static ZString api_control_gethandler (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int type = aCsl->get ("type").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetScriptFunction (type));
  return "0";
}


static ZString api_control_settag (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int slot = aCsl->get ("slot").asInt ();
  int tag = aCsl->get ("tag").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->SetData (slot, tag);
        return "0";
      }
  return "-1";
}

static ZString api_control_fade (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  unsigned int alpha = aCsl->get ("alpha").asInt ();
  unsigned int time = aCsl->get ("time").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        control->FadeTo (alpha, time);
        return "0";
      }
  return "-1";
}

static ZString api_control_gettag (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int slot = aCsl->get ("slot").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
    return ZString (control->GetData (slot));
  return "0";
}

static ZString api_input_settext (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->setText (aCsl->get ("text").buffer ());
        return "0";
      }
  return "-1";
}


static ZString api_input_gettext (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->text ());
      }
  return "";
}

static ZString api_input_getwidth (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->width ());
      }
  return "";
}

static ZString api_input_getheight (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->height ());
      }
  return "";
}


static ZString api_input_gethue (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->hue ());
      }
  return "";
}

static ZString api_input_getfont (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->font ());
      }
  return "";
}

static ZString api_input_getpasswordchar (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          return ZString (((InputField *) control)->passwordChar ());
      }
  return "";
}




static ZString api_input_setwidth (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->setWidth (aCsl->get ("width").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_input_sethue (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)

      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->setHue (aCsl->get ("hue").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_input_setfont (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->setFont (aCsl->get ("font").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_input_setpasswordchar (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->setPasswordChar (aCsl->
                                                     get ("passwordchar").
                                                     asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_input_setignorecursorkeys (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_INPUTFIELD)
          ((InputField *) control)->SetIgnoreCursorKeys (aCsl->
                                                         get
                                                         ("ignorecursorkeys").
                                                         asInt ());
        return "0";

      }
  return "-1";
}

static ZString api_label_settext (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          ((Label *) control)->setText (aCsl->get ("text").buffer ());
        return "0";
      }
  return "-1";
}

static ZString api_label_crop (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int width = aCsl->get ("width").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          ((Label *) control)->Crop (width, 0);
        return "0";
      }
  return "-1";
}

static ZString api_label_wrap (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int width = aCsl->get ("width").asInt ();
  Control *control = api_findcontrol (id);
  int lines = 0;
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          lines = ((Label *) control)->Wrap (width);
        return ZString (lines);
      }
  return "-1";
}

static ZString api_label_gettext (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->text ());
      }
  return "";
}

static ZString api_label_sethue (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          ((Label *) control)->setHue (aCsl->get ("hue").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_label_gethue (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->hue ());
      }
  return "";
}

static ZString api_label_setfont (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          ((Label *) control)->setFont (aCsl->get ("font").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_label_getfont (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->font ());
      }
  return "";
}

static ZString api_border_setgump (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BORDER)
          ((Border *) control)->setGump (aCsl->get ("gump").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_border_getgump (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BORDER)
          return ZString (((Border *) control)->gump ());
      }
  return "";
}

static ZString api_label_setalign (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          ((Label *) control)->setAlign (aCsl->get ("align").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_label_getalign (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->align ());
      }
  return "";
}

static ZString api_label_getwidth (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->labelWidth ());
      }
  return "";
}

static ZString api_label_getheight (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_LABEL)
          return ZString (((Label *) control)->labelHeight ());
      }
  return "";
}

static ZString api_button_setgump (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int type = aCsl->get ("type").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BUTTON)
          ((Button *) control)->SetButton (type, aCsl->get ("gump").asInt ());
        return "0";
      }
  return "-1";
}

static ZString api_button_getgump (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int type = aCsl->get ("type").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BUTTON)
          return ZString (((Button *) control)->GetButton (type));
      }
  return "";
}

static ZString api_art_setid (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int artid = aCsl->get ("artid").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_IMAGEART)
          ((ImageArt *) control)->SetTexID (artid);

        return "0";
      }
  return "-1";
}

static ZString api_textbox_addmessage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int hue = 0xFFFF;
  int argcount = aCsl->get ("argCount").asInt ();
  if (argcount == 4)
    hue = aCsl->get ("hue").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_TEXTBOX)
          ((Textbox *) control)->AddMessage (aCsl->get ("message").buffer (),
                                             aCsl->get ("timeout").asInt (),
                                             (unsigned short) hue);
      }
  return "";
}

static ZString api_art_getid (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_IMAGEART)
          return ZString (((ImageArt *) control)->GetTexID ());
      }
  return "";
}

static ZString api_gui_gumpexists (ZCsl * aCsl)
{

  int id = aCsl->get ("id").asInt ();
    if (pGumpLoader.CheckGump (id))
      return "-1";
  return "0";
}

static ZString api_quit (ZCsl * aCsl)
{

    pUOGUI.SetQuitFlag();
  return "-1";
}

static ZString api_config_getvalue (ZCsl * aCsl)
{
  switch (aCsl->get ("id").asInt ())
      {
      case 0:
        return ZString ( Config::GetMusic() );
      case 1:
        return ZString ( Config::GetSound() );
      case 2:
        return ZString ( Config::GetMP3() );
      case 3:
        return ZString ( Config::GetLogin().c_str() );
      case 4:
        return ZString ( Config::GetPassword().c_str() );
      case 5:
        return ZString ( Config::GetServer().c_str() );
      case 6:
        return ZString ( Config::GetIsSphere() );
      case 10:
        return ZString ( Config::GetPerspective() );
      case 12:
        return ZString ( Config::GetViewDistance() );
      case 15:
        return ZString ( Config::GetWidth() );
      case 16:
        return ZString ( Config::GetHeight() );
      case 17:
        return ZString ( Config::GetBrightness() );
      case 19:
        return ZString ( Config::GetVersion().c_str() );
      case 20:
        return ZString ( Config::GetAOS() );
      case 21:
        return ZString ( Config::GetSpeechHue() );
      case 22:
         return ZString ( Config::GetRoofFadeTime() );
       case 23:
         return ZString ( Config::GetRoofFade() );
      default:
        return "0";
      }
}

static ZString api_config_setvalue (ZCsl * aCsl)
{
  switch (aCsl->get ("id").asInt ())
      {
      case 3:
        Config::SetLogin( aCsl->get("value").buffer() );
        break;
      case 4:
        Config::SetPassword( aCsl->get("value").buffer() );
        break;
      case 5:
        Config::SetServer( aCsl->get("value").buffer() );
        break;
      case 10:
       Config::SetPerspective( aCsl->get("value").asInt() );
	   break;
      case 17:
        Config::SetBrightness( aCsl->get("value").asInt() );
        break;
      case 21:
        Config::SetSpeechHue( aCsl->get("value").asInt() );
        break;
      case 22:
        Config::SetRoofFadeTime( aCsl->get("value").asInt() );
      case 23:
        Config::SetRoofFade( aCsl->get("value").asInt() );
		break;
      }
  return "0";
}

static ZString api_net_getserverlist (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int index = aCsl->get ("index").asInt ();

  if (!pClient)
    return "";

  cLoginListEntry *entry = pClient->GetServerListEntry (index);
  if (!entry)
    return "";

  if (argCount == 2)
    aCsl->set ("id", ZString ((int) ((Uint16) entry->id ())));
  return ZString (entry->name ().c_str ());
}


static ZString api_net_getcharlist (ZCsl * aCsl)
{
  int index = aCsl->get ("index").asInt ();

  if (!pClient)
    return "";

  cLoginListEntry *entry = pClient->GetCharListEntry (index);
  if (!entry)
    return "";

  return ZString (entry->name ().c_str ());
}

static ZString api_net_selectserver (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();

  if ( Config::GetIsPol() )
    id = 1;

  if (pClient)
    pClient->Send_SelectServer (id);

  return "0";
}

static ZString api_net_sendtradecheck (ZCsl * aCsl)
{

  int id = aCsl->get ("id").asInt ();
  int check1 = aCsl->get ("check1").asInt ();
  int check2 = aCsl->get ("check2").asInt ();

  if (!pClient)
    return "";

  pClient->SendSecureTradeCheck ((Uint32) id, (Uint32) check1,
                                 (Uint32) check2);
  return "0";

}

static ZString api_net_updatebuylist (ZCsl * aCsl)
{
  int mode = aCsl->get ("mode").asInt ();
  int argcount = aCsl->get ("argCount").asInt ();
  if (mode == 0)
    pClient->updateBuyList (0);
  else
      {
        if (argcount == 2)
          pClient->updateBuyList (mode, aCsl->get ("itemid").asInt ());
        else if (argcount == 3)
          pClient->updateBuyList (mode, aCsl->get ("itemid").asInt (),
                                  aCsl->get ("amount").asInt ());
      }
  return "0";
}


static ZString api_net_updateselllist (ZCsl * aCsl)
{
  int mode = aCsl->get ("mode").asInt ();
  int argcount = aCsl->get ("argCount").asInt ();
  if (mode == 0)
    pClient->updateSellList (0);
  else
      {

        if (argcount == 2)
          pClient->updateSellList (mode, aCsl->get ("itemid").asInt ());
        else if (argcount == 3)
          pClient->updateSellList (mode, aCsl->get ("itemid").asInt (),
                                   aCsl->get ("amount").asInt ());
      }
  return "0";
}

static ZString api_net_selectchar (ZCsl * aCsl)
{
  Uint8 id = (Uint8) aCsl->get ("id").asInt ();

  if (pClient)
    pClient->Send_SelectChar (id);

  return "0";
}

static ZString api_net_createchar (ZCsl * aCsl)
{
  sNewCharSettings doll;
  int errorid = 0;
  Uint8 i;
  std::string town_name;
  cLoginListEntry *location;

  for(i = 0; i < 31; i++)
    doll.name[i] = 0;


  strncpy (doll.name, aCsl->get ("name").buffer (), 30);
  doll.sex = (Uint8) aCsl->get ("sex").asInt ();
  doll.sex = doll.sex ? 1 : 0;
  doll.stat[0] = (Uint8) aCsl->get ("str_val").asInt ();
  doll.stat[1] = (Uint8) aCsl->get ("dex_val").asInt ();
  doll.stat[2] = (Uint8) aCsl->get ("int_val").asInt ();
  doll.skill[0] =(Uint8)  aCsl->get ("skill1").asInt ();
  doll.skill[1] = (Uint8) aCsl->get ("skill2").asInt ();
  doll.skill[2] = (Uint8) aCsl->get ("skill3").asInt ();
  doll.skill_val[0] = (Uint8) aCsl->get ("skill1_val").asInt ();
  doll.skill_val[1] = (Uint8) aCsl->get ("skill2_val").asInt ();
  doll.skill_val[2] = (Uint8) aCsl->get ("skill3_val").asInt ();
  doll.skin_color = (Uint16) aCsl->get ("skin_color").asInt ();
  doll.hair = (Uint16) aCsl->get ("hair").asInt ();
  doll.hair_color = (Uint16) aCsl->get ("hair_color").asInt ();
  doll.facial_hair = (Uint16) aCsl->get ("facial_hair").asInt ();
  doll.facial_hair_color = (Uint16) aCsl->get ("facial_hair_color").asInt ();
  doll.starting_location = (Uint16) aCsl->get ("starting_location").asInt ();
  switch(doll.starting_location){
  case 0:
    town_name = "Yew";
    break;
  case 1:
    town_name = "Minoc";
    break;
  case 2:
    town_name = "Britain";
    break;
  case 3:
    town_name = "Moonglow";
    break;
  case 4:
    town_name = "Trinsic";
    break;
  case 5:
    town_name = "Magincia";
    break;
  case 6:
    town_name = "Jhelom";
    break;
  case 7:
    town_name = "Skara Brae";
    break;
  case 8:
    town_name = "Vesper";
    break;
  }
  for(i = 0; (location = pClient->GetLocationListEntry (i)); i++){
    if((location->name ()) == town_name)
      {
	doll.starting_location = (Uint16) location->id ();
	break;
      }
  }
  doll.slot = (Uint16) aCsl->get ("slot").asInt ();
  doll.pants_color = (Uint16) aCsl->get ("pants_color").asInt ();
  doll.shirt_color = (Uint16) aCsl->get ("shirt_color").asInt ();

  if (doll.stat[0] < 10)
    errorid = 1;
  else if (doll.stat[0] > 60)
    errorid = 2;
  else if (doll.stat[1] < 10)
    errorid = 3;
  else if (doll.stat[1] > 60)
    errorid = 4;
  else if (doll.stat[2] < 10)
    errorid = 5;
  else if (doll.stat[2] > 60)
    errorid = 6;
  else if ((doll.stat[0] + doll.stat[1] + doll.stat[2]) != 80)
    errorid = 7;
  else if (doll.skill[0] > 49)
    errorid = 8;
  else if (doll.skill[1] > 49)
    errorid = 9;
  else if (doll.skill[2] > 49)
    errorid = 10;
  else if (doll.skill[0] == doll.skill[1] || doll.skill[1] == doll.skill[2] || doll.skill[0] == doll.skill[2])
    errorid = 11;
  else if (doll.skill_val[0] > 50)
    errorid = 12;

  else if (doll.skill_val[1] > 50)
    errorid = 13;
  else if (doll.skill_val[2] > 50)
    errorid = 14;
  else if ((doll.skill_val[0] + doll.skill_val[1] + doll.skill_val[2]) != 100)
    errorid = 15;

  else if ((doll.skin_color <= 0x83EA) || (doll.skin_color >= 0x8422))
    doll.skin_color = 0x83EA;
  else if ((doll.hair_color <= 0x44E) || (doll.hair_color >= 0x4AD))
    doll.hair_color = 0x044E;
  else if ((doll.facial_hair_color <= 0x44E) || (doll.facial_hair_color >= 0x4AD))
    doll.facial_hair_color = 0x044E;

  else if (!((doll.hair >= 0x203b && doll.hair <= 0x203d ) || ( doll.hair >= 0x2044 && doll.hair <= 0x204a )))
    doll.hair = 0;

  else if (!(((doll.facial_hair >= 0x203e && doll.facial_hair <= 0x2041) ||
              (doll.facial_hair >= 0x204b && doll.facial_hair <= 0x204d )) || doll.sex ))
    doll.facial_hair = 0;

  else if ((doll.pants_color < 2) || (doll.pants_color > 0x3e9))
    errorid = 21;
  else if ((doll.shirt_color < 2) || (doll.shirt_color > 0x3e9))
    errorid = 22;
  else if (!strlen(doll.name))
    errorid = 23;

  if(errorid)
      {
	char buf[15];
	sprintf(buf, "%d", errorid);
	if (pCSLHandler.ExecuteFunction ("net_on_createchar_error", buf))
	  Logger::WriteLine ("No Character Creation Error Handler - Please Check your Scripts", __FILE__, __LINE__, LEVEL_WARNING);
      }
  else
    pClient->Send_CreateChar(doll);

  return "0";
}

static ZString api_net_selectmenu (ZCsl * aCsl)
{
  assert (aCsl);
  int dialogid = aCsl->get ("dialogid").asInt ();
  int menuid = aCsl->get ("menuid").asInt ();
  int index = aCsl->get ("index").asInt ();
  int model = aCsl->get ("model").asInt ();
  if (pClient)
    pClient->Send_MenuChoice (dialogid, menuid, index, model);
  return "0";
}

static ZString api_net_getpopupentry (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Uint16 entry = pClient->GetPopupEntry (id);
  return ZString (entry);
}

static ZString api_net_getpopuptag (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Uint16 entry = pClient->GetPopupTag (id);
  return ZString (entry);
}

static ZString api_net_sendpopupchoice (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  pClient->SendPopupChoice (id);
  return ZString (id);
}


static ZString api_net_deletechar (ZCsl* aCsl)
{
  Uint16 id = aCsl->get("charindex").asInt();

  if(id >= 5)
    return "-1";

  pClient->Send_DeleteChar (id);

  return "0";
}


static ZString api_net_sendspeech (ZCsl * aCsl)
{

  int argCount = aCsl->get ("argCount").asInt ();
  int mode = 0;

  if (argCount == 2)
    mode = aCsl->get ("mode").asInt ();


  if (pClient)
    pClient->Send_Speech (aCsl->get ("text").buffer (), mode);

  return "0";
}


static ZString api_net_getplayercharacter (ZCsl * aCsl)
{
  if (pClient)
    return ZString (pClient->player_charid ());
  return "";
}

static ZString api_net_statusrequest (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int argCount = aCsl->get ("argCount").asInt ();
  Uint32 mode = 0;

  if (argCount == 2)
    mode = aCsl->get ("mode").asInt ();


  if (pClient)
    pClient->Send_StatusRequest (id, mode);

  return "0";
}


static ZString api_net_rename (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

  if (pClient)
      {
        cCharacter *character = pCharacterList.Get ((Uint32) id);
        if (character)
            {
              if (character->name_change ())
                pClient->Send_CharName (id, aCsl->get ("name").buffer ());
            }
      }

  return "0";
}

static ZString api_net_warmode (ZCsl * aCsl)
{
  Uint32 mode = aCsl->get ("mode").asInt ();

  if (pClient)
    pClient->Send_Warmode (mode);

  return "0";
}

static ZString api_net_attackrequest (ZCsl * aCsl)
{
  Uint32 charid = aCsl->get ("charid").asInt ();

  if (pClient)
    pClient->Send_AttackRequest (charid);

  return "0";
}

static ZString api_net_skilllock (ZCsl * aCsl)
{
  Uint32 skillid = aCsl->get ("skillid").asInt ();
  Uint32 lock = aCsl->get ("lock").asInt ();

  if (pClient)
    pClient->Send_SkillLock (skillid, lock);

  return "0";
}

static ZString api_net_dye (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();
  Uint32 model = aCsl->get ("model").asInt ();
  Uint32 color = aCsl->get ("color").asInt ();

  if (pClient)
    pClient->Send_Dye (id, model, color);

  return "0";
}

static ZString api_net_doaction (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  Uint32 type = aCsl->get ("type").asInt ();

  if (pClient)
      {
        if (argCount == 1)

          pClient->Send_Action (type);
        else
          pClient->Send_Action (type, aCsl->get ("mode").asInt ());
      }

  return "0";
}

static ZString api_object_getcontentcount (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

  Uint32 count = 0;
      {
        dynamiclist_t *dynamics = pDynamicObjectList.GetList ();
        dynamiclist_t::iterator iter;
        for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
          if (iter->second->parent == id)
            count++;
      }

  return ZString (count);
}


static ZString api_object_getcontententry (ZCsl * aCsl)
{
  Uint32 index = aCsl->get ("index").asInt ();
  Uint32 id = aCsl->get ("id").asInt ();

      {
        dynamiclist_t *dynamics = pDynamicObjectList.GetList ();
        dynamiclist_t::iterator iter;
        for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
          if (iter->second->parent == id)
              {
                if (index == 0)
                  return ZString (iter->second->id);
                index--;
              }
      }

  return "0";
}

static ZString api_net_getaostooltip (ZCsl * aCsl)
{

  int id = aCsl->get ("id").asInt ();
  int index = aCsl->get ("index").asInt ();

  cCharacter *character = NULL;
  cDynamicObject *object = NULL;

  character = pCharacterList.Get (id);
  object = pDynamicObjectList.Get (id);
  std::string msg = "";
  if (character)
    msg = character->GetAOSTooltip (index);
  else if (object)
    msg = object->GetAOSTooltip (index);

  return ZString (msg.c_str ());
  //return "-1";
}

static ZString api_net_getmenuitem (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int index = aCsl->get ("index").asInt ();

  if (!pClient)
    return "";


  std::string text = "";
  int model = 0;

  pClient->GetMenuItem (index, text, model);

  if (argCount > 1)
    aCsl->set ("model", ZString ((int) model));

  return ZString (text.c_str ());
}

static ZString api_object_gethue (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

        cDynamicObject *object = pDynamicObjectList.Get (id);
        if (object)
          return ZString (object->dye);

  return "0";
}


static ZString api_object_getbody (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

        cDynamicObject *object = pDynamicObjectList.Get (id);
        if (object)
          return ZString (object->model);

  return "0";
}

static ZString api_object_getquantity (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

        cDynamicObject *object = pDynamicObjectList.Get (id);
        if (object)

          return ZString (object->itemcount);

  return "0";
}

static ZString api_object_getposition (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

        cDynamicObject *object = pDynamicObjectList.Get (id);
        if (object)
            {
              aCsl->set ("x", ZString (object->x));
              aCsl->set ("y", ZString (object->y));
              aCsl->set ("z", ZString (object->z));
            }

  return "0";
}


static ZString api_object_getparent (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();

        cDynamicObject *object = pDynamicObjectList.Get (id);
        if (object)
          return ZString (object->parent);

  return "0";
}

static ZString api_net_openbackpack( ZCsl *aCsl )
{
	if ( pClient )
	{
		cCharacter *character = pClient->player_character();

		if ( character )
		{
			cCharacterEquip *equip = character->GetEquip( 0x15 );
			if (equip)
			{
                pClient->Send_DoubleClick( equip->id () );
			}
		}
	}

	return "0";
}

//SiENcE: Quest
static ZString api_net_openquestgump( ZCsl *aCsl )
{
  Uint32 charid = aCsl->get ("charid").asInt ();
  Uint32 subcmd = aCsl->get ("subcmd").asInt ();

	if ( pClient )
	{
		cCharacter *character = pClient->player_character();

		if ( character )
		{
           //Send Quest request
           pClient->Send_AoSCommand(charid, subcmd);
        }
    }

	return "0";
}

static ZString api_char_getstatus (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();
  int property = aCsl->get ("property").asInt ();
      {
        cCharacter *character = pCharacterList.Get ((Uint32) id);
        if (character)
            {
              switch (property)
                  {
                  case 0:
                    return ZString (character->name ().c_str ());
                  case 1:
                    return ZString (character->hits_current ());
                  case 2:
                    return ZString (character->hits_max ());
                  case 3:
                    return ZString ((int) character->name_change ());
                  case 4:
                    return ZString (character->sex ());
                  case 5:
                    return ZString (character->strength ());
                  case 6:
                    return ZString (character->dexterity ());
                  case 7:
                    return ZString (character->intelligence ());
                  case 8:
                    return ZString (character->stamina_current ());
                  case 9:
                    return ZString (character->stamina_max ());
                  case 10:
                    return ZString (character->mana_current ());
                  case 11:
                    return ZString (character->mana_max ());
                  case 12:
                    return ZString (character->gold ());
                  case 13:
                    return ZString (character->armor ());
                  case 14:
                    return ZString (character->weight ());
                  case 15:
                    return ZString (character->statcap ());
                  case 16:
                    return ZString ((int) character->petscurrent ());
                  case 17:
                    return ZString ((int) character->petsmax ());
                  case 18:
                    return ZString (character->fireresist ());
                  case 19:
                    return ZString (character->coldresist ());
                  case 20:
                    return ZString (character->poisonresist ());
                  case 21:
                    return ZString (character->energyresist ());
                  case 22:
                    return ZString (character->luck ());
                  case 23:
                    return ZString (character->maxdamage ());
                  case 24:
                    return ZString (character->mindamage ());
                  case 25:
                    return ZString (character->tithings ());
                  }
            }
      }
  return "";
}

static ZString api_char_getskill (ZCsl * aCsl)
{
  Uint32 id = aCsl->get ("id").asInt ();
  int argCount = aCsl->get ("argCount").asInt ();
      {
        cCharacter *character = pCharacterList.Get ((Uint32) id);
        if (character)
            {
              Uint32 skillid = aCsl->get ("skillid").asInt ();
              sSkillEntry *entry = character->skill (skillid);
              cSkillEntry *skillname = pSkillLoader.GetSkill (skillid);
              if (skillname)
                  {
                    if (argCount >= 3)
                      aCsl->set ("name", skillname->name ().c_str ());
                    if (argCount >= 7)
                      aCsl->set ("button", skillname->use_button ());
                  }
              if (entry)
                  {
                    if (argCount >= 4)
                      aCsl->set ("unmodified", ZString (entry->unmodified));
                    if (argCount >= 5)
                      aCsl->set ("skillcap", ZString (entry->skillcap));
                    if (argCount >= 6)
                      aCsl->set ("skilllock", ZString (entry->skillLock));
                    return ZString (entry->value);
                  }

            }
      }
  return "";

}

static ZString api_paperdoll_refresh (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int charid = aCsl->get ("charid").asInt ();
      {
        cCharacter *character = pCharacterList.Get ((Uint32) charid);
        Control *control = api_findcontrol (id);
        if (control && character)
            {
              if (control->getType () == CONTROLTYPE_PAPERDOLL)
                  {
                    ((Paperdoll *) control)->AssignChar (character);
                  }
              return "0";
            }
      }
  return "-1";
}

static ZString api_gui_setcontainershape (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  Container *container = api_findcontainer (id);
  Control *control = api_findcontrol (id);
  if (container && control)
    if (control->GetParent () == container)
        {
          container->SetShape (control->GetID ());
          return "0";
        }

  return "-1";
}

static ZString api_char_addtext (ZCsl * aCsl)
{
      {
        Uint32 id = aCsl->get ("id").asInt ();
        int argCount = aCsl->get ("argCount").asInt ();
        int hue = 0xFFFF;
        int timeout = 5000;
        if (argCount >= 3)
          timeout = aCsl->get ("timeout").asInt ();
        if (argCount >= 4)
          hue = aCsl->get ("hue").asInt ();

        cCharacter *character = pCharacterList.Get ((Uint32) id);
        if (character)
            {
				character->AddText (std::string (aCsl->get ("text").buffer ()),
                                  timeout, hue);
              return "1";
            }
      }
  return "0";
}

static ZString api_font_register (ZCsl * aCsl)
{
  int argCount = aCsl->get ("argCount").asInt ();
  int defaulthue = 0xFFFF;
  if (argCount == 4)
    defaulthue = aCsl->get ("defaulthue").asInt ();

	FontManager::GetInstance()->AddTTFFont( aCsl->get( "id" ).asInt(), aCsl->get( "filename" ).buffer(),
		aCsl->get( "filename" ).buffer(), aCsl->get( "size" ).asInt(), defaulthue );
  //if (SDLScreen::GetInstance())
	 // SDLScreen::GetInstance()->RegisterFont (aCsl->get ("id").asInt (),
  //                           aCsl->get ("filename").buffer (),
  //                           aCsl->get ("size").asInt (), defaulthue);

  return "0";
}

static ZString api_font_unregister (ZCsl * aCsl)
{
	FontManager::GetInstance()->UnregisterTTFFont( aCsl->get( "id" ).asInt() );
	// Disabled / Deprecated
	/*
	if (SDLScreen::GetInstance())
		SDLScreen::GetInstance()->UnregisterFont (aCsl->get ("id").asInt ());
*/
  return "0";
}

static ZString api_gui_addhtmllabel (ZCsl * aCsl)
{
  int x = aCsl->get ("x").asInt ();
  int y = aCsl->get ("y").asInt ();
  int width = aCsl->get ("width").asInt ();
  int height = aCsl->get ("height").asInt ();
  std::string htmltext = std::string (aCsl->get ("htmltext").buffer ());
  int scrollbar = aCsl->get ("scrollbar").asInt ();

        Logger::WriteLine (htmltext.c_str ());

        cMultiLabel *label = new cMultiLabel (x, y, width, height, scrollbar);
        //Logger::WriteLine("CSL1");
        cHTMLGumpParser parser;
        //Logger::WriteLine("CSL2");
        parser.Parse (htmltext, label);
        label->Create ();
        //Logger::WriteLine("CSL3");
        api_addcontrol (label);
        //Logger::WriteLine("CSL4");
        return ZString (label->GetID ());
        //Logger::WriteLine("CSL5");


}

static ZString api_button_setdestpage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int page = aCsl->get ("page").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BUTTON)
          ((Button *) control)->SetDestinationPage (page);
        return "0";
      }
  return "-1";
}

static ZString api_button_getdestpage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  //int page = aCsl->get("page").asInt();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_BUTTON)
          return ZString (((Button *) control)->GetDestinationPage ());

      }
  return "";
}

static ZString api_container_setcurpage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int page = aCsl->get ("page").asInt ();
  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_CONTAINER)
          ((Container *) control)->SetCurrentPage (page);
        return "0";
      }
  return "-1";
}

static ZString api_container_getcurpage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();

  Control *control = api_findcontrol (id);
  if (control)
      {
        if (control->getType () == CONTROLTYPE_CONTAINER)
          return ZString (((Container *) control)->GetCurrentPage ());

      }
  return "";
}

static ZString api_char_gethighlight (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  cCharacter *character = NULL;
  character = pCharacterList.Get (id);
  if (character)
    return ZString (character->getHighlightColor ());
  return "-1";
}

static ZString api_control_setpage (ZCsl * aCsl)
{
  int id = aCsl->get ("id").asInt ();
  int page = aCsl->get ("page").asInt ();

  Control *control = api_findcontrol (id);

  if (control)
      {
        control->SetPage (page);
        return "0";
      }
  return "-1";
}

static ZString api_utils_bitand (ZCsl * aCsl)
{
  int a = aCsl->get ("a").asInt ();
  int b = aCsl->get ("b").asInt ();
  int c = a & b;
  return ZString (c);
}

static ZString api_utils_bitor (ZCsl * aCsl)
{
  int a = aCsl->get ("a").asInt ();
  int b = aCsl->get ("b").asInt ();
  int c = a | b;
  return ZString (c);
}

static ZString api_utils_rightshift (ZCsl * aCsl)
{
  int a = aCsl->get ("a").asInt ();
  int b = aCsl->get ("b").asInt ();

  int c = a >> b;
  return ZString (c);
}

static ZString api_utils_leftshift (ZCsl * aCsl)
{
  int a = aCsl->get ("a").asInt ();
  int b = aCsl->get ("b").asInt ();
  int c = a << b;
  return ZString (c);
}

static ZString api_net_castspell (ZCsl * aCsl)
{
  int spellid = aCsl->get ("spellid").asInt ();
  if (spellid)
      {
        pClient->CastSpell (spellid);
        return "0";
      }
  return "";
}

static ZString api_net_buyitems (ZCsl * aCsl)
{
  if (!pClient)
    return "";

  pClient->buy ();
  return "0";
}

static ZString api_net_sellitems (ZCsl * aCsl)
{
  if (!pClient)
    return "";
  int id = aCsl->get ("id").asInt ();
  pClient->sell ((Uint32) id);
  return "0";
}

static ZString api_net_senddoubleclick (ZCsl * aCsl)
{

  int id = aCsl->get ("id").asInt ();

  if (!pClient)
    return "";

  pClient->Send_DoubleClick ((Uint32) id);
  return "0";

}

//SiENcE: added to get Cliloc Messages
static ZString api_loader_getmessage (ZCsl * aCsl)
{
	int id = aCsl->get ("id").asInt ();

	if (!pClient)
		return "";

	return pClilocLoader.GetMessage(id).c_str();
}

//Tensor: Loads world environment lightmap
static ZString api_load_world_light_map (ZCsl * aCsl)
{
  Renderer *renderer = Game::GetInstance()->GetRenderer ();

  if (!renderer)
    return "0";

  renderer->world_environment().LoadLightColorMap (aCsl->get ("ambientfilename").buffer (), aCsl->get ("sunfilename").buffer ());

  return "0";
}

//Tensor: Loads world environment fogmap
static ZString api_load_world_fog_map (ZCsl * aCsl)
{
  Renderer *renderer = Game::GetInstance()->GetRenderer ();

  if (!renderer)
    return "0";

  renderer->world_environment().LoadFogColorMap (aCsl->get ("fogfilename").buffer ());

  return "0";
}

CSLHandler::CSLHandler ()
{
}


CSLHandler::~CSLHandler ()
{
  if (initialized)
    DeInit ();
}

void CSLHandler::Init ()
{
  if (initialized)
    return;

  try
  {
    Logger::WriteLine ("CSL | Initializing...");
    csl = new ZCsl ();
    cslOk = zTrue;

    initialized = true;
  }
  catch (const ZException & err)
  {
    for (int i = 0; i < err.count (); i++)
      Logger::WriteLine (err[i]);
    if (cslOk)
      delete csl;
    csl = NULL;
    cslOk = zFalse;
    initialized = false;
  }                             // catch
}

void CSLHandler::DeInit ()
{
	if ( (initialized) && (csl) )
	{
		try
		{
			delete csl;
/*
			if ( cont_content_list )
				delete cont_content_list;

			cont_content_list = NULL;
*/
			initialized = false;
		}
		catch ( const ZException &err )
		{
			for ( int i = 0; i < err.count(); i++ )
			{
				Logger::WriteLine (err[i]);
			}

			if ( cslOk )
			{
				delete csl;
			}

			  cslOk = zFalse;
			  csl = NULL;
			  initialized = false;
		}                       // catch
	}
}

void CSLHandler::Load (char *filename)
{
  try
  {
    csl->loadScript (filename);
  }
  catch (const ZException & err)
  {
    for (int i = 0; i < err.count (); i++)
      Logger::WriteLine (err[i]);
//                cerr << err[i] << endl;
    if (cslOk)
      delete csl;
    csl = NULL;
    initialized = false;
  }                             // catch
}

int CSLHandler::ExecuteFunction (char *funcname, char *p1, char *p2, char *p3,
                                 char *p4)
{
	if (!initialized)
		return -1;

  try
  {
    if (p2 && p1 && p3 && p4)
      csl->call (module, funcname, 4, p1, p2, p3, p4);
    else if (p2 && p1 && p3)
      csl->call (module, funcname, 3, p1, p2, p3);
    else if (p2 && p1)
      csl->call (module, funcname, 2, p1, p2);
    else if (p1)
      csl->call (module, funcname, 1, p1);
    else
      csl->call (module, funcname);
  }
  catch (const ZException & err)
  {
    for (int i = 0; i < err.count (); i++)
      Logger::WriteLine (err[i]);
//                cerr << err[i] << endl;
    return -1;
  }
  return 0;
}

int CSLHandler::ExecuteFunctionPlus (char *funcname, char *p1, char *p2,
                                     char *p3, char *p4, char *p5, char *p6,
                                     char *p7, char *p8)
{
  if (!initialized)
    return -1;

  try
  {
    if (p2 && p1 && p3 && p4 && p5 && p6 && p7 && p8)
      csl->call (module, funcname, 8, p1, p2, p3, p4, p5, p6, p7, p8);
    else if (p2 && p1 && p3 && p4 && p5 && p6 && p7)
      csl->call (module, funcname, 7, p1, p2, p3, p4, p5, p6, p7);
    else if (p2 && p1 && p3 && p4 && p5 && p6)
      csl->call (module, funcname, 6, p1, p2, p3, p4, p5, p6);
    else if (p2 && p1 && p3 && p4 && p5)
      csl->call (module, funcname, 5, p1, p2, p3, p4, p5);
    else if (p2 && p1 && p3 && p4)
      csl->call (module, funcname, 4, p1, p2, p3, p4);
    else if (p2 && p1 && p3)
      csl->call (module, funcname, 3, p1, p2, p3);
    else if (p2 && p1)
      csl->call (module, funcname, 2, p1, p2);
    else if (p1)
      csl->call (module, funcname, 1, p1);
    else
      csl->call (module, funcname);
  }
  catch (const ZException & err)
  {
    for (int i = 0; i < err.count (); i++)
      Logger::WriteLine (err[i]);
//                cerr << err[i] << endl;
    return -1;
  }
  return 0;
}

void CSLHandler::InitAPI (void)
{
  if (!initialized)
    return;
  try
  {
    // Added in 0.3.0
    csl->addFunc (module, "log(const text)", api_iris_log);

    csl->addFunc (module, "label_settext (const id, const text)",
                  api_label_settext);
    csl->addFunc (module, "label_setfont (const id, const font)",
                  api_label_setfont);
    csl->addFunc (module, "label_sethue (const id, const hue)",
                  api_label_sethue);
    csl->addFunc (module, "label_setalign (const id, const align)",
                  api_label_setalign);
    csl->addFunc (module, "label_gettext (const id)", api_label_gettext);
    csl->addFunc (module, "label_getfont (const id)", api_label_getfont);
    csl->addFunc (module, "label_gethue (const id)", api_label_gethue);
    csl->addFunc (module, "label_getalign (const id)", api_label_getalign);
    csl->addFunc (module, "label_getwidth(const id)", api_label_getwidth);
    csl->addFunc (module, "label_getheight(const id)", api_label_getheight);
    csl->addFunc (module, "label_crop(const id, const width)",
                  api_label_crop);
    csl->addFunc (module, "label_wrap(const id, const width)",
                  api_label_wrap);

    csl->addFunc (module, "border_setgump (const id, const gump)",
                  api_border_setgump);
    csl->addFunc (module, "border_getgump (const id)", api_border_getgump);

    csl->addFunc (module, "config_setvalue (const id, const value)",
                  api_config_setvalue);
    csl->addFunc (module, "config_getvalue (const id)", api_config_getvalue);

    // Added in 0.3.1

    csl->addFunc (module, "iris_connect()", api_iris_connect);
    csl->addFunc (module, "iris_disconnect()", api_iris_disconnect);
    csl->addFunc (module, "iris_deinit()", api_iris_deinit);
    csl->addFunc (module, "iris_init()", api_iris_init);
    csl->addFunc (module, "iris_setlightlevel(const level)",
                  api_iris_setlightlevel);
    csl->addFunc (module, "iris_quit()", api_quit);
    csl->addFunc (module,
                  "iris_playmusic(const id, const format, [const volume])",
                  api_iris_playmusic);
    csl->addFunc (module, "iris_playsound(const id, const volume)",
                  api_iris_playsound);

    csl->addFunc (module, "input_settext (const id, const text)",
                  api_input_settext);
    csl->addFunc (module, "input_gettext (const id)", api_input_gettext);
    csl->addFunc (module, "input_getwidth (const id)", api_input_getwidth);
    csl->addFunc (module, "input_getheight (const id)", api_input_getheight);
    csl->addFunc (module, "input_gethue (const id)", api_input_gethue);
    csl->addFunc (module, "input_getfont (const id)", api_input_getfont);
    csl->addFunc (module, "input_getpasswordchar (const id)",
                  api_input_getpasswordchar);
    csl->addFunc (module, "input_setwidth (const id, const width)",
                  api_input_setwidth);
    csl->addFunc (module, "input_sethue (const id, const hue)",
                  api_input_sethue);
    csl->addFunc (module, "input_setfont (const id, const font)",
                  api_input_setfont);
    csl->addFunc (module,
                  "input_setpasswordchar (const id, const passwordchar)",
                  api_input_setpasswordchar);
    csl->addFunc (module,
                  "input_setignorecursorkeys (const id, const ignorecursorkeys)",
                  api_input_setignorecursorkeys);

    csl->addFunc (module, "button_setgump (const id, const type, const gump)",
                  api_button_setgump);
    csl->addFunc (module, "button_getgump (const id, const type)",
                  api_button_getgump);
    csl->addFunc (module, "button_setdestinationpage (const id, const page)",
                  api_button_setdestpage);
    csl->addFunc (module, "button_getdestinationpage (const id)",
                  api_button_getdestpage);

    csl->addFunc (module, "camera_move (const dir, const amount)",
                  api_camera_move);
    csl->addFunc (module, "camera_zoom (const type, const amount)",
                  api_camera_zoom);
    csl->addFunc (module, "camera_firstperson (const selfview)", api_camera_firstperson);

    csl->addFunc (module,
                  "textbox_addmessage(const id, const message, const timeout, [const hue])",
                  api_textbox_addmessage);

    csl->addFunc (module, "container_setcurrentpage(const id, const page)",
                  api_container_setcurpage);
    csl->addFunc (module, "container_getcurrentpage(const id)",
                  api_container_getcurpage);


    csl->addFunc (module, "control_fade (const id, const alpha, const time)",
                  api_control_fade);
    csl->addFunc (module, "control_close (const id)", api_control_close);
    csl->addFunc (module, "control_getparent (const id)",
                  api_control_getparent);
    csl->addFunc (module, "control_settag (const id, const slot, const tag)",
                  api_control_settag);
    csl->addFunc (module, "control_gettag (const id, const slot)",
                  api_control_gettag);
    csl->addFunc (module, "control_setposition (const id, const x, const y)",
                  api_control_setposition);
    csl->addFunc (module,
                  "control_setsize (const id, const width, const height)",
                  api_control_setsize);
    csl->addFunc (module, "control_setx (const id, const x)",
                  api_control_setx);
    csl->addFunc (module, "control_sety (const id, const y)",
                  api_control_sety);
    csl->addFunc (module, "control_setflags (const id, const flags)",
                  api_control_setflags);
    csl->addFunc (module,
                  "control_setflag (const id, const flag, const value)",
                  api_control_setflag);
    csl->addFunc (module, "control_setalpha (const id, const alpha)",
                  api_control_setalpha);
    csl->addFunc (module, "control_getx (const id)", api_control_getx);
    csl->addFunc (module, "control_gety (const id)", api_control_gety);
    csl->addFunc (module, "control_getfocus (const id)",
                  api_control_getfocus);
    csl->addFunc (module, "control_getwidth (const id)",
                  api_control_getwidth);
    csl->addFunc (module, "control_getheight (const id)",
                  api_control_getheight);
    csl->addFunc (module, "control_getflags (const id)",
                  api_control_getflags);
    csl->addFunc (module, "control_getflag (const id, const flag)",
                  api_control_getflag);
    csl->addFunc (module, "control_getalpha (const id)",
                  api_control_getalpha);
    csl->addFunc (module,
                  "control_sethandler (const id, const type, [const funcname])",
                  api_control_sethandler);
    csl->addFunc (module, "control_gethandler (const id, const type)",
                  api_control_gethandler);
    csl->addFunc (module, "control_setpage (const id, const page)",
                  api_control_setpage);

    csl->addFunc (module, "paperdoll_refresh(const id, const charid)",
                  api_paperdoll_refresh);
    csl->addFunc (module, "gui_setcontainershape(const id)",
                  api_gui_setcontainershape);

    csl->addFunc (module, "gui_translate (const x, const y)",
                  api_gui_translate);
    csl->addFunc (module, "gui_gumpexists(const id)", api_gui_gumpexists);
    csl->addFunc (module, "gui_setcontainer( [const id] )",
                  api_gui_setcontainer);
    csl->addFunc (module, "gui_clear([const id])", api_gui_clear);
    csl->addFunc (module,
                  "gui_addart(const x, const y, const artid, [ const flags, const hue ])",
                  api_gui_addart);
    csl->addFunc (module, "gui_registergump(const filename)",
                  api_gui_registergump);
    csl->addFunc (module, "gui_unregistergump(const id)",
                  api_gui_unregistergump);
    csl->addFunc (module, "gui_setfocus(const id)", api_gui_setfocus);
    csl->addFunc (module, "gui_setdefaultfocus(const id)",
                  api_gui_setdefaultfocus);
    csl->addFunc (module, "gui_rewind([const id])", api_gui_rewind);
    csl->addFunc (module, "gui_findnext([const id])", api_gui_findnext);
    csl->addFunc (module,
                  "gui_addcontainer(const x, const y, const width, const height)",
                  api_gui_addcontainer);
    csl->addFunc (module,
                  "gui_addgump(const x, const y, const gump, [ const flags ])",
                  api_gui_addgump);
    csl->addFunc (module,
                  "gui_additemcontainer(const x, const y, const containerid)",
                  api_gui_additemcontainer);
    csl->addFunc (module,
                  "gui_addborder(const x, const y, const gump, [ const flags ])",
                  api_gui_addborder);
    csl->addFunc (module,
                  "gui_addbutton(const x, const y, [ const normal, const mouseover, const pressed ])",
                  api_gui_addbutton);
    csl->addFunc (module,
                  "gui_addlabel(const x, const y, const text, [ const font, const hue ])",
                  api_gui_addlabel);
    csl->addFunc (module,
                  "gui_addinput(const x, const y, const width, const height, [ const hue, const font, const background, const passwordchar ])",
                  api_gui_addinputfield);
    csl->addFunc (module, "gui_addpaperdoll(const x, const y)",
                  api_gui_addpaperdoll);
    csl->addFunc (module, "gui_addtextbox()", api_gui_addtextbox);
    csl->addFunc (module, "gui_gettextwidth(const text, const font)",
                  api_gui_gettextwidth);
    csl->addFunc (module, "gui_gettextheight(const text, const font)",
                  api_gui_gettextheight);
    csl->addFunc (module,
                  "gui_addhtmllabel(const x, const y , const width, const height, const htmltext, const scrollbar)",
                  api_gui_addhtmllabel);

    csl->addFunc (module, "art_setid(const id, const artid)", api_art_setid);
    csl->addFunc (module, "art_getid(const id)", api_art_getid);

    csl->addFunc (module, "net_getserverlist (const index,[ var & id ])",
                  api_net_getserverlist);
    csl->addFunc (module, "net_selectserver (const id)",
                  api_net_selectserver);
    csl->addFunc (module, "net_getcharlist (const index)",
                  api_net_getcharlist);
    csl->addFunc (module, "net_selectchar (const id)", api_net_selectchar);
    csl->addFunc (module, "net_createchar (const name, const sex, const str_val, const dex_val, const int_val, const skill1, const skill1_val, const skill2, const skill2_val, const skill3, const skill3_val, const skin_color, const hair, const hair_color, const facial_hair, const facial_hair_color, const shirt_color, const pants_color, const starting_location, const slot)", api_net_createchar);
    csl->addFunc (module, "net_deletechar (const charindex)",
		  api_net_deletechar);
    csl->addFunc (module, "net_sendspeech (const text, [ const mode ])",
                  api_net_sendspeech);
    csl->addFunc (module, "net_getplayercharacter ()",
                  api_net_getplayercharacter);
    csl->addFunc (module, "net_statusrequest (const id, [ const mode ])",
                  api_net_statusrequest);
    csl->addFunc (module, "net_dye(const id, const model, const color)",
                  api_net_dye);
    csl->addFunc (module, "net_rename (const id, const name)",
                  api_net_rename);
    csl->addFunc (module, "net_doaction(const type, [const mode])",
                  api_net_doaction);
    csl->addFunc (module, "net_warmode (const mode)", api_net_warmode);
    csl->addFunc (module, "net_getmenuitem (const index, [ var & model ])",
                  api_net_getmenuitem);
    csl->addFunc (module,
                  "net_selectmenu (const dialogid, const menuid, const index, const model)",
                  api_net_selectmenu);
    csl->addFunc (module, "net_getpopupentry (const id)",
                  api_net_getpopupentry);
    csl->addFunc (module, "net_getpopuptag (const id)", api_net_getpopuptag);
    csl->addFunc (module, "net_sendpopupchoice (const id)",
                  api_net_sendpopupchoice);
    csl->addFunc (module, "net_senddoubleclick (const id)",
                  api_net_senddoubleclick);
    csl->addFunc (module, "net_castspell(const spellid)", api_net_castspell);
    csl->addFunc (module,
                  "net_updatebuylist (const mode, [const itemid, const amount])",
                  api_net_updatebuylist);
    csl->addFunc (module, "net_buyitems ()", api_net_buyitems);
    csl->addFunc (module,
                  "net_updateselllist (const mode, [const itemid, const amount])",
                  api_net_updateselllist);
    csl->addFunc (module, "net_sellitems (const id)", api_net_sellitems);
    csl->addFunc (module,
                  "net_sendtradecheck (const id, const check1, const check2)",
                  api_net_sendtradecheck);
    csl->addFunc (module,
                  "net_sendpickup (const id, const model, const count)",
                  api_net_sendpickup);
    csl->addFunc (module, "net_getaostooltip (const id, const index)",
                  api_net_getaostooltip);

    csl->addFunc(module, "net_lastobject ()", api_net_lastobj);
    csl->addFunc(module, "net_lasttarget ()", api_net_lasttarget);
    csl->addFunc(module, "net_sendtarget (const id)", api_net_sendtarget);
    csl->addFunc(module, "net_lastskill ()", api_net_lastskill);
    csl->addFunc(module, "net_lastspell ()", api_net_lastspell);
    csl->addFunc(module, "net_waitfortarget(const funcname)", api_net_waittarget);
    csl->addFunc(module, "net_lastattack ()", api_net_lastattack);
    csl->addFunc (module, "game_settimer(const time, const func)",
                  api_game_settimer);

    csl->addFunc (module, "char_gethighlighthue (const id)",
                  api_char_gethighlight);
    csl->addFunc (module, "char_getstatus (const id, const property)",
                  api_char_getstatus);
    csl->addFunc (module, "net_attackrequest(const charid)",
                  api_net_attackrequest);
    csl->addFunc (module, "net_openbackpack()", api_net_openbackpack);
    csl->addFunc (module, "net_skilllock(const skillid, const lock)",
                  api_net_skilllock);
    csl->addFunc (module,
                  "char_getskill(const id, const skillid, [var & name, var & unmodified, var & skillcap, var & skilllock, var & skill_button ])",
                  api_char_getskill);
    csl->addFunc (module, "net_toggle_runmode()",
                  api_net_togglerun);

    csl->addFunc (module,
                  "char_addtext (const id, const text, [ const timeout , const hue])",
                  api_char_addtext);

    csl->addFunc(module, "control_getid (const name)", api_control_getid);
    csl->addFunc(module, "gui_addform (const x, const y, const gfm_file, [const is_container])", api_gui_addform);

    //Object Functions
    csl->addFunc (module, "object_getcontentcount(const id)",
                  api_object_getcontentcount);
    csl->addFunc (module, "object_getcontententry(const id, const index)",
                  api_object_getcontententry);
    csl->addFunc (module, "object_gethue(const id)", api_object_gethue);
    csl->addFunc (module, "object_getbody(const id)", api_object_getbody);
    csl->addFunc (module, "object_getquantity(const id)",
                  api_object_getquantity);
    csl->addFunc (module, "object_getparent(const id)", api_object_getparent);
    csl->addFunc (module,
                  "object_getposition(const id, var & x, var & y, var & z)",
                  api_object_getposition);

    csl->addFunc (module,
                  "font_register(const id, const filename, const size, [const defaulthue])",
                  api_font_register);
    csl->addFunc (module, "font_unregister(const id)", api_font_unregister);

    csl->addFunc (module, "utils_bit_and(const a, const b)",
                  api_utils_bitand);
    csl->addFunc (module, "utils_bit_or(const a, const b)", api_utils_bitor);
    csl->addFunc (module, "utils_bit_rightshift(const a, const b)",
                  api_utils_rightshift);
    csl->addFunc (module, "utils_bit_leftshift(const a, const b)",
                  api_utils_leftshift);

    // added in 0.8 Artix
    csl->addFunc (module, "camera_reset()", api_camera_reset);

    // added in 0.86 SiENcE
    //Harkon: get locale string from cliloc.*
    csl->addFunc(module, "cliloc_getmessage(const id)", api_loader_getmessage);

    // added from Tensor
    csl->addFunc (module, "load_world_light_map(const ambientfilename, const sunfilename)",
                           api_load_world_light_map);
    csl->addFunc (module, "load_world_fog_map(const fogfilename)",
                           api_load_world_fog_map);

    // added in 0.86 SiENcE
    csl->addFunc (module, "net_openquestgump(const charid, const subcmd)", api_net_openquestgump);
  }

  catch (const ZException & err)
  {
    for (int i = 0; i < err.count (); i++)
      Logger::WriteLine (err[i]);
    if (cslOk)
      delete csl;
    csl = NULL;
    cslOk = zFalse;
    initialized = false;
  }                             // catch
}


void CSLHandler::InitNet (void)
{
  if (pClient)
      {
        pClient->OnServerList (net_handler_serverlist);
        pClient->OnCharList (net_handler_charlist);
        pClient->OnGameStart (net_handler_startgame);
        pClient->OnNetError (net_handler_error);
        pClient->OnLightLevel (net_handler_lightlevel);
        pClient->OnSpeech (net_handler_speech);
        pClient->OnStatChange (net_statchange);
        pClient->OnStatus (net_status);
        pClient->OnOpenContainer (net_opencontainer);
        pClient->OnContainerContent (net_containercontent);
        pClient->OnSkill (net_skill);
        pClient->OnPaperdoll (net_paperdoll);
        pClient->OnAttackReply (net_attackreply);
        pClient->OnDyeWindow (net_dyewindow);
        pClient->OnWarmode (net_warmodechange);
        pClient->OnMenuItems (net_menuitems);
        pClient->OnPopupDisplay (net_displaypopup);
        pClient->OnOpenSpellBook (net_spellbook);
        pClient->OnBuyWindowOpen (net_buy);
        pClient->OnBuyWindowAdd (net_buyadd);
        pClient->OnSellWindowOpen (net_sell);
        pClient->OnSellWindowAdd (net_selladd);
        pClient->OnTradeStart (net_trade);
        pClient->OnTradeCheck (net_tradecheck);
        pClient->OnTradeAdd (net_tradeadd);

      }
  Game::GetInstance()->OnStatusDrag (game_onstatusdrag);
  Game::GetInstance()->OnDynamicDrag (game_ondynamicdrag);
  Game::GetInstance()->OnAOSTooltip (game_onaostooltip);
}
