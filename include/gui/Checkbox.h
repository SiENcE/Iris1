//
// File: Checkbox.h
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

#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "Control.h"

// #include "../Fluid/mmgr.h"

#define CHECKBOX_UNCHECKED 0
#define CHECKBOX_CHECKED   1

// CHECKBOX X Y UNSELECTEDPIC SELECTEDPIC STARTSTATE IDNR

class Checkbox : public Control
{
public:
	Checkbox( int iCheckedPic, int iUncheckedPic );
	Checkbox( int iX, int iY, int iCheckedPic, int iUnCheckedPic, bool bStartState );
	virtual ~Checkbox();

	void SetGump( int iType, int iGump );
	void SetChecked( bool bChecked );
	bool IsChecked();

	virtual int HandleMessage( gui_message *kMsg );
	virtual void Draw( GumpHandler *kGumps );

private:
	int m_iCheckedGump;
	int m_iUncheckedGump;
	bool m_bChecked;
};

#endif	// _CHECKBOX_H_
