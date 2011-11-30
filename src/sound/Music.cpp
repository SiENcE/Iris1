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

#include "sound/Music.h"




Music::Music() : m_kMusic( NULL ), m_bPlaying( false )
{ 
#ifdef WIN32
	m_kMpeg = NULL;
#endif
}

Music::~Music()
{
	m_bPlaying = false;
	Stop();
#ifdef _WIN32
	if ( m_kMpeg )
	{
		SMPEG_stop( m_kMpeg );
		SMPEG_delete( m_kMpeg );
		Mix_HookMusic(NULL, NULL);
	}
#endif
}


// Function to read music.ini, will finish when xml support is done
int Music::Config()
{
	return Config::GetMusic();
}


int Music::PlayMusic( std::string sMusicName, int iVolume )
{
	std::string sFileName = Config::GetMulPath();

	if ( !iVolume )
	{
		iVolume = Config::GetMusicVolume();
	}

	if ( Config::GetMP3() )
	{
		sFileName += "music/digital/" + sMusicName;

#ifdef WIN32
		if ( m_kMpeg )
		{
			SMPEG_stop( m_kMpeg );
			SMPEG_delete( m_kMpeg );
		//SiENcE
			Mix_HookMusic(NULL, NULL);
		}
		m_kMpeg = SMPEG_new( sFileName.c_str(), &m_kInfo, 0 );

		char *cError = SMPEG_error( m_kMpeg );

		Logger::WriteLine( cError );
		assert( m_kMpeg );
		assert( m_kInfo.has_audio );

		// Tell SMPEG what the audio format is.
		SDL_AudioSpec kAudioFmt;
		int iFrequency, iChannels;
		Uint16 uiFormat;
		Mix_QuerySpec( &iFrequency, &uiFormat, &iChannels );
		kAudioFmt.format = uiFormat;
		kAudioFmt.freq = iFrequency;
		kAudioFmt.channels = iChannels;

		SMPEG_actualSpec( m_kMpeg, &kAudioFmt );

		// Hook in the MPEG music mixer.
		Mix_HookMusic( SMPEG_playAudioSDL, m_kMpeg );
		SMPEG_enableaudio( m_kMpeg, 1 );
		SMPEG_enablevideo( m_kMpeg, 0 );
		SMPEG_setvolume( m_kMpeg, iVolume );
		SMPEG_loop( m_kMpeg, true );

		SMPEG_play( m_kMpeg );
		m_bPlaying = true;
		return 1;
#endif
	}
	else
	{
		sFileName += "music/" + sMusicName;
	}

	while ( !Mix_FadeOutMusic( 100 ) && Mix_PlayingMusic() )
	{
		// wait for any fades to complete
		SDL_Delay( 100 );
	}

	/* Stop the music from playing */
	Mix_HaltMusic();
	
	//Unload the music from memory, since we don't need it anymore
	Mix_FreeMusic( m_kMusic );
	m_kMusic = NULL;
	Logger::WriteLine( sFileName.c_str() );

	if ( m_kMusic == NULL )
	{
		m_kMusic = Mix_LoadMUS( sFileName.c_str() );
	}

	if ( !m_kMusic )
	{
		Logger::WriteLine( "\t| -> PlayMusic() can not load file", __FILE__, __LINE__, LEVEL_WARNING );
		// this might be a critical error...
		Config::SetMusic( 0 );
		return false;
	}
      
	if ( Mix_FadeInMusic( m_kMusic, -1, 3000 ) == -1 )
	{
		printf( "Mix_FadeInMusic: %s\n", Mix_GetError() );
		// well, there's no music, but most games don't break without music...
		Config::SetMusic( 0 );
		return false;
	}

	m_bPlaying = true;

	return true;
}


