//
// File: Textbox.cpp
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

#include "gui/Textbox.h"
#include "Logger.h"
#include "Config.h"




using namespace std;

Textbox::Textbox ()
{
  control_type = CONTROLTYPE_TEXTBOX;
}

Textbox::~Textbox ()
{
}

void Textbox::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);
  int x, y;

  GetPosition (&x, &y);
  unsigned int index;

  for (index = 0; index < labels.size (); index++)
      {
        Label *label = labels[index];
        if (!label->OutTimed ())
          y -= labels[index]->GetElement ()->height ();
      }

  for (index = 0; index < labels.size (); index++)
      {
        Label *label = labels[index];

        label->SetPosition (x, y);
        if (!label->OutTimed ())
            {
              label->Draw (gumps);
              y += label->GetElement ()->height ();
            }
      }
}

void Textbox::AddMessage (char *message, unsigned int timeout,
                          unsigned short hue)
{
  Label *label = new Label (0, 0, message);
  if (hue)
    label->setHue (hue);

  label->SetTimeOut (timeout);
  labels.push_back (label);
}
