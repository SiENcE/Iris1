//
// File: Macros.cpp
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
 

#include "Macros.h"

#include "Config.h"
#include "Logger.h"
#include <vector>
#include <iostream>
#include "xml.h"
#include "iris_endian.h"




MacroLoader * pMacroLoader = NULL;

MacroLoader::MacroLoader()
{
	XML::Parser parser;
	XML::Node *macroentries, *document;
	  
	try
	{
		parser.loadData( "./xml/Macros.xml" );
		document = parser.parseDocument();
		  
		macroentries = document->findNode( "MACROS" );
		  
		if ( !macroentries )
		{
			throw "Couldn't find macros node.";
		}
	}
	catch( ... )
	{
		Logger::WriteLine( "\t| -> Couldn't load Macros.xml" );
		return;
	}
  
	XML::Node *macro_node, *value;
	int idx = 0, iMacroCount = 0;
     
	while ( ( macro_node = macroentries->findNode( "MACRO", idx ) ) )
	{
		int id = idx;
		SDLKey key = (SDLKey)0;
		int keymod = 0x0000;
		std::vector<m_Parameter*> parameters;
		std::string script_function = "none";
		//Logger::WriteLine( "MCR1" );
		iMacroCount++;
		value = macro_node->findNode( "ID" );

		id = ( value != NULL ) ? value->asInteger() : idx;

		if ( ( value = macro_node->findNode( "KEY" ) ) )
		{
			key = getkey_byname(value->asString());
		}

		if ( ( value = macro_node->findNode( "LEFT_SHIFT" ) ) )
		{
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_LSHIFT;
			}
		}
		if ( ( value = macro_node->findNode( "RIGHT_SHIFT" ) ) )
		{
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_RSHIFT;
			}
		}
		if ( ( value = macro_node->findNode( "RIGHT_ALT" ) ) )
        {
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_RALT;
			}
		}
        if ( ( value = macro_node->findNode( "LEFT_ALT" ) ) )
        {
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_LALT;
			}
        }
		if ( ( value = macro_node->findNode( "RIGHT_CONTROL" ) ) )
		{
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_RCTRL;
			}
		}
        if ( ( value = macro_node->findNode( "LEFT_CONTROL" ) ) )
		{
			if ( value->asInteger() == 1 )
			{
				keymod |= KMOD_LCTRL;
			}
		}
		if ( ( value = macro_node->findNode( "SCRIPT_FUNCTION" ) ) )
		{
			script_function = value->asString();
		}
		std::string parameter_type;
        
		int idx2 = 0;
		while ( ( value = macro_node->findNode( "PARAMETER", idx2 ) ) )
		{
			m_Parameter * param = NULL;
			if ( value->lookupAttribute( "TYPE", parameter_type ) )
			{
				// std::cout << "PARAMETER" << parameter_type << std::endl;
				param = new m_Parameter();
				if ( parameter_type == "string" )
				{
					param->type = PARAMETERTYPE_STRING;
					param->str_value = value->asString();
					param->int_value = 0;
				}
				else if ( parameter_type == "integer" )
				{
					param->type = PARAMETERTYPE_INTEGER;
					param->str_value = "";
					param->int_value = IRIS_SwapI32(value->asInteger());
				}
				else
				{
					Logger::WriteLine("Warning: Macros.xml: wrong parameter type, skipping...");
					continue;
				}
			}
			else
			{
				Logger::WriteLine("Warning: Macros.xml: no parameter type specified, skipping...");
				continue;
			}
			if ( parameters.size() < 5 )
			{
				parameters.push_back(param);
			}
			idx2++;
		}
		MacroEntry * entry = new MacroEntry();
		entry->id = IRIS_SwapI32(id);
		entry->key = key;
		entry->keymod = (SDLMod)keymod;
		entry->script_function = script_function;
		entry->parameters = parameters;
		macros.insert( std::make_pair( (int)key, entry ) );
		idx++;
	}
	char cMacros[2];
	sprintf( cMacros, "%d", iMacroCount );
	Logger::WriteLine( "\t| -> Sucessfuly loaded " + std::string( cMacros ) );
}


