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
#include "gui/Label.h"
#include "gui/TextManager.h"
#include "gui/MultiLabel.h"
#include "gui/Button.h"
#include "gui/GUIHandler.h"
#include "gui/Container.h"
#include "gui/Control.h"
#include "Logger.h"
#include <iostream>




int onscrollup (Control * contr)
{
  cMultiLabel *mlabel = NULL;
  Control * control;
  if (contr->GetData (1))
      {
        control = pUOGUI.GetControl (contr->GetData (1));
        if (control->getType () != CONTROLTYPE_CONTAINER) {
            Logger::WriteLine ("Invalid multilabel container cast in onscrolldown!!");
            return -1;
        }
        Container *container = (Container *) control;

        if (container)
            {
              control = container->GetControl (contr->GetData (0));
              if (control->getType () != CONTROLTYPE_MULTILABEL) {
                    Logger::WriteLine ("Invalid multilabel cast onscrolldown!!");
                    return -1;
              }
              mlabel = (cMultiLabel *) control;

            }
      }
  else  {
    control = pUOGUI.GetControl (contr->GetData (0));
              if (control->getType () != CONTROLTYPE_MULTILABEL) {
                    Logger::WriteLine  ("Invalid multilabel cast onscrolldown!!");
                    return -1;
              }
    mlabel = (cMultiLabel *) control;
  }

  if (mlabel)
      {
        mlabel->ScrollUp ();
      }
  return -1;
}

int onscrolldown (Control * contr)
{
  cMultiLabel *mlabel = NULL;
  Control * control;
  if (contr->GetData (1))
      {
        control = pUOGUI.GetControl (contr->GetData (1));
        if (control->getType () != CONTROLTYPE_CONTAINER) {
            Logger::WriteLine  ("Invalid multilabel container cast in onscrolldown!!");
            return -1;
        }
        Container *container = (Container *) control;
        
        if (container)
            {
              control = container->GetControl (contr->GetData (0));
              if (control->getType () != CONTROLTYPE_MULTILABEL) {
                    Logger::WriteLine  ("Invalid multilabel cast onscrolldown!!");
                    return -1;
              }
              mlabel = (cMultiLabel *) control;

            }
      }
  else  {
    control = pUOGUI.GetControl (contr->GetData (0));
              if (control->getType () != CONTROLTYPE_MULTILABEL) {
                    Logger::WriteLine  ("Invalid multilabel cast onscrolldown!!");
                    return -1;
              }
    mlabel = (cMultiLabel *) control;
  }

  if (mlabel)
      {
        mlabel->ScrollDown ();
      }
  return -1;
}

cMultiLabel::cMultiLabel (int x, int y, int width, int height, int scrollbar)
{
  SetPosition (x, y);
  control_type = CONTROLTYPE_MULTILABEL;
  _width = width;
  _height = height;
  _x = x;
  _y = y;

  _scrollbar = scrollbar;
  scrollbuts_created = false;
  
  scrollup = NULL;
  scrolldown = NULL;

  //refresh();
}

cMultiLabel::~cMultiLabel ()
{
  for(unsigned int i = 0; i < labels.size(); i++){
     Label * l =  labels.at(i);
      //(if(labels.at(i))
      delete l;
  }
   /*
   if(scrollup)
    delete scrollup;
   if(scrolldown)
    delete scrolldown; 
   */
   labels.clear();
   line.clear ();   
   lines.clear ();
}


