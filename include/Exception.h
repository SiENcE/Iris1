//
// File: Exception.h
// Created by:
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
 
/*
   Simple exception class
*/

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <string>


class cException
{
public:
	cException( std::string message, std::string file, int line );
	virtual ~cException();
	
	virtual std::string message();
	std::string file();
	int line();
	std::string debug_message();

private:
	std::string m_message;
	std::string m_file;
	int m_line;
};

class cAssertException : public cException 
{
public:
    cAssertException (std::string file, int line);
    virtual std::string message ();
};


class cXMLException : public cException 
{
public:
    cXMLException (std::string message, std::string file, int line);
    virtual std::string message ();
};

// exception macros
#define THROWEXCEPTION(ErrorString) throw cException(ErrorString, __FILE__, __LINE__)
#define THROWASSERTEXCEPTION throw cAssertException(__FILE__, __LINE__)
#define THROWXMLEXCEPTION(ErrorString) throw cXMLException(ErrorString, __FILE__, __LINE__)

// assert macro
#define ASSERT(value) if (!(value)) THROWASSERTEXCEPTION;

#endif //_EXCEPTION_H_