int Music::PlayMusic( int iId, int iFormat, int iVolume )
{
	std::string sFileName;

	std::string sMP3Path = pMusicListLoader->mp3path();
	std::string sMidiPath = pMusicListLoader->midipath();

	if ( !iVolume )
	{
		iVolume = Config::GetMusicVolume();
	}

	std::string sMusicName = pMusicListLoader->GetMusic( iId, iFormat );
	if ( sMusicName == "" )
	{
		return 0;
	}

	cMusicListEntry *entry = pMusicListLoader->GetMusicListEntry( iId );

	if ( sMusicName.empty() )
	{
		Logger::WriteLine( "\t| -> Wrong musicfile name" );
		return 0;
	}

	if ( iFormat == FORMAT_MP3 )
	{
		sFileName = sMP3Path + sMusicName;

#ifdef WIN32
		if ( m_kMpeg )
		{
			Logger::WriteLine( "PLAYING" );
			SMPEG_stop( m_kMpeg );
			SMPEG_delete( m_kMpeg );
		//SiENcE
			Mix_HookMusic(NULL, NULL);
		}
		m_kMpeg = SMPEG_new( sFileName.c_str(), &m_kInfo, 0 );

		char *cError = SMPEG_error( m_kMpeg );

		if ( cError != NULL )
		{
			Logger::Write( "\t| -> " );
			Logger::WriteLine( cError );

			return 0;
		}

		assert( m_kMpeg );
		assert( m_kInfo.has_audio );

        //SiENcE:
		/* Play the movie, using SDL_mixer for audio */
		SMPEG_enableaudio(m_kMpeg, 0);

		// Tell SMPEG what the audio format is.
		SDL_AudioSpec kAudioFmt;
		int iFrequency, iChannels;
		Uint16 uiFormat;
		Mix_QuerySpec( &iFrequency, &uiFormat, &iChannels );
		kAudioFmt.format = uiFormat;
		kAudioFmt.freq = iFrequency;
		kAudioFmt.channels = iChannels;

		SMPEG_actualSpec( m_kMpeg, &kAudioFmt );

		// Hook in the MPEG music mixer.
		Mix_HookMusic( SMPEG_playAudioSDL, m_kMpeg );
		SMPEG_enableaudio( m_kMpeg, 1 );
		SMPEG_enablevideo( m_kMpeg, 0 );
		SMPEG_setvolume( m_kMpeg, iVolume );
		SMPEG_loop( m_kMpeg, entry->loop );

		SMPEG_play( m_kMpeg );
		m_bPlaying = true;

		return true;
#endif
	}
	else
	{
		sFileName = sMidiPath + sMusicName;
		//Logger::WriteLine(file.c_str());
	}

	while ( !Mix_FadeOutMusic( 100 ) && Mix_PlayingMusic() )
	{
		// wait for any fades to complete
		SDL_Delay( 100 );
	}

	// MIDI doesn't fade nor does it turn MusicPlaying off when it completes 
	if ( Mix_PlayingMusic() && !Config::GetMP3() )
	{
		// force music to end
		Mix_HaltMusic(); 
		Mix_FreeMusic( m_kMusic );
		m_kMusic = NULL;
	}
	Logger::WriteLine( sFileName.c_str() );

	if ( m_kMusic == NULL )
	{
		m_kMusic = Mix_LoadMUS( sFileName.c_str() );
	}

	if ( m_kMusic == NULL )
	{
		Logger::WriteLine( "PlayMusic() can not load file ", __FILE__, __LINE__, LEVEL_WARNING );
		// this might be a critical error...
		Config::SetMusic( 0 );
		
		return false;
	}

	if ( Mix_FadeInMusic( m_kMusic, -1, 3000 ) == -1 )
	{
		printf( "Mix_FadeInMusic: %s\n", Mix_GetError() );
		// well, there's no music, but most games don't break without music...
		Config::SetMusic( 0 );

		return false;
	}

	m_bPlaying = true;

	return true;
}


void Music::MusicVolume( int iVolume )
{
	Mix_VolumeMusic( iVolume );
}


void Music::Pause()
{
#ifdef _WIN32
	SMPEG_pause( m_kMpeg );
#endif
}


void Music::Stop()
{
	if ( m_bPlaying )
	{
		// fade music out for .1 seconds 
		while ( Mix_PlayingMusic() && !Mix_FadeOutMusic( 100 ) )
		{
			// wait for any fades to complete
			SDL_Delay( 100 );
		}

		// MIDI doesn't fade nor does it turn MusicPlaying off when it completes
		if ( Mix_PlayingMusic() && !Config::GetMP3() )
		{
			// force music to end
			Mix_HaltMusic();
			Mix_FreeMusic( m_kMusic );
			m_kMusic = NULL;
		}
#ifdef _WIN32
		SMPEG_stop( m_kMpeg );
#endif
	}
}
