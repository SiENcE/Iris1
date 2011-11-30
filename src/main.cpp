/*! \file Main.cpp
 * \brief Main - Program entry point
 *
 * This is where it all starts, by initializing the engine and making everything work.
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Alexander Oster.
 * Last change: 19-02-06 (Nuno Ramiro)
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

#include "Common.h"
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Logger.h"
#include "Exception.h"
#include "Game.h"
#include "net/Client.h"
#include "Config.h"
#include "csl/CSLHandler.h"
#include "sound/SoundMixer.h"
#include "sound/MusicListLoader.h"
#include "FontManager.h"

// #include "../Fluid/mmgr.h"


#if defined( _WIN32 ) && !defined( _DEBUG )
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif


/// Program Entry point
int main( int argc, char **args )
{
/*
	Engine *eng = new Engine();
	SAFE_DELETE( eng );
*/

	// Initializes Logger
	if ( !Logger::Init( Config::GetVersion() ) )
	{
		std::cout << "WARNING: Could not create Log file." << std::endl;
	}

	// Initialize Game
	Game *kGame = new Game();

	SDLScreen *SDLscreen;

	try
	{
		// Initialize the Font System
		FontManager *kFontManager = new FontManager();

		// Initialize SDL
		if ( SDL_Init( 0 ) == -1 )
		{
			Logger::WriteLine( "| -> Could not initialize SDL: " + std::string( SDL_GetError() ) );
		}

		// Initializes SDLNet
		if ( SDLNet_Init() == -1 )
		{
			Logger::WriteLine( "| -> Could not initialize SDLNet: " + std::string( SDL_GetError() ) );
		}

		// Try to load config file
		if ( !Config::Init() )
		{
			Logger::WriteLine( "\t| ->Unable to load configuration file - Using defaults ",
				__FILE__, __LINE__, LEVEL_WARNING );
		}

		/*
		 * FIXME: do this after all heavy loading
		 */
		// Initialize Video(SDL)
		SDLscreen = new SDLScreen();
		// Initialize Input(SDL) Event
		SDLEvent *SDLevent = new SDLEvent();

		// Initialize SDL_music
		SoundMix *pSoundMix;
		if ( Config::GetMusic() || Config::GetSound() )
        {
			pSoundMix = new SoundMix();
			pSoundMix->Init();

			pMusicListLoader = new cMusicListLoader();

			if ( Config::GetMusic() )
			{
				pSoundMix->PlayMusic( 1, Config::GetMP3(), Config::GetMusicVolume() );
			}
		}
		else
		{
			pSoundMix = NULL;
		}

		// Initialize Scripting
		pCSLHandler.Init();
		pCSLHandler.InitAPI();

		// Configure scripts path
		std::string sScriptPath = Config::GetScriptPath();

		if ( sScriptPath == "root" )
		{
			sScriptPath = "script/";
		}
		else
		{
			sScriptPath = "script/" + sScriptPath;
		}

		Logger::Write( "DBG | Script path: " );
		Logger::WriteLine( sScriptPath.c_str() );
		sScriptPath += "iris.csl";
		pCSLHandler.Load( (char *)sScriptPath.c_str() );

		// Init game engine
		if ( !kGame->Init() )
		{
			Logger::WriteLine( "\t| -> Error while trying to initialize Game." );
		}

		// Start main script
		pCSLHandler.ExecuteFunction( "main" );

		// Main loop
		while ( !SDLevent->GetStatus() )
		{
			// Handle events in the queue
			SDLevent->PollEvent();
			kGame->Handle();
			kGame->RenderScene();

			if ( pClient )
			{
				pClient->Poll();
			}
		}

		// Clean up
		pCSLHandler.DeInit();
		pUOGUI.DeInit();

		SAFE_DELETE( kFontManager );
		SAFE_DELETE( SDLevent );
		SAFE_DELETE( pSoundMix );
	}
	catch ( cException kException )
	{
		Logger::WriteLine( kException.debug_message() );
	}
	catch ( std::string sException )
	{
		Logger::WriteLine( sException );
	}
	catch ( ... )
	{
		Logger::WriteLine( "Unhandled exception" );
	}

	SAFE_DELETE( SDLscreen );
	SAFE_DELETE( pMusicListLoader );

	SAFE_DELETE( kGame );

	Config::Close();
	Logger::Close();

	SDLNet_Quit();

	SDL_Quit();

	return 0;
}
