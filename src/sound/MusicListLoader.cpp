//
// File: ModelInfoLoader.cpp
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


#include "sound/MusicListLoader.h"

#include "Config.h"
#include "Logger.h"
#include <iostream>
#include "xml.h"
#include <cassert>
#include "iris_endian.h"




cMusicListLoader *pMusicListLoader = NULL;

cMusicListLoader::cMusicListLoader()
{
	XML::Parser parser;
	XML::Node *musics, *document;

	std::string file = Config::GetMulPath();

	m_mp3path = file + "music/digital/";
	m_midipath = file + "music/";

	try
	{
		parser.loadData( "./xml/MusicList.xml" );
		document = parser.parseDocument();

		musics = document->findNode( "MUSICLIST" );

		if ( !musics )
		{
			throw "Couldn't find musiclist node.";
		}
	}
	catch ( ... )
	{
		Logger::WriteLine( "Couldn't load MusicList" );
		return;
	}

	XML::Node *music_node, *value;

	if ( ( value = musics->findNode( "MP3PATH" ) ) )
	{
		m_mp3path = value->asString();
	}
	
	if ( ( value = musics->findNode( "MIDIPATH" ) ) )
	{
		m_midipath = value->asString();
	}

	int idx = 0;
	int loop;
	
	while ( ( music_node = musics->findNode( "MUSIC", idx ) ) )
	{
		std::string mp3 = "";
		std::string midi = "";
		std::string ogg = "";
		loop = 0;

		value = music_node->findNode( "ID" );
		Uint32 id = ( value != NULL ) ? value->asInteger() : 0;

		if ( ( value = music_node->findNode( "MP3" ) ) )
		{
			mp3 = value->asString();
		}
        if ( ( value = music_node->findNode( "MIDI" ) ) )
		{
			midi = value->asString();
		}
        if ( ( value = music_node->findNode( "OGG" ) ) )
		{
			ogg = value->asString();
		}
        if ( ( value = music_node->findNode( "LOOP" ) ) )
		{
			loop = value->asInteger();
		}

		cMusicListEntry *musicinfo = new cMusicListEntry();
		musicinfo->mp3 = mp3;
		musicinfo->midi = midi;
		musicinfo->ogg = ogg;
		musicinfo->loop = IRIS_SwapI32(loop);

		music_list.insert( std::make_pair( (int)id, musicinfo ) );
		idx++;
	}
}


cMusicListLoader::~cMusicListLoader()
{
	std::map<int, cMusicListEntry*>::iterator iter;
	for ( iter = music_list.begin(); iter != music_list.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	
	music_list.clear();
}

cMusicListEntry *cMusicListLoader::GetMusicListEntry (int id)
{

  std::map < int, cMusicListEntry * >::iterator iter;
  iter = music_list.find (id);

  if (iter != music_list.end ())
    return iter->second;
  else
    return NULL;
}

std::string cMusicListLoader::GetMusic (int id, int format)
{

  cMusicListEntry *entry = GetMusicListEntry (id);
  if (!entry)
    return "";

  switch (format)
      {
      case FORMAT_MIDI:
        return entry->midi;
      case FORMAT_MP3:
        return entry->mp3;
      case FORMAT_OGG:
        return entry->ogg;
      default:
        return "";
      }
}
