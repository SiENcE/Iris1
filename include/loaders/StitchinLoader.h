//
// File: StitchinLoader.h
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#ifndef _STITCHINLOADER_H_
#define _STITCHINLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include "include.h"

// #include "../Fluid/mmgr.h"

class cModelEntry
{
   private:
     int m_id;
     int m_type;
     int m_anim;
     unsigned int m_defhue;
     std::string m_name;
	int coverlist[13];
     std::vector<int> removelist;
     std::map<int, int> replacelist;
     
   public:
     //cModelEntry (int id, int type, int anim, unsigned int defhue, std::string name);
     cModelEntry (int id, int * clist, std::vector<int> rlist, std::map<int, int> replist);
     int * GetCovers () { return coverlist; }
     std::vector<int> & GetRemoveList() { return removelist; }
     std::map<int, int> & GetReplaceList() { return replacelist; }
};

class cStitchinLoader
{
protected:
    std::map <int, cModelEntry *> models;
public:
    cStitchinLoader ();
    ~cStitchinLoader ();

    void Init (std::string modelfilename, std::string stitchindeffilename);
    void DeInit ();
    
    cModelEntry * GetModel(int id);
};

extern cStitchinLoader pStitchinLoader;

#endif //_STITCHINLOADER_H_
