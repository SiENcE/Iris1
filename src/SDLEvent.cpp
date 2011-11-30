/*
 * Created by Gustav Nylander.
 * Last change: 22-02-06 (Nuno Ramiro)
 */

/*
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
 */

#include "SDLEvent.h"




bool SDLEvent::m_bQuit = false;


SDLEvent::SDLEvent() : m_iLastTick( 0 ), m_iLastX( 0 ), m_iLastY( 0 ), m_iLastButton( 0 ), m_uiLastClick( 0 ), 
						m_iClickDownX( 0 ), m_iClickDownY( 0 ), m_bIsDragging( false )
{

}


SDLEvent::~SDLEvent()
{
	// TODO: put destructor code here
}


void SDLEvent::PollEvent()
{
	/* handle the events in the queue */
	//SDL_WarpMouse( 320, 240 );

	unsigned int uiCurrentTime = SDL_GetTicks();

	if ( m_uiLastClick )
	{
		if ( ( uiCurrentTime - m_uiLastClick ) >= CLICK_TIME )
		{
			Game::GetInstance()->HandleClick( m_iLastX, m_iLastY, m_iLastButton, false );
			// printf("Single Click!\n");
			m_uiLastClick = 0;
		}
	}

	while ( SDL_PollEvent( &m_kEvent ) )
	{
		HandleEvent( m_kEvent, uiCurrentTime );
	}
	
	HandleMovement();
}


void SDLEvent::WaitEvent()
{
	unsigned int uiCurrentTime = SDL_GetTicks();

	while ( !m_bQuit && SDL_WaitEvent( &m_kEvent ) )
	{
		HandleEvent( m_kEvent, uiCurrentTime );
	}
}


void SDLEvent::HandleEvent( SDL_Event kEvent, unsigned int uiCurrentTime )
{
	gui_message msg;

	switch ( kEvent.type )
	{
/*
      case SDL_ACTIVEEVENT:
        //Something's happend with our focus
        //If we lost focus or we are iconified, we
        //shouldn't draw the screen
        if ( event.active.gain == 0 )
           isActive = false;
        else
            isActive = TRUE;
        break;
*/
	case SDL_VIDEORESIZE:
		// handle resize event
#ifndef WIN32
		// Do not reinitialize window in Win32
		SDLScreen::GetInstance()->m_kScreen = SDL_SetVideoMode( kEvent.resize.w, kEvent.resize.h, Config::GetBPP(), SDLScreen::GetInstance()->videoFlags );
        
		if ( !SDLScreen::GetInstance()->m_kScreen )
		{
			cerr << "Could not get a surface after resize: " <<
			SDL_GetError() << endl;
			// NOTE: This is bad programming, you should NEVER exit the program without dealocating stuff this should be m_bQuit = true
			exit( 1 );
		};
#endif
		SDLScreen::GetInstance()->ResizeWindow( kEvent.resize.w, kEvent.resize.h );
		break;

	case SDL_KEYDOWN:
		/* handle key presses */
		HandleKeyPress( &kEvent.key.keysym );
		break;

	case SDL_MOUSEMOTION:
		/* handle mouse movements */
		HandleMouseMotion( &kEvent.motion );
		break;

	case SDL_MOUSEBUTTONDOWN:
		if ( !Game::GetInstance()->IsPaused() )
		{
			if ( ( kEvent.button.button == 4 ) )
			{
				pCamera.ChangeZoom( -0.8f );    // Handle mouse wheel up
			}

			if ( ( kEvent.button.button == 5 ) )
			{
				pCamera.ChangeZoom( 0.8f );     // Handle mouse wheel down
			}
		}
		//break;	// ??

	case SDL_MOUSEBUTTONUP:
		if ( ( kEvent.button.button == 4 ) || ( kEvent.button.button == 5 ) )
		{
			break;                // Do nothing on mouse wheel event
		}

		if ( kEvent.type == SDL_MOUSEBUTTONDOWN )
		{
			msg.type = MESSAGE_MOUSEDOWN;
		}
		else
		{
			msg.type = MESSAGE_MOUSEUP;
			pUOGUI.SetDragging( false );
			m_bIsDragging = false;

			if ( Game::GetInstance()->CheckDragDrop( kEvent.button.x, kEvent.button.y ) )
			{
                break;
			}
		}

		msg.mouseevent.x = kEvent.button.x - pUOGUI.GetX ();
		msg.mouseevent.y = kEvent.button.y - pUOGUI.GetY ();
		msg.mouseevent.button = kEvent.button.button;

		if ( !pUOGUI.HandleMessage( &msg ) )
		{
			if ( kEvent.type == SDL_MOUSEBUTTONUP )
			{
				Game::GetInstance()->HandleMouseUp( kEvent.button.x, kEvent.button.y, kEvent.button.button );

				if ( !m_uiLastClick )
				{
					m_uiLastClick = uiCurrentTime;
					m_iLastX = kEvent.button.x;
					m_iLastY = kEvent.button.y;
					m_iLastButton = kEvent.button.button;
				}
				else
				{
					if ( ( uiCurrentTime - m_uiLastClick ) < CLICK_TIME )
					{
						Game::GetInstance()->HandleClick( m_iLastX, m_iLastY, kEvent.button.button, true );
						// printf( "Double Click!\n" );
					}

					m_uiLastClick = 0;
				}
			}
			else
			{
				Game::GetInstance()->HandleMouseDown( kEvent.button.x, kEvent.button.y, kEvent.button.button );
				m_iClickDownX = kEvent.button.x;
				m_iClickDownY = kEvent.button.y;
			}
		}
        break;

	case SDL_QUIT:
		/* handle quit requests */
		m_bQuit = true;
		break;

	default:
		break;
	}
}


