//
// File: Container.h
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

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"
#include <map>

// #include "../Fluid/mmgr.h"


class Container : public Control
{
 public:
  Container();
  ~Container();

  virtual void Draw(GumpHandler * gumps);
  virtual int HandleMessage(gui_message * msg);

  void ClearControls (void);
  void AddControl(Control * control);
  void AddControl(Control * control, int page);
  Control * GetControl(int controlid);
  ControlList_t* GetControlList();
  int GetCurrentPage(void);
  void SetCurrentPage(int);
  void SetFocus(int controlid);
  
  void Rewind(void);
  Control * GetNext(void);
  
  virtual void SetAlpha (unsigned char alpha);
  virtual bool CheckPixel(int x, int y);
  
  void SetShape(int controlid);
  
  virtual Uint32 FindDragContainer(int x, int y, int * drop_x, int * drop_y, Uint32 * charid);
  //Control* RemoveControl(int controlid);
  //Control* RemoveControl(Control *control);
   
   void SetGumpID(int id){gump_id = id;}
   void SetPlayerID(int id){pl_id = id;}
   int GetPlayerID(){return pl_id;}
   int GetGumpID(){return gump_id;}

 private:
  ControlList_t	control_root;
  int idcounter;
  int search_index;
  int focusid;
  int current_page;
  int shape_id;
   int gump_id;
   int pl_id;   

  int SendMessageToItems(gui_message * msg);
  void ReleaseFocus(int controlid);
  void HandleMessageQueues(void);

};


#endif
