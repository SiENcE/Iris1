//
// File: WorldEnvironment.h
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

#ifndef _WORLDENVIRONMENT_H_
#define _WORLDENVIRONMENT_H_

#include <string>
#include "uotype.h"
#include "SDL/SDL_image.h"
#include "Logger.h"
#include "Exception.h"

// #include "../Fluid/mmgr.h"

class cWorldEnvironment
{
protected:

private: 
   sColor m_ambient_light_color_map [256];
   sColor m_sun_light_color_map [256];
   sColor m_fog_color_map [256];
   void LoadGradientFromImage (SDL_Surface * image, sColor * array);
public:
   cWorldEnvironment ();
   virtual ~cWorldEnvironment ();  
   void LoadLightColorMap (std::string ambientfilename, std::string sunfilename);
   void LoadFogColorMap (std::string filename);
   sColor getSunLightColor (unsigned int time); // 0 = 0:00h, 255 = 23:59h
   sColor getAmbientLightColor (unsigned int time); // 0 = 0:00h, 255 = 23:59h
   sColor getFogColor (unsigned int time); // 0 = 0:00h, 255 = 23:59h
};

#endif //_WORLDENVIRONMENT_H_
