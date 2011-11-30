//
// File: RadioButton.cpp
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

#include "gui/RadioButton.h"




RadioButton::RadioButton( int iUnChecked, int iChecked ) : Checkbox( iUnChecked, iChecked )
{
	m_iGroup = 0;
}


RadioButton::RadioButton( int iX, int iY, int iUnChecked, int iChecked, bool bChecked ) : 
			Checkbox( iX, iY, iUnChecked, iChecked, bChecked )
{
	m_iGroup = 0;
}


RadioButton::~RadioButton()
{
}


void RadioButton::SetGroup( int iGroupId )
{
	m_iGroup = iGroupId;
}


int RadioButton::GetGroup()
{
	return m_iGroup;
}


int RadioButton::HandleMessage( gui_message *kMsg )
{
	if ( !kMsg )
	{
		Logger::WriteLine( "NULL msg in RadioButton::HandleMessage(gui_message *)", __FILE__, __LINE__, LEVEL_ERROR );
		return false;
	}

	switch ( kMsg->type )
	{
	case MESSAGE_MOUSEDOWN:
		if ( MouseIsOver( kMsg->mouseevent.x, kMsg->mouseevent.y ) )
		{
			printf( "%d %d %d\n", x, y, IsChecked() );
			if ( !IsChecked() )
			{
				// Now uncheck all other radios in group:
				ControlList_t *list = NULL;
				ControlList_t::iterator iter;

				if ( parent == NULL )
				{
					list = pUOGUI.GetControlList();
				}
				else
				{
					list = parent->GetControlList();
				}

				for ( iter = list->begin(); iter != list->end(); iter++ )
				{
					if ( dynamic_cast<RadioButton *>( iter->second ) )
					{
						RadioButton *kRadio = (RadioButton *)iter->second;
						if ( kRadio->GetGroup() == GetGroup() )
						{
							kRadio->SetChecked( false );
						}
					}
				}

				// I know I am repeating this instruction but it's the fastest way!
				SetChecked( true );

				return true;
			}
		}
		break;

	default:
		return Checkbox::HandleMessage( kMsg );
	}

	return false;
}
