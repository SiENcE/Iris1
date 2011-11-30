//
// File: WorldEnvironment.cpp
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

#include "renderer/3D/WorldEnvironment.h"



cWorldEnvironment::cWorldEnvironment ()
{
   for (int i = 0; i < 256; i++) {
       m_ambient_light_color_map[i].color32 = 0x808080;
       m_sun_light_color_map[i].color32 = 0x808080;
       m_fog_color_map[i].color32 = 0x808080;
       m_sun_light_color_map[i].colorRGB.a = 255;
       m_fog_color_map[i].colorRGB.a = 255;
       m_ambient_light_color_map[i].colorRGB.a = 255;      
   }
}


cWorldEnvironment::~cWorldEnvironment ()
{
}

// Loads a gradient from an Image into a color array (256 bytes!)
void cWorldEnvironment::LoadGradientFromImage (SDL_Surface * image, sColor * array)
{
   ASSERT (image);
   ASSERT (array);
   
   Uint32 rmask = 0x000000ff;
   Uint32 gmask = 0x0000ff00;
   Uint32 bmask = 0x00ff0000;
   
   // Convert Image to 32bit rgb
   SDL_Surface * dummy_image = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 1, 32, 
                                  rmask, gmask, bmask, 0);
   if (dummy_image) {
         SDL_Rect rect;
         rect.x = 0; rect.y = 0; rect.w = 256; rect.h = 1;         
         SDL_BlitSurface(image, &rect, dummy_image, &rect);
         
         for (int i = 0; i < 256; i++) {
             Uint32 color = ((Uint32 *)dummy_image->pixels)[i];
             array[i].colorRGB.r = color & rmask;
             array[i].colorRGB.g = (color & gmask) >> 8;
             array[i].colorRGB.b = (color & bmask) >> 16;
             array[i].colorRGB.a = 255;
         }
         SDL_FreeSurface (dummy_image);
   }
}

// Loads a color light map from an 256x1-pixel sized image
void cWorldEnvironment::LoadLightColorMap (std::string ambientfilename, std::string sunfilename)
{
   SDL_Surface * image;
   image = IMG_Load (ambientfilename.c_str ());
   if (image) {
       if ((image->w != 256) || (image->h < 1)) 
           Logger::WriteLine( "Color table has invalid size: " + ambientfilename);
       else 
           LoadGradientFromImage (image, m_ambient_light_color_map); 
       SDL_FreeSurface (image);
   } else
       Logger::WriteLine( "Could not load color table: " + ambientfilename);
      
   image = IMG_Load (sunfilename.c_str ());
   if (image) {
       if ((image->w != 256) || (image->h < 1)) 
           Logger::WriteLine( "Color table has invalid size: " + sunfilename);
       else 
           LoadGradientFromImage (image, m_sun_light_color_map); 
       SDL_FreeSurface (image);
   } else
       Logger::WriteLine( "Could not load color table: " + sunfilename);
}

void cWorldEnvironment::LoadFogColorMap (std::string filename)
{
   SDL_Surface * image;
   int i;

   image = IMG_Load (filename.c_str ());
   if (image) {
       if ((image->w != 256) || (image->h < 1)) 
           Logger::WriteLine( "Color table has invalid format: " + filename);
       else 
           LoadGradientFromImage (image, m_fog_color_map); 
       SDL_FreeSurface (image);
   } else
       Logger::WriteLine( "Could not load color table: " + filename);
}

sColor cWorldEnvironment::getSunLightColor (unsigned int time) // 0 = 0:00h, 255 = 23:59h
{
       sColor color;
       if (time < 256)
          color = m_sun_light_color_map[time];
       else
           color.color32 = 0;
       color.colorRGB.a = 255;
       return color;
}

sColor cWorldEnvironment::getAmbientLightColor (unsigned int time) // 0 = 0:00h, 255 = 23:59h
{
       sColor color;
       if (time < 256)
          color = m_ambient_light_color_map[time];
       else
           color.color32 = 0;
       color.colorRGB.a = 255;
       return color;
}

sColor cWorldEnvironment::getFogColor (unsigned int time) // 0 = 0:00h, 255 = 23:59h
{
       sColor color;
       if (time < 256)
          color = m_fog_color_map[time];
       else
           color.color32 = 0;
       color.colorRGB.a = 255;
       return color;
}
