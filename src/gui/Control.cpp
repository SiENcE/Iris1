//
// File: Control.cpp
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

#include "gui/Control.h"
#include "gui/Container.h"

#include "loaders/GumpLoader.h"
#include "loaders/HueLoader.h"




Control::Control()
{
	x = 0;
	y = 0;
	z = 0;
	focus = 0;
	alpha = 255;
	fade = false;
	id = 0;
	width = 0;
	height = 0;
	next = NULL;
	tag = NULL;
	control_type = CONTROLTYPE_UNKNOWN;
	flags = GUMPFLAG_MOVABLE | GUMPFLAG_CLOSABLE | GUMPFLAG_FOCUSABLE;
	callback_OnClose = NULL;
	callback_OnMouseUp = NULL;
	callback_OnMouseDown = NULL;
	parent = NULL;
	__page = 0;

	script_funcs = (char **) malloc( SCRIPT_FUNC_COUNT * sizeof(char *));
	int i;
	for ( i = 0; i < SCRIPT_FUNC_COUNT; i++ )
	{
		script_funcs[i] = NULL;
	}
	for ( i = 0; i < 5; i++ )
	{
		_data[i] = 0;
	}
}

Control::~Control()
{
	for (int i = 0; i < SCRIPT_FUNC_COUNT; i++)
	{
		SAFE_DELETE_ARRAY( script_funcs[i] );
	}
	
	free( script_funcs );
}

void Control::SetPosition (int x, int y)
{
	this->x = x;
	this->y = y;
}

void Control::SetSize (int width, int height)
{
  this->width = width;
  this->height = height;
}

void Control::SetX (int x)
{
  this->x = x;
}

void Control::SetY (int y)
{
  this->y = y;
}

void Control::SetZ (int z)
{
  this->z = z;
}

void Control::SetID (int id)
{
  this->id = id;
}

void Control::SetFocus (int focus)
{
  this->focus = focus;
}

void Control::SetFlags (int flags)
{
  this->flags = flags;
}

void Control::SetTag (void *tag)
{
  this->tag = tag;
}

void Control::SetFlag( int flag, int value )
{
  switch (flag)
      {
      case GUMPFLAG_MOVABLE:
      case GUMPFLAG_CLOSABLE:
      case GUMPFLAG_FOCUSABLE:
        flags &= !flag;
        if (value)
          flags |= flag;

      default:
        break;
      }
}

void Control::SetAlpha (unsigned char alpha)
{
  this->alpha = alpha;
}

void Control::SetParent (Container * cont)
{
  parent = cont;
}

void Control::RemoveFlag( int flag )
{
	flags ^= flag;
}

Container *Control::GetParent (void)
{
  return parent;
}

void Control::GetPosition (int *x, int *y)
{
  *x = this->x;
  *y = this->y;
}

void Control::OnMouseDown (int (*callback) (Control * sender))
{
  callback_OnMouseDown = callback;
}

void Control::OnMouseUp (int (*callback) (Control * sender))
{
  callback_OnMouseUp = callback;
}

void Control::OnClose (int (*callback) (Control * sender))
{
  callback_OnClose = callback;
}

void Control::DoOnClose (void)
{
  if (callback_OnClose)
    callback_OnClose (this);
}

void Control::DoOnMouseUp (void)
{
  if (callback_OnMouseUp)
    callback_OnMouseUp (this);
}

void Control::DoOnMouseDown (void)
{
  if (callback_OnMouseDown)
    callback_OnMouseDown (this);
}

int Control::GetX (void)
{
  return x;
}
int Control::GetY (void)
{
  return y;
}
int Control::GetZ (void)
{
  return z;
}
int Control::GetID (void)
{
  return id;
}
int Control::GetFocus (void)
{
  return focus;
}
int Control::GetWidth (void)
{
  return width;
}
int Control::GetHeight (void)
{
  return height;
}

int Control::GetFlags (void)
{
  return flags;
}

int Control::GetFlag (int flag)
{
  return ((flags & flag) != 0);
}
unsigned char Control::GetAlpha (void)
{
  return alpha;
}

void *Control::GetTag (void)
{
  return tag;
}


bool Control::MouseIsOver (int x, int y)
{
  if ((x >= this->x) && (y >= this->y) && (x < this->x + this->width)
      && (y < this->y + this->height))
      {

        return CheckPixel (x - this->x, y - this->y);
      }
  return false;
}

