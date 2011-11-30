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

#include "Game.h"
#include "renderer/3D/Renderer3D.h"




// Singleton
Game *Game::m_sgGame = NULL;


Game::Game()
{
	assert( m_sgGame == NULL );
	m_sgGame = this;

	m_MouseLastTick = SDL_GetTicks();
	m_AOSToolTip = false;

	// Default values
	m_kRenderer = NULL;
	m_kMapBuffer3D = NULL;
	m_kArtLoader = NULL;
	m_paused = true;

	cursor3d[0] = 0;
	cursor3d[1] = 0;
	cursor3d[2] = 0;

	cursorx = 0;
	cursory = 0;
	button_left = false;
	button_right = false;

	cursor_character = 0;

	drag_id = 0;
	drag_model = 0;

	timer_func = "";
    timer = 0;

	m_click_mode = CLICK_NORMAL;

	callback_OnStatusDrag = NULL;
	callback_OnDynamicDrag = NULL;
	callback_OnAOSTooltip = NULL;

	pMapLoader = NULL;
	pGrannyLoader = NULL;
	pMultisLoader = NULL;
	pTextManager = NULL;
	m_kMapBuffer3D = NULL;
	m_kMapInfoLoader = NULL;
	m_kStaticModelLoader = NULL;
	m_kModelInfoLoader = NULL;
	m_kTextureBuffer = NULL;
}


Game::~Game()
{
	assert( m_sgGame != NULL );
	m_sgGame = NULL;

	Logger::WriteLine( "SYS | Deinitializing Iris...." );

	DeInitRenderer();

	Logger::WriteLine( "\t| -> Particle Engine" );
	pParticleEngine.Reset();
	pParticleLoader.DeInit();

	pCamera.Reset();

	Logger::WriteLine( "\t| -> Dynamic Objects" );
	pDynamicObjectList.Clear();
	Logger::WriteLine( "\t| -> Character List" );
	pCharacterList.Clear();

	Logger::WriteLine( "\t| -> Renderer" );
	SAFE_DELETE( m_kRenderer );
	Logger::WriteLine( "\t| -> Texture Manager" );
	SAFE_DELETE( pTextManager );

	if ( pClient )
	{
		Disconnect();
	}
}


Game *Game::GetInstance()
{
	return m_sgGame;
}


bool Game::IsPaused() const
{
	return m_paused;
}


/* Callback handlers */
void OnAddDynamic( cDynamicObject * object )
{
	Game::GetInstance()->AddDynamic( object );
}


void OnDeleteDynamic( cDynamicObject * object )
{
	Game::GetInstance()->DelDynamic( object );
}


void OnAddCharacter( cCharacter * character )
{
	Game::GetInstance()->AddCharacter( character );
}


void OnDeleteCharacter( cCharacter * character )
{
	Game::GetInstance()->DelCharacter( character );
}


bool Game::Init( void )
{
	//Profiler::Init();
	//Profiler::Begin("Game");

	Logger::WriteLine( "SYS | Initializing Iris...." );

	InitRenderer( Config::GetMulPath() );

	pCamera.Reset();
	pTextManager = new cTextManager();

	CreateSinTable();

	pDynamicObjectList.Clear();
	pDynamicObjectList.OnAdd( OnAddDynamic );
	pDynamicObjectList.OnDelete( OnDeleteDynamic );

	pCharacterList.Clear();
	pCharacterList.OnAdd( OnAddCharacter );
	pCharacterList.OnDelete( OnDeleteCharacter );

	Logger::WriteLine( "\t| -> loading particles" );
	pParticleLoader.Init( "xml/particles.xml" );

	Logger::WriteLine( "\t| -> renderer" );
	m_kRenderer = new Renderer3D();

	if ( !m_kRenderer->Init() )
	{
		return false;
	}

	SetPosition( Config::GetStartX(), Config::GetStartY(), Config::GetStartZ() );
	SetPause( true );

	m_click_mode = CLICK_NORMAL;

	//Profiler::End();
	//Profiler::Output();

	return true;
}


