/*
 * Created by Robert Noll
 */

/*
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
 */

// ****** ****** ****** robstring.h
#ifndef _ROBSTRING_H_
#define _ROBSTRING_H_

#include <string>
#include <vector>

// string generation
std::string	strprintf	(const char* szFormat,...);

//unused
//std::string	strprintvf	(const char* szFormat,void* arglist);


/// also known as split,  explode("#","abc#def#ghi",res)  pushes  "abc","def","ghi" onto res
void	explodestr 		(const char* separator,const char* str,std::vector<std::string>& res);

// char-ranges
bool	charmatchrange	(const char c,const char* r); // \ to escape, a-z as range
int		cinrange		(const char* str,const char* range); // count chars in range
int		coutrange		(const char* str,const char* range); // count chars out of range

// string manipulation
unsigned int	stringhash	(const char* str); // generate a hash value
std::string		addslashes	(const char* str); // escape backslash and quotes
std::string		strtolower	(const char* str); // UPPERCASE
std::string		strtoupper	(const char* str); // lowercase

// paths
std::string		pathgetdir		(const std::string &path); // C:/zeug/grafik/datei.txt -> C:/zeug/grafik
std::string		pathgetfile		(const std::string &path); // C:/zeug/grafik/datei.txt -> datei.txt
std::string		pathgetext		(const std::string &path); // C:/zeug/grafik/datei.txt -> .txt
char			pathgetdirslash	(const std::string &path); // C:/zeug/grafik/datei.txt -> / 
char			pathgetwindrive	(const std::string &path); // C:/zeug/grafik/datei.txt -> C
bool			pathisabsolute	(const std::string &path); // C:/zeug/grafik/datei.txt -> true
std::string		pathadd			(const std::string &base,std::string &add);
bool			pathissubpath	(const std::string &container,std::string &path);




#endif
// ****** ****** ****** END