void cMultiLabel::Create ()
{

  int cur_x = 0, cur_y = 0;
  Label *label;
  //int check = 0;
  //std::cout << "Lines size: " << lines.size () << std::endl;
  for (unsigned int i = 0; i < lines.size (); i++)
      {
        __line curr_line = lines.at (i);

       
        for (unsigned int idx = 0; idx < curr_line.size (); idx++)
            {
              //std::cout << "Line: " << i <<" Comp: " << idx << std::endl;
              
              MultiLabelComponent comp = curr_line.at (idx);

              int xalign = 0;

              if (comp.alignment == 1)
                xalign = _width / 2;
              else if (comp.alignment == 2)
                xalign = _width;

              /*
                 Don't know why, but this is necessary to render font style for the correct word
               */
/*SiENcE: this couses chrashes
			  Label *prelab =
                new Label ((cur_x + x + xalign), cur_y + y,
                           comp.text.c_str (), 0, comp.font);
              if (comp.style)
                  {

                    prelab->SetStyle (comp.style);
                  }
              else
                  {
                    prelab->SetStyle (0);
                  }
              delete prelab;
*/
              label =
                new Label ((cur_x + x + xalign), cur_y + y,
                           comp.text.c_str (), 0, comp.font);

              if (comp.x_fix)
                label->SetX (label->GetX () -
                             (comp.x_fix *
                              (label->labelWidth () / comp.text.size ())));
              if (_scrollbar)
                label->SetData (0, label->GetY ());

              label->SetRGBColor (comp.r, comp.g, comp.b);

              label->setAlign (comp.alignment);


              if (label->labelWidth () > _width)
                  {
                    int lines = label->Wrap (_width - 60);
                    cur_y += label->labelHeight () * lines;
                  }
              //label->Draw(gumps);
              labels.push_back (label);


              cur_x +=
                label->labelWidth () -
                (comp.x_fix * (label->labelWidth () / comp.text.size ()));
            }
        cur_y += label->labelHeight ();
        cur_x = 0;
      }
  lines.clear ();
}

void cMultiLabel::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);

  for (unsigned int i = 0; i < labels.size (); i++)
      {
        Label *lab = labels.at (i);

        if ((lab->GetY () >= _y)
            && (lab->GetY () + lab->labelHeight () <= _y + _height))
          lab->Draw (gumps);


        if (_scrollbar && !scrollbuts_created)
            {
              scrollup = new Button (_x + _width - 15, _y);
              scrolldown = new Button (_x + _width - 15, _y + _height - 20);

              scrollup->SetButton (BUTTONGUMP_NORMAL, 0xFA);
              scrollup->SetButton (BUTTONGUMP_MOUSEOVER, 0xFB);
              scrollup->SetButton (BUTTONGUMP_PRESSED, 0xFB);

              scrolldown->SetButton (BUTTONGUMP_NORMAL, 0xFC);
              scrolldown->SetButton (BUTTONGUMP_MOUSEOVER, 0xFC);
              scrolldown->SetButton (BUTTONGUMP_PRESSED, 0xFD);

              scrollup->SetData (0, GetID ());
              scrolldown->SetData (0, GetID ());
              Container *parent = GetParent ();
              if (parent)
                  {
                    scrollup->SetData (1, parent->GetID ());
                    scrolldown->SetData (1, parent->GetID ());
                  }
              else
                  {
                    scrollup->SetData (1, 0);
                    scrolldown->SetData (1, 0);
                  }
              scrollup->OnClick (onscrollup);
              scrolldown->OnClick (onscrolldown);
              if (parent)
                  {
                    parent->AddControl (scrollup);
                    parent->AddControl (scrolldown);
                  }
              else
                  {
                    pUOGUI.AddControl (scrollup);
                    pUOGUI.AddControl (scrolldown);
                  }
              scrollbuts_created = true;
            }
      }
}


void cMultiLabel::AddLabel (MultiLabelComponent component)
{
  if (!line.empty ())
      {
        MultiLabelComponent & first_component = line.front ();
        if (component.alignment == 1 && first_component.alignment == 1)
            {
              first_component.x_fix += (component.text.size () / 2) + 2;


            }
        else if (component.alignment == 2 && first_component.alignment == 2)
            {
              first_component.x_fix += component.text.size () + 2;


            }
      }
  line.push_back (component);
}

void cMultiLabel::AddLine ()
{
  lines.push_back (line);
  line.clear ();
}

void cMultiLabel::ScrollUp ()
{
  for (unsigned int i = 0; i < labels.size (); i++)
      {
        Label *lab = labels.at (i);

        if (lab->GetY () == lab->GetData (0))
          continue;

        lab->SetY (lab->GetY () + lab->labelHeight ());
      }
}

void cMultiLabel::ScrollDown ()
{
  for (unsigned int i = 0; i < labels.size (); i++)
      {
        Label *lab = labels.at (i);
        lab->SetY (lab->GetY () - lab->labelHeight ());
      }
}