int Control::HandleMessage (gui_message * msg)
{
  gui_message new_message;

  if (!msg)
      {
        Logger::WriteLine ("NULL msg in Control::HandleMessage(gui_message *)",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return (false);
      }

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y))
            {
              stack.Push (CreateCallbackMessage (CALLBACK_ONMOUSEDOWN));


              if ((msg->mouseevent.button == SDL_BUTTON_LEFT)
                  && (flags & GUMPFLAG_FOCUSABLE) && (!focus))
                  {
                    new_message.type = MESSAGE_SETFOCUS;
                    new_message.windowaction.controlid = id;
                    stack.Push (new_message);
                    new_message.type = MESSAGE_BRINGTOFRONT;
                    new_message.windowaction.controlid = id;
                    stack.Push (new_message);
                    return true;
                  }

              if ((msg->mouseevent.button == SDL_BUTTON_RIGHT)
                  && (flags & GUMPFLAG_CLOSABLE))
                  {
                    new_message.type = MESSAGE_CLOSEGUMP;
                    new_message.windowaction.controlid = id;
                    stack.Push (new_message);
                    return true;
                  }
            }
        else if (focus)
            {
              new_message.type = MESSAGE_RELEASEFOCUS;
              new_message.windowaction.controlid = id;
              stack.Push (new_message);
            }
        break;
      case MESSAGE_MOUSEMOTION:
        if ((focus) && (msg->mousemotionevent.button)
            && (flags & GUMPFLAG_MOVABLE))
            {
              x += msg->mousemotionevent.relx;
              y += msg->mousemotionevent.rely;
              return true;
            }
        break;
      case MESSAGE_MOUSEUP:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y))
            {
              stack.Push (CreateCallbackMessage (CALLBACK_ONMOUSEUP));
              return true;
            }
        break;
      }
  return false;
}

void Control::Draw (GumpHandler * gumps)
{
  gui_counter++;
  if (fade)
      {
        unsigned int ticks = SDL_GetTicks ();
        if (ticks > (fade_start + fade_time))
            {
              SetAlpha (fade_to);
              fade = false;
            }
        else
            {
              float new_alpha =
                (float) (fade_to -
                         fade_from) / (float) fade_time * (float) (ticks -
                                                                   fade_start);
              SetAlpha ((int) (fade_from + new_alpha));
            }
      }
  return;
}

void Control::DrawRectHued (int x, int y, int width, int height,
                            Texture * texture, int hue, unsigned char alpha,
                            bool swapy)
{
  if (!texture)
    return;

  if ((texture->GetWidth () <= 0) || (texture->GetHeight () <= 0))
    return;

  if (width <= 0)
    width = texture->GetWidth ();

  if (height <= 0)
    height = texture->GetHeight ();

  glBindTexture (GL_TEXTURE_2D, texture->GetGLTex ());

  glBegin (GL_QUADS);

  int y1 = Config::GetHeight() - 1 - (y);
  int y2 = Config::GetHeight() - 1 - (y + height);

  int x1 = x;
  int x2 = x + width;

  if (swapy)
      {
        int dummy = y1;
        y1 = y2;
        y2 = dummy;
      }
  float r, b, g;
  pHueLoader.GetRGBHue (hue, r, g, b);
  float u = (float) width / (float) texture->GetWidth ();
  float v = (float) height / (float) texture->GetHeight ();

  glColor4f (r, g, b, (float) alpha / 255.0);
  glTexCoord2f (0.0, 0.0);
  glVertex2i (x1, y1);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (0.0, v);
  glVertex2i (x1, y2);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (u, v);
  glVertex2i (x2, y2);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (u, 0.0);
  glVertex2i (x2, y1);

  glEnd ();

  glColor4f (1.0, 1.0, 1.0, 1.0);
}

void Control::DrawRect (int x, int y, int width, int height,
                        Texture * texture, unsigned char alpha, bool swapy)
{
  if (!texture)
    return;

  if ((texture->GetWidth () <= 0) || (texture->GetHeight () <= 0))
    return;

  if (width <= 0)
    width = texture->GetWidth ();

  if (height <= 0)
    height = texture->GetHeight ();

  glBindTexture (GL_TEXTURE_2D, texture->GetGLTex ());

  glBegin (GL_QUADS);

  int y1 = Config::GetHeight() - 1 - (y);
  int y2 = Config::GetHeight() - 1 - (y + height);

  int x1 = x;
  int x2 = x + width;

  if (swapy)
      {
        int dummy = y1;
        y1 = y2;
        y2 = dummy;
      }

  float u = (float) width / (float) texture->GetWidth ();
  float v = (float) height / (float) texture->GetHeight ();

  glColor4f (1.0, 1.0, 1.0, (float) alpha / 255.0);
  glTexCoord2f (0.0, 0.0);
  glVertex2i (x1, y1);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (0.0, v);
  glVertex2i (x1, y2);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (u, v);
  glVertex2i (x2, y2);

  //glColor4b( 255, 255, 255, alpha );
  glTexCoord2f (u, 0.0);
  glVertex2i (x2, y1);

  glEnd ();

  glColor4f (1.0, 1.0, 1.0, 1.0);
}

