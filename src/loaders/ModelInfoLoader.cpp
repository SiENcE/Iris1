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


#include "loaders/ModelInfoLoader.h"




//cModelInfoLoader pModelInfoLoader;

ModelInfoLoader *ModelInfoLoader::m_sgModelInfoLoader = NULL;

cModelInfoEntry::cModelInfoEntry (int id, float scalex, float scaley, float scalez, int alpha, int defhue, int alt_body) : m_id( id ),
					m_scalex( scalex ), m_scaley( scaley ), m_scalez( scalez ), m_alpha( alpha ), m_defhue( defhue ), m_alt_body( alt_body )
{
}


int cModelInfoEntry::id()
{
	return m_id;
}


float cModelInfoEntry::scalex()
{
	return m_scalex;
}


float cModelInfoEntry::scaley()
{
	return m_scaley;
}


float cModelInfoEntry::scalez()
{
	return m_scalez;
}


int cModelInfoEntry::alpha()
{
	return m_alpha;
}


int cModelInfoEntry::defhue()
{
	return m_defhue;
}

int cModelInfoEntry::alt_body()
{
	return m_alt_body;
}


ModelInfoLoader::ModelInfoLoader( std::string filename ) : m_scale_factor( 1.0f )
{
	assert( m_sgModelInfoLoader == NULL );
	m_sgModelInfoLoader = this;

	XML::Parser parser;
	XML::Node *models, *document, *s_factor;

	try
	{
		parser.loadData( filename );
		document = parser.parseDocument();

		models = document->findNode( "MODELS" );

		if ( !models )
		{
			throw "Couldn't find models node.";
		}
	}
	catch ( ... )
	{
		Logger::WriteLine( "Couldn't load ModelsInfo" );
		return;
	}

	XML::Node *model_node, *value;

	if ( ( s_factor = models->findNode( "SCALE_FACTOR" ) ) )
	{
		m_scale_factor = s_factor->asFloat();
	}

	int idx = 0;
	float scalex, scaley, scalez;
	int alpha, defhue, altbody;

	while ( ( model_node = models->findNode( "MODEL", idx ) ) )
	{
		scalex = 1.0f;
		scaley = 1.0f;
		scalez = 1.0f;
		alpha = 255;
		defhue = 0;
		altbody = 0;

		value = model_node->findNode( "ID" );

		Uint32 id = ( value != NULL ) ? value->asInteger() : 0;

		if ( ( value = model_node->findNode( "SCALE_X" ) ) )
		{
			scalex = value->asFloat();
		}

		if ((value = model_node->findNode( "SCALE_Y" ) ) )
		{
			scaley = value->asFloat();
		}

		if ( ( value = model_node->findNode( "SCALE_Z" ) ) )
		{
			scalez = value->asFloat();
		}

		if ( ( value = model_node->findNode( "ALPHA" ) ) )
		{
			alpha = value->asInteger();
		}

		if ( ( value = model_node->findNode( "DEFAULT_HUE" ) ) )
		{
			defhue = value->asInteger();
		}

		if ( ( value = model_node->findNode( "ALT_BODY" ) ) )
		{
			altbody = value->asInteger();
		}

		cModelInfoEntry *modelinfo = new cModelInfoEntry( id, scalex, scaley, scalez, alpha, defhue, altbody );
		model_infos.insert( std::make_pair( (int)id, modelinfo ) );
		idx++;
	}
}


ModelInfoLoader::~ModelInfoLoader()
{
	assert( m_sgModelInfoLoader != NULL );
	m_sgModelInfoLoader = NULL;

	std::map<int, cModelInfoEntry*>::iterator iter;
	for ( iter = model_infos.begin(); iter != model_infos.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}

	model_infos.clear();
}


ModelInfoLoader *ModelInfoLoader::GetInstance()
{
	return m_sgModelInfoLoader;
}


cModelInfoEntry *ModelInfoLoader::GetModelEntry( int id )
{
	std::map<int, cModelInfoEntry *>::iterator iter;
	
	iter = model_infos.find( id );
	if ( iter != model_infos.end() )
	{
		return iter->second;
	}
    
	return NULL;
}


float ModelInfoLoader::getScaleFactor()
{
	return m_scale_factor;   
}
