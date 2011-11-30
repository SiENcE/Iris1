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

#ifndef UOTYPE_H
#define UOTYPE_H

/**
   This file contains structures and definitions which are important to handle uo connectivity
   For more information, please take a look krios' file format reference
   http://doc.wpdev.org/formats/
*/

// Compiler directives
#ifdef WIN32
#pragma pack(push, 1)
#endif

#if  	defined(WIN32) && !defined(__MINGW32__)
// Visual C pragma
#define STRUCT_PACKED
#else
// GCC packed attribute
#define STRUCT_PACKED	__attribute__ ((packed))
#endif

/*
// Compiler directives
#ifdef WIN32
#pragma pack(1)
#endif
#if  	defined(WIN32) && !defined(__MINGW32__)
// Visual C pragma
#define STRUCT_PACKED
#else
// GCC packed attribute
#define STRUCT_PACKED	__attribute__ ((packed))
#endif
*/

#include "iris_endian.h"

/** UO has a 15bit color model - we need 32bit */
inline unsigned int color15to32( unsigned short color )
{
	if( !color )
		return color;

	unsigned char r = ( ( color >> 10 ) & 0x1F );
	unsigned char g = ( ( color >> 5 ) & 0x1F );
	unsigned char b = ( color & 0x1F );


#if SDL_BYTEORDER==SDL_BIG_ENDIAN
	unsigned int color32 = (( r * 8 ) << 24) | ( ( g * 8 ) << 16 ) | ( ( b * 8 ) << 8) | 0x000000FF;
#else
	unsigned int color32 = ( r * 8 ) | ( ( g * 8 ) << 8 ) | ( ( b * 8 ) << 16 ) | 0xFF000000;
#endif

	return color32;
}


/** 
   an index record - used in uo's .idx files
*/
struct stIndexRecord
{
  unsigned int offset;
  unsigned int length;
  unsigned int extra;
};


/** struct for staticsX.mul - contains information about static tiles */
struct staticentry {
   unsigned short TileID : 16;
   char x : 8;
   char y : 8;
   signed char z : 8;
   short Hue : 16;
}  STRUCT_PACKED;


/** struct for mapX.mul - contains information about ground tiles */
struct MulCell {
 unsigned short tileid;
 signed char z;
} STRUCT_PACKED;

struct MulBlock {
 struct MulCell cells[8][8];
} STRUCT_PACKED;


/** struct for verdata.mul */
struct VerDataEntry {
 unsigned int fileid;
 unsigned int blockid;
 unsigned int start;
 unsigned int len;
 unsigned int extra;
} STRUCT_PACKED;


/** struct for tiledata.mul */
struct TileDataStaticEntry {
 unsigned int flags;
 unsigned char weight;
 unsigned char quality;
 unsigned short unknown;
 unsigned char unknown1;
 unsigned char quantity;
 unsigned short animid;
 unsigned char unknown2;
 unsigned char hue;
 unsigned short unknown3;
 unsigned char height;
 char name[20];
} STRUCT_PACKED;



struct FrameInfo2D {
	float u1, v1, u2, v2;
	int height, width;
	int centerx, centery;
};

/* Used by iris' static patches - depreciated*/
struct staticinfo {
   unsigned short TileID : 16;
   char x : 8;
   char y : 8;
   signed char z : 8;
   short Hue : 16;
   unsigned int obj_id : 32;
}  STRUCT_PACKED;


struct StaticPatch {
	unsigned char type;
	unsigned int obj_id;
	unsigned short tileid;
	unsigned char x, y;
	unsigned short blockx, blocky;
	signed short z;
} STRUCT_PACKED;

/* struct for skills.mul */

struct skill_entry {
	Uint16 skill, unmodified, skillcap;
	Uint8 skilllock;
} STRUCT_PACKED;

struct skill_name {
	char * name;
	bool use_button;
};


/* helper structs */

struct script_Playerinfo {
	unsigned int count;
	struct
	{
		char	m_name[30];
		char	m_pass[30];
	} m_names[5];
};

struct clothinginfo {
  unsigned int serial;
  unsigned short body;
  unsigned char layer;
  unsigned short hue;
  unsigned short animid;
  unsigned int flags;
};

