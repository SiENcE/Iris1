//
// File: Stack.h
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

#ifndef _STACK_H_
#define _STACK_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <stdlib.h>



#define MESSAGE_NONE                   0
#define MESSAGE_MOUSEDOWN              1
#define MESSAGE_MOUSEUP                2
#define MESSAGE_MOUSEMOTION            3
#define MESSAGE_CLOSEGUMP              4
#define MESSAGE_SETFOCUS               5
#define MESSAGE_RELEASEFOCUS           6
#define MESSAGE_BRINGTOFRONT           7
#define MESSAGE_QUIT                   8
#define MESSAGE_STARTGAME              9
#define MESSAGE_KEYPRESSED             10
#define MESSAGE_CHARDELETE             11
#define MESSAGE_ONDRAGITEM             12
#define MESSAGE_REBUILDITEMCONTAINER   13
#define MESSAGE_ADDCONTAINERITEM       14
#define MESSAGE_UPDATEPAPERDOLL        15
#define MESSAGE_ONCLICKITEM            16
#define MESSAGE_REFRESHREQUEST         17
#define MESSAGE_DOREFRESH              18
#define MESSAGE_ITEMDELETE             19
#define MESSAGE_CALLBACK               20

#define CALLBACK_ONCLICK       0 
#define CALLBACK_ONKEYPRESS    1
#define CALLBACK_ONMOUSEUP     2
#define CALLBACK_ONMOUSEDOWN   3
#define CALLBACK_ONCLOSE       4

struct message_MouseEvent {
 int type;
 int x, y, button;
};

struct message_KeyPressed {
  int type;
  unsigned short key;
};

struct message_MouseMotionEvent {
 int type;
 int x, y, relx, rely, button;
};

struct message_WindowAction {
 int type;
 int controlid;
};

struct message_CharDelete {
 int type;
 unsigned int charid;
};

struct message_OnDragItem {
 int type;
 unsigned int itemid;
 unsigned short model;
};

struct message_OnRebuildItemContainer {
 int type;
 unsigned int containerid;
};

struct message_AddContainerItem {
 int type;
 unsigned int containerid;
 unsigned int id;
};

struct message_UpdatePaperdoll {
 int type;
 unsigned int id;
};

struct message_OnClickItem {
 int type;
 unsigned int itemid;
 bool doubleclick;
};

struct message_Refresh {
 int type;
 unsigned int time;
};

struct message_ItemDelete {
 int type;
 unsigned int itemid;
};

struct message_Callback {
 int type;
 int callback_type;
 unsigned int id;
 unsigned int containerid;
 int key;
};

union gui_message {
   int type;
   struct message_MouseEvent mouseevent;
   struct message_MouseMotionEvent mousemotionevent;
   struct message_WindowAction windowaction;
   struct message_KeyPressed keypressed;
   struct message_CharDelete chardelete;
   struct message_OnDragItem dragitem;
   struct message_OnRebuildItemContainer rebuilditemcontainer;
   struct message_AddContainerItem addcontaineritem;
   struct message_UpdatePaperdoll updatepaperdoll;
   struct message_OnClickItem onclickitem;
   struct message_Refresh refresh;
   struct message_ItemDelete itemdelete;
   struct message_Callback callback;
};

struct message_entry {
   gui_message msg;
   struct message_entry * next;
};

class MessageStack {
  public:
    MessageStack ();
    virtual ~ MessageStack ();
    void Clear(void);
    void Push(gui_message message);
    int Pop(gui_message * message);
  private:
    struct message_entry * root;
};



#endif
