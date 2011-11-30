//
// File: ModelInfoLoader.h
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

#ifndef _MUSICINFO_LOADER_
#define _MUSICINFO_LOADER_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <string>
#include <map>

// #include "../Fluid/mmgr.h"

#define FORMAT_MIDI 0
#define FORMAT_MP3  1
#define FORMAT_OGG  2

struct cMusicListEntry
{
 int id;
 std::string mp3;
 std::string midi;
 std::string ogg;
 int loop;
};

class cMusicListLoader
{
 private:
     std::map<int, cMusicListEntry*> music_list;
     std::string m_mp3path;
     std::string m_midipath;
 public:
  cMusicListLoader();
  ~cMusicListLoader();     
  cMusicListEntry * GetMusicListEntry(int id);
  std::string GetMusic(int id, int format);
  std::string mp3path(){ return m_mp3path; }
  std::string midipath(){ return m_midipath; }
};

extern cMusicListLoader * pMusicListLoader;

#endif