void Game::InitRenderer( std::string sMulPath )
{
	std::string sMulTexmaps = sMulPath + "texmaps.mul";
	std::string sMulTexidx = sMulPath + "texidx.mul";

	//Profiler::Begin( "Map Info" );
	Logger::WriteLine( "\t| -> mapinfo" );
	m_kMapInfoLoader = new MapInfoLoader( "xml/Maps.xml" );
	//Profiler::End();

	//Profiler::Begin( "Map" );
	Logger::WriteLine( "\t| -> map" );
	//Load Standard Map0.mul
	pMapLoader = new UOMapLoader(0);
	//Profiler::End();

	//Profiler::Begin( "Ground Textures" );
	Logger::WriteLine( "\t| -> ground textures" );
	pGroundTextureLoader.Init( sMulTexmaps, sMulTexidx );
	//Profiler::End();

	//Profiler::Begin( "Arts" );
	Logger::WriteLine( "\t| -> arts" );
	m_kArtLoader = new ArtLoader( sMulPath + "art.mul", sMulPath + "artidx.mul" );
	//Profiler::End();

	//Profiler::Begin( "Fonts" );
	Logger::WriteLine( "\t| -> fonts" );
	FontManager::GetInstance()->AddMulFont( sMulPath + "fonts.mul" );
	//Profiler::End();

	//Profiler::Begin( "Hues" );
	Logger::WriteLine( "\t| -> hues" );
	pHueLoader.Init( sMulPath + "hues.mul" );
	//Profiler::End();

	//Profiler::Begin( "TileData" );
	Logger::WriteLine( "\t| -> tiledata" );
	pTileDataLoader.Init( sMulPath + "tiledata.mul" );
	//Profiler::End();

	//Profiler::Begin( "Gumps" );
	Logger::WriteLine( "\t| -> gumps" );
	pGumpLoader.Init( sMulPath + "gumpart.mul", sMulPath + "gumpidx.mul" );
	//Profiler::End();

	//Profiler::Begin( "Skills" );
	Logger::WriteLine( "\t| -> skills" );
	pSkillLoader.Init( sMulPath + "skills.mul", sMulPath + "Skills.idx" );
	//Profiler::End();

	// Note that it should only be initialized if we are using an old expansion ( < AOS ).
	if ( !Config::GetAOS() )
	{
		//Profiler::Begin( "Verdata" );
		Logger::WriteLine( "\t| -> verdata" );
		pVerdataLoader.Init( sMulPath + "verdata.mul" );
		//Profiler::End();
	}

	//Profiler::Begin( "Map Buffer" );
	Logger::WriteLine( "\t| -> map buffer" );
	m_kMapBuffer3D = new Mapbuffer3D();
	pMapbufferHandler.Init( m_kMapBuffer3D );
	//Profiler::End();

	//Profiler::Begin("Tiledata Buffer");
	Logger::WriteLine( "\t| -> tiledata buffer" );
	pTileDataBuffer.Clear();
	//Profiler::End();

	// Needs tweaking (It's killing performance)
	//Profiler::Begin( "3D Character Models" );
	Logger::WriteLine ("\t| -> 3D character models");
	if ( Config::GetAOS() )
	{
		pGrannyLoader = new cGrannyLoader( "xml/granny_aos.xml", sMulPath );
	}
	else
	{
		pGrannyLoader = new cGrannyLoader( "xml/granny.xml", sMulPath );
	}
	//Profiler::End();

	// Needs tweaking (It's killing performance)
	//Profiler::Begin( "3D Static Models" );
	Logger::WriteLine( "\t| -> 3D static models" );
	m_kStaticModelLoader = new StaticModelLoader( "./data/models.uim" );
	//Profiler::End();

	if ( Config::GetAOS() )
	{
		//Profiler::Begin( "Stitchin Loader" );
		Logger::WriteLine( "\t| -> stitchinloader" );
		pStitchinLoader.Init( sMulPath + "Models/models.txt", sMulPath + "stitchin.def" );
		//Profiler::End();
	}

	if ( Config::GetClilocs() )
	{
		//Profiler::Begin( "Clilocs" );
		Logger::WriteLine( "\t| -> clilocs" );
		if ( !pClilocLoader.Init( sMulPath ) )
		{
			Logger::WriteLine ("\t| -> Clilocs loader Error");
		}
		//Profiler::End();
    }

	//Profiler::Begin( "Texture Buffer" );
	Logger::WriteLine( "\t| -> Texture Buffer" );
	m_kTextureBuffer = new TextureBuffer();
	//Profiler::End();


	//Profiler::Begin( "Model infos" );
	Logger::WriteLine( "\t| -> model infos" );
	m_kModelInfoLoader = new ModelInfoLoader( "xml/ModelsInfo.xml" );
	//Profiler::End();

	//Profiler::Begin( "Multis" );
	Logger::WriteLine( "\t| -> multis" );
	pMultisLoader = new cMultisLoader( sMulPath + "multi.mul", sMulPath + "multi.idx" );
	//Profiler::End();

	//Profiler::Begin( "Speech" );
    //Artix: speechloader loaded only if config option for speech is enabled
    if(Config::GetUseSpeech())
    {
        Logger::WriteLine( "\t| -> speech" );
        m_kSpeechLoader = new SpeechLoader( sMulPath );
        //Logger::WriteDebug( "\t| -> Bank word: " + m_kSpeechLoader->GetID( "*bank*" ) );
    }
	//Profiler::End();

	//Profiler::Begin( "Macros" );
    Logger::WriteLine( "\t| -> macros" );
    pMacroLoader = new MacroLoader();
	//Profiler::End();
}