void Control::DrawRect (int x, int y, int width, int height,
                        Texture * texture)
{
  DrawRect (x, y, width, height, texture, 255);
}

void Control::DrawRect (int x, int y, Texture * texture)
{
  DrawRect (x, y, 0, 0, texture);
}

Texture *Control::LoadGump (int id, GumpHandler * gumps, bool tileable)
{
  Texture *texture = gumps->GetTexture (id);

  if ((!texture) && (id < 65536))
      {
        texture = pGumpLoader.LoadGump (id);
        if (!texture)
          texture = new Texture;
        gumps->AddTexture (id, texture);
      }
  return texture;
}

Texture *Control::GetTiledGump (unsigned short id, short width, short height)
{
  // These can't be cached = very bad
  //unsigned int gHeight, gWidth;

  /*unsigned int *srcData = pUOLib.LoadGump(id, gWidth, gHeight, 0, true);

     if(width <= 0)
     width = gWidth;

     if(height <= 0)
     height = gHeight;

     // Now determin the "realsize"
     unsigned short tWidth = 64;
     unsigned short tHeight = 64;

     while (tWidth < width)
     tWidth *= 2;

     while (tHeight < height)
     tHeight *= 2;

     // RGBA
     unsigned int *data = new unsigned int[tHeight * tWidth];

     memset(data, 0, tHeight * tWidth * 4);

     // Fill the image row-by-row
     for (int y = 0; y < height; ++y)
     for (int x = 0; x < width; ++x) {
     // x and y are the absolute offsets for the *TARGET*
     // the offsets for the source are x % sourcewidth, y % sourcewidth
     unsigned int srcX = x % gWidth;
     unsigned int srcY = y % gHeight;

     if(srcData[srcY * gWidth + srcX] == 0
     || srcData[srcY * gWidth + srcX] == 0xFF000000)
     continue;

     // Copy it over from the sprite RGBA data
     data[y * tWidth + x] = srcData[srcY * gWidth + srcX];
     }

     delete srcData;

     Texture *texture = new Texture;

     texture->LoadFromData(data, tWidth, tHeight, 32, GL_NEAREST, true);

     delete data;

     return texture; */
  return NULL;
}

void Control::Close (void)
{
  gui_message new_message;

  new_message.type = MESSAGE_CLOSEGUMP;
  new_message.windowaction.controlid = id;
  stack.Push (new_message);
}

void Control::SetPage (int p)
{
  __page = p;
}

int Control::GetPage (void)
{
  return __page;
}


void Control::SetScriptFunction( int id, char *funcname )
{
	if ( ( id >= 0 ) && ( id < SCRIPT_FUNC_COUNT ) )
	{
		SAFE_DELETE_ARRAY( script_funcs[id] );

		if ( funcname )
		{
			int len = strlen( funcname );
			script_funcs[id] = new char[len + 1];
			memcpy( script_funcs[id], funcname, len + 1 );
		}
	}
}


char *Control::GetScriptFunction (int id)
{
  if ((id >= 0) && (id < SCRIPT_FUNC_COUNT))
    return script_funcs[id];
  return NULL;
}


int Control::GetData (int slot)
{
  if ((slot >= 0) && (slot < 5))
    return _data[slot];
  return 0;
}


void Control::SetData (int slot, int tag)
{
  if ((slot >= 0) && (slot < 5))
    _data[slot] = tag;
}

void Control::FadeTo (unsigned char to_alpha, unsigned int fade_time)
{
  if (fade_time == 0)
    return;
  this->fade = true;
  this->fade_to = to_alpha;
  this->fade_time = fade_time;
  this->fade_start = SDL_GetTicks ();
  this->fade_from = alpha;
}

bool Control::CheckPixel (int x, int y)
{
  return true;
}

Uint32 Control::FindDragContainer (int x, int y, int *drop_x, int *drop_y,
                                   Uint32 * charid)
{
  return 0;
}


gui_message Control::CreateCallbackMessage (int type)
{
  gui_message msg;
  msg.type = MESSAGE_CALLBACK;
  msg.callback.callback_type = type;
  msg.callback.id = GetID ();
  if (parent)
    msg.callback.containerid = parent->GetID ();
  else
    msg.callback.containerid = 0;
  msg.callback.key = 0;
  return msg;
}
