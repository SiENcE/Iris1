//
// File: UOMap.cpp
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#include "loaders/UOMap.h"

MapLoader *pMapLoader = NULL;

UOMapLoader::UOMapLoader( int index )
{
	cMapInfoEntry *mapinfo_entry = MapInfoLoader::GetInstance()->GetMapInfo( index );
	int fileindex = mapinfo_entry->base_id() < 0 ? index : mapinfo_entry->base_id();

	if ( !mapinfo_entry )
	{
          THROWEXCEPTION( "Tried to load unknown map file" );
	}
	m_iWidth = mapinfo_entry->width();
	m_iHeight = mapinfo_entry->height();

	std::string mulpath = Config::GetMulPath();
	char filename[30]; // buffer: 18 filename-chars, 11 digit, 1 \0

	sprintf( filename, "map%i.mul", fileindex );
	m_mapstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_mapstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );
		THROWEXCEPTION( "Could not load map file: " + std::string(filename) );
	}

	sprintf( filename, "statics%i.mul", fileindex );
	m_staticstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_staticstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );
		SAFE_DELETE( m_staticstream );
		THROWEXCEPTION( "Could not load static file: " + std::string( filename ) );
	}

	sprintf( filename, "staidx%i.mul", fileindex );
	m_staidxstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_staidxstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );
		SAFE_DELETE( m_staticstream );
		SAFE_DELETE( m_staidxstream );
		THROWEXCEPTION( "Could not load static index file: " + std::string( filename ) );
	}

	// Initialize Diffs
	m_stadifidxstream = NULL;
	m_staticdifstream = NULL;
	InitMapDif( index );
	InitStaticsDif( index );
}

UOMapLoader::~UOMapLoader()
{
	m_mapstream->close();
	SAFE_DELETE( m_mapstream );

	m_staticstream->close();
	SAFE_DELETE( m_staticstream );

	m_staidxstream->close();
	SAFE_DELETE( m_staidxstream );
}

bool UOMapLoader::InitMapDif( int index )
{	// Iris has no support of diff-cropping packets, so I'll load the whole file and ignore them.
	//////////////////////////////
	std::string mulpath = Config::GetMulPath();
	char filename[30]; // buffer: 18 filename-chars, 11 digit, 1 \0

	// create Lookup
	sprintf( filename, "mapdifl%i.mul", index );
	std::ifstream diflookupstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !diflookupstream.is_open() )
	{
		Logger::WriteLine( "Could not load map diff lookup file: " + std::string( filename ) );
		return false;
	}

	// open data-stream
	sprintf( filename, "mapdif%i.mul", index );
	m_mapdifstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_mapdifstream->is_open() )
	{
		diflookupstream.close();
		SAFE_DELETE( m_mapdifstream );

		Logger::WriteLine( "Could not load map diff file: " + std::string( filename ) );
		return false;
	}

	Uint32 blockid;
	std::ifstream::pos_type position;
	// read lookup data
	while( !diflookupstream.eof() )
	{
		position = diflookupstream.tellg();
		diflookupstream.read( (char*)&blockid, sizeof(Uint32) );
		blockid = IRIS_SwapU32( blockid );

		m_maplookup[blockid] = position / sizeof(Uint32);
	}

	diflookupstream.close();
	return true;
}