void SDLEvent::KillApplication()
{
	m_bQuit = true;
}


bool SDLEvent::GetStatus()
{
	return m_bQuit;
}

/* function to handle key press events */
void SDLEvent::HandleKeyPress( SDL_keysym *kKeysym )
{
	gui_message msg;
	MacroEntry *kEntry = pMacroLoader->GetMacro( (int)kKeysym->sym );

	//if (entry/* && (SDL_GetModState() == entry->keymod)*/)
	if ( kEntry )
	{
		if ( SDL_GetModState() == kEntry->keymod )
		{
			HandleMacro( kEntry );
		}
		else
		{
			int iMacroCount = pMacroLoader->GetEntriesCount( (int)kKeysym->sym );

			if ( iMacroCount > 1 )
			{
				for ( int i = 0; i < iMacroCount; i++ )
				{
					kEntry = pMacroLoader->GetMultiMacro( (int)kKeysym->sym, i );
					
					if ( kEntry && kEntry->keymod == SDL_GetModState() )
					{
						HandleMacro( kEntry );
					}
				}
			}
		}
	}

	msg.keypressed.type = MESSAGE_KEYPRESSED;
	msg.keypressed.key = kKeysym->unicode & 0xFF;

	if ( kKeysym->sym == SDLK_LEFT )
	{
		msg.keypressed.key = SDLK_LEFT;
	}
	else if ( kKeysym->sym == SDLK_RIGHT )
	{
		msg.keypressed.key = SDLK_RIGHT;
	}

	if ( pUOGUI.HandleMessage( &msg ) )
	{
		pUOGUI.HandleMessageQueues();

		return;
	}

	// Check if the GUI can handle it
	Uint8 *uiKeys;

	uiKeys = SDL_GetKeyState( NULL );

	/* ESC key was pressed */
	if ( uiKeys[SDLK_ESCAPE] == SDL_PRESSED )
	{
		m_bQuit = true;
	}

	Renderer *pRenderer = Game::GetInstance()->GetRenderer();
	if ( uiKeys[SDLK_a] == SDL_PRESSED && pRenderer )
	{
		pRenderer->FadeStatics( 50, 1000 );
	}

	if ( uiKeys[SDLK_s] == SDL_PRESSED && pRenderer )
	{
		pRenderer->FadeStatics( 255, 1000 );
	}

	Game::GetInstance()->OnKeyPress( kKeysym );
	/* F1 key was pressed this toggles fullscreen mode - does not work under windows currently */

#ifndef WIN32
	if ( uiKeys[SDLK_F1] == SDL_PRESSED )
	{
		SDLScreen::GetInstance()->ToggleFullScreen();
	}

	if ( uiKeys[SDLK_F12] == SDL_PRESSED )
	{
		SDLScreen::GetInstance()->ScreenSave();
	}
#endif
}


