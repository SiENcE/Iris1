//
// File: GumpDialog.cpp
// Created by: Netzzwerg
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

#include "gui/GumpDialog.h"




GumpDialog::GumpDialog ()
{
  Container::Container ();
  __refObjUID = 0;
  __gumpid = 0;
}


GumpDialog::GumpDialog (unsigned int objid, unsigned int gumpid, int x, int y)
{
  Container::Container ();
  __refObjUID = objid;
  __gumpid = gumpid;
  SetX (x);
  SetY (y);
}


void GumpDialog::setGumpID (unsigned int id)
{
  __gumpid = id;
}


void GumpDialog::setRefObjUID (unsigned int id)
{
  __refObjUID = id;
}


unsigned int GumpDialog::getGumpID ()
{
  return __gumpid;
}


unsigned int GumpDialog::getRefObjUID ()
{
  return __refObjUID;
}
