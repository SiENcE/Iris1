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

#include "FontManager.h"




FontManager *FontManager::m_sgFontManager;


FontManager::FontManager()
{
	assert( m_sgFontManager == NULL );
	m_sgFontManager = this;

	if ( TTF_Init() < 0 )
	{
		std::cerr << "TTF Initialisation failed: " << SDL_GetError () << std::endl;
	}
}


FontManager::~FontManager()
{
	assert( m_sgFontManager != NULL );
	m_sgFontManager = NULL;

	// TTF Font cleanup
	for ( unsigned int i = 0; i < m_vFonts.size(); i++ )
	{
		SAFE_DELETE( m_vFonts[i] );
	}

	// Mul Font cleanup
	std::map<unsigned short, stFont>::iterator it;
	for ( it = m_mFonts.begin(); it != m_mFonts.end(); ++it )
	{
		for ( unsigned int i = 0; i < 224; ++i )
		{
			SAFE_DELETE_ARRAY( it->second.chars[i].pixels );
			SAFE_DELETE_ARRAY( it->second.chars[i].redmask );
		}
	}
	m_mFonts.clear();

	TTF_Quit();
}


FontManager *FontManager::GetInstance()
{
	return m_sgFontManager;
}


void FontManager::AddTTFFont( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue )
{
	UnregisterTTFFont( iId );
	m_vFonts.push_back( new Font( iId, sFileName, sFontName, iSize, iHue ) );
}


void FontManager::AddMulFont( std::string sFileName )
{
	std::ifstream FontStream;
	
	FontStream.open( sFileName.c_str(), std::ios::in | std::ios::binary );

	if ( !FontStream.is_open() )
	{
		THROWEXCEPTION( "Couldn't open " + sFileName );
	}

	// Header
	unsigned char header;

	stFont kFont;

	// Index
	unsigned short Index = 0;

	// Read as many fonts as we can
	while ( !FontStream.eof() )
	{
		FontStream.get( reinterpret_cast<char &>( header ) );

		// The only valid header in found is an one
		if ( header != 1 )
		{
			break;
		}

		// Read in the 224 characters
		kFont.maxHeight = 0;

		for ( int i = 0; i < 224; ++i )
		{
			// Width + Height + Header
			FontStream.get( reinterpret_cast<char &>( kFont.chars[i].width ) );
			FontStream.get( reinterpret_cast<char &>( kFont.chars[i].height ) );
			FontStream.get( reinterpret_cast<char &>( kFont.chars[i].header ) );

			if ( kFont.chars[i].height > kFont.maxHeight )
			{
				kFont.maxHeight = kFont.chars[i].height;
			}

			// Real Character Data
			kFont.chars[i].pixels = new unsigned int[kFont.chars[i].width * kFont.chars[i].height];
			kFont.chars[i].redmask = new unsigned char[kFont.chars[i].width * kFont.chars[i].height];

			unsigned short *pixels = new unsigned short[kFont.chars[i].width * kFont.chars[i].height];
			FontStream.read( reinterpret_cast<char *>( pixels ), 
				kFont.chars[i].width * kFont.chars[i].height * 2 );

			for ( int j = 0; j < kFont.chars[i].width * kFont.chars[i].height; ++j )
			{
				if ( IRIS_SwapU16( pixels[j] ) == 0 )
				{
					kFont.chars[i].pixels[j] = 0;
					kFont.chars[i].redmask[j] = 0;
				}
				else
				{
					kFont.chars[i].pixels[j] = color15to32( IRIS_SwapU16( pixels[j] ) );
					kFont.chars[i].redmask[j] = ( IRIS_SwapU16( pixels[j] ) >> 10 ) & 0x1F;
				}
			}

			SAFE_DELETE_ARRAY( pixels );
		}

		m_mFonts.insert( std::make_pair( Index++, kFont ) );
	}

	FontStream.close();
}


void FontManager::UnregisterTTFFont( int iId )
{
	std::vector<Font *>::iterator it = m_vFonts.begin();
	int i = 0;
	for ( it = m_vFonts.begin(); it != m_vFonts.end(); i++, it++ )
	{
		if ( m_vFonts[i]->GetId() == iId )
		{
			TTF_CloseFont( m_vFonts[i]->GetFont() );
			m_vFonts.erase( it );
		}
	}
}


TTF_Font *FontManager::GetTTFFont( int iId )
{
	for ( unsigned int i = 0; i < m_vFonts.size(); i++ )
	{
		if ( m_vFonts[i]->GetId() == iId )
		{
			return m_vFonts[i]->GetFont();
		}
	}

	return NULL;
}


const stFont *FontManager::GetMulFont( unsigned short iId ) const
{
	std::map<unsigned short, stFont>::const_iterator it;
	it = m_mFonts.find( iId );

	if ( it == m_mFonts.end() )
	{
		return NULL;
	}

	return &(it->second);
}
