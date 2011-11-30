//
// File: Radiobutton.h
// Created by: Netzzwerg
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

#ifndef _RADIOBUTTON_H_
#define _RADIOBUTTON_H_

#include "gui/Checkbox.h"
#include "gui/Container.h"
#include "gui/GUIHandler.h"

// #include "../Fluid/mmgr.h"

// RADIO X Y UNSELECTEDPIC SELECTEDPIC STARTSTATE IDNR

class RadioButton : public Checkbox
{
public:
	RadioButton( int iUnChecked, int iChecked );
	RadioButton( int iX, int iY, int iUnChecked, int iChecked, bool bChecked );
	~RadioButton();

	void SetGroup( int iGroupId );
	int GetGroup();
	virtual int HandleMessage( gui_message *kMsg );

private:
	int m_iGroup;
};

#endif	// _RADIOBUTTON_H_
