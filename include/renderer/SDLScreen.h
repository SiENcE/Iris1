/*! \file SDLScreen.h
 * \brief SDLScreen class handles all video stuff.
 * 
 * This class handles all SDL Video operations.
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Gustav Nylander.
 * Last change: 23-02-06 (Nuno Ramiro)
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

#ifndef _SDLSCREEN_H_
#define _SDLSCREEN_H_

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "Config.h"
#include "irisgl.h"
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "loaders/HueLoader.h"

// #include "../Fluid/mmgr.h"

class SDLScreen
{
public:
	SDLScreen();
	~SDLScreen();

	static SDLScreen *GetInstance();

	int ResizeWindow( int width, int height );
	void ToggleFullScreen();
	void ClearScreen();
	void ClearZBuffer();
	int DrawGL( GLvoid );
	int ScreenSave();
	void SetPerspective( void );
	float GetRatio( void );

	void SetAlpha( Uint8 alpha, bool force = false);
	void SetHue( Uint16 hue = 0 );
  
	Uint16 GetDefaultHue( Uint32 id );

//	void SetLight( float factor );
	void SetFogColor (Uint8 r, Uint8 g, Uint8 b);

public:
	SDL_Surface *m_kScreen;
	int videoFlags;

private:
	int Init( int width, int height, int bpp );
	int InitGL( GLvoid );
	void DisplayFps();

private:
	static SDLScreen *m_sgSDLScreen;
	const SDL_VideoInfo *videoInfo;
	GLfloat ratio;
	std::map <Uint32, TTF_Font *> fonts;
	std::map <Uint32, Uint16> default_hues;
	Uint8 act_alpha;
	float light_factor;
};

#endif //_SDLSCREEN_H_
