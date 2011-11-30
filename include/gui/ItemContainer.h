//
// File: ItemContainer.h
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

#ifndef _ITEMCONTAINER_H_
#define _ITEMCONTAINER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <map>
#include "Common.h"
#include "Game.h"
#include "SDL/SDL.h"
#include "Control.h"
#include "renderer/Texture.h"
#include "renderer/DynamicObjects.h"

// #include "../Fluid/mmgr.h"

class cItemContainerEntry
{
public:
	cItemContainerEntry(Uint32 id, Uint16 model, int x, int y, Uint16 hue);
	~cItemContainerEntry();

	Texture *texture () { return m_texture; }
	int x() { return m_x; }
	int y() { return m_y; }
	Uint32 id() { return m_id; }
	Uint16 model() { return m_model; }

private:
	Texture * m_texture;
	int m_x, m_y;
	Uint32 m_id;
	Uint16 m_model;
};


class ItemContainer : public Control
{
 public:
  ItemContainer ();
  virtual ~ItemContainer ();

  virtual void Draw(GumpHandler * gumps);
  
  Uint32 containerid() { return m_containerid; }
  void setContainerID (Uint32 containerid) { m_containerid = containerid; }
  virtual int HandleMessage(gui_message * msg);
  
  void Rebuild();
  void Clear ();
  
  cItemContainerEntry * FindEntryAt(int posx, int posy);
  virtual Uint32 FindDragContainer(int x, int y, int * drop_x, int * drop_y, Uint32 * charid);
  
  void RemoveItem(Uint32 id);
  void AddItem(cDynamicObject * object);
  
 
private:
  Uint32 m_containerid;
  Uint32 last_click;
  Uint32 last_click_id;
  int clickdown_x, clickdown_y;
  int drag_id;
  Uint16 drag_model;
  Uint32 mouseover_id;
  
  bool dragging;
  
  std::map <Uint32, cItemContainerEntry *> entries;
};


#endif