MacroLoader::~MacroLoader()
{
 std::multimap<int, MacroEntry*>::iterator iter;
 for(iter = macros.begin(); iter!= macros.end(); iter++)
   {
    for( unsigned int i=0; i < iter->second->parameters.size(); i++)
            delete iter->second->parameters.at(i);
    delete iter->second; 
   }
  macros.clear();
}


SDLKey MacroLoader::getkey_byname(std::string keyname) 
{
 if(keyname == "space")
  return (SDLKey)32;
 else if(keyname == "backspace")
  return (SDLKey)8;
 else if(keyname == "enter")
  return (SDLKey)13;
 else if(keyname == "clear")
  return (SDLKey)12;
 else if(keyname == "tab")
  return (SDLKey)9;
 else if(keyname == "esc")
  return (SDLKey)27;  
 else if(keyname == "keypad0")
  return (SDLKey)256;
 else if(keyname == "keypad1")
  return (SDLKey)257;
 else if(keyname == "keypad2")
  return (SDLKey)258;
 else if(keyname == "keypad3")
  return (SDLKey)259;
 else if(keyname == "keypad4")
  return (SDLKey)260;
 else if(keyname == "keypad5")
  return (SDLKey)261;
 else if(keyname == "keypad6")
  return (SDLKey)262;
 else if(keyname == "keypad7")
  return (SDLKey)263;
 else if(keyname == "keypad8")
  return (SDLKey)264;
 else if(keyname == "keypad9")
  return (SDLKey)265;
 else if(keyname == "keypad_period")
  return (SDLKey)266;
 else if(keyname == "keypad_divide")
  return (SDLKey)267;
 else if(keyname == "keypad_multiply")
  return (SDLKey)268; 
 else if(keyname == "keypad_minus")
  return (SDLKey)269;
 else if(keyname == "keypad_plus")
  return (SDLKey)270;
 else if(keyname == "keypad_enter")
  return (SDLKey)271;
 else if(keyname == "keypad_equals")
  return (SDLKey)272;
 else if(keyname == "keypad_equals")
  return (SDLKey)272;
 else if(keyname == "up")
  return (SDLKey)273;
 else if(keyname == "down")
  return (SDLKey)274;
 else if(keyname == "right")
  return (SDLKey)275;
 else if(keyname == "left")
  return (SDLKey)276;
 else if(keyname == "ins")
  return (SDLKey)277;
 else if(keyname == "home")
  return (SDLKey)278;
 else if(keyname == "end")
  return (SDLKey)279;
 else if(keyname == "page_up")
  return (SDLKey)280;
 else if(keyname == "page_down")
  return (SDLKey)281;
 else if(keyname == "F1")
  return (SDLKey)282;
 else if(keyname == "F2")
  return (SDLKey)283;
 else if(keyname == "F3")
  return (SDLKey)284;
 else if(keyname == "F4")
  return (SDLKey)285;
 else if(keyname == "F5")
  return (SDLKey)286;
 else if(keyname == "F6")
  return (SDLKey)287;
 else if(keyname == "F7")
  return (SDLKey)288;
 else if(keyname == "F8")
  return (SDLKey)289; 
 else if(keyname == "F9")
  return (SDLKey)290;
 else if(keyname == "F10")
  return (SDLKey)291;
 else if(keyname == "F11")
  return (SDLKey)292;
 else if(keyname == "F12")
  return (SDLKey)293;
 else if(keyname.size() == 1)
 {
    char * asciikey = (char*)keyname.c_str();
 
    return (SDLKey)((int)*asciikey);
    delete[] asciikey;
 }
 else
  return (SDLKey)0;
    
}

MacroEntry * MacroLoader::GetMacro(int id)
{
    std::multimap<int, MacroEntry*>::iterator iter;
    iter = macros.find(id);
    if(iter != macros.end())
     return iter->second;
    else
     return NULL;
}

int MacroLoader::GetEntriesCount(int id)
{
    return (int)macros.count(id);                       
}

MacroEntry * MacroLoader::GetMultiMacro(int id, int index)
{
	typedef std::multimap <int, MacroEntry*>::const_iterator c_iter;
	typedef std::pair<c_iter, c_iter> Range;
    Range range=macros.equal_range(id);
    int idx = 0;
	for ( c_iter i = range.first; i != range.second; ++i )
    {
     if(idx == index)
     {
      return i->second;
      break;
     }
     idx++;
    }
    return NULL;
}
