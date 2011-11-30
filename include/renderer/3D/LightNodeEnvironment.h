//
// File: LightNodeEnvironment.h
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

/*
   This Module handles 3x3 blocks of lightnodes in order to prevent the lighting "borders"
*/
 
#ifndef _LIGHTNODEENVIRONMENT_H_
#define _LIGHTNODEENVIRONMENT_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include "LightNodes.h"


// #include "../Fluid/mmgr.h"

class cLightNodeEnvironment {
    private:
        cLightNodeHandler * m_nodes[3][3];  // a 3x3 array of nodes which contains the nodelist of the block and all 8 surrounding light node lists
    public:        
        cLightNodeEnvironment ();
        ~cLightNodeEnvironment ();
        void set (int x, int y, cLightNodeHandler * nodes); // x and y are element of (-1, 0, 1)
        cLightNodeHandler * get (int x, int y); // x and y are element of (-1, 0, 1)
};

 
#endif //_LIGHTNODEENVIRONMENT_H_
