//
// File: Paperdoll.h
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

#ifndef _PAPERDOLL_H_
#define _PAPERDOLL_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"

#include "renderer/Characters.h"

// #include "../Fluid/mmgr.h"

class Paperdoll : public Control
{
 public:
  Paperdoll ();
  Paperdoll (int x, int y);
  Paperdoll (int x, int y, int flags);
  ~Paperdoll ();
  virtual void Draw(GumpHandler * gumps);
  
  void AssignChar(cCharacter * character);
  virtual int HandleMessage(gui_message * msg);
  
  cCharacterEquip * FindObjectAt(int x, int y, int &layer);
  void DelObject(int layer);
  void Rebuild();
  
  virtual Uint32 FindDragContainer(int x, int y, int * drop_x, int * drop_y, Uint32 * charid);
  
 private:
  Uint32 character_id;
  Uint32 last_click;
  Uint32 last_click_id;
  int clickdown_x, clickdown_y;
  int drag_layer;
  int drag_id;
  Uint16 drag_model;
  
  int drop_id;
  int drop_layer;
  
  Texture * texture;
  char * layermap;
};


#endif