void Game::DeInitRenderer( void )
{
	Logger::WriteLine( "\t| -> mapinfo" );
	SAFE_DELETE( m_kMapInfoLoader );

	Logger::WriteLine( "\t| -> map" );
	SAFE_DELETE( pMapLoader );

	Logger::WriteLine( "\t| -> ground textures" );
	pGroundTextureLoader.DeInit();

	Logger::WriteLine( "\t| -> Art Loader" );
	SAFE_DELETE( m_kArtLoader );

	Logger::WriteLine( "\t| -> hues" );
	pHueLoader.DeInit();

	Logger::WriteLine( "\t| -> tiledata" );
	pTileDataLoader.DeInit();

	Logger::WriteLine( "\t| -> gumps" );
	pGumpLoader.DeInit();

	Logger::WriteLine( "\t| -> skills" );
	pSkillLoader.DeInit();

	if ( Config::GetAOS() )
	{
		Logger::WriteLine( "\t| -> verdata" );
		pVerdataLoader.DeInit();
	}

	Logger::WriteLine( "\t| -> map buffer" );
	pMapbufferHandler.DeInit();
	//SAFE_DELETE( m_kMapBuffer3D );

	Logger::WriteLine( "\t| -> tiledata buffer" );
	pTileDataBuffer.Clear();

	Logger::WriteLine ("\t| -> 3D character models");
	SAFE_DELETE( pGrannyLoader );

	Logger::WriteLine( "\t| -> 3D static models" );
	SAFE_DELETE( m_kStaticModelLoader );

	Logger::WriteLine( "\t| -> macros" );
	SAFE_DELETE( pMacroLoader );

	Logger::WriteLine( "\t| -> Texture Buffer" );
	SAFE_DELETE( m_kTextureBuffer );

	pLightManager.Clear();


	if ( Config::GetAOS() )
	{
		Logger::WriteLine( "\t| -> stitchinloader" );
		pStitchinLoader.DeInit();
	}

	if ( Config::GetClilocs() )
	{
		Logger::WriteLine( "\t| -> clilocs" );
		pClilocLoader.DeInit();
	}

	Logger::WriteLine( "\t| -> model infos" );
	SAFE_DELETE( m_kModelInfoLoader );

	Logger::WriteLine( "\t| -> multis" );
	SAFE_DELETE( pMultisLoader );

    //Artix: speechloader loaded only if config option is enabled
    if(Config::GetUseSpeech())
    {
        Logger::WriteLine( "\t| -> Speech" );
	    SAFE_DELETE( m_kSpeechLoader );
    }
}


void Game::RenderScene( void )
{
	if ( !m_kRenderer )
	{
		return;
	}

	if ( m_paused )
	{
		SDLScreen::GetInstance()->ClearScreen();
		pUOGUI.Draw();
		SDLScreen::GetInstance()->DrawGL();

		return;
	}

	//if (pCamera)
	if ( drag_id /*pCamera->CamHasChanged_DontReset() ) */ )
	{
		//      GrabMousePosition(cursorx, cursory);
		if ( drag_id )
        {
			if ( drag_in_world )
			{
				m_kRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
			}
			else
			{
				m_kRenderer->setDragModel( 0 );
			}
		}
	}

	m_kRenderer->RenderScene();
}


