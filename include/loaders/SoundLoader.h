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

#ifndef _SOUNDLOADER_H_
#define _SOUNDLOADER_H_

#include <fstream>
#include <string>
#include <vector>
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_types.h"

// #include "../Fluid/mmgr.h"

/*
 * To find data for a certain entry, you seek to (EntryID * 12), and read the data.
 * Then, in the actual MUL file (not the index file), you seek to (lookup), and read (length) number of bytes.
 * Some data relies on the extra field.
 */

struct stSoundIndx
{
	int lookup;
	int length;
	int extra;
};

class SoundLoader
{
public:
	SoundLoader();
	virtual ~SoundLoader();

	int SoundInit(std::string mulPath);
	Mix_Chunk *Load(int soundindx);

private:
	std::ifstream IndexFile;
	std::ifstream SoundFile;
	std::vector<stSoundIndx> Soundindx;
	int indx;
};

#endif	// _SOUNDLOADER_H_
