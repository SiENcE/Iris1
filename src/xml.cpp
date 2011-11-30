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


#include "xml.h"




// Charsets
const char *letterSet =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char *whitespaceSet = " \x09\x0A\x0D";
const char *digitSet = "0123456789";
const char *extenderSet = "\xb7";
const char *firstNameSet =
  "_:ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char *nameSet =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789\xb7.-_:";

inline bool isInSet (const char *set, char c)
{
  int i = 0;
  while (set[i])
    if (set[i++] == c)
      return true;

  return false;
}

namespace XML
{

	// NODE IMPLEMENTATION

	Node::~Node()
	{
		// Delete all subnodes
		for ( unsigned int i = 0; i < nodes.size(); i++ )
		{
			SAFE_DELETE( nodes[i] );
		}
	}

  int Node::addNode( Node *node )
  {
		nodes.push_back( node );

		return nodes.size () - 1;
  }

// Search for the node
  bool Node::hasNode (const std::string & name) const
  {
    register bool found = false;
    register unsigned int i;

    for (i = 0; i < nodes.size (); ++i)
      if (nodes[i]->nodeType () == Element && nodes[i]->name () == name)
          {
            found = true;
            break;
          }

    return found;
  }

  void Node::removeNode (unsigned int index)
  {
    if (index >= nodes.size ())
      return;

    removeNode (nodes[index]);
  }

  void Node::removeNode (Node * node)
  {
    for (std::vector < Node * >::iterator it = nodes.begin ();
         it != nodes.end (); ++it)
        {
          if (*it == node)
              {
                delete *it;     // or mem leak ...
                nodes.erase (it);
                break;
              }
        }
  }

  Node *Node::findNode (const std::string & name, int index, bool raiseError) const
  {
    int found = 0;

    for (std::vector < Node * >::const_iterator it = nodes.begin ();
         it != nodes.end (); ++it)
        {
          if ((*it)->name () == name)
              {
                if (found == index)
                  return (*it);
                ++found;
              }
        }

    if ( raiseError )
	{
        THROWXMLEXCEPTION ("Node was not found.");
	}

    return 0;
  }

  Node *Node::getNode (unsigned int index) const
  {
    if (index >= nodes.size ())
      return 0;

    return nodes[index];
  }

  unsigned int Node::getTotalNodeCount () const
  {
    int count = 1;

    for (std::vector < Node * >::const_iterator it = nodes.begin ();
         it != nodes.end (); ++it)
        {
          count += (*it)->getTotalNodeCount ();
        }

    return count;
  }

  int Node::asInteger() const
  {
	int data=0;
	if( _data.find( "0x" ) == 0 )	{
		data = strtol( _data.substr( 2, _data.length() - 2 ).c_str(), 0, 16 );
	} else {
		data = atoi( _data.c_str() );
	}

	return data;
  }

  float Node::asFloat () const
  {
    return::atof (_data.c_str ());
  }

  const std::string & Node::asString () const
  {
    return _data;
  }
  bool Node::asBool () const
  {
    if (_data.find ('1') != std::string::npos)
      return 1;
    if (_data == "true")
      return 1;
    return 0;
  }

// Parser implementation

  bool Parser::parseAttribute (Node * node)
  {
    std::string name;

    if (!parseName (name))
      return false;

    if (!parseEq ())
      reportError ("Equals sign missing in attribute.");

    std::string value;

    if (!parseAttributeValue (value))
      reportError ("Value missing in attribute");

    Node *thisNode = new Node;
    thisNode->setNodeType (Attribute);
    thisNode->setName (name);
    thisNode->setData (value);
    node->addNode (thisNode);
    return true;
  }

