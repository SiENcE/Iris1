//
// File: LightNodeEnvironment.cpp
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

#include "renderer/3D/LightNodeEnvironment.h"

using namespace std;

#include "Exception.h"
#include <cassert>




cLightNodeEnvironment::cLightNodeEnvironment ()
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      m_nodes[i][j] = NULL;

}

cLightNodeEnvironment::~cLightNodeEnvironment ()
{
}

void cLightNodeEnvironment::set (int x, int y, cLightNodeHandler * nodes)     // x and y are element of (-1, 0, 1)
{
  assert ((x >= -1) && (x <= 1));
  assert ((y >= -1) && (y <= 1));
  assert (nodes);
  m_nodes[y + 1][x + 1] = nodes;
}

cLightNodeHandler *cLightNodeEnvironment::get (int x, int y)      // x and y are element of (-1, 0, 1)
{
  assert ((x >= -1) && (x <= 1));
  assert ((y >= -1) && (y <= 1));
  cLightNodeHandler *result = m_nodes[y + 1][x + 1];
  if (!result)
    THROWEXCEPTION ("NULL Node in Node environment");
  return result;
}
