//
// File: StaticModelLoader.cpp
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

#include "loaders/StaticModelLoader.h"




StaticModelLoader *StaticModelLoader::m_sgStaticModelLoader = NULL;

char FILEID_GAMEMODELCOLLECTION[] = "UI3D";


bool CheckHeaderID( char *ID1, char *ID2 )
{
	for ( int index = 0; index < 4; index++ )
	{
		if ( ID1[index] != ID2[index] )
		{
			return false;
		}
	}
	
	return true;
}


StaticModelLoader::StaticModelLoader( std::string filename ) : modelstream( NULL ), modelstream_size( 0 )
{
	assert( m_sgStaticModelLoader == NULL );
	m_sgStaticModelLoader = this;
	modelstream = new std::ifstream( filename.c_str (), std::ios::in | std::ios::binary );

	if ( !modelstream->is_open() )
	{
		THROWEXCEPTION( std::string( "model file not found: " ) + filename );
	}

	sModelCollectionHeader header;

	modelstream->seekg( 0, std::ios::end );
	modelstream_size = modelstream->tellg();

	if ( modelstream_size < sizeof(header) )
	{
		THROWEXCEPTION( "invalid model size (<headersize)" );
	}

	modelstream->seekg( 0, std::ios::beg );
	modelstream->read( (char *)&header, sizeof(header) );
	header.Length = IRIS_SwapU32( header.Length );
	header.Version = IRIS_SwapU32( header.Version );
	header.ModelTableStart = IRIS_SwapU32( header.ModelTableStart );
	header.ModelTableCount = IRIS_SwapU32( header.ModelTableCount );
	header.TextureStart = IRIS_SwapU32( header.TextureStart );
	header.TextureLength = IRIS_SwapU32( header.TextureLength );

	for ( int ii = 0; ii < 9; ii++ )
	{
		header.Reserved[ii] = IRIS_SwapU32( header.Reserved[ii] );
	}

	/* Check some header information */
	if ( !CheckHeaderID( header.Sign, FILEID_GAMEMODELCOLLECTION ) )
	{
		THROWEXCEPTION( "invalid model file" );
	}
	if ( header.Version != GAMEMODEL_VERSION )
	{
		THROWEXCEPTION( "model file has an invalid version!" );
	}
	if ( modelstream_size != header.Length )
	{
		THROWEXCEPTION( "invalid model file size" );
	}

	if ( ( modelstream_size < header.ModelTableStart ) || ( modelstream_size < ( header.ModelTableStart + 
		header.ModelTableCount * sizeof(sModelTableEntry) ) ) || (modelstream_size < header.TextureStart) 
		|| ( modelstream_size < ( header.TextureStart + header.TextureLength ) ) )
	{
		THROWEXCEPTION( "invalid model file" );
	}

	modelstream->seekg( header.TextureStart, std::ios::beg );

	static_texture_loader = new cStaticTextureLoader( modelstream, header.TextureLength );

	modelstream->seekg( header.ModelTableStart, std::ios::beg );
	sModelTableEntry entry;

	for ( unsigned int index = 0; index < header.ModelTableCount; index++ )
	{
		modelstream->read( (char *)&entry, sizeof(sModelTableEntry) );
		entry.id = IRIS_SwapU32( entry.id );
		entry.start = IRIS_SwapU32( entry.start);
		entry.length = IRIS_SwapU32( entry.length );
		model_entries.insert( std::make_pair( entry.id, entry ) );
	}

	std::map<Uint32, sModelTableEntry>::iterator iter;
	for ( iter = model_entries.begin(); iter != model_entries.end(); iter++ )
	{
		modelstream->seekg( iter->second.start, std::ios::beg );

		cStaticModel *model = new cStaticModel( modelstream, iter->second.length, static_texture_loader );
		models.insert( std::make_pair( iter->second.id, model ) );
	}
}


StaticModelLoader::~StaticModelLoader()
{
	assert( m_sgStaticModelLoader != NULL );
	m_sgStaticModelLoader = NULL;

	std::map<Uint32, cStaticModel *>::iterator iter;
	for ( iter = models.begin(); iter != models.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	models.clear();

	SAFE_DELETE( static_texture_loader );

	SAFE_DELETE( modelstream );
}


StaticModelLoader *StaticModelLoader::GetInstance()
{
	return m_sgStaticModelLoader;
}


cStaticModel *StaticModelLoader::getModel( Uint32 id )
{
	ASSERT( modelstream );
  
	std::map<Uint32, cStaticModel *>::iterator iter;
	iter = models.find( id );

	if ( iter != models.end() )
	{
		return iter->second;
	}

	return NULL;
}


Texture *StaticModelLoader::GetGroundTexture( Uint32 id ) // Get replaced ground texture
{
	return static_texture_loader->GetGroundTexture( id );
}