struct status_info {
	Uint32 id;
	char name[30];
	Uint16 hits, maxhits;
	bool namechange;
	Uint8 sex;
	Uint16 m_str, m_dex, m_int;
	Uint16 stamina, maxstamina;
	Uint16 mana, maxmana;
	Uint32 gold;
	Uint16 armor;
	Uint16 weight;
};


/** generic color struct for lighting */

struct	sColorRGB
{
   Uint8 r, g, b, a;
} STRUCT_PACKED;


struct	sColor
{
	union
	{
            Uint32 color32;
            sColorRGB colorRGB;
            Uint8 color_array[4];
        };
} STRUCT_PACKED;


/** Tiledata flags - take a look at krios' file formats */
#define TILEDATAFLAG_BACKGROUND    0x00000001
#define TILEDATAFLAG_WEAPON        0x00000002
#define TILEDATAFLAG_TRANSPARENT   0x00000004
#define TILEDATAFLAG_TRANSLUCENT   0x00000008
#define TILEDATAFLAG_WALL          0x00000010
#define TILEDATAFLAG_DAMAGING      0x00000020
#define TILEDATAFLAG_IMPASSABLE    0x00000040
#define TILEDATAFLAG_WET           0x00000080
#define TILEDATAFLAG_UNKNOWN1      0x00000100
#define TILEDATAFLAG_SURFACE       0x00000200
#define TILEDATAFLAG_BRIDGE        0x00000400
#define TILEDATAFLAG_GENERIC       0x00000800
#define TILEDATAFLAG_WINDOW        0x00001000
#define TILEDATAFLAG_NOSHOOT       0x00002000
#define TILEDATAFLAG_ARTICLEA      0x00004000
#define TILEDATAFLAG_ARTICLEAN     0x00008000
#define TILEDATAFLAG_INTERNAL      0x00010000
#define TILEDATAFLAG_FOLIAGE       0x00020000
#define TILEDATAFLAG_PARTIALHUE    0x00040000
#define TILEDATAFLAG_UNKNOWN2      0x00080000
#define TILEDATAFLAG_MAP           0x00100000
#define TILEDATAFLAG_CONTAINER     0x00200000
#define TILEDATAFLAG_WEARABLE      0x00400000
#define TILEDATAFLAG_LIGHTSOURCE   0x00800000
#define TILEDATAFLAG_ANIMATION     0x01000000
#define TILEDATAFLAG_NODIAGONAL    0x02000000
#define TILEDATAFLAG_UNKNOWN3      0x04000000
#define TILEDATAFLAG_ARMOR         0x08000000
#define TILEDATAFLAG_ROOF          0x10000000
#define TILEDATAFLAG_DOOR          0x20000000
#define TILEDATAFLAG_STAIRBACK     0x40000000
#define TILEDATAFLAG_STAIRRIGHT    0x80000000

/* Used by iris' static patches - depreciated*/
#define PATCH_DELETE   1
#define PATCH_ADD      2

/* Script definitions */
#define NETERROR_UNKNOWN           0
#define NETERROR_SOCKET            1
#define NETERROR_UNKNOWNHOST       2
#define NETERROR_NOCONNECTION      3
#define NETERROR_CONNECTIONABORTED 4
#define NETERROR_UNKNOWNUSER       5
#define NETERROR_ACCOUNTINUSE      6
#define NETERROR_ACCOUNTBLOCKED    7
#define NETERROR_BADPASSWORD       8

#define REQUEST_BASIC 0
#define REQUEST_SKILLS 1

#define ACTIONTYPE_SKILL    0
#define ACTIONTYPE_SPELL    1
#define ACTIONTYPE_OPENDOOR 2
#define ACTIONTYPE_BOW      3
#define ACTIONTYPE_SALUTE   4

#define VERDATAPATCH_ART  0x4
#define VERDATAPATCH_GUMP 0xC

#define TARGET_ID 0
#define TARGET_XYZ 1

#define GAMEMODEL_VERSION   2

#ifdef WIN32
#pragma pack(pop)
#endif

#endif
