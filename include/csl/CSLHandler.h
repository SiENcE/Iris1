//
// File: CSLHandler.h
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

#ifndef _CSLHANDLER_H_
#define _CSLHANDLER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#if ZC_WIN
  #include <csl/ZCslWrap.hpp>
#else
  #include <csl/ZCsl.hpp>
#endif

// #include "../Fluid/mmgr.h"


class CSLHandler
{
private:


public:
   CSLHandler ();
   virtual ~CSLHandler ();

   void Init();
   void DeInit();
   void Load(char * filename);

   void InitAPI(void);
   void InitNet(void);

   int ExecuteFunction(char * funcname, char * p1 = NULL, char * p2 = NULL, char * p3 = NULL, char * p4 = NULL);
      int ExecuteFunctionPlus(char * funcname, char * p1 = NULL, char * p2 = NULL, char * p3 = NULL, char * p4 = NULL, char * p5 = NULL, char * p6 = NULL, char * p7 = NULL, char * p8 = NULL);
protected:
};

extern CSLHandler pCSLHandler;
extern int gamestate;

#endif //_CSLHANDLER_H_
