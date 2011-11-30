//
// File: Button.h
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

#ifndef _BUTTON_H_
#define _BUTTON_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Image.h"

// #include "../Fluid/mmgr.h"

#define BUTTONGUMP_NORMAL    0
#define BUTTONGUMP_MOUSEOVER 1
#define BUTTONGUMP_PRESSED   2

class Button : public Control
{
 public:
  Button ();
  Button (int x, int y);
  ~Button ();
  void SetButton (int type, int gump);
  int GetButton (int type);
  void SetPressed(int pressed);
  int GetPressed(void);

  bool IsPageSelector(void);
  void SetPageSelector(bool sel);
  void SetDestinationPage(int dp);
  int GetDestinationPage(void);

  void SetClickMessage(gui_message * msg);
  void OnClick (int (*callback) (Control *sender));

  virtual int HandleMessage(gui_message * msg);
  virtual void Draw(GumpHandler * gumps);
  virtual bool CheckPixel(int x, int y);
  
  void DoOnClick (void);
  
  int GetReturnMsg(){return m_return_message;}
  void SetReturnMsg(int ret_msg){m_return_message = ret_msg;}
  void SetCloseGump(bool cl_gump){m_closegump = cl_gump;}
  bool isCloseGump(){return m_closegump;}

 private:
  int button_gump[3];
  int pressed;
  int mouseover;
  gui_message onclick_message;
  bool isPageSelector;
  int destPage;
  int m_return_message;
  bool m_closegump;

  int (*callback_OnClick) (Control *sender);
};


#endif
