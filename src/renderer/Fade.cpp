//
// File: Fade.cpp
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

#include "include.h"
#include "renderer/Fade.h"




using namespace std;

cFader::cFader (float from, float to, Uint32 time, Uint32 id)
{
  fade_start = 0;
  fade_from = from;
  fade_to = to;
  fade_time = time;
  m_id = id;
}

void cFader::Start ()
{
  fade_start = SDL_GetTicks ();
  m_value = fade_from;
  m_finished = false;
}

void cFader::Handle ()
{
  if (fade_start)
      {
        Uint32 currenttime = SDL_GetTicks ();
        if (fade_start + fade_time < currenttime)
            {
              m_value = fade_to;
              fade_start = 0;
              m_finished = true;
            }
        else
          m_value =
            (float) (fade_to -
                     fade_from) / (float) fade_time *(float) (currenttime -
                                                              fade_start) +
          fade_from;
      }
}
