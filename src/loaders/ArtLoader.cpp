//
// File: ArtLoader.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
//

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


#include "loaders/ArtLoader.h"




ArtLoader *ArtLoader::m_sgArtLoader = NULL;

ArtLoader::ArtLoader( std::string sFileName, std::string sIndexName ) : m_kArtFile( NULL ), m_kArtIndex( NULL ), m_uiArtCount( 0 )
{
	assert( m_sgArtLoader == NULL );
	m_sgArtLoader = this;
	std::string sError;

	m_kArtFile = new std::ifstream( sFileName.c_str (), std::ios::in | std::ios::binary );
	m_kArtIndex = new std::ifstream( sIndexName.c_str (), std::ios::in | std::ios::binary );

	sError = "Could not load art file: ";
	if ( !m_kArtFile->is_open () || !m_kArtIndex->is_open() )
	{
		sError += sFileName;
		Logger::WriteLine( (char *) sError.c_str(), __FILE__, __LINE__, LEVEL_ERROR );
		SAFE_DELETE( m_kArtFile );
		SAFE_DELETE( m_kArtIndex );
		THROWEXCEPTION( sError );
	}

	ASSERT( m_kArtIndex );

	m_kArtIndex->seekg( 0, std::ios::end );
	unsigned long idxE = m_kArtIndex->tellg();
	m_uiArtCount = idxE / 12;
}


ArtLoader::~ArtLoader()
{
	m_sgArtLoader = NULL;

	m_kArtFile->close();
	SAFE_DELETE( m_kArtFile );
	m_kArtIndex->close();
	SAFE_DELETE( m_kArtIndex );
}


/// Note that it MUST be created first (by using constructor which is being done under Game).
ArtLoader *ArtLoader::GetInstance()
{
	return m_sgArtLoader;
}


/// Note that when using this functions _YOU_ are responsible for deleting.
Texture *ArtLoader::LoadArt( int index, bool is2D, bool isCursor, Uint16 hue )
{
	ASSERT( m_kArtFile );
	ASSERT( m_kArtIndex );

	if ( ( index < 0 ) || ( (unsigned int) index >= m_uiArtCount ) )
	{
		return NULL;
	}

	if ( index < 0x4000 )
	{
		return LoadGroundArt( index );
	}

	return LoadStaticArt( index, is2D, isCursor );
}


Texture *ArtLoader::LoadGroundArt( int index )
{
	if ( ( index < 0 ) || ( (unsigned int)index >= 0x4000 ) )
	{
		return NULL;
	}

	if ( !m_kArtFile )
	{
		THROWEXCEPTION ("NULL artfile pointer");
	}
	if ( !m_kArtIndex )
	{
		THROWEXCEPTION ("NULL artindex pointer");
	}

	struct sPatchResult patch = pVerdataLoader.FindPatch( VERDATAPATCH_ART, index );

	struct stIndexRecord idx;

	if ( patch.file )
	{
		idx = patch.index;
	}
	else
	{
		if ( index >= (int)m_uiArtCount )
		{
			return NULL;
		}

		patch.file = m_kArtFile;
		m_kArtIndex->seekg( index * 12, std::ios::beg );
		m_kArtIndex->read( (char *)&idx, sizeof(struct stIndexRecord) );
		idx.offset = IRIS_SwapU32( idx.offset );
		idx.length = IRIS_SwapU32( idx.length );
		idx.extra = IRIS_SwapU32( idx.extra );
	}

	if (idx.offset == 0xffffffff)
	{
		return NULL;
	}

	Uint16 *imagecolors = new Uint16[1024];

	patch.file->seekg( idx.offset, std::ios::beg );
	patch.file->read( (char *) imagecolors, 1024 * 2 );

	Uint32 *data = new Uint32[44 * 44];
	Uint32 *rdata = new Uint32[44 * 44];
	memset (data, 0, 44 * 44 * 4);

	Uint16 *actcol = imagecolors;

	int x = 22;
	int y = 0;
	int linewidth = 2;
	int i, j;

	for ( i = 0; i < 22; i++ )
	{
		x--;
		Uint32 *p = data + x + y * 44;
		for ( j = 0; j < linewidth; j++ )
		{
			*p = color15to32 (IRIS_SwapU16 (*actcol)) | 0xff000000;
			p++;
			actcol++;
		}
		y++;
		linewidth += 2;
	}

	linewidth = 44;
	for ( i = 0; i < 22; i++ )
	{
		Uint32 *p = data + x + y * 44;
		for ( j = 0; j < linewidth; j++ )
		{
			*p = color15to32 (IRIS_SwapU16 (*actcol)) | 0xff000000;
			p++;
			actcol++;
		}
		x++;
		y++;
		linewidth -= 2;
	}

	Uint32 col;
	Uint32 *buf = data;
	Uint32 *res = rdata;
	int pos, lw;

	for ( y = 1; y <= 22; y++ )
	{
		for ( x = 0; x < y * 2; x++ )
		{
			lw = y * 2 - x - 1;

			pos = (y - 1) * 44 + 22 - y + x;
			col = *(buf + pos);
			// NECESSARY ?
			col = IRIS_SwapU32( col );

			*(res + lw * 44 + x) = col;

			if ( lw > 0 )
			{
                *(res + (lw - 1) * 44 + x) = col;
			}
		}
	}

	for ( y = 43; y >= 22; y-- )
	{
		for ( x = 0; x < (44 - y) * 2; x++ )
		{
			lw = 42 - (43 - y) * 2 + x;

			pos = y * 44 + y - 22 + x;
			col = *(buf + pos);
			*(res + (43 - x) * 44 + lw) = col;

			if ( x > 0 )
			{
                *(res + (44 - x) * 44 + lw) = col;
			}
		}
	}

	Texture *kTexture = new Texture();
	kTexture->LoadFromData( rdata, 44, 44, 32, GL_LINEAR );

	SAFE_DELETE_ARRAY( data );
	SAFE_DELETE_ARRAY( rdata );
	SAFE_DELETE_ARRAY( imagecolors );
	m_vTextures.push_back( kTexture );

	return kTexture;
}


