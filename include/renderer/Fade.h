//
// File: Fade.h
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

#ifndef _FADE_H_
#define _FADE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <vector>

//// #include "../Fluid/mmgr.h"

class cFader
{
  private:
	Uint32 fade_start;
	Uint32 fade_time;
	float fade_from;
	float fade_to;
		
	float m_value;
	bool m_finished;
	Uint32 m_id;
  
  public:
  	cFader (float from, float to, Uint32 time, Uint32 id = 0);
	void Start ();
	void Handle ();
	
	float value () { return m_value; }
	bool finished () { return m_finished; }
	Uint32 id () { return m_id; }
	
  protected:

};


#endif //_FADE_H_
