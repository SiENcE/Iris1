//
// File: SpeechLoader.cpp
// Created by: ArT-iX ArT-iX@libero.it
// 12.06.2005 modified by SiENcE
//            -added support for all Cliloc Strings for non-english Languages 

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

#include "loaders/SpeechLoader.h"




SpeechLoader *SpeechLoader::m_sgSpeechLoader = NULL;

SpeechLoader::SpeechLoader( std::string sPath )
{
	assert( m_sgSpeechLoader == NULL );
	m_sgSpeechLoader = this;

	std::ifstream ifSpeechLoader;

	const std::string sFileName = sPath + "speech.mul";
	Logger::WriteDebug( "\t| -> Speech file: " + sFileName );

	ifSpeechLoader.open( sFileName.c_str(), std::ios::in | std::ios::binary );

	if ( !ifSpeechLoader.is_open() )
	{
		Logger::WriteLine( "\t| -> Warning: Couldn't open Speech file" );
		ifSpeechLoader.close();

		return;
	}

	ifSpeechLoader.seekg( 0, std::ios::end );
	const int filelen = ifSpeechLoader.tellg();
	ifSpeechLoader.seekg( 0, std::ios::beg );

	Uint16 index;
	Uint16 keywordlen;
	char *c_keyword;
	std::string s_keyword = "";

	 while ( ifSpeechLoader.tellg() < filelen )
	//while ( !ifSpeechLoader.eof() )
	{
		ifSpeechLoader.read( (char *)&index, 2 );
		index = SDL_Swap16( index );

		if ( index != 0 )
		{
			m_languages.push_back( m_keywords );
			m_keywords.clear();
		}

		ifSpeechLoader.read( (char *)&keywordlen, 2 );
		keywordlen = SDL_Swap16( keywordlen );
		c_keyword = new char[keywordlen + 1];
		ifSpeechLoader.read( (char *)c_keyword, keywordlen );
		c_keyword[keywordlen] = 0;
		s_keyword = std::string( c_keyword );
		SAFE_DELETE_ARRAY( c_keyword );

		//std::cout << "ID: " << index << " WORD: " << s_keyword << "   len: " << keywordlen << std::endl;
		//Logger::WriteLine( s_keyword.c_str() );

		m_keywords.insert( std::make_pair( s_keyword, index ) );
	}

	ifSpeechLoader.close();
}


SpeechLoader::~SpeechLoader()
{
	assert( m_sgSpeechLoader != NULL );
	m_sgSpeechLoader = NULL;

	m_keywords.clear();
	m_languages.clear();
}

SpeechLoader *SpeechLoader::GetInstance()
{
	return m_sgSpeechLoader;
}


Uint16 SpeechLoader::GetID( std::string keyword )
{
	for ( unsigned int i = 0; i < m_languages.size(); i++ )
	{
		speech_language act_lang = m_languages.at( i );       
		std::map<std::string, Uint16>::iterator iter;
		iter = act_lang.find( keyword );

		if ( iter != act_lang.end() )
		{
			return iter->second;
		}
	}

	return 0xFFFF;
}


std::vector<Uint16> SpeechLoader::GetIDs( std::string keyword )
{
	std::vector<Uint16> result;

	for ( unsigned int i = 0; i < m_languages.size(); i++ )
	{
		speech_language act_lang = m_languages.at( i );       
		std::map<std::string, Uint16>::iterator iter;
		iter = act_lang.find( keyword );
		
		if ( iter != act_lang.end() )
		{
			result.push_back( iter->second );
		}
	}
	if ( result.size() > 0 )
	{
		return result;
	}
	
	return result;
}