void SDLEvent::HandleMovement( void )
{
	Uint8 *uiKeys;

	uiKeys = SDL_GetKeyState( NULL );

	static Uint32 uiLastTick = 0;
	float fFactor = 30.0f;

	Uint32 uiTick = SDL_GetTicks();
	if ( uiLastTick )
	{
		fFactor = (float) ( uiTick - uiLastTick ) / 150.0f;
	}

	uiLastTick = uiTick;

	if ( fFactor > 30.0f )
	{
		fFactor = 30.0f;
	}

/*
	if ( uiKeys[SDLK_y] == SDL_PRESSED )
	{
		pCamera.SetZ( pCamera.GetZ() + fFactor * 2.0f );
	}

	if ( uiKeys[SDLK_x] == SDL_PRESSED )
	{
		pCamera.SetZ( pCamera.GetZ() - fFactor * 2.0f ); 
	}
*/

	if ( uiKeys[SDLK_LEFT] == SDL_PRESSED )
	{
		Game::GetInstance()->Walk_Simple( WALK_LEFT );
	}
	// pCamera.Rotate( 0.0, 0.0, 8.0 * fFactor );

	if ( uiKeys[SDLK_RIGHT] == SDL_PRESSED )
	{
		Game::GetInstance()->Walk_Simple( WALK_RIGHT );
	}
	// pCamera.Rotate( 0.0, 0.0, -8.0 * fFactor );

	if ( uiKeys[SDLK_UP] == SDL_PRESSED )
	{
		Game::GetInstance()->Walk_Simple( WALK_FORWARD );
	}
	// pCamera.Move( fFactor );

	if ( uiKeys[SDLK_DOWN] == SDL_PRESSED )
	{
		//pCamera.Move (-factor);
		int iDirection =  pClient->player_character()->direction();
		
		if ( iDirection >= 4 )
		{
			iDirection -= 4;
		}
		else
		{
			iDirection += 4;
		}

		pClient->Walk( iDirection );
	}
}


void SDLEvent::HandleMouseMotion( SDL_MouseMotionEvent *kEvent )
{
	Uint8 *uiKeys;
	uiKeys = SDL_GetKeyState( NULL );

	if ( !kEvent )
	{
		Logger::WriteLine( "Null event pointer in SDLEvent::HandleMouseMotion", __FILE__, __LINE__, LEVEL_WARNING );
	
		return;
	}

	if ( !pCamera.forceRotation() )
	{
		pUOGUI.SetCursorPos( kEvent->x, kEvent->y );
	}

	Game::GetInstance()->UpdateDragMode( kEvent->x, kEvent->y );

	gui_message msg;

	msg.type = MESSAGE_MOUSEMOTION;
	msg.mousemotionevent.x = kEvent->x - pUOGUI.GetX();
	msg.mousemotionevent.y = kEvent->y - pUOGUI.GetY();
	msg.mousemotionevent.button = kEvent->state;
	msg.mousemotionevent.relx = kEvent->xrel;
	msg.mousemotionevent.rely = kEvent->yrel;

	if ( pUOGUI.HandleMessage( &msg ) )
	{
		pUOGUI.HandleMessageQueues();
	}
	else
	{
		// This should not be done this way, we should have a method for our GUI/Menu controls
		// and another method for in game actions, so we can gain some performance.
		if ( !Game::GetInstance()->IsPaused() )
		{
			if ( ( kEvent->state & ( SDL_BUTTON( 2 ) | SDL_BUTTON( 3 ) ) ) )
			{
				/*
				if ( SDL_GetModState() & KMOD_LSHIFT )
				{
					pCamera.Rotate( 0.0f, 0.0f, -kEvent->xrel / 3.0f );
					pCamera.ChangeZoom( kEvent->yrel * 0.05f );
				}
				else if ( SDL_GetModState() & ( KMOD_RSHIFT ) )
				{
					pCamera.Rotate( kEvent->yrel / 3.0f, 0.0f, -kEvent->xrel / 3.0f );
				}
				*/

				if ( kEvent->state & SDL_BUTTON( 1 ) )
				{
					pCamera.Rotate( kEvent->yrel / 3.0f, 0.0f, -kEvent->xrel / 3.0f );
					pCamera.SetForceRotation( true );
				}

				float fAmount = pCamera.GetAngleZ() - -pClient->player_character()->angle();

				if ( uiKeys[SDLK_UP] == SDL_PRESSED && Config::GetPerspective() == 1 )
				{
					int iWalkDirection = WALK_FORWARD;

					if ( fAmount > 215 )
					{
						iWalkDirection = WALK_LEFT;
					}
					else if ( fAmount < 145 )
					{
						iWalkDirection = WALK_RIGHT;
					}

					if ( iWalkDirection == WALK_LEFT || iWalkDirection == WALK_RIGHT)
					{
						Game::GetInstance()->Walk_Simple( iWalkDirection );
					}
				}
			}

			if ( !( kEvent->state & ( SDL_BUTTON( 2 ) | SDL_BUTTON( 3 ) ) ) )
			{
				pCamera.SetForceRotation( false );
			}

			if ( kEvent->state & SDL_BUTTON( 1 ) )
			{
				if ( ( abs( m_iClickDownX - kEvent->x ) > 10 ) && ( abs( m_iClickDownY - kEvent->y ) > 10 ) && !m_bIsDragging )
				{
					Game::GetInstance()->HandleDrag( m_iClickDownX, m_iClickDownY );
					m_bIsDragging = true;
				}
			}
			Game::GetInstance()->HandleMouseMotion( kEvent );
		}
	}
}


