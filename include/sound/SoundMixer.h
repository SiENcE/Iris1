/*! \file SoundMixer.h
 * \brief Sound main class.
 * 
 * Handles all kinds of Sound/Music.
 *
 * Copyright (©) Iris Team
 */

/*
 * Last change: 05-03-06 (Nuno Ramiro)
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

#ifndef _SOUNDMIX_H_
#define _SOUNDMIX_H_

#include <iostream>
#include <string>
#include <assert.h>
#include "Logger.h"
#include "Config.h"
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "Music.h"
#include "loaders/SoundLoader.h"
#include "net/Client.h"
#include "renderer/Characters.h"
#include "math.h"

// #include "../Fluid/mmgr.h"

class SoundMix : public Music, public SoundLoader
{
public:
	SoundMix();
	~SoundMix();

	static SoundMix *GetInstance();

	void Init();
	int Quit();
	void PlaySound( int sound, int volume, char flags, int x, int y, int z );
	void PlaySoundWithVolume( int sound, int volume );

private:
	Mix_Chunk *wave;
	static SoundMix *m_sgSoundMix;
};

//extern SoundMix * pSoundMix;

#endif