void Game::Handle( void )
{
	static int counter = 0;
	int new_tick = SDL_GetTicks();

    if ( timer && !timer_func.empty() )
    {
		if ( new_tick >= timer )
		{
			pCSLHandler.ExecuteFunction( (char*)timer_func.c_str() );
			timer = 0;
			timer_func == "";
		}
    }

	float time = ( new_tick - counter ) / 1000.0;

	if ( time > 1.0 )
	{
		time = 1.0;
	}

	if ( time <= 0.0f )
	{
		time = 0.001f;
	}

	counter = new_tick;

	pUOGUI.HandleMessageQueues();

	pCharacterList.Handle( time );

	if ( !m_paused )
	{
		pParticleEngine.Handle();
		pLightManager.Handle( new_tick );

		if ( m_kRenderer )
		{
			pMapbufferHandler.buffer()->FreeBuffer( m_kRenderer->GetViewDistance() + 2 );
		}
	}

	if ( button_right )
	{
		MoveToMouse();
	}
}


Renderer *Game::GetRenderer( void )
{
	return m_kRenderer;
}


void Game::OnKeyPress( SDL_keysym *key )
{
}


void Game::HandleMouseMotion( SDL_MouseMotionEvent * event )
{
	cursorx = event->x;
	cursory = event->y;

	if ( !Config::GetMouseMotionTimer() )
	{
		return;
	}

	if ( SDL_GetTicks() - (unsigned int)m_MouseLastTick > (unsigned int)Config::GetMouseMotionTimer() )
	{
        	int artid;
		GrabMousePosition( cursorx, cursory, artid );

//		GrabMousePosition( cursorx, cursory );

		cDynamicObject *object;
		cCharacter *character;
		GrabDynamic( cursorx, cursory, &object, &character );

		if ( character )
		{
			pointed_obj = character->id ();
		}
		else if ( object )
		{
			pointed_obj = object->id;
		}
        else
		{
			if( Config::GetIsUox3() )
			{
				pointed_obj = (dword)-1;
			}
			else
			{
				pointed_obj = 0;
			}
		}

        if ( Config::GetAOSToolTips() )
		{
			if ( callback_OnAOSTooltip )
			{
				if ( ( !m_AOSToolTip ) && ( character || object ) )
				{
					if ( character )
					{
						int count = character->aostooltips_count();
						callback_OnAOSTooltip( character->id (), count, cursorx +10, cursory +10);
					}

					if ( object )
					{
						int count = object->aostooltips_count();
						callback_OnAOSTooltip( object->id, count, cursorx +10, cursory +10);
					}

					m_AOSToolTip = true;
				}
				else
				{
					if ( m_AOSToolTip )
					{
						callback_OnAOSTooltip( 0, 0, 0, 0 );
						m_AOSToolTip = false;
					}
				}
			}
		}

		m_MouseLastTick = SDL_GetTicks();
	}
}


void Game::HandleClick( int x, int y, unsigned int buttonstate, bool double_click )
{
/*	float vecP[3];
	vecP[0] = -2.0f;
	vecP[1] = -2.0f;
	vecP[2] = -1.0f;
	int tx, ty;
	pCamera->GetRenderCoords(vecP, tx, ty); */

	int artid;

	if ( ( buttonstate == SDL_BUTTON_LEFT ) && pClient )
	{
		cDynamicObject *object;
		cCharacter *character;

		switch ( click_mode() )
		{
		case CLICK_NORMAL:
			GrabDynamic( x, y, &object, &character );
			if ( object )
			{
				if ( double_click )
				{
					pClient->Send_DoubleClick( object->id );
				}
				else
				{
					pClient->Send_Click( object->id );

					if ( Config::GetPopup() == 1 )
					{
						pClient->Send_PopupRequest( object->id, x, y );
					}
				}
			}
			else if ( character )
			{
				if ( double_click )
				{
					if ( pClient->warmode() )
					{
						pClient->Send_AttackRequest( character->id() );
					}
					else
					{
						pClient->Send_DoubleClick( character->id() );
					}
				}
				else
				{
					pClient->Send_Click( character->id() );

					if ( Config::GetPopup() == 1 )
					{
						pClient->Send_PopupRequest( character->id(), x, y );
					}
				}
			}
			break;

		case CLICK_TARGET_ID:
			GrabDynamic( x, y, &object, &character );

			if ( object )
			{
                pClient->Send_Target ( (Uint32)cursorid(), (Uint32)object->id );
			}

			if ( character )
			{
				pClient->Send_Target( (Uint32)cursorid(), (Uint32)character->id() );
			}

			m_click_mode = CLICK_NORMAL;
			break;

		case CLICK_TARGET_XYZ:
			GrabMousePosition( x, y, artid);
//			printf ("%i %i %.2f\n", x, y, artid);
			pClient->Send_Target( (Uint32)cursorid(), cursor3d[0], cursor3d[1], cursor3d[2], artid );

//			GrabMousePosition( x, y );
//			pClient->Send_Target( (Uint32)cursorid(), cursor3d[0], cursor3d[1], cursor3d[2] );
			m_click_mode = CLICK_NORMAL;
			break;
		}
	}
}