void SDLEvent::HandleMacro( MacroEntry *kEntry )
{
	char cIntegers[4][15];

	int iNumInts = 0;
	std::vector<char *> vStrs;

	if ( kEntry->parameters.size() == 4 )
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( kEntry->parameters.at( i )->type == 2 )
			{
				sprintf( cIntegers[i- vStrs.size()], "%d", kEntry->parameters.at( i )->int_value );
				iNumInts++;
			}
			else if ( kEntry->parameters.at( i )->type == 1 )
			{
				vStrs.push_back( (char *)kEntry->parameters.at( i )->str_value.c_str() );
			}
		}

		if ( iNumInts == 4 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1], cIntegers[2], cIntegers[3] );
		}
		else if ( iNumInts == 3 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1], cIntegers[2], vStrs.at( 0 ) );
		}
		else if ( iNumInts == 2 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1], vStrs.at( 0 ), vStrs.at( 1 ) );
		}
		else if ( iNumInts == 1 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], vStrs.at( 0 ), vStrs.at( 1 ), vStrs.at( 2 ) );
		}
		else if ( iNumInts == 0 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), vStrs.at( 0 ), vStrs.at( 1 ), vStrs.at( 2 ), vStrs.at( 3 ) ); 
		}
	}
	else if ( kEntry->parameters.size() == 3 )
	{
		for ( int i = 0; i < 3; i++ )
		{
			if ( kEntry->parameters.at( i )->type == 2 )
			{
				sprintf( cIntegers[i - vStrs.size()], "%d", kEntry->parameters.at( i )->int_value );
				iNumInts++;
			}
			else if ( kEntry->parameters.at( i )->type == 1 )
			{
				vStrs.push_back( (char *)kEntry->parameters.at( i )->str_value.c_str() );
			}
		}

		if ( iNumInts == 3 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1], cIntegers[2] );
		}
		else if ( iNumInts == 2 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1], vStrs.at( 0 ) );
		}
		else if ( iNumInts == 1 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], vStrs.at( 0 ), vStrs.at( 1 ) );
		}
		else if ( iNumInts == 0 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), vStrs.at( 0 ), vStrs.at( 1 ), vStrs.at( 2 ) );
		}
	}
	else if ( kEntry->parameters.size() == 2 )
	{
		for ( int i = 0; i < 2; i++ )
		{
			if ( kEntry->parameters.at( i )->type == 2 )
			{
				sprintf( cIntegers[i - vStrs.size()], "%d", kEntry->parameters.at( i )->int_value );
				iNumInts++;
			}
			else if ( kEntry->parameters.at( i )->type == 1 )
			{
				vStrs.push_back( (char *)kEntry->parameters.at( i )->str_value.c_str() );
			}
		}
        
		if ( iNumInts == 2 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], cIntegers[1] );
		}
		else if ( iNumInts == 1 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0], vStrs.at( 0 ) );
		}
		else if ( iNumInts == 0 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), vStrs.at( 0 ), vStrs.at( 1 ) );
		}
	}
	else if ( kEntry->parameters.size() == 1 )
	{
		if ( kEntry->parameters.at( 0 )->type == 2 )
		{
			sprintf( cIntegers[0], "%d", kEntry->parameters.at( 0 )->int_value );
			iNumInts++;
		}
		else if ( kEntry->parameters.at( 0 )->type == 1 )
		{
			vStrs.push_back( (char *)kEntry->parameters.at( 0 )->str_value.c_str() );
		}

		if ( iNumInts == 1 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), cIntegers[0] );
		}
		else if ( iNumInts == 0 )
		{
			pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str(), vStrs.at( 0 ) );
		}
	}
	else if ( kEntry->parameters.size() == 0 )
	{
		pCSLHandler.ExecuteFunction( (char *)kEntry->script_function.c_str() );
	}

	vStrs.clear();
}