bool UOMapLoader::InitStaticsDif( int index )
{
	// Iris has no support of diff-cropping packets, so I'll load the whole file and ignore them.
	//////////////////////////////
	std::string mulpath = Config::GetMulPath();
	char filename[30]; // buffer: 18 filename-chars, 11 digit, 1 \0

	// create Lookup
	sprintf( filename, "stadifl%i.mul", index );
	std::ifstream diflookupstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !diflookupstream.is_open() )
	{
		Logger::WriteLine( "Could not load static diff lookup file: " + std::string( filename ) );
		return false;
	}

	// open index-stream
	sprintf( filename, "stadifi%i.mul", index );
	m_stadifidxstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_staidxstream->is_open() )
	{
		diflookupstream.close();
		SAFE_DELETE( m_stadifidxstream );

		Logger::WriteLine( "Could not load static diff index file: " + std::string( filename ) );
		return false;
	}

	// open data-stream
	sprintf( filename, "stadif%i.mul", index );
	m_staticdifstream = new std::ifstream( (mulpath + filename).c_str(), std::ios::in | std::ios::binary );
	if ( !m_staticdifstream->is_open() )
	{
		diflookupstream.close();
		m_stadifidxstream->close();
		SAFE_DELETE( m_stadifidxstream );
		SAFE_DELETE( m_staticdifstream );

		Logger::WriteLine( "Could not load static diff file: " + std::string( filename ) );
		return false;
	}

	Uint32 blockid;
	std::ifstream::pos_type position;
	// read lookup data
	while( !diflookupstream.eof() )
	{
		position = diflookupstream.tellg();
		diflookupstream.read( (char*)&blockid, sizeof(Uint32) );
		blockid = IRIS_SwapU32( blockid );

		m_staticlookup[blockid] = position / sizeof(Uint32);
	}

	diflookupstream.close();
	return true;
}

void UOMapLoader::LoadMapBlock( int x, int y, MulBlock *block )
{
	if ( ( x >= m_iWidth ) || ( y >= m_iHeight ) || ( x < 0 ) || ( y < 0 ) )
	{
		memset( (char *) block, 0, sizeof(struct MulBlock) );
		return;
	}
	std::ifstream *datastream; // switch between diff and standard

	LookupMap::iterator diffposition = m_maplookup.find( x * m_iHeight + y );
	if( diffposition != m_maplookup.end() ) // block was found!
	{
		m_mapdifstream->seekg( (*diffposition).second * 196 + 4, std::ios::beg );
		datastream = m_mapdifstream;
	}
	else
	{
		m_mapstream->seekg( ( ( x * m_iHeight ) + y ) * 196 + 4, std::ios::beg );
		datastream = m_mapstream;
	}

	datastream->read( (char *) block, 192 );
  
	for ( int j = 0; j < 64; j++ )
	{
		block->cells[j / 8][j % 8].tileid =
		IRIS_SwapU16( block->cells[j / 8][j % 8].tileid );
	}
}

struct staticinfo *UOMapLoader::LoadStatics( int x, int y, int &len )
{
	stIndexRecord block;
	std::ifstream *datastream; // switch between diff and standard
	if ( ( x >= m_iWidth ) || ( y >= m_iHeight ) || ( x < 0 ) || ( y < 0 ) )
	{
		return NULL;
	}

	LookupMap::iterator diffposition = m_staticlookup.find( x * m_iHeight + y );
	if( diffposition != m_staticlookup.end() ) // block was found!
	{
		m_stadifidxstream->seekg( (*diffposition).second * 12, std::ios::beg );
		m_stadifidxstream->read( (char *) &block, 12 );
		datastream = m_staticdifstream;
	}
	else // no diff, read as usual
	{
//		printf( "std found" );
		m_staidxstream->seekg( ( x * m_iHeight + y ) * 12, std::ios::beg );
		m_staidxstream->read( (char *) &block, 12 );
		datastream = m_staticstream;
	}

	block.offset = IRIS_SwapU32( block.offset );
	block.length = IRIS_SwapU32( block.length );
	block.extra = IRIS_SwapU32( block.extra );

	if ( block.length == 0xffffffff )
	{
		return NULL;
	}

	len = block.length / 7;

	struct staticentry *uostatics = (struct staticentry *) malloc (block.length);
	struct staticinfo *static_infos = (struct staticinfo *) malloc (len * sizeof (struct staticinfo));


	datastream->seekg( block.offset, std::ios::beg );
	datastream->read((char *) uostatics, block.length );


	struct staticentry *uo_p = uostatics;
	struct staticinfo *info_p = static_infos;


	for ( int index = 0; index < len; index++ )
	{
		info_p->x = uo_p->x;
		info_p->y = uo_p->y;
		info_p->z = uo_p->z;
		info_p->TileID = IRIS_SwapU16( uo_p->TileID );
		info_p->Hue = IRIS_SwapI16( uo_p->Hue );
		info_p->obj_id = index;
		info_p++;
		uo_p++;
	}
	
	free( uostatics );

	return static_infos;
}
