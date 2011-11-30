//
// File: ClilocLoader.h
// Created by: ArT-iX ArT-iX@libero.it
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

#ifndef _SPEECHLOADER_H_
#define _SPEECHLOADER_H_

#include "Common.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <SDL/SDL_endian.h>

#include "Logger.h"

// #include "../Fluid/mmgr.h"

typedef std::map<std::string, Uint16> speech_language;

class SpeechLoader
{
public:
	SpeechLoader( std::string sPath );
	~SpeechLoader();

	static SpeechLoader *GetInstance();

	Uint16 GetID( std::string keyword );
	std::vector<Uint16> GetIDs( std::string keyword );

private:
	static SpeechLoader *m_sgSpeechLoader;
	std::map<std::string, Uint16> m_keywords;
	std::vector<speech_language> m_languages;
};

#endif // _SPEECHLOADER_H_
