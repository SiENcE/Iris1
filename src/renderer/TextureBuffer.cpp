//
// File: TextureBuffer.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
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

#include "renderer/TextureBuffer.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/ArtLoader.h"




TextureBuffer *TextureBuffer::m_sgTextureBuffer = NULL;


TextureBuffer::TextureBuffer()
{
	assert( m_sgTextureBuffer == NULL );
	m_sgTextureBuffer = this;

	groundTiles.setMaxSize( 1000 );
	groundTiles.setCacheSize( 0x4000 );

	artTiles.setMaxSize( 500 );
	artTiles.setCacheSize( 0x4000 );
}


TextureBuffer::~TextureBuffer()
{
	assert( m_sgTextureBuffer != NULL );
	m_sgTextureBuffer = NULL;

	artTiles.Clear();
	groundTiles.Clear();
}


TextureBuffer *TextureBuffer::GetInstance()
{
	return m_sgTextureBuffer;
}


Texture *TextureBuffer::GetGroundTexture( int index )
{
	if ( ( index < 0 ) || ( index >= 0x4000 ) )
	{
		return NULL;
	}

	Texture * result;
	
	result = StaticModelLoader::GetInstance()->GetGroundTexture( index );
	if ( result )
	{
		return result;
	}
 
	result = groundTiles.findEntry( index );

	if ( !result )
	{
		result = pGroundTextureLoader.LoadTexture( index );

		if ( !result )
		{
			result = ArtLoader::GetInstance()->LoadArt( index );
		}

		if (!result)
		{
			result = new Texture();
		}
		groundTiles.addEntry( index, result );
	}

	return result;
}


Texture *TextureBuffer::GetArtTexture( int index )
{
	if ( ( index < 0x4000 ) && ( index >= 0 ) )
	{
		return GetGroundTexture( index );
	}

	if ( ( index < 0x0 ) || ( index >= 65536 ) )
	{
		return NULL;
	}

	Texture *result = NULL;

	result = artTiles.findEntry( index - 0x4000 );

	if ( !result )
	{
		result = ArtLoader::GetInstance()->LoadArt( index, false );
        
		if ( !result )
		{
			result = new Texture();
		}
		artTiles.addEntry( index - 0x4000, result );
	}

	return result;
}
