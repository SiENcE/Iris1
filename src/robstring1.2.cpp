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

#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "robstring1.2.h"

// global buffer, for strprintf
#define kRobStringBufferSize 1024*64

#ifdef WIN32
	#define vsnprintf	_vsnprintf
#endif

char	gRobStringBuffer[kRobStringBufferSize] = "";


// ****** ****** ****** string generation


std::string	strprintf(const char* szFormat,...)
{
	va_list ap;
	va_start(ap,szFormat);
	gRobStringBuffer[0] = 0;
	vsnprintf(gRobStringBuffer,kRobStringBufferSize-1,szFormat,ap);
	std::string s = std::string(gRobStringBuffer);
	va_end(ap);
	return s;
}

//unused
/*
std::string	strprintvf(const char* szFormat,void* arglist)
{
	gRobStringBuffer[0] = 0;
	//possible Error with portability -> http://perfec.to/vsnprintf/
	vsnprintf(gRobStringBuffer,kRobStringBufferSize-1,szFormat,(va_list)arglist);
	return std::string(gRobStringBuffer);
}
*/

void explodestr(const char* separator,const char* str,std::vector<std::string>& res)
{
	assert(separator); if (!separator) return;
	assert(str); if (!str) return;
	assert(strlen(separator)>0); if (strlen(separator)==0) return;
	const char* end;
	do {
		end = strstr(str,separator);
		if (end) {
			res.push_back(std::string(str,end-str));
			str = end + strlen(separator);
			if (!*str) res.push_back(std::string()); // explode("#","a#b#",res) should push 3 strings, the last one being empty
		} else {
			res.push_back(std::string(str));
			return;
		}
	} while (*str) ;
}


// ****** ****** ****** char-ranges



// \ to escape, a-z as range
bool charmatchrange(const char c,const char* r)
{
	for (;*r;r++)
		if (*r == '\\') // escaped char
			if (c == r[1]) return true;
			else r += 1; // skip escape char
		else if (c == *r) return true; // also valid in case of range match with start
		else if (r[1] == '-') // range detected
			if (c >= *r && c <= r[2]) return true;
			else r += 2; // skip range
	return false;
}

// count chars in range
int	cinrange(const char* str,const char* range)
{
	int c = 0;
	for (;*str && charmatchrange(*str,range);str++) c++;
	return c;
}

// count chars out of range
int	coutrange(const char* str,const char* range)
{
	int c = 0;
	for (;*str && !charmatchrange(*str,range);str++) c++;
	return c;
}

// ****** ****** ****** string manipulation

// generate a hash value
unsigned int stringhash (const char* str)
{
	if (!str) return 0;
	int res = 0;
	for (;*str;str++)
		res = (res + *str)*31;
	return res;
}

// escape backslash and quotes
std::string addslashes (const char* str)
{
	std::string res;
	for (;*str;str++) {
		if (strchr("\\\"'",*str))
			res += "\\";
		res += *str;
	}
	return res;
}

// UPPERCASE
std::string		strtoupper	(const char* str)
{
	std::string res;
	for (;*str;str++) res += toupper(*str);
	return res;
}

// lowercase
std::string		strtolower	(const char* str)
{
	std::string res;
	for (;*str;str++) res += tolower(*str);
	return res;
}



// ****** ****** ****** paths

const char* directory_dividers = "\\/:"; // win \\, linux /, mac :

// get directory without trailing slash
std::string	pathgetdir(const std::string &path)
{
	return path.substr(0,path.find_last_of(directory_dividers));
}

// get filename only
std::string	pathgetfile(const std::string &path)
{
	return path.substr(path.find_last_of(directory_dividers)+1);
}

// get file extension including dot
std::string	pathgetext(const std::string &path)
{
	std::string file = pathgetfile(path);
	int pos = file.rfind(".");
	if (pos >= file.npos) return std::string("");
	return file.substr(pos);
}

// get the type of slash used in this path
char pathgetdirslash(const std::string &path)
{
	int pos = path.find_last_of(directory_dividers);
	if (pos >= path.npos) return 0;
	return path[pos];
}

// get the drive letter (A: C: ...) in a win path , 0 if not absolute
char pathgetwindrive(const std::string &path)
{
	if (path.length() >= 2 && path[1] == ':')
			return path[0];
	else	return 0;
}

// is the path absolute (true) or relative(false)
bool pathisabsolute(const std::string &path)
{
	return pathgetwindrive(path) || path.find_first_of(directory_dividers) == 0;
}

// add a relative path to an absolute path
std::string	pathadd(const std::string &base,std::string &add)
{
	//this function can handle quite strange relative paths ;)
	//base must contain last slash if only dir
	//abs path in add overrides base
	if (pathisabsolute(add)) return add;
	char baseslash = pathgetdirslash(base);
	std::string s = pathgetdir(base);
	do {
		if (add.compare(0,2,"..") == 0) {
			// eval ../
			add = add.substr(3);
			s = pathgetdir(s);
		} else if (add.compare(0,1,".") == 0) {
			// eval ./
			add = add.substr(2);
		} else {
			// eval mydir/
			int pos = add.find_first_of(directory_dividers);
			if (pos == add.npos) break; // only filename left
			s += baseslash + add.substr(0,pos);
			add = add.substr(pos+1);
		}
	} while (1) ;
	return s + baseslash + add;
}

// test if one abs-path is inside another
bool pathissubpath(const std::string &base,std::string &path)
{
	// pathissubpath("C:\\a\\b\\","C:/a/b/d") true
	// pathissubpath("C:\\a\\b\\","C:/a/c") false
	std::string b = base,p = path;
	do {
		int posa = b.find_first_of(directory_dividers);
		int posb = p.find_first_of(directory_dividers);
		if (posa == b.npos) return true;		// pathissubpath("C:/","C:/a") true
		if (posb == p.npos) return false;	// pathissubpath("C:/a","C:/") false
		if (b.compare(0,posa,p,0,posb) != 0) return false; // pathissubpath("C:/a","C:/b") false
		b = b.substr(posa+1);
		p = p.substr(posb+1);
	} while (1) ;
}
