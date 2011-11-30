/*
 * File: Client.cpp
 * Created by Alexander Oster.
 * last Change: 14-04-06 (Florian Fischer^SiENcE)
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

#include "gui/Paperdoll.h"
#include "loaders/PaperdollBuilder.h"
#include "Logger.h"
#include "Config.h"
#include <cassert>




using namespace std;

Paperdoll::Paperdoll ()
{
	Control::Control ();
	texture = NULL;
	control_type = CONTROLTYPE_PAPERDOLL;
	layermap = new char[256 * 256];
	drag_id = 0;
	clickdown_x = 0;
	clickdown_y = 0;
	last_click = 0;
	last_click_id = 0;
	character_id = 0;
	drop_id = 0;
	SetSize (256, 256);
}

Paperdoll::Paperdoll (int x, int y)
{
	Control::Control ();
	SetPosition (x, y);
	texture = NULL;
	control_type = CONTROLTYPE_PAPERDOLL;
	layermap = new char[256 * 256];
	drag_id = 0;
	clickdown_x = 0;
	clickdown_y = 0;
	last_click = 0;
	last_click_id = 0;
	character_id = 0;
	SetSize (256, 256);
}

Paperdoll::Paperdoll (int x, int y, int flags)
{
	Control::Control ();
	SetPosition (x, y);
	SetFlags (flags);
	texture = NULL;
	control_type = CONTROLTYPE_PAPERDOLL;
	layermap = new char[256 * 256];
	drag_id = 0;
	clickdown_x = 0;
	clickdown_y = 0;
	last_click = 0;
	last_click_id = 0;
	character_id = 0;
	SetSize (256, 256);
}

Paperdoll::~Paperdoll ()
{
	SAFE_DELETE( texture );
	SAFE_DELETE_ARRAY( layermap );
}


void Paperdoll::Draw (GumpHandler * gumps)
{
  assert (gumps);
  Control::Draw (gumps);
  if (texture)
      {
        if ((!GetWidth ()) || (!GetHeight ()))
          SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
        DrawRect (GetX (), GetY (), GetWidth (), GetHeight (), texture,
                  GetAlpha ());
      }
}

void Paperdoll::AssignChar (cCharacter * character)
{
  assert (character);
  assert (layermap);

  character_id = character->id ();
  memset (layermap, 0, 256 * 256);

  SAFE_DELETE( texture );

  cPaperdollBuilder *builder = new cPaperdollBuilder();
  assert (builder);

  int base_id = 50000;

//Should be seperated into an XML-File !!!!!!!!!!!!!
  //Human-Male Paperdoll
  if (character->body () == 400 ||
      character->body () == 744)  //Necromancy Transfromed Model
      {
        builder->AddGump (0x0C, 0, layermap, character->hue ());
        base_id = 50000;
      }
  //Human-Savage_Male
  else if (character->body () == 183 ||
           character->body () == 185 ||
           character->body () == 750)
      {
        builder->AddGump (0x79, 0, layermap, character->hue ());
        base_id = 50000;
      }
  //Human-Female Paperdoll
  else if (character->body () == 401 ||
           character->body () == 745)  //Necromancy Transfromed Model
      {
        builder->AddGump (0x0D, 0, layermap, character->hue ());
        base_id = 60000;
      }
  //Human-Savage_Female
  else if (character->body () == 184 ||
           character->body () == 186 ||
           character->body () == 751)
      {
        builder->AddGump (0x78, 0, layermap, character->hue ());
        base_id = 60000;
      }
  //Male-Elf Paperdoll
  else if (character->body () == 605)
      {
        builder->AddGump (0x0E, 0, layermap, character->hue ());
        base_id = 50000;
      }
  //Female-Elf Paperdoll
  else if (character->body () == 606)
      {
        builder->AddGump (0x0F, 0, layermap, character->hue ());
        base_id = 60000;
      }
//SiENcE: new test!
  //Lord British
  else if (character->body () == 990)
       {
        builder->AddGump (0x3DE, 0, layermap, character->hue ());
        base_id = 50000;
       }
  //Blackthorn
  else if (character->body () == 991)
       {
        builder->AddGump (0x3DF, 0, layermap, character->hue ());
        base_id = 50000;
       }
  //Dupre (wrong paperdoll ?!)
  else if (character->body () == 994)
      {
        builder->AddGump (0x3E2, 0, layermap, character->hue ());
      }
  //Player Ghosts
  else if (character->body () == 402 ||
           character->body () == 403 ||
           character->body () == 607 ||
           character->body () == 608 ||
           character->body () == 970)
      {
        builder->AddGump (0x3DB, 0, layermap, character->hue ());
      }

  int layerOrder[0x18] = { 0x09,    // - N/A (not used)
    0x14,                       // - Back (Cloak)
    0x0B,                       // - Hair
    0x10,                       // - Facial Hair
    0x12,                       // - Earrings
    0x04,                       // - Leg Covering (including Pants, Shorts, Bone/Chain/Ring legs)
    0x0E,                       // - Bracelet
    0x03,                       // - Foot Covering/Armor
    0x08,                       // - Ring
    0x18,                       // - Legs (inner)(Leg Armor)
    0x05,                       // - Chest Clothing/Female Chest Armor
    0x0D,                       // - Torso (inner)(Chest Armor)
    0x11,                       // - Torso (Middle)(Surcoat, Tunic, Full Apron, Sash)
    0x0A,                       // - Neck Covering/Armor
    0x06,                       // - Head Covering/Armor
    0x13,                       // - Arm Covering/Armor
    0x07,                       // - Gloves
    0x16,                       // - Torso (outer)(Robe)
    0x17,                       // - Legs (outer)(Skirt/Kilt)
    0x0C,                       // - Waist (Half-Apron)
    0x01,                       // - Single-Hand item/weapon
    0x02,                       // - Two-Hand item/weapon (including Shield)
    0x15,                       // - BackPack
    0x0F                        // - Unused !?
  };
  for (int layer = 0x01; layer <= 0x18; layer++)
      {
        cCharacterEquip *equip = character->GetEquip (layerOrder[layer]);
        if (equip)
            {
              builder->AddGump (base_id + equip->anim (), layerOrder[layer],
                                layermap, equip->hue ());
            }
      }
  texture = builder->CreateTexture();

  SAFE_DELETE( builder );
}

int Paperdoll::HandleMessage (gui_message * msg)
{
  if (!character_id)
    return false;

  cCharacterEquip *object;
  int layer;

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        object = FindObjectAt (msg->mouseevent.x, msg->mouseevent.y, layer);
        if (object)
            {
              drag_id = object->id ();
              drag_model = object->model ();
              drag_layer = layer;
              clickdown_x = msg->mouseevent.x;
              clickdown_y = msg->mouseevent.y;
              return true;
            }
        break;
      case MESSAGE_MOUSEMOTION:

        if ((abs (clickdown_x - msg->mousemotionevent.x) > 5) &&
            (abs (clickdown_y - msg->mousemotionevent.y) > 5) && drag_id
            && (drag_layer != 0x15))
            {

              gui_message msg;
              msg.type = MESSAGE_ONDRAGITEM;
              msg.dragitem.itemid = drag_id;
              msg.dragitem.model = drag_model;
              stack.Push (msg);

              DelObject (drag_layer);

              drag_id = 0;
              return true;

            }

        if (drag_id)
          return true;

        break;

      case MESSAGE_MOUSEUP:
        if ((abs (clickdown_x - msg->mouseevent.x) <= 5) &&
            (abs (clickdown_y - msg->mouseevent.y) <= 5) && drag_id)
            {
              bool handled = false;
              Uint32 currenttime = SDL_GetTicks ();
              if (last_click)
                if (currenttime - last_click < CLICK_TIME)
                    {
                      gui_message msg;
                      msg.type = MESSAGE_ONCLICKITEM;
                      msg.onclickitem.itemid = drag_id;
                      msg.onclickitem.doubleclick = true;
                      stack.Push (msg);
                      last_click = 0;
                      handled = true;
                    }
              if (!handled)
                  {
                    last_click = currenttime;
                    last_click_id = drag_id;
                    gui_message msg;
                    msg.type = MESSAGE_REFRESHREQUEST;
                    msg.refresh.time = currenttime + CLICK_TIME;
                    stack.Push (msg);
                  }
            }

        drag_id = 0;
        clickdown_x = 0;
        clickdown_y = 0;
        break;
      case MESSAGE_DOREFRESH:
        if (last_click && (last_click + CLICK_TIME < msg->refresh.time))
            {
              if (last_click_id)
                  {
                    gui_message msg;
                    msg.type = MESSAGE_ONCLICKITEM;
                    msg.onclickitem.itemid = last_click_id;
                    msg.onclickitem.doubleclick = false;
                    stack.Push (msg);
                  }
              last_click_id = 0;
              last_click = 0;
            }
        break;
      case MESSAGE_UPDATEPAPERDOLL:
        if (msg->updatepaperdoll.id == character_id)
            {
              Rebuild ();
            }
        break;
      }
  return false;
}


cCharacterEquip *Paperdoll::FindObjectAt (int x, int y, int &layer)
{
  if ((x < 0) || (y < 0) || (x >= 256) || (y >= 256))
    return NULL;

  assert (layermap);
  layer = layermap[x + y * 256];
  if (!layer)
    return NULL;

  cCharacter *character = pCharacterList.Get (character_id);

  if (!character)
    return NULL;

  return character->GetEquip (layer);
}



void Paperdoll::DelObject (int layer)
{

  cCharacter *character = pCharacterList.Get (character_id);

  if (!character)
    return;

  character->DelEquip (layer);
  AssignChar (character);

}

void Paperdoll::Rebuild ()
{

  cCharacter *character = pCharacterList.Get (character_id);

  if (!character)
    return;

  AssignChar (character);
}

Uint32 Paperdoll::FindDragContainer (int x, int y, int *drop_x, int *drop_y,
                                     Uint32 * charid)
{
  int layer = 0;
  cCharacterEquip *equip = FindObjectAt (x, y, layer);

  if (drop_x)
    *drop_x = 0;
  if (drop_y)
    *drop_y = 0;
  if (charid && (layer != 0x15) /* 0x15 = backpack */ )
    *charid = character_id;

  if ((layer == 0x15) && (equip) /* backpack */ )
    return equip->id ();

  return character_id;
}