void Game::ItemClick2D( Uint32 id, bool double_click )
{
	switch ( click_mode() )
	{
	case CLICK_NORMAL:
		if ( double_click )
		{
			pClient->Send_DoubleClick( id );
		}
        else
		{
			pClient->Send_Click( id );
		}
        break;

	case CLICK_TARGET_ID:
		pClient->Send_Target( (Uint32)cursorid(), (Uint32)id );
		m_click_mode = CLICK_NORMAL;
		break;
	}
}


void Game::SetPosition( int x, int y, int z )
{
	pCamera.SetX( (float) -( x % 8 ) );
	pCamera.SetY( (float) -( y % 8 ) );
	pCamera.SetZ( (float) -z );

	pCamera.SetBlockX( x / 8 );
	pCamera.SetBlockY( y / 8 );
}


void game_OnGameStart( void )
{
	Game::GetInstance()->SetPause( false );
}


void game_OnTeleport( int x, int y, int z )
{
	Game::GetInstance()->SetPosition( x, y, z );
}


void game_OnDragCancel( void )
{
	Game::GetInstance()->DragCancel();
}


void game_OnDrag( unsigned int id, unsigned int model )
{
	Game::GetInstance()->Drag( id, model );
}


void game_OnItemClick( unsigned int id, bool double_click )
{
	Game::GetInstance()->ItemClick2D( id, double_click );
}


void game_OnTarget( unsigned int cursorid, unsigned int type )
{
	if ( type == TARGET_ID )
	{
		// printf("Targetting Entity...\n");
		Game::GetInstance()->click_mode( CLICK_TARGET_ID );
	}
	else
	{
		// printf("Targetting Position...\n");
		Game::GetInstance()->click_mode( CLICK_TARGET_XYZ );
	}

	Game::GetInstance()->cursorid( cursorid );
}


void Game::Connect( void (*error_callback)(unsigned int error) )
{
	// This should be replaced by a kClient = new Client( .. ); if ( !kClient->Connect() ) Disconnect();
	try
	{
		pClient = new cClient( error_callback );
	}
	catch ( ... )
	{
		Disconnect();
	}
	pClient->OnGameStart( game_OnGameStart );
	pClient->OnTeleport( game_OnTeleport );
	pClient->OnDragCancel( game_OnDragCancel );
	pClient->OnTarget( game_OnTarget );

	pUOGUI.OnDrag( game_OnDrag );
	pUOGUI.OnItemClick( game_OnItemClick );
}


void Game::Disconnect()
{
	Logger::WriteLine( "\tNET | Disconnecting..." );
	SAFE_DELETE( pClient );
}


void Game::Walk( Uint8 direction )
{
	if ( pClient )
	{
		pClient->Walk( direction );
	}
}


void Game::Walk_Simple( Uint8 action )
{
	if ( pClient )
	{
		pClient->Walk_Simple( action );
	}
}


void Game::GrabMousePosition( int x, int y, int & artid, int max_z)
{
	if ( m_paused || !m_kRenderer )
	{
		return;
	}

	m_kRenderer->GrabMousePosition( x, y, max_z, cursor3d, artid );

	cursor_character = 0;
}


