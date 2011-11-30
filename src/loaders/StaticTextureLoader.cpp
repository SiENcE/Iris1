//
// File: StaticTextureLoader.cpp
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

#include "loaders/StaticTextureLoader.h"




char FILEID_GAMEMODELTEXTURE[] = "UI3F";

struct sTextureFileHeader
{
  char Sign[4];
  Uint32 Length;
  Uint32 Version;
  Uint32 TextureTableStart;
  Uint32 TextureTableCount;
  Uint32 GroundTableStart;
  Uint32 GroundTableCount;
  Uint32 Reserved[9];
} STRUCT_PACKED;

struct sTextureTableEntry
{
  Uint32 start;
  Uint32 length;
} STRUCT_PACKED;

struct sTextureFileEntry
{
  char Name[32];
  char md5sum[32];
  Uint32 Width;
  Uint32 Height;
  Uint32 Length;
  Uint32 ColorKey;
  Uint32 Flags;
  Uint32 Reserved[3];
} STRUCT_PACKED;

struct sGroundTextureTableEntry
{
  Uint32 id;
  char md5sum[32];
} STRUCT_PACKED;

extern bool CheckHeaderID (char *ID1, char *ID2);




cStaticTexture::cStaticTexture (Uint32 stream_start, Uint32 stream_length)
{
  m_stream_start = stream_start;
  m_stream_length = stream_length;
  m_texture = NULL;
  m_hascolorkey = false;
  m_colorkey = 0;
}

cStaticTexture::~cStaticTexture ()
{
  delete m_texture;
  m_texture = NULL;
}

void cStaticTexture::LoadTexture (std::ifstream * stream)
{
  assert (stream);

  char *tempdata = (char *) malloc (m_stream_length);

  int oldpos = stream->tellg ();

  //ENDIANNESS ? Prolly yes
  stream->seekg( m_stream_start, std::ios::beg );
  stream->read( tempdata, m_stream_length );
  /*
     for( int ii=0; ii<m_stream_length/4; ii++)
     tempdata = (char *) IRIS_SwapU32( (unsigned int) tempdata+ii*4);
   */

  delete m_texture;
  m_texture = NULL;

  SDL_Surface *image;
  image = IMG_Load_RW (SDL_RWFromMem (tempdata, m_stream_length), 1);
  free (tempdata);

  if (!image)
	  THROWEXCEPTION (std::string ("IMG_Load_RW: ") + std::string (IMG_GetError ()));

  m_texture = new Texture;

  if (m_hascolorkey)
    m_texture->LoadFromSurfaceWithColorKey (image, m_colorkey);
  else
    m_texture->LoadFromSurface (image, GL_LINEAR);

  SDL_FreeSurface (image);

  stream->seekg( oldpos, std::ios::beg );
}


void cStaticTexture::SetColorKey (Uint32 colorkey)
{
  m_colorkey = colorkey;
  m_hascolorkey = true;
}


