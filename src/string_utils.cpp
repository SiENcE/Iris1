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
#include <stdio.h>
#include <vector>
#include <string_utils.h>




using namespace std;

bool stringSplit (vector < std::string > &vec, const std::string & str,
                  const std::string & delim)
{
	vec.clear();

	if ( delim.empty() )
	{
		vec.push_back( str );
		return false;
	}

	string::size_type i = 0;
	string::size_type j = 0;

	for ( ; ; )
	{
		j = str.find( delim, i );
		if ( j == string::npos )
		{
			vec.push_back( str.substr( i ) );
			break;
		}

		vec.push_back( str.substr( i, j - i ) );
		i = j + delim.size();

		if ( i == str.size() )
		{
			vec.push_back( string() );
			break;
		}
	}
	return true;
}
