//
// File: Control.h
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

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "SDL/SDL.h"
#include <map>
#include <list>
#include "../renderer/Texture.h"
#include "Stack.h"
#include "Gump.h"
#include "Logger.h"
#include "Config.h"

// #include "../Fluid/mmgr.h"


#define GUMPFLAG_MOVABLE 1
#define GUMPFLAG_CLOSABLE 2
#define GUMPFLAG_FOCUSABLE 4
#define GUMPFLAG_ALPHA 8

#define SCRIPT_FUNC_COUNT 10
#define FUNC_ONCLICK      0
#define FUNC_ONCLOSE      1
#define FUNC_ONMOUSEUP    2
#define FUNC_ONMOUSEDOWN  3
#define FUNC_ONKEYPRESSED  4

#define CONTROLTYPE_UNKNOWN       0
#define CONTROLTYPE_BORDER        1
#define CONTROLTYPE_CHECKBOX      2
#define CONTROLTYPE_CONTAINER     3
#define CONTROLTYPE_IMAGE         4
#define CONTROLTYPE_INPUTFIELD    5
#define CONTROLTYPE_LABEL         6
#define CONTROLTYPE_RADIOBUTTON   7
#define CONTROLTYPE_TEXTBOX       8
#define CONTROLTYPE_BUTTON        9
#define CONTROLTYPE_IMAGEART      10
#define CONTROLTYPE_ITEMCONTAINER 11
#define CONTROLTYPE_PAPERDOLL     12
#define CONTROLTYPE_MULTILABEL    13


class Container;

class Control
{
public:
	Control();
	virtual ~Control();

	void SetPosition( int x, int y );
	void SetSize( int width, int height );
	void SetX( int x );
	void SetY( int y );
	void SetZ( int z );
	void SetFlags( int flags );
	void SetFlag( int flag, int value );
	void SetID( int id );
	void SetFocus( int focus );
	virtual void SetAlpha( unsigned char alpha );
	void SetTag( void *tag );
	void SetData( int slot, int tag );
	void SetParent( Container * );

	void RemoveFlag( int flag );
	
	Container *GetParent( void );
	
	void GetPosition( int * x, int * y );
	
	int GetX( void );
	int GetY( void );
	int GetZ( void );
	int GetID( void );
	int GetFocus( void );
	int GetWidth( void );
	int GetHeight( void );
	int GetFlags( void );
	int GetFlag( int flag );

	void *GetTag( void );
	unsigned char GetAlpha( void );
	int GetData( int slot );

	virtual bool MouseIsOver( int x, int y );

	virtual int HandleMessage( gui_message *msg );
	virtual void Draw( GumpHandler *gumps );

	void DrawRectHued( int x, int y, int width, int height, Texture * texture, int hue, unsigned char alpha, bool swapy = false );
	void DrawRect( int x, int y, int width, int height, Texture * texture, unsigned char alpha, bool swapy = false );
	void DrawRect( int x, int y, int width, int height, Texture * texture );
	void DrawRect( int x, int y, Texture * texture );
	Texture *LoadGump( int id, GumpHandler * gumps, bool tileable );
	Texture *GetTiledGump( unsigned short id, short width, short height );

	void OnMouseDown( int (*callback)(Control *sender) );
	void OnMouseUp( int (*callback)(Control *sender) );
	void OnClose( int (*callback)(Control *sender) );

	void FadeTo( unsigned char to_alpha, unsigned int fade_time );

	void SetScriptFunction( int id, char *funcname );
	char *GetScriptFunction( int id );

	void DoOnClose( void );
	void DoOnMouseUp( void );
	void DoOnMouseDown( void );
	void Close( void );

	void SetPage( int );
	int GetPage();

	int getType( void ) { return control_type; }

	Control *next;
	MessageStack stack;

	virtual bool CheckPixel( int x, int y );
	virtual Uint32 FindDragContainer( int x, int y, int *drop_x, int *drop_y, Uint32 *charid );
  
	gui_message CreateCallbackMessage( int type );

protected:
	int x, y, z;
	int width, height;
	int id;
	int focus;
	int flags;
	unsigned char alpha;
	void *tag;
	Container *parent;
	int control_type;
  
	bool fade;
	unsigned int fade_to;
	unsigned int fade_from;
	unsigned int fade_time;
	unsigned int fade_start;

	int (*callback_OnClose)(Control *sender);
	int (*callback_OnMouseDown)(Control *sender);
	int (*callback_OnMouseUp)(Control *sender);

private:
	int gui_counter;
	char **script_funcs;
	int _data[5];
	int __page;
};

typedef std::map<Uint32, Control *>	ControlList_t;

#endif
