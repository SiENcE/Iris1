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

#ifndef _IRIS_ENDIAN_H_
#define _IRIS_ENDIAN_H_

#include "SDL/SDL_endian.h"
//#include "Game.h"
//#include "posh.h"

Uint32	IRIS_SwapU32( Uint32 val);
Sint32	IRIS_SwapI32( Sint32 val);
Uint16	IRIS_SwapU16( Uint16 val);
Sint16	IRIS_SwapI16( Sint16 val);
float   IRIS_FloatFromLittle( float val);

#endif