  bool Parser::parseAttributeValue (std::string & attValue)
  {
    if (!isInSet ("\"'", current))
      return false;

    char quoteChar = current;
    std::string value;

    readChar (false);

    while (current != quoteChar)
        {
          if (current == '<')
            reportError ("Invalid character '<' in attribute value");
          else if (current == '&')
            current = parseReference ();

          value += current;
          readChar (false);
        }

    attValue = value;

    readChar (false);           // Skip Quote
    return true;
  }

  bool Parser::parseCDSection (Node * node)
  {
    std::string cdata;
    if (!doesStreamMatchString ("<![CDATA["))
      return false;

    do
        {
          cdata += current;
        }
    while (!doesStreamMatchString ("]]>"));

    node->setData (node->data () + cdata);
    return true;
  }

  bool Parser::parseCharData (Node * node)
  {
    std::string data;

    while (!isInSet ("<&\0", current))
        {
          data += current;
          readChar ();
        }

    if (data.find ("]]>") != data.npos)
      reportError ("Character data cannot contain ']]>'");

    node->setData (node->data () + data);
    return (data.length () > 0);
  }

  bool Parser::parseComment ()
  {
    if (!doesStreamMatchString ("<!--"))
      return false;

    std::string comment;
    // bool completed = false;
    // *at least* one char
    readChar (false);

    while (!doesStreamMatchString ("-->"))
        {
          readChar (false);
        }
    return true;
  }

  void Parser::parseContent (Node * node)
  {
    bool foundContent = true;

    do
        {
          if (parseCDSection (node))
              {
              }
          else if (parseProcessingInstruction (node))
              {
              }
          else if (parseComment ())
              {
              }
          else if (parseElement (node))
              {
              }
          else if (parseCharData (node))
              {
              }
          else if (current == '&')
            node->setData (node->data () + parseReference ());
          else
            foundContent = false;
        }
    while (foundContent);
  }

  void Parser::parseDocType (Node * node)
  {
    if (!doesStreamMatchString ("<!DOCTYPE"))
      return;

    reportError ("DocType is not yet implemented.");
  }

	bool Parser::parseElement (Node * node)
	{
		if (current != '<')
		{
			return false;
		}

		State state = SaveState();
		std::string name;
		readChar( false );

		if ( !parseName( name ) )
		{
			RestoreState( state );
			return false;
		}

		Node *thisNode = new Node();
		thisNode->setName (name);
		thisNode->setNodeType (Element);

		try
		{
			do
			{
				parseWhitespace();
			} while ( parseAttribute( thisNode ) );

			// Empty Element Tag
			if ( current == '/' )
			{
				readChar( false );
				if ( current != '>' )
				{
					reportError( "Tag not terminated properly." );
				}
				readChar();        // Skip Char
			}
			// Normal Start Tag
			else if ( current == '>' )
			{
				readChar( false );
				parseContent( thisNode );
				//thisNode->setData( thisNode->data().replace( ... ) );
				// was: ThisNode.Data := TrimChars(ThisNode.Data, [' ', #9, #13, #10]);

				if ( !parseETag( thisNode ) )
				{
					reportError( "No ending tag for " + name );
				}
			}
			// Not terminated properly
			else
			{
				reportError( "Tag not terminated properly." );
			}
		}
		catch (...)
		{
			throw;
		}

		node->addNode( thisNode );

		return true;
	}

  bool Parser::parseEq ()
  {
    parseWhitespace ();
    bool found = (current == '=');
    readChar (false);
    parseWhitespace ();

    return found;
  }

  bool Parser::parseETag (Node * node)
  {
    if (!doesStreamMatchString ("</"))
      return false;

    std::string name;

    if (!parseName (name))
      return false;

    if (name != node->name ())
      reportError ("End tag name '" + name +
                   "' does not match start tag name '" + node->name () + "'");

    parseWhitespace ();

    if (current != '>')
      reportError ("End tag missing: '>'");

    readChar ();                // Skip to the next char
    return true;
  }

  bool Parser::parseMisc (Node * node)
  {
    if (parseWhitespace ())
      return true;

    if (parseComment ())
      return true;

    if (parseProcessingInstruction (node))
      return true;

    return false;
  }

