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

#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <iostream>
#include <assert.h>
#include <string>
#include "SDL/SDL_mixer.h"
//#include "SDL/SDL.h"
#include "sound/MusicListLoader.h"
#include "Logger.h"
#include "Config.h"

#ifdef WIN32
#include "smpeg/smpeg.h"
#endif

// #include "../Fluid/mmgr.h"

class Music
{
public:
	Music();
	virtual ~Music();

	int Config();
	// will change later to play song that server tells us
	int PlayMusic( std::string sMusicName, int iVolume = 0 );
	int PlayMusic( int iId, int iFormat, int iVolume = 0 );
	void MusicVolume( int iVolume );	// * deprecated *
	void Pause();
	void Stop();

private:
	Mix_Music *m_kMusic;
	bool m_bPlaying;

#ifdef WIN32
	SMPEG *m_kMpeg;
	SMPEG_Info m_kInfo;
#endif
};

#endif	//_MUSIC_H_
