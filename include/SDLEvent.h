/*! \file SDLEvent.h
 * \brief SDLEvent class handles all SDL inputs.
 * 
 * This class handles all inputs, it parses SDL input commands.
 *
 * Copyright (©) Iris Team
 */

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
 
#ifndef _SDLEVENT_H_
#define _SDLEVENT_H_

#include <cassert>
#include "SDL/SDL.h"
#include "Macros.h"
#include "Logger.h"
#include "Config.h"
#include "Game.h"
#include "renderer/SDLScreen.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "net/Client.h"
#include "gui/GUIHandler.h"
#include "csl/CSLHandler.h"

// #include "../Fluid/mmgr.h"


///\brief SDLEvent class, handles all input.
class SDLEvent
{
public:
	// Constructor
	SDLEvent();
	// Destructor
	~SDLEvent();

	/// Handles all event on queue.
	void PollEvent();
	/// Event wait list.
	void WaitEvent();	
	/// Kills the application.
	static void KillApplication();
	/// Gets status application (if it needs to be closed/forced to be closed).
	static bool GetStatus();

private:
	static bool m_bQuit;
	int m_iLastTick;
	int m_iLastX, m_iLastY, m_iLastButton;
	unsigned int m_uiLastClick;
	int m_iClickDownX, m_iClickDownY;
	bool m_bIsDragging;
	SDL_Event m_kEvent;

private:
	void HandleKeyPress( SDL_keysym *kKeysym );
	void HandleMouseMotion( SDL_MouseMotionEvent *kEvent );
	void HandleMouseDown( SDL_MouseButtonEvent *kEvent );
	void HandleEvent( SDL_Event kEvent, unsigned int uicurrenttime );
	void HandleMovement( void );
	void HandleMacro( MacroEntry *kEntry );
};

#endif //_SDLEVENT_H_
