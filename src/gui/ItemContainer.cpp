//
// File: ItemContainer.cpp
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

#include "gui/ItemContainer.h"
#include "Logger.h"
#include "Config.h"
#include "Game.h"

#include "renderer/DynamicObjects.h"
#include "renderer/TextureBuffer.h"

#include "loaders/ArtLoader.h"
#include <cassert>




using namespace std;

bool c_aostooltip = false;

cItemContainerEntry::cItemContainerEntry (Uint32 id, Uint16 model, int x,
                                          int y, Uint16 hue)
{
	cDynamicObject *obj = pDynamicObjectList.Get (id);
	int count = obj->itemcount;
	if (model == 0xEEA || model == 0xEED || model == 0xEF0)
	{
		if (count > 9 && count < 20)
			model++;
		else if (count >= 20)
			model += 2;
	}
	m_texture = ArtLoader::GetInstance()->LoadArt( model + 16384, true, false, hue );
	m_id = id;
	m_model = model;
	m_x = x;
	m_y = y;
}


cItemContainerEntry::~cItemContainerEntry ()
{
	SAFE_DELETE( m_texture );
}

ItemContainer::ItemContainer ()
{
  Control::Control ();
  control_type = CONTROLTYPE_ITEMCONTAINER;
  SetFlags (0);
  SetSize (256, 256);
  m_containerid = 0;
  drag_id = 0;
  clickdown_x = 0;
  clickdown_y = 0;
  last_click = 0;
  last_click_id = 0;
  mouseover_id = 0;
  dragging = false;
}

ItemContainer::~ItemContainer ()
{
  Clear ();
}


void ItemContainer::Draw (GumpHandler * gumps)
{


  Control::Draw (gumps);

  std::map < Uint32, cItemContainerEntry * >::iterator iter;
  for (iter = entries.begin (); iter != entries.end (); iter++)
      {
        cDynamicObject *obj = pDynamicObjectList.Get (iter->second->id ());
        if (obj)
            {
              int hue = (int) obj->dye;

              Texture *texture = iter->second->texture ();
              assert (texture);
              int divisor = 1;
              if (iter->second->id () == mouseover_id)
                divisor = 2;
              int stackcheck = 1;
              if ((obj->itemcount > 1)
                  && (obj->model < 0xEEA || obj->model > 0xEF2))
                stackcheck = 2;

              for (int i = 0; i < stackcheck; i++)
                  {
                    if (hue != 0)
                      DrawRectHued (GetX () + iter->second->x () + (6 * i),
                                    GetY () + iter->second->y () + (6 * i),
                                    texture->GetWidth (),
                                    texture->GetHeight (), texture, hue,
                                    GetAlpha () / divisor, true);
                    else
                      DrawRect (GetX () + iter->second->x () + (6 * i),
                                GetY () + iter->second->y () + (6 * i),
                                texture->GetWidth (), texture->GetHeight (),
                                texture, GetAlpha () / divisor, true);
                  }
            }
      }
}

int ItemContainer::HandleMessage (gui_message * msg)
{
  if (!m_containerid)
    return false;

  cItemContainerEntry *entry;

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        entry = FindEntryAt (msg->mouseevent.x, msg->mouseevent.y);
        if (entry)
            {
              drag_id = entry->id ();
              drag_model = entry->model ();
              clickdown_x = msg->mouseevent.x;
              clickdown_y = msg->mouseevent.y;
              return true;
            }
        break;
      case MESSAGE_MOUSEMOTION:
        entry =
          FindEntryAt (msg->mousemotionevent.x, msg->mousemotionevent.y);
        if (entry)
            {
              mouseover_id = entry->id ();
              if (Config::GetAOSToolTips() && !c_aostooltip)
                  {
                    cDynamicObject *obj =
                      pDynamicObjectList.Get (entry->id ());
                    int count = obj->aostooltips_count ();
					Game::GetInstance()->DrawAOSTooltip (entry->id (), count,
                                          msg->mousemotionevent.x,
                                          msg->mousemotionevent.y);
                    c_aostooltip = true;
                  }
            }
        else
            {
              mouseover_id = 0;
              if (Config::GetAOSToolTips() && c_aostooltip)
                Game::GetInstance()->DrawAOSTooltip (0, 0, 0, 0);
              c_aostooltip = false;
            }
        if ((abs (clickdown_x - msg->mousemotionevent.x) > 5) &&
            (abs (clickdown_y - msg->mousemotionevent.y) > 5) && drag_id
            && (!dragging))
            {
              RemoveItem (drag_id);

              gui_message msg;
              msg.type = MESSAGE_ONDRAGITEM;
              msg.dragitem.itemid = drag_id;
              msg.dragitem.model = drag_model;
              stack.Push (msg);

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
      case MESSAGE_REBUILDITEMCONTAINER:
        if (msg->rebuilditemcontainer.containerid == m_containerid)
          Rebuild ();
        break;
      case MESSAGE_ADDCONTAINERITEM:
        if (msg->addcontaineritem.containerid == m_containerid)
            {
              cDynamicObject *object;
              object = pDynamicObjectList.Get (msg->addcontaineritem.id);
              if (object)
                AddItem (object);
              return true;
            }
        break;
      }
  return false;
}

void ItemContainer::Clear ()
{
	std::map < Uint32, cItemContainerEntry * >::iterator iter;
	
	for (iter = entries.begin (); iter != entries.end (); iter++)
	{
		// NOTE: Something's wrong with this, we are trying to de allocate memory that was not allocated.
		// SAFE_DELETE( iter->second );
	}
  entries.clear ();
}

void ItemContainer::Rebuild ()
{
  Clear ();

  dynamiclist_t *dynamics = pDynamicObjectList.GetList ();
  assert (dynamics);

  dynamiclist_t::iterator iter;

  for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
      {
        if ((iter->second->parent == m_containerid)
            && (iter->second->type == DYNAMICTYPE_CONTAINER))
          AddItem (iter->second);
      }
}

cItemContainerEntry *ItemContainer::FindEntryAt (int posx, int posy)
{
  std::map < Uint32, cItemContainerEntry * >::reverse_iterator iter;
  for (iter = entries.rbegin (); iter != entries.rend (); iter++)
      {
        Texture *texture = iter->second->texture ();
        assert (texture);
        int x = iter->second->x () + GetX ();
        int y = iter->second->y () + GetY ();
        if (texture->CheckPixel (posx - x, posy - y, true))
          return iter->second;
      }
  return NULL;
}

Uint32 ItemContainer::FindDragContainer (int x, int y, int *drop_x,
                                         int *drop_y, Uint32 * charid)
{
  if (drop_x)
    *drop_x = x;
  if (drop_y)
    *drop_y = y;
  if (charid)
    *charid = 0;

  return m_containerid;
}

void ItemContainer::RemoveItem (Uint32 id)
{
  std::map < Uint32, cItemContainerEntry * >::iterator iter;
  iter = entries.find (id);
  if (iter != entries.end ())
      {
        delete iter->second;
        entries.erase (id);
      }
}


void ItemContainer::AddItem (cDynamicObject * object)
{
  assert (object);
  if ((object->parent == m_containerid)
      && (object->type == DYNAMICTYPE_CONTAINER))
      {
        cItemContainerEntry *entry =
          new cItemContainerEntry (object->id, object->model, object->x,
                                   object->y, object->dye);
        assert (entry);
        if (!entry->texture ())
          delete entry;
        else
            {
              entries.insert (make_pair (entry->id (), entry));
            }
      }
}