  bool Parser::parseName (std::string & name)
  {
    if (!isInSet (firstNameSet, current))
      return false;

    name = "";

    do
        {
          name += current;
          readChar (true);
        }
    while (isInSet (nameSet, current));

    return true;
  }

  bool Parser::parseProcessingInstruction (Node * node)
  {
    // Processing instructions start with <?
    if (!doesStreamMatchString ("<?"))
      return false;

    std::string name;

    if (!parseName (name))
      reportError ("Valid processing instructions have to have a name.");

    if (name == "xml")
      reportError
        ("Processing instructions cannot have the reserved name 'xml'");

    Node *thisNode = new Node;
    thisNode->setName (name);
    thisNode->setNodeType (ProcessingInstruction);

    try
    {
      parseWhitespace ();
      std::string chars;

      while (!doesStreamMatchString ("?>"))
          {
            readChar (false);
            data += current;

            // We're in the middle of parsing a processing instruction
            if (endOfData ())
              reportError ("Unterminated processing instruction!");
          }

      thisNode->setData (data);
    }
    catch (...)
    {
      delete thisNode;
      throw;
    }

    node->addNode (thisNode);
    return true;
  }

  bool Parser::parseProlog (Node * node)
  {
    parseXMLDeclaration (node);
    while (parseMisc (node))
        {
        }

    parseDocType (node);
    while (parseMisc (node))
        {
        }

    return true;
  }

  char Parser::parseReference ()
  {
    char result = 0;
    std::string reference;

    if (current != '&')
      reportError ("All references have to begin with '&'");

    readChar (false);           // We *have* to have a next character

    // Search for the end character
    while (current != ';')
        {
          reference += current;
          readChar (false);
        }

    if (reference.substr (0, 2) == "#x")
        {
          result =
            static_cast <
            char
            >(strtol
              (reference.substr (2, reference.length () - 2).c_str (), 0,
               16));
        }
    else if (reference[0] == '#')
        {
          result =
            static_cast <
            char
            >(strtol
              (reference.substr (1, reference.length () - 1).c_str (), 0,
               10));
        }
    else if (reference == "amp")
      result = '&';
    else if (reference == "lt")
      result = '<';
    else if (reference == "gt")
      result = '>';
    else if (reference == "quot")
      result = '"';
    else if (reference == "apos")
      result = '\'';
    else
      reportError ("Unknown char reference: " + reference);

    // Advance to the next position
    readChar ();

    return result;
  }

  bool Parser::parseWhitespace ()
  {
    bool found = false;

    while (isInSet (whitespaceSet, current))
        {
          found = true;
          readChar ();
          if (endOfData ())
            break;
        }

    return found;
  }

  void Parser::parseXMLDeclaration (Node * node)
  {
    if (!doesStreamMatchString ("<?xml"))
      return;

    Node *thisNode = new Node;
    thisNode->setNodeType (XMLDeclaration);

    try
    {
      do
        parseWhitespace ();
      while (parseAttribute (thisNode));

      parseWhitespace ();
      if (!doesStreamMatchString ("?>"))
        reportError ("XML declaration does not end with ?>");
    }
    catch (...)
    {
      delete thisNode;
      throw;
    }

    node->addNode (thisNode);
  }

  void Parser::reportError (std::string error)
  {
    // We should normally raise a proper exception here
    char *err = new char[error.length () + 1];
    strcpy (err, error.c_str ());
    THROWXMLEXCEPTION (err);
	SAFE_DELETE_ARRAY( err );
  }

// This is kind of a fetch-ahead
  bool Parser::doesStreamMatchString (std::string str)
  {
    unsigned int i = 0;
    bool match = true;
    State state = SaveState ();

    if (str.length () == 0)
      return false;

    do
        {
          match = (current == str[i]);
          ++i;
          readChar ();
        }
    while (match && !endOfData () && i < str.length ());

    if (!match)
      RestoreState (state);

    return match;
  }

