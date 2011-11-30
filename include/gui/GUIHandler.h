//
// File: GUIHandler.h
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

#ifndef _GUIHANDLER_H_
#define _GUIHANDLER_H_

#include "Common.h"
#include "SDL/SDL.h"
#include <map>
#include <list>

#include "Stack.h"
#include "Control.h"
#include "Gump.h"
#include "renderer/Texture.h"
#include "SDLEvent.h"

// #include "../Fluid/mmgr.h"

class GUIHandler
{
public:
  GUIHandler ();
  ~GUIHandler ();

  void DeInit ();
  
  void Draw (void);

  void ClearControls (void);

  void AddControl(Control * control);
  Control * GetControl(int controlid);
  void CloseWindow(int controlid);
  void SetFocus(int controlid);
  void ReleaseFocus(int controlid);
  void BringToFront(int controlid);

  int HandleMessage(gui_message * msg);
  void HandleMessageQueues(void);

  void SetQuitFlag();

  int StartFlag(void);
  void SetStartFlag(int flag);

  void SetDefaultFocus(Control * control);

  void AddMessageToStack(gui_message * msg);

  ControlList_t* GetControlList(void);
  
  void Rewind(void);
  Control * GetNext(void);

  void SetPosition(int x, int y);
  int GetX(void);
  int GetY(void);
  
  Uint32 RegisterGump(Texture * texture);
  void UnregisterGump(Uint32 id);
  
  void OnDrag(void (*callback) (unsigned int itemid, unsigned int model));
  void OnItemClick(void (*callback) (unsigned int itemid, bool doubleclick));
  
  void LoadCursor(int id, int artid);
  void SetCursorPos(int x, int y);
  
  bool FindDragContainer(int x, int y, Uint32 * containerid = NULL, int * drop_x = NULL, int * drop_y = NULL, Uint32 * charid = NULL);
  
  void LoadDragCursor (Uint16 model);
  
  void SetDragging(bool dragging);
  
  void AdjustDropPosition(int &x, int &y);
  
  void setwarmode(bool mode){m_warmode = mode;}
  bool getwarmode(){return m_warmode;}

private:

 ControlList_t	control_root;
 ControlList_t	z_root;

 MessageStack stack;

 int default_focusid;

 int focusid;
 int search_index;

 int idcounter;
 int zcounter;
 int startflag;
 int posx, posy;
 
 Uint32 drag_id;
 Uint16 drag_model;
 bool m_dragging;
 bool m_warmode;
 
 Texture * tex_cursors[9];
 Texture * drag_cursor;
 int cursorx, cursory;
 
 std::list <Uint32> refresh_times;
 
 void Reset ();
	
 void (*callback_OnDrag) (unsigned int itemid, unsigned int model);
 void (*callback_OnItemClick) (unsigned int itemid, bool doubleclick);
};

extern   GUIHandler  pUOGUI;

#endif
