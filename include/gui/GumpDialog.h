//
// File: GumpDialog.h
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

#ifndef _GUMPDIALOG_H_
#define _GUMPDIALOG_H_

#include "Container.h"

// #include "../Fluid/mmgr.h"


class GumpDialog : public Container
{
 public:
  GumpDialog ();
  GumpDialog (unsigned int objid, unsigned int gumpid, int x, int y);

  void setGumpID(unsigned int id);
  void setRefObjUID(unsigned int id);
  unsigned int getGumpID();
  unsigned int getRefObjUID();


 private:
    unsigned int __refObjUID;
    unsigned int __gumpid;

};


#endif
