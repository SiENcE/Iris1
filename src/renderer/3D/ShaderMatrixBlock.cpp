//

// File: ShaderMatrixBlock.cpp

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

#include <iostream>
#include <cassert>
#include <vector>
#include <map>
#include <math.h>

#include "renderer/3D/MapBuffer3D.h"
#include "renderer/3D/SceneMaker.h"
#include "renderer/3D/ShaderMatrixBlock.h"



  
using namespace std;


cShaderMatrixBlock::cShaderMatrixBlock () 

{
  
memset (&bitmask[0], 0, 52 * 16 * 16);
  
m_block_x = 0;
  
m_block_y = 0;

}




cShaderMatrixBlock::~cShaderMatrixBlock () 

{

}



void cShaderMatrixBlock::AddModel (int x, int y, int z,
                                      cStaticModel * model) 

{
  
assert (model);
  
cStaticModelRaster * raster = model->raster ();
  
assert (raster);
  
int dx = x * 2 + raster->min_x ();
  
int dy = y * 2 + raster->min_y ();
  
int dz = (z + 128) * 2 / 10 + raster->min_z ();
  

Uint8 * data = raster->data ();
  

for (int ix = 0; ix < raster->size_x (); ix++)
    
for (int iy = 0; iy < raster->size_y (); iy++)
      
for (int iz = 0; iz < raster->size_z (); iz++)
          {
            
int nx = ix + dx;
            
int ny = iy + dy;
            
int nz = iz + dz;
            
if (*data)
              
if ((nx >= 0) && (ny >= 0) && (nz >= 0) && (nx < 16)
                   && (ny < 16) && (nz < 52))
                  {
                    
int value =
                      bitmask[nx + ny * 16 + nz * 16 * 16] + (*data);
                    
bitmask[nx + ny * 16 + nz * 16 * 16] =
                      (value <= 255) ? value : 255;
                  
}
            
data++;

} 
} 

void cShaderMatrixBlock::CheckRay (float x, float y, float z,
                                           float light_dir[3],
                                           float &intensity) 

{
  
assert (light_dir[0] || light_dir[1] || light_dir[2]);
  

x *= 2.0f;
  
y *= 2.0f;
  
z = (z + 128.0f) * 0.2f;
  

float vec[3];
  

vec[0] = light_dir[0] * 2.0f;
  
vec[1] = light_dir[1] * 2.0f;
  
vec[2] = light_dir[2] * 2.0f;
  

float len = sqrt (vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
  
vec[0] /= len;
  
vec[1] /= len;
  
vec[2] /= len;
  

while ((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && (x < 16.0f)
           && (y < 16.0f) && (z < 45.0f))
      {
        
int mask = bitmask[((int) x) + ((int) y) * 16 + ((int) z) * 16 * 16];
        
if (mask)
          
intensity -= mask * 0.01f;
        
if (intensity < 0.0f)
          
break;
        
x += vec[0];
        
y += vec[1];
        
z += vec[2];
      
}
  

if ((z < 0.0f) || (z >= 45.0f))
    
return;
  

if (intensity <= 0.0f)
    
return;
  

x /= 2.0f;                  // back-transform

  y /= 2.0f;
  
z = (z / 0.2f) - 128.0f;
  

int new_x = 0, new_y = 0;
  

if (x < 0.0f)
    new_x = -1;
  
if (x >= 8.0f)
    new_x = 1;
  

if (y < 0.0f)
    new_y = -1;
  
if (y >= 8.0f)
    new_y = 1;
  

assert (new_x || new_y);
  
x -= new_x * 8.0f;
  
y -= new_y * 8.0f;
  

cMapblock * block =
    pMapbufferHandler.buffer ()->Get (m_block_x + new_x, m_block_y + new_y);
  
if (!block)
    
return;
  

//    printf ("%i %i | %.2f %.2f %.2f\n", new_x, new_y, x, y, z);

    


    ((cMapblock3D *) block)->GetShaderMatrix ()->CheckRay (x, y, z, light_dir,
                                                           intensity);

}


void cShaderMatrixBlock::setBlock (int x, int y) 

{
  
m_block_x = x;
  
m_block_y = y;


} 

void cShaderMatrixBlock::DoRender (float move_x, float move_y) 

{
  

    // to be removed

} 

void cShaderMatrixBlock::AddGround (signed char *groundmap) 

{
  

    // to be removed

    return;                     // do nothing... 


}



