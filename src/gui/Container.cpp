//
// File: Container.cpp
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

#include "gui/Container.h"
#include "Logger.h"
#include "Config.h"




using namespace std;

Container::Container ()
{
  Control::Control ();
  idcounter = 1;
  search_index = 0;
  focusid = 0;
  current_page = 0;
  shape_id = 0;
  control_type = CONTROLTYPE_CONTAINER;
}

Container::~Container()
{
	ClearControls();
}


void Container::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);
  glPushMatrix ();
  glTranslatef( (GLfloat)GetX(), (GLfloat)-GetY(), 0.0 );

  ControlList_t::iterator iter;

  for (iter = control_root.begin (); iter != control_root.end (); iter++)
      {
        if (iter->second->GetPage () == current_page
            || iter->second->GetPage () == 0)
            {
              iter->second->Draw (gumps);
            }
      }

  glPopMatrix ();
}

int Container::HandleMessage (gui_message * msg)
{
  gui_message new_message;

  if (!msg)
      {
        Logger::WriteLine ("NULL msg in Container::HandleMessage(gui_message *)",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return (false);
      }

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
      case MESSAGE_MOUSEUP:
        new_message = *msg;
        new_message.mouseevent.x -= GetX ();
        new_message.mouseevent.y -= GetY ();

        if (SendMessageToItems (&new_message))
            {
              HandleMessageQueues ();
              return true;
            }

        break;
      case MESSAGE_MOUSEMOTION:
        new_message = *msg;
        new_message.mousemotionevent.x -= GetX ();
        new_message.mousemotionevent.y -= GetY ();

        if (SendMessageToItems (&new_message))
            {
              HandleMessageQueues ();
              return true;
            }

        break;
      case MESSAGE_KEYPRESSED:
      case MESSAGE_REBUILDITEMCONTAINER:
      case MESSAGE_ADDCONTAINERITEM:
      case MESSAGE_UPDATEPAPERDOLL:
      case MESSAGE_DOREFRESH:
        SendMessageToItems (msg);
      }

  int result = Control::HandleMessage (msg);

  HandleMessageQueues ();

  return result;
}

void Container::HandleMessageQueues (void)
{
  MessageStack cont_stack;
  gui_message msg;

  ControlList_t::iterator iter;
  for (iter = control_root.begin (); iter != control_root.end (); iter++)
    while (iter->second->stack.Pop (&msg))
      cont_stack.Push (msg);

  while (cont_stack.Pop (&msg))
    switch (msg.type)
        {
        case MESSAGE_SETFOCUS:
          SetFocus (msg.windowaction.controlid);
          break;
        case MESSAGE_RELEASEFOCUS:
          ReleaseFocus (msg.windowaction.controlid);
          break;
        case MESSAGE_QUIT:
        case MESSAGE_STARTGAME:
        case MESSAGE_ONDRAGITEM:
        case MESSAGE_ONCLICKITEM:
        case MESSAGE_REFRESHREQUEST:
        case MESSAGE_CALLBACK:
          stack.Push (msg);
          break;
        }
}

void Container::ClearControls(void)
{
	ControlList_t::iterator iter;

	for ( iter = control_root.begin (); iter != control_root.end (); iter++ )
	{
		iter->second->DoOnClose();
		SAFE_DELETE( (*iter).second );
	}

	control_root.clear ();
}

Control *Container::GetControl (int controlid)
{
  ControlList_t::iterator iter;

  iter = control_root.find ((Uint32) controlid);
  if (iter == control_root.end ())
    return NULL;
  else
    return (*iter).second;
}

void Container::AddControl( Control *control )
{
	control->SetID( idcounter | ( GetID() << 16 ) );
	control->SetParent( this );
	control_root.insert( std::make_pair( (Uint32)idcounter | ( GetID() << 16 ), control ) );
	idcounter++;
}

void Container::AddControl (Control * control, int page)
{
  control->SetPage (page);
  AddControl (control);
}


int Container::SendMessageToItems (gui_message * msg)
{
  ControlList_t::reverse_iterator iter;
  for (iter = control_root.rbegin (); iter != control_root.rend (); iter++)
      {
        if ((iter->second->GetPage () == current_page)
            || (iter->second->GetPage () == 0))
            {
              if (iter->second->HandleMessage (msg))
                return true;
            }
      }

  return false;
}

void Container::SetFocus (int controlid)
{
  ReleaseFocus (focusid);
  Control *control = GetControl (controlid);

  if (!control)
    return;
  control->SetFocus (true);
  focusid = controlid;
}

void Container::ReleaseFocus (int controlid)
{
  Control *control = GetControl (controlid);

  if (control)
    control->SetFocus (false);
  if (controlid == focusid)
    focusid = 0;
}


ControlList_t *Container::GetControlList ()
{
  return &control_root;
}


int Container::GetCurrentPage (void)
{
  return current_page;
}


void Container::SetCurrentPage (int curpage)
{
  current_page = curpage;
}



void Container::Rewind (void)
{
  search_index = 0;
}

Control *Container::GetNext (void)
{
  ControlList_t::iterator iter;

  if ((search_index >= 0)
      && ((unsigned int) search_index < control_root.size ()))
      {
        unsigned int i = search_index;
        search_index++;
        for (iter = control_root.begin ();
             (iter != control_root.end ()) && (i > 0); iter++)
          i--;
        if (iter != control_root.end ())
          return iter->second;
      }
  return NULL;
}

void Container::SetAlpha (unsigned char alpha)
{
  Control::SetAlpha (alpha);
  ControlList_t::iterator iter;
  for (iter = control_root.begin (); iter != control_root.end (); iter++)
    iter->second->SetAlpha (alpha);

}

void Container::SetShape (int controlid)
{
  shape_id = controlid;
}

bool Container::CheckPixel (int x, int y)
{
  if (shape_id)
      {
        Control *control = GetControl (shape_id);
        if (control)
          return control->CheckPixel (x, y);
      }
  return true;
}

Uint32 Container::FindDragContainer (int x, int y, int *drop_x, int *drop_y,
                                     Uint32 * charid)
{
  x -= GetX ();
  y -= GetY ();
  ControlList_t::reverse_iterator iter;
  for (iter = control_root.rbegin (); iter != control_root.rend (); iter++)
    if (iter->second->MouseIsOver (x, y))
      return iter->second->FindDragContainer (x, y, drop_x, drop_y, charid);
  return 0;
}
