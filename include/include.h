//
// File: include.h
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

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#define NODE_TOLERANCE 0.01
#define QUADS_PER_NODE 24

const float PI = 3.14159f;
//! Constant for converting bettween degrees and radiants.
const double GRAD_PI = 180.0 / 3.1415926535897932384626433832795;
const float ROUNDING_ERROR = 0.0001f;
//! Constant for converting bettween degrees and radiants.
const double GRAD_PI2 = 3.1415926535897932384626433832795 / 180.0;

#define QUADFLAG_BACKFACE 1
#define QUADFLAG_TRIANGLE 2
#define QUADFLAG_NEARONLY 4

#define QUADFLAG_CAN_BE_MERGED 4096

#define MODELFLAG_TILEABLE    1

// Time in Milliseconds to determine a double click
#define CLICK_TIME 300

//#define	MSL	512
//disabled because 512 as GumpBuffer is to small

#define	MSL	4096


#define WALK_BUFFER 50
#define WALK_INTERVAL 20


#define PICK_BUFSIZE 256

// Win32 related
#ifdef	WIN32
#define strcasecmp	strcmpi
#endif

extern int BLOCKRESOLUTION;
extern int MAPRESOLUTION;
extern int FOGDISTANCE;
extern int VIEWDISTANCE;

#define GAMESTATE_MENU  0
#define GAMESTATE_GAME  1

#define ROOF_NONE 1000
#define ROOF_WAIT 1001

#endif //_INCLUDE_H_

