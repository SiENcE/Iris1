//
// File: Stack.cpp
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

#include "gui/Stack.h"
#include <iostream>




using namespace std;

MessageStack::MessageStack ()
{
  root = NULL;
}

MessageStack::~MessageStack ()
{
  Clear ();
}

void MessageStack::Clear (void)
{
  struct message_entry *entry = root;

  while (entry)
      {
        root = entry->next;
        free (entry);
        entry = root;
      }
  root = NULL;
}

void MessageStack::Push (gui_message message)
{
  struct message_entry *entry =
    (struct message_entry *) malloc (sizeof (struct message_entry));
  entry->msg = message;
  entry->next = root;
  root = entry;
}

int MessageStack::Pop (gui_message * message)
{
  if (!root)
    return false;
  *message = root->msg;
  struct message_entry *entry = root;

  root = root->next;
  free (entry);
  return true;
}