Texture *ArtLoader::LoadStaticArt( int index, bool is2D, bool isCursor )
{
	if ( ( index < 0x4000 ) || ( (unsigned int)index >= m_uiArtCount ) )
	{
		return NULL;
	}

	if (!m_kArtFile)
	{
		THROWEXCEPTION ("NULL artfile pointer");
	}
	if (!m_kArtIndex)
	{
		THROWEXCEPTION ("NULL artindex pointer");
	}

	struct sPatchResult patch = pVerdataLoader.FindPatch( VERDATAPATCH_ART, index );

	struct stIndexRecord idx;

	if ( patch.file )
	{
		idx = patch.index;
	}
	else
	{
		patch.file = m_kArtFile;
		m_kArtIndex->seekg( index * 12, std::ios::beg );
		m_kArtIndex->read( (char *)&idx, sizeof(struct stIndexRecord) );
		idx.offset = IRIS_SwapU32( idx.offset );
		idx.length = IRIS_SwapU32( idx.length );
		idx.extra = IRIS_SwapU32( idx.extra );
	}

	if ( idx.offset == 0xffffffff )
	{
		return NULL;
	}

	int header;
	Uint16 width, height;

	patch.file->seekg( idx.offset, std::ios::beg );
	patch.file->read( (char *)&header, 4 );
	patch.file->read( (char *)&width, 2 );
	patch.file->read( (char *)&height, 2 );
	header = IRIS_SwapU32( header );
	width = IRIS_SwapU16( width );
	height = IRIS_SwapU16( height );

	int w = width;
	int h = height;

	if ( is2D )
	{
		w = 64;
		h = 64;
		while ( w < width )
		{
			w *= 2;
		}
		while ( h < height )
		{
			h *= 2;
		}
	}

	Uint16 *lookuptable = new Uint16[height];
	Uint32 *data = new Uint32[w * h];
	memset( data, 0, w * h * 4 );

	patch.file->read( (char *)lookuptable, height * 2 );

	for ( int y = 0; y < height; y++ )
	{
		Uint32 *p = data + (h - 1 - y) * w;
		Uint16 xOffset = 0, xRun = 0;
		patch.file->seekg( idx.offset + (IRIS_SwapU16( lookuptable[y] ) + height + 4) * 2, std::ios::beg );
        patch.file->read( (char *)&xOffset, 2 );
        patch.file->read( (char *)&xRun, 2);
		xOffset = IRIS_SwapU16( xOffset );
		xRun = IRIS_SwapU16( xRun );

		while ( ( xOffset != 0 ) || ( xRun != 0 ) )
		{
			p += xOffset;
			for ( int i = 0; i < xRun; i++ )
			{
				Uint16 color;
				patch.file->read( (char *)&color, 2 );
				*p = color15to32( IRIS_SwapU16( color ) );
				p++;
			}
			patch.file->read( (char *)&xOffset, 2 );
			patch.file->read( (char *)&xRun, 2 );
			xOffset = IRIS_SwapU16( xOffset );
			xRun = IRIS_SwapU16( xRun );
		}
	}

	if ( isCursor )
	{                         // Remove ugly border
		Uint32 *p = data;
		int i;
		for ( i = 0; i < h; i++ )
		{
			*p = 0;
			*(p + width - 1) = 0;
			p += w;
		}
		p = data + w * (h - height);
		Uint32 *q = data + w * (h - 1);
		for ( i = 0; i < width; i++ )
		{
			*p = 0;
			p++;
			*q = 0;
			q++;
		}
	}

	Texture *kTexture = new Texture();
	kTexture->LoadFromData (data, w, h, 32, GL_LINEAR, is2D);
	kTexture->SetRealSize (width, height);

	SAFE_DELETE_ARRAY( lookuptable );
	SAFE_DELETE_ARRAY( data );
	m_vTextures.push_back( kTexture );

	return kTexture;
}
