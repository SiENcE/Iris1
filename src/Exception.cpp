//
// File: Exception.cpp
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


#include <iostream>
#include <string>
#include "Exception.h"

using namespace std;

cException::cException (std::string message, std::string file, int line)
{
  this->m_message = message;
  this->m_file = file;
  this->m_line = line;
}

cException::~cException ()
{
}

std::string cException::message ()
{
  return m_message;
}


std::string cException::file ()
{
  return m_file;
}

int cException::line ()
{
  return m_line;
}

std::string cException::debug_message ()
{
  char str_line[32];
  sprintf (str_line, "%i", line ());
  return ("an exception occured: " + message () + "(" + file () + ", " +
          string (str_line) + ")");
}



cAssertException::cAssertException (std::string file, int line):cException ("", file,
            line)
{
}

std::string cAssertException::message ()
{
  return "Assertion failed";
}

cXMLException::cXMLException (std::string message, std::string file, int line):cException (message, file,
            line)
{
}


std::string cXMLException::message ()
{
  return "XML: " + cException::message ();
}