void Game::GrabDynamic( int x, int y, cDynamicObject **r_object, cCharacter **r_character )
{
	if ( r_object )
	{
		*r_object = NULL;
	}

	if ( r_character )
	{
		*r_character = NULL;
	}

	if ( m_paused || !m_kRenderer )
	{
		return;
	}

	m_kRenderer->GrabDynamic( x, y, r_object, r_character );
}


void Game::HandleMouseDown( int x, int y, int button )
{
	if ( paused() )
	{
		return;
	}

	int artid;

	cursorx = x;
	cursory = y;
	GrabMousePosition( cursorx, cursory, artid );

	if ( button == 1 )
	{
		button_left = true;
	}

	if ( ( button == 2 ) || ( button == 3 ) )
	{
		button_right = true;
	}
}


void Game::HandleMouseUp( int x, int y, int button )
{
	if ( paused() )
	{
		return;
	}

	int artid;

	cursorx = x;
	cursory = y;
	GrabMousePosition( cursorx, cursory, artid );

	if ( button == 1 )
	{
		button_left = false;
	}

	if ( ( button == 2 ) || ( button == 3 ) )
	{
		button_right = false;
	}
}


void Game::MoveToMouse()
{
	if ( !pClient )
	{
		return;
	}

	if ( SDL_GetModState() & ( KMOD_LSHIFT | KMOD_RSHIFT ) )
	{
		return;
	}

	if ( SDL_GetModState() & ( KMOD_LALT | KMOD_RALT ) )
	{
		return;
	}

	int run_flag = 0;

	int px, py, pz;
	pClient->GetPlayerPosition( px, py, pz );

	if ( ( cursorx > Config::GetWidth() * 0.80f ) || ( cursorx < Config::GetWidth() * 0.20f )
      || ( cursory > Config::GetHeight() * 0.80f )
      || ( cursory < Config::GetHeight() * 0.20f ) )
	{
		run_flag = 0x80;
	}

	int artid;
	GrabMousePosition( cursorx, cursory, artid, pz + 20 );
//	printf ("%i %i %i\n", cursor3d[0] - px, cursor3d[1] - py, cursor3d[2]);

//	GrabMousePosition( cursorx, cursory, pz + 20 );

	int dx = cursor3d[0] - px;
	int dy = cursor3d[1] - py;
	float angle = 0.0f;

	if ( dx )
	{
		float m = (float) dy / (float) dx;
		angle = atan (m) / 3.14159f * 180.0f;

		if ( dx < 0 )
		{
			angle = 180.0f + angle;
		}

		if ( angle < 0.0f )
		{
			angle += 360.0f;
		}
	}
	else if ( dy > 0 )
	{
		angle = 90.0f;
	}
	else if ( dy < 0 )
	{
		angle = 270.0f;
	}

	angle += 22.5f;

	int dir = (int)( ( angle + 90.0f ) / 45.0f );

	if ( dir > 7 )
	{
		dir -= 8;
	}

	if ( Config::GetPerspective() == 0 && !pCamera.forceRotation() )
	{
		pClient->Walk (dir | run_flag);
	}
}


void Game::HandleDrag( int x, int y )
{
	if ( m_paused || !pClient )
	{
		return;
	}

	if ( drag_id )
	{
		return;
	}

	cDynamicObject *object;
	cCharacter *character;
	GrabDynamic( x, y, &object, &character );

	if ( object )
	{
		drag_id = object->id;
		drag_model = object->model;
		SetDragInWorld( true );
		m_kRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
		pClient->Send_PickupRequest( object->id );
	}

	if ( character && callback_OnStatusDrag )
	{
		callback_OnStatusDrag( character->id (), cursorx, cursory );
	}

	pUOGUI.SetDragging( true );
}


void Game::DragCancel()
{
	drag_id = 0;
    pUOGUI.LoadDragCursor( 0 );

	if ( m_kRenderer )
	{
		m_kRenderer->setDragModel( 0 );
	}

	pUOGUI.SetDragging( false );
}


void Game::Drag( Uint32 id, Uint16 model )
{
	if ( m_paused || !pClient )
	{
		return;
	}

	if ( drag_id )
	{
		return;
	}

	drag_id = id;

	drag_model = model;
	SetDragInWorld( false );
	cDynamicObject *obj = pDynamicObjectList.Get( id );

	if ( obj->itemcount > 1 )
	{
		Uint32 cont = obj->parent;

		if ( callback_OnDynamicDrag )
		{
			callback_OnDynamicDrag( id, model, obj->itemcount, obj->x, obj->y, cont );

			return;
		}
	}

	m_kRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
	pClient->Send_PickupRequest( id );
	pUOGUI.SetDragging( true );
}


