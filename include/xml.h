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
/*
	File: xml.h
	Created: DarkStorm
	Purpose: xml parser class
	Changelog:
		- added asBool / asString (Gigi)
		- fixed mem leak (Gigi)
*/
#if !defined(__XMLDOM_H__)
#define __XMLDOM_H__

#include "Common.h"
#include <stdio.h>
#include <vector>
#include <string>
#include "Exception.h"

// #include "../Fluid/mmgr.h"


#ifdef XMLDOM_EXPORTS
#define XMLDOM_API __declspec( dllexport )
#else
#define XMLDOM_API
#endif

namespace XML
{
	class Node;

	class XMLDOM_API Parser
	{
	private:
		char current;
		std::string data;
		unsigned int position;
		Node *m_kParseDocument;

		// Parse Helpers
		bool parseAttribute( Node *node );
		bool parseAttributeValue( std::string &attValue );
		bool parseCDSection( Node *node );
		bool parseCharData( Node *node );
		bool parseComment();
		void parseContent( Node *node );
		void parseDocType( Node *node );
		bool parseElement( Node *node );
		bool parseEq(); // Parse '='
		bool parseETag( Node *node );
		bool parseMisc( Node *node );
		bool parseName( std::string &name );
		bool parseProcessingInstruction( Node *node );
		bool parseProlog( Node *node );
		char parseReference();
		bool parseWhitespace();
		void parseXMLDeclaration( Node *node );
		void reportError( std::string error );

	protected:
		bool doesStreamMatchString( std::string str );
		bool endOfData();
		void readChar( bool ignoreError = true );
		
		struct State
		{
			unsigned int position;
			char current;
		};

		State SaveState();
		void RestoreState( State );
	
	public:
		Node *parseDocument();
		void setData( const std::string &data );
		void loadData( const std::string &filename );

		Parser( const std::string &data ) { setData( data ); }
		Parser() {}
		virtual ~Parser();
	};

	enum eNodeType
	{
		Document,
		Element,
		Attribute,
		ProcessingInstruction,
		XMLDeclaration
	};

	class XMLDOM_API Node
	{
	private:
		std::string _data;
		std::string _name;
		std::vector< Node* > nodes;
		eNodeType _nodeType;

	public:
		Node() {}
		virtual ~Node();

		int addNode( Node *node );
		bool hasNode( const std::string &name ) const;
		void removeNode( unsigned int index );
		void removeNode( Node *node );
		Node *findNode( const std::string &name, int index, bool raiseError = false ) const;
		Node *findNode( const std::string &name, bool raiseError = false ) const { return findNode( name, 0, raiseError ); }
		
		bool lookupAttribute( const std::string &name, unsigned int &data ) const;
		bool lookupAttribute( const std::string &name, int &data ) const;
		bool lookupAttribute( const std::string &name, std::string &data ) const;

		unsigned int getNodeCount() const { return (unsigned int)nodes.size(); }
		Node *getNode( unsigned int index ) const;
		unsigned int getTotalNodeCount() const;
		
		int asInteger() const;
		float asFloat() const;
		bool asBool() const;
		const std::string & asString() const;

		// Overloading the int operator
		Node *operator[]( unsigned int index ) const { return getNode( index ); }
		Node *operator[]( int index ) const { return getNode( (unsigned int)index ); }
		Node *operator[]( const char* name ) const { return findNode( name ); }

		// Setters/Getters
		eNodeType nodeType() const { return _nodeType; }
		void setNodeType( eNodeType nodeType ) { _nodeType = nodeType; }
		const std::string &name() const { return _name; }
		const std::string &data() const { return _data; }
		void setName( const std::string &name ) { _name = name; }
		void setData( const std::string &data ) { _data = data; }
                
                std::string findString (const std::string & name);
                int findInteger (const std::string & name);
                float findFloat (const std::string & name);

	};
};

#endif