  bool Parser::endOfData ()
  {
    return position >= data.length ();
  }

  void Parser::readChar (bool ignoreError)
  {
    if (position >= data.length ())
        {
          current = 0;

          if (!ignoreError)
            reportError ("Unexpected end of file.");
        }
    else
        {
          current = data[position++];
        }
  }

  Parser::State Parser::SaveState ()
  {
    State rVal;
    rVal.current = current;
    rVal.position = position;
    return rVal;
  }

  void Parser::RestoreState (State state)
  {
    position = state.position;
    current = state.current;
  }

	Node *Parser::parseDocument ()
	{
		readChar (false);
		m_kParseDocument = new Node();
		m_kParseDocument->setName( "" );
		m_kParseDocument->setNodeType( Document );

		if ( !parseProlog( m_kParseDocument ) )
		{
			reportError( "Missing prolog" );
		}

		if ( !parseElement( m_kParseDocument ) )
		{
			reportError( "Missing content" );
		}

		while ( parseMisc( m_kParseDocument ) )
		{
		}

		return m_kParseDocument;
	}

  void Parser::setData (const std::string & data)
  {
    position = 0;
    this->data = data;
  }

  Parser::~Parser()
  {
	  SAFE_DELETE( m_kParseDocument );
  }

	void Parser::loadData (const std::string & filename)
	{
		FILE *fp = fopen (filename.c_str (), "rt");

		if (!fp)
			throw "Couldn't open file.";

		fseek (fp, 0, SEEK_END);
		//unsigned int size = ftell( fp );
		fseek (fp, 0, SEEK_SET);

		char buffer[1025];
		std::string result;

		while (!feof (fp))
		{
			int bytesRead = fread (buffer, 1, 1024, fp);
			buffer[bytesRead] = 0;
			result += buffer;
		}

		fclose (fp);

		setData (result);
  }

  bool Node::lookupAttribute (const std::string & name, unsigned int &data) const
  {
    for (unsigned int i = 0; i < getNodeCount (); ++i)
        {
          Node *node = getNode (i);
          if (node->nodeType () != Attribute || node->name () != name)
              continue;

          if (node->data ().find ("0x") == 0)
              {
                data =
                  strtol (node->data ().
                          substr (2, node->data ().length () - 2).c_str (), 0,
                          16);
              }
          else
              data = atoi (node->data ().c_str ());

          return true;
        }

    return false;
  }

  bool Node::lookupAttribute (const std::string & name, int &data) const
  {
    for (unsigned int i = 0; i < getNodeCount (); ++i)
        {
          Node *node = getNode (i);
          if (node->nodeType () != Attribute || node->name () != name)
              continue;

          if (node->data ().find ("0x") == 0)
              {
                data =
                  strtol (node->data ().
                          substr (2, node->data ().length () - 2).c_str (), 0,
                          16);
              }
          else
              data = atoi (node->data ().c_str ());
          return true;
        }

    return false;
  }

  bool Node::lookupAttribute (const std::string & name, std::string & data) const
  {
    for (unsigned int i = 0; i < getNodeCount (); ++i)
        {
          Node *node = getNode (i);
          if (node->nodeType () != Attribute || node->name () != name)
              continue;

            data = node->data ();
            return true;
        }

    return false;
  }

  std::string Node::findString (const std::string & name)
  {
    Node *node = findNode (name);
    if (node)
      return node->asString ();
    else
      return "";
  }

  int Node::findInteger (const std::string & name)
  {
    Node *node = findNode (name);
    if (node)
      return node->asInteger ();
    else
      return 0;
  }

  float Node::findFloat (const std::string & name)
  {
    Node *node = findNode (name);
    if (node)
      return node->asFloat ();
    else
      return 0.0f;
  }

};