void Game::OnStatusDrag( void (*callback) ( Uint32 charid, int mousex, int mousey ) )
{
	callback_OnStatusDrag = callback;
}


void Game::OnDynamicDrag( void (*callback) ( Uint32 id, Uint16 model, int count, int x, int y, Uint32 container ) )
{
	callback_OnDynamicDrag = callback;
}


void Game::OnAOSTooltip( void (*callback) ( Uint32 id, int count, int x, int y ) )
{
	callback_OnAOSTooltip = callback;
}


void Game::UpdateDragMode( int mousex, int mousey )
{
	if ( drag_id )
	{
		int artid;
		GrabMousePosition (mousex, mousey, artid);

//		GrabMousePosition (mousex, mousey);
		Uint32 containerid = 0;
		bool value = !pUOGUI.FindDragContainer( mousex, mousey, &containerid );

		value = value || ( !containerid );  /* if gui element is not a container -> put to world */

        if ( value )
		{
			value = value && ( !cursor_character ); // if abough character, show 2D cursor
		}

		SetDragInWorld( value );

	}
	else
	{
		SetDragInWorld( true );
	}
}


void Game::SetDragInWorld( bool value )
{
	if ( value != drag_in_world )
	{
		if ( value )            // True = Drag in World = Don't show any 2D Cursor
		{
			pUOGUI.LoadDragCursor( 0 );
		}
		else
		{
            pUOGUI.LoadDragCursor( drag_model );
		}
	}

	drag_in_world = value;
}


bool Game::CheckDragDrop( int mousex, int mousey )
{
	if ( !drag_id )
	{
		return false;
	}

	Uint32 containerid = 0;
	Uint32 charid = 0;
	int drop_x = 0, drop_y = 0;

	assert( pClient );

	if ( pUOGUI.FindDragContainer( mousex, mousey, &containerid, &drop_x, &drop_y, &charid ) )
	{
		if ( charid )
		{
			pClient->Send_ItemEquipReq( drag_id, charid, drag_model );
		}
        else if ( containerid )
		{
			pUOGUI.AdjustDropPosition( drop_x, drop_y );
			pClient->Send_DropRequest( drag_id, drop_x, drop_y, 0, containerid );
		}
	}
	else
	{
		if ( cursor_character )
		{
			pClient->Send_DropRequest( drag_id, 0, 0, 0, cursor_character );
		}
		else
		{
			pClient->Send_DropRequest( drag_id, cursor3d[0], cursor3d[1], cursor3d[2] );
		}
	}

	pUOGUI.SetDragging( false );

	pUOGUI.LoadDragCursor( 0 );

	if ( m_kRenderer )
	{
		m_kRenderer->setDragModel( 0 );
	}

	drag_id = 0;
	drag_model = 0;

	return true;
}


void Game::AddDynamic( cDynamicObject *object )
{
	if ( m_kRenderer )
	{
		m_kRenderer->AddDynamic( object );
	}
}


void Game::DelDynamic( cDynamicObject *object )
{
	if ( m_kRenderer )
	{
		m_kRenderer->DelDynamic( object );
	}
}


void Game::AddCharacter( cCharacter *character )
{
	if ( m_kRenderer )
	{
		m_kRenderer->AddCharacter( character );
	}
}


void Game::DelCharacter( cCharacter *character )
{
	if ( m_kRenderer )
	{
		m_kRenderer->DelCharacter( character );
	}
}


void Game::SendPickup( int id, int model, int count )
{
	drag_id = id;
	drag_model = model;
	m_kRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
	pClient->Send_PickupRequest( id, count );
	pUOGUI.SetDragging( true );
}


void Game::DrawAOSTooltip( int id, int count, int x, int y )
{
	if ( callback_OnAOSTooltip )
	{
		callback_OnAOSTooltip( id, count, cursorx, cursory );
	}
}


void Game::SetTimerFunction( std::string function_name, int time )
{
	timer_func = function_name;
	timer = SDL_GetTicks() + time;
}
