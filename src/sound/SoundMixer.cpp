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

#include "sound/SoundMixer.h"





SoundMix *SoundMix::m_sgSoundMix = NULL;


SoundMix::SoundMix() : wave( NULL )
{
	assert( m_sgSoundMix == NULL );
	m_sgSoundMix = this;
}


SoundMix::~SoundMix()
{
	assert( m_sgSoundMix != NULL );
	m_sgSoundMix = NULL;
	Mix_CloseAudio();
	SDL_QuitSubSystem( SDL_INIT_AUDIO );
	m_sgSoundMix = NULL;
}


SoundMix *SoundMix::GetInstance()
{
	return m_sgSoundMix;
}


void SoundMix::Init()
{
	int stereo_channels = MIX_DEFAULT_CHANNELS;

	if ( Config::GetStereo() == 1 )
	{
		stereo_channels = 2;
	}
	else
	{
		stereo_channels = 1;
	}
  
	// start SDL with audio support
	if ( SDL_InitSubSystem (SDL_INIT_AUDIO) != 0 )
	{
		Logger::WriteLine( "Initializing SDL Audio failed...", __FILE__, __LINE__, LEVEL_ERROR );
		Config::SetMusic( false );
		Config::SetSound( false );
	}
	else if ( Mix_OpenAudio( Config::GetFrequency(), MIX_DEFAULT_FORMAT, stereo_channels, Config::GetChunkSize() ) < 0 )
	{
		Logger::WriteLine( "Open SDL Audio failed...", __FILE__, __LINE__, LEVEL_ERROR );
		Config::SetMusic( false );
		Config::SetSound( false );
	}

	std::cout << "Starting sound system...\n";
	if ( Config::GetMusic() )
	{
		std::cout << "->Enabling music support....\n";
		MusicVolume( Config::GetMusicVolume() );
		//Mix_VolumeMusic( Config::GetMusicVolume() );
	}
	else
	{
		std::cout << "->Music support disabled....\n";
	}

	if ( Config::GetSound() )
	{
		if ( SoundInit( Config::GetMulPath() ) )
		{
			// allocate mixing channels
			std::cout << "->Enabling sound support....\n";
		}
		else
		{
			std::cout << "->Sound support disabled....\n";
       		Config::SetSound( 0 );
		}
	}
}


void SoundMix::PlaySound( int sound, int volume, char flags, int x, int y, int z )
{

	if (Config::GetSound()!=1)
	{
		return;
	}

	wave = Load( sound );

	if ( !wave )
	{
		return;
	}

	cCharacter *mychar = pClient->player_character();
	assert( mychar );

	int my_x = mychar->x();
	int my_y = mychar->y();

	int xdist = my_x - x;         // Calculate distance
	int ydist = my_y - y;
	float dist = sqrt( (float)(xdist * xdist + ydist * ydist) );

	if ( wave )
	{
		int volume = (int)( (1.0f - (dist * 0.05f) ) * Config::GetSoundVolume() );  // Set Volume
        //if volume is smaler than min 5 set it to 5
		if ( volume <= 5 )
		{
			volume = 5;
		}
		//if volume is greater than max=128 set it to 128
		if (volume > 128)
		{
            volume = 128;
        }
		Mix_VolumeChunk( wave, volume );

		if ( Mix_PlayChannel( -1, wave, 0 ) == -1 )
		{
			printf( "Mix_PlayChannel: %s\n", Mix_GetError() );
			// may be critical error, or maybe just no channels were free.
			// you could allocated another channel in that case...
			Mix_FreeChunk( wave );
			wave = NULL;
		}
	}
}


void SoundMix::PlaySoundWithVolume( int sound, int volume )
{

	if (Config::GetSound()!=1)
	{
		return;
	}

	wave = Load( sound );

	if ( !wave )
	{
		return;
	}

	if ( wave )
	{
		Mix_VolumeChunk( wave, volume );
		if ( Mix_PlayChannel( -1, wave, 0 ) == -1 )
		{
			printf( "Mix_PlayChannel: %s\n", Mix_GetError() );
			// may be critical error, or maybe just no channels were free.
			// you could allocated another channel in that case...
			Mix_FreeChunk( wave );
			wave = NULL;
		}
	}
}


int SoundMix::Quit()
{
	Mix_CloseAudio();
	return true;
}
