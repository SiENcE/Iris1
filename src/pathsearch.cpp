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

#include "pathsearch.h"
#include "robstring1.2.h"

#include <vector>
#include <string>
#include "stdio.h"
/// TODO : check platform ifdefs ?

#ifdef WIN32
// TODO : #elseif MAC or something like that
// TODO : #elseif LINUX or something like that
#else
	// this is the linux code
	#include <dirent.h>
#endif


/// attempts to search for an equivalent path on case-sensitive file systems
std::string rob_pathsearch (const std::string& sOldPath)
{
	//printf("pathsearch(%s)\n",sOldPath.c_str());
	
	std::string res;
	std::string test;
	std::string pathpart;
	std::vector<std::string> pathparts;
	std::vector<std::string> contents;
	explodestr("/",sOldPath.c_str(),pathparts);

	unsigned int i;
	for (i=0;i<pathparts.size();++i) {
		pathpart = pathparts[i];
		test = strprintf("%s/%s",res.c_str(),pathpart.c_str());
		if (i==pathparts.size()-1) {
			// last part of path is filename 
			if (rob_fileexists(test.c_str())) {
				return test;
			} else {
				test = rob_dirfindi(res.c_str(),pathpart,false,true);
				if (test.size() == 0) return ""; // nothing found
				return strprintf("%s/%s",res.c_str(),test.c_str()); // success !
			}
		} else {
			// directory
			if (rob_direxists(test.c_str())) {
				res = test;
			} else {
				// quick search : capitalize first letter
				pathpart[0] = toupper(pathpart[0]);
				test = strprintf("%s/%s",res.c_str(),pathpart.c_str());
				if (rob_direxists(test.c_str())) {
					// quick search success
					res = test;
				} else {
					// long search : list directory contents and compare case insensitive
					test = rob_dirfindi(res.c_str(),pathpart,true,false);
					if (test.size() == 0) return ""; // nothing found
					res = strprintf("%s/%s",res.c_str(),test.c_str()); // success !
				}
			}
		}
	}
	return "";
}

bool rob_direxists(const char* path)
{
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
	// this is the linux code
		DIR* d = opendir(path);
		if (d) { closedir(d); return true; }
	#endif
	return false;
}

bool rob_fileexists(const char* path)
{
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
	// this is the linux code
		FILE* f = fopen(path,"r");
		if (f) { fclose(f); return true; }
	#endif
	return false;
}

void rob_dirlist(const char* path,std::vector<std::string>& res,const bool bDirs,const bool bFiles)
{
	#ifdef WIN32
	// TODO : #elseif MAC or something like that
	// TODO : #elseif LINUX or something like that
	#else
		// this is the linux code
		DIR *d = opendir(path);
		if (!d) return;
		struct dirent *e;
		e = readdir(d);
		while (e != NULL) {
			if ((bDirs && e->d_type == DT_DIR) ||
				(bFiles && e->d_type != DT_DIR)) {
				res.push_back(std::string(e->d_name));
			}
			e = readdir(d);
		}
		closedir(d);
	#endif
}

std::string rob_dirfindi(const char* path,const std::string& name,const bool bDirs,const bool bFiles)
{
	std::vector<std::string> filelist;
	std::string lowername = strtolower(name.c_str());
	rob_dirlist(path,filelist,bDirs,bFiles);
	unsigned int i;
	for (i=0;i<filelist.size();++i) if (strtolower(filelist[i].c_str()) == lowername) return filelist[i];
	return "";
}