cStaticTextureLoader::cStaticTextureLoader( std::ifstream *stream, Uint32 length ) : modelstream( stream )
{
	std::map <std::string, Uint32> texture_md5_map;
	unsigned int index;

	std::string md5str;

	ASSERT( stream );
	texturestream_start = stream->tellg ();
	texturestream_length = length;
	texturestream_end = texturestream_start + texturestream_length;

	sTextureFileHeader header;

	if ( texturestream_length < sizeof(header) )
	{
		THROWEXCEPTION ("invalid texture stream size (<headersize)");
	}

	modelstream->read( (char *)&header, sizeof(header) );
	header.Length = IRIS_SwapU32( header.Length );
	header.Version = IRIS_SwapU32( header.Version );
	header.TextureTableStart = IRIS_SwapU32( header.TextureTableStart );
	header.TextureTableCount = IRIS_SwapU32( header.TextureTableCount );
	header.GroundTableStart = IRIS_SwapU32( header.GroundTableStart );
	header.GroundTableCount = IRIS_SwapU32( header.GroundTableCount );

	for ( int ii = 0; ii < 9; ii++ )
	{
		header.Reserved[ii] = IRIS_SwapU32( header.Reserved[ii] );
	}

	/* Check some header information */
	if ( !CheckHeaderID( header.Sign, FILEID_GAMEMODELTEXTURE ) )
	{
		THROWEXCEPTION( "invalid texture stream" );
	}
	if ( header.Version != GAMEMODEL_VERSION )
	{
		THROWEXCEPTION( "texture stream has an invalid version!" );
	}

	/*
	if ( texturestream_length != header.Length )
	{
	THROWEXCEPTION( "invalid texture stream size" );
	}
	*/

	if ( ( (texturestream_end + texturestream_start) < header.TextureTableStart ) 
		|| ( (texturestream_end + texturestream_start) < 
		(header.TextureTableStart + header.TextureTableCount * sizeof(sTextureTableEntry) ) ) )
	{
		THROWEXCEPTION( "invalid texture stream" );
	}

	sTextureTableEntry entry;
	for ( index = 0; index < header.TextureTableCount; index++ )
	{
		modelstream->seekg( header.TextureTableStart + index * sizeof(entry) + texturestream_start, std::ios::beg );
		modelstream->read( (char *)&entry, sizeof(entry) );
		entry.start = IRIS_SwapU32( entry.start );
		entry.length = IRIS_SwapU32( entry.length );

		/* don't trust anything :) */
		if ( (texturestream_end < entry.start) ||
			(texturestream_end < entry.start + entry.length) ||
			(entry.length < sizeof(sTextureFileEntry) ) )
		{
			THROWEXCEPTION( "invalid texture stream" );
		}

		sTextureFileEntry fileentry;
		modelstream->seekg( entry.start + texturestream_start, std::ios::beg );
		modelstream->read( (char *)&fileentry, sizeof(fileentry) );
		fileentry.Width = IRIS_SwapU32( fileentry.Width );
		fileentry.Height = IRIS_SwapU32( fileentry.Height );
		fileentry.Length = IRIS_SwapU32( fileentry.Length );
		fileentry.ColorKey = IRIS_SwapU32( fileentry.ColorKey );
		fileentry.Flags = IRIS_SwapU32( fileentry.Flags );

		for ( int ii = 0; ii < 3; ii++ )
		{
			fileentry.Reserved[ii] = IRIS_SwapU32( fileentry.Reserved[ii] );
		}

		if ( fileentry.Length + sizeof(fileentry) != entry.length )
		{
			THROWEXCEPTION( "invalid texture stream" );
		}

		cStaticTexture *texture = new cStaticTexture( entry.start + sizeof(fileentry) + texturestream_start, fileentry.Length );
		if ( fileentry.Flags & TEXTUREFLAG_COLORKEY )
		{
			texture->SetColorKey (fileentry.ColorKey);
		}

		// Generate MD5 lookup list
		md5str = "";
		for (int i = 0; i < 32; i++)
		{
			md5str += tolower( fileentry.md5sum[i] );
		}
		texture_md5_map.insert( std::make_pair( md5str, textures.size() ) );

		textures.push_back( texture );
	}

	// load ground replacement list
	ground_texture_map.clear();
	for ( index = 0; index < header.GroundTableCount; index++ )
	{
		sGroundTextureTableEntry groundentry;
		modelstream->seekg( header.GroundTableStart + index * sizeof(groundentry) + texturestream_start, std::ios::beg );
		modelstream->read( (char *)&groundentry, sizeof(groundentry) );

		md5str = "";
		for ( int i = 0; i < 32; i++ )
		{
			md5str += tolower( groundentry.md5sum[i] );
		}

		// find texture
		std::map<std::string, Uint32>::iterator iter = texture_md5_map.find( md5str );

		// if texture can be found, add to ground_texture_map
		if ( iter != texture_md5_map.end() )
		{
			ground_texture_map.insert( std::make_pair( IRIS_SwapU32( groundentry.id ), iter->second ) );
			// printf( "%i: %s (%i)\n", groundentry.id, md5str.c_str(), iter->second ); 
		}
	}

	texture_md5_map.clear();
}

cStaticTextureLoader::~cStaticTextureLoader()
{
	for ( unsigned int index = 0; index < textures.size(); index++ )
	{
		SAFE_DELETE( textures[index] );
	}
	textures.clear();

	modelstream = NULL;
	ground_texture_map.clear();
}


Texture *cStaticTextureLoader::GetTexture( Uint32 index )
{
	ASSERT( modelstream );
	ASSERT( index < textures.size() );
  
	Texture *result = textures[index]->texture();
	if ( !result )
	{
		textures[index]->LoadTexture( modelstream );
		result = textures[index]->texture();
	}
	assert( result );

	return result;
}


Texture *cStaticTextureLoader::GetGroundTexture( Uint32 id )
{
	std::map<Uint32, Uint32>::iterator iter = ground_texture_map.find( id );
	if ( iter != ground_texture_map.end() )
	{
		return GetTexture( iter->second );
	}

	return NULL;
}
