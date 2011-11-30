//
// File: MotiveBasedGroundLight.cpp
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
#include "renderer/3D/MotiveBasedGroundLight.h"
#include "Geometry.h"
#include "Logger.h"
#include "Exception.h"
#include "include.h"
#include "Config.h"

#include <vector>
#include <cassert>
#include <map>
#include <math.h>




using namespace std;

cGroundLightMotive::cGroundLightMotive (int blockx, int blocky,
                                        vertex * ground_vertieces,
                                        float *ground_normals,
                                        cLight3D * light)
{
  m_light = light;
  ApplyLight (blockx, blocky, ground_vertieces, ground_normals);
}

cGroundLightMotive::~cGroundLightMotive ()
{
}

// Calculates the light intensity dependent from the model and the model position
void cGroundLightMotive::ApplyLight (int blockx, int blocky,
                                     vertex * ground_vertieces,
                                     float *ground_normals)
{
  float *fac_iter = factor_array ();

  assert (ground_vertieces);

  float light_x = m_light->x () - blockx * 8.0f;
  float light_y = m_light->y () - blocky * 8.0f;
  float light_z = m_light->z ();
  float radius_square = m_light->radius () * m_light->radius ();


  vertex *v = ground_vertieces;
  float *normal = ground_normals;
  for (int nodeindex = 0; nodeindex < 81; nodeindex++)
      {
        float vec_P[3];
        vec_P[0] = v->x - light_x;
        vec_P[1] = v->y - light_y;
        vec_P[2] = v->z - (light_z * 0.1f);


        float distance =
          (vec_P[0] * vec_P[0]) + (vec_P[1] * vec_P[1]) +
          (vec_P[2] * vec_P[2]);
        if ((distance < radius_square) && (distance > 0.0f))
            {
              float light_fac = 0.05f / distance * radius_square;
              distance = sqrt (distance);
              vec_P[0] /= -distance;
              vec_P[1] /= -distance;
              vec_P[2] /= -distance;

              float dot_product =
                vec_P[0] * normal[0] + vec_P[1] * normal[1] +
                vec_P[2] * normal[2];
              if (dot_product > 0.0f)
                *fac_iter = light_fac * dot_product;
              else
                *fac_iter = 0.0f;

            }
        else
          *fac_iter = 0.0f;
        fac_iter++;
        v++;
        normal += 3;            // 3 components (x, y, z)
      }
}

cLight3D *cGroundLightMotive::light ()
{
  return m_light;
}

float *cGroundLightMotive::factor_array ()
{
  return &m_factor_array[0][0];
}



cMotiveBasedGroundLight::cMotiveBasedGroundLight (int blockx, int blocky,
                                                  vertex * ground_vertieces,
                                                  float *ground_normals)
{
  assert (ground_vertieces);
  assert (ground_normals);
  m_blockx = blockx;
  m_blocky = blocky;
  m_ground_vertieces = ground_vertieces;
  m_ground_normals = ground_normals;

  memset (ambient_color_array, 0xff, 9 * 9 * sizeof (sColor));
}

cMotiveBasedGroundLight::~cMotiveBasedGroundLight ()
{
}

// Calculates the ambient light of the ground
void cMotiveBasedGroundLight::CalcAmbientLight (sColor ambient_color,
                                                sColor sun_color,
                                                float direction[3],
                                                cShaderMatrixBlock *
                                                shader_matrix)
{
  int index;
  assert (shader_matrix);

  sColor *color_iter = ambient_color_array;

  if (direction)
      {
        float *normal_iter = m_ground_normals;
        vertex *v = m_ground_vertieces;

        for (index = 0; index < 81; index++)
            {
              float intensity = 1.0f;
              shader_matrix->CheckRay ( (float) (index % 9), index / 9.0, v->z * 10.0f, direction, intensity);
              if (intensity <= 0.0f)
                *color_iter = ambient_color;
              else
                  {

                    float dot_product =
                      normal_iter[0] * direction[0] +
                      normal_iter[1] * direction[1] +
                      normal_iter[2] * direction[2];
                    float factor =
                      (dot_product > 0.0f) ? dot_product * intensity : 0.0f;

                    for (int compindex = 0; compindex < 3; compindex++)
                        {
                          int value =
                            (int) ambient_color.color_array[compindex] +
                            (int) ((float) sun_color.color_array[compindex] *
                                   factor);
                          color_iter->color_array[compindex] =
                            (value < 255) ? value : 255;
                        }
                  }
              color_iter++;
              normal_iter += 3;
              v++;
            }
      }
  else
    for (index = 0; index < 81; index++)
        {
          *color_iter = ambient_color;
          color_iter++;
        }

}

// Adds a new light source 
void cMotiveBasedGroundLight::AddLight (cLight3D * light)
{
  assert (light);
  cGroundLightMotive *motive =
    new cGroundLightMotive (m_blockx, m_blocky, m_ground_vertieces,
                            m_ground_normals, light);
  motives.push_back (motive);
}

// Removes an existing light source
void cMotiveBasedGroundLight::RemoveLight (cLight3D * light)
{
  list < cGroundLightMotive * >::iterator iter;
  for (iter = motives.begin (); iter != motives.end ();)
      {
        list < cGroundLightMotive * >::iterator next_iter = iter;
        next_iter++;
        if ((*iter)->light () == light)
            {
              delete *iter;
              motives.erase (iter);
            }
        iter = next_iter;
      }
}

// Sums the different light sources (motives) and calculates a color for each point
// Call this immediately before rendering
void cMotiveBasedGroundLight::PrepareForRendering ()
{
  unsigned int colorindex, nodeindex;


  int *colors = (int *) malloc (81 * sizeof (int) * 3);
  int *color_iter = colors;

  for (nodeindex = 0; nodeindex < 81; nodeindex++)    // Apply Ambient Light
    for (colorindex = 0; colorindex < 3; colorindex++)
        {
          *color_iter =
            ambient_color_array[nodeindex].color_array[colorindex];
          color_iter++;
        }


  list < cGroundLightMotive * >::iterator iter;
  for (iter = motives.begin (); iter != motives.end (); iter++)
      {
        cGroundLightMotive *motive = *iter;

        cLight3D *light = motive->light ();
        sColor light_color = light->color ();

        color_iter = colors;
        float *factor_iter = motive->factor_array ();

        if (light->white_light ())
          for (nodeindex = 0; nodeindex < 81; nodeindex++)
              {                 // Apply White Light
                int value = (int) ((*factor_iter) * light_color.colorRGB.r);  // Take red value as intensity
                (*color_iter) += value;
                color_iter++;   // r
                (*color_iter) += value;
                color_iter++;   // g
                (*color_iter) += value;
                color_iter++;   // b
                factor_iter++;
              }
        else
            {
              for (nodeindex = 0; nodeindex < 81; nodeindex++)
                  {             // Apply Colored Light
                    for (colorindex = 0; colorindex < 3; colorindex++)
                        {
                          (*color_iter) +=
                            (int) ((*factor_iter) *
                                   (float) light_color.
                                   color_array[colorindex]);
                          color_iter++;
                        }
                    factor_iter++;
                  }
            }
      }


  vertex *v = m_ground_vertieces;
  color_iter = colors;
  for (nodeindex = 0; nodeindex < 81; nodeindex++)
      {                         // Apply Light to Model
        for (colorindex = 0; colorindex < 3; colorindex++)
            {
              v->color.color_array[colorindex] =
                (*color_iter > 255) ? 255 : *color_iter;
              color_iter++;
            }
        v++;
      }



  free (colors);
}

/*

       
void cMotiveBasedLight::PrepareModelForRendering ()
{
    //tStaticNodes * nodes = m_model->nodes();
    tStaticFaceLightNodes * face_light_nodes = m_model->face_light_nodes();
    unsigned int colorindex, nodeindex;
    
    
    int * colors = (int *) malloc (face_light_nodes->size () * sizeof (int) * 3);
    int * color_iter = colors;
    
    for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex ++) // Apply Ambient Light
           for (colorindex = 0; colorindex < 3; colorindex ++) {
                *color_iter = ambient_color_array[(*face_light_nodes)[nodeindex]->node_index()].color_array[colorindex];
                color_iter ++;
           }
    
    
    list <cLightMotive *>::iterator iter;
    for (iter = motives.begin (); iter != motives.end (); iter ++) {
          cLightMotive *motive = *iter;
              
          cLight3D * light = motive->light ();
          sColor light_color = light->color ();
          
          color_iter = colors;
          float * factor_array = motive->factor_array ();
          
          if (light->white_light ()) 
                for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex ++) {// Apply White Light
                        float * factor_iter = factor_array + (*face_light_nodes)[nodeindex]->point_light_index();
                        int value = (int) ((*factor_iter) * light_color.colorRGB.r); // Take red value as intensity
                        (*color_iter) += value; color_iter ++; // r
                        (*color_iter) += value; color_iter ++; // g
                        (*color_iter) += value; color_iter ++; // b
                }
          else {
                for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex ++) {// Apply Colored Light
                     float * factor_iter = factor_array + (*face_light_nodes)[nodeindex]->point_light_index();
                     for (colorindex = 0; colorindex < 3; colorindex ++) {
                           (*color_iter) += (int) ((*factor_iter) * (float) light_color.color_array[colorindex]); 
                           color_iter ++;
                     } 
                     
                }
        }
    } 
    
    
    vertex * v = m_model->vertieces ();
    color_iter = colors;
    for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex ++) {// Apply Light to Model
        for (colorindex = 0; colorindex < 3; colorindex ++) {
              v->color.color_array[colorindex] = (*color_iter > 255) ? 255 : *color_iter; 
              color_iter ++;
        }
        v++;
    }
    
    
    
    free (colors); 
}

void cMotiveBasedLight_Entity::CalcAmbientLight (sColor ambient_color, sColor sun_color, float direction [3])
{
        assert (m_model);
        int index;
        
        sColor * color_iter = ambient_color_array;
        tStaticNodes * nodes = m_model->nodes();
        
        int nodecount = nodes->size();
        
        if (direction) {
            for (index = 0; index < nodecount; index++) {
                cStaticModelNode * node = (*nodes)[index];
                float factor = node->calc_light_factor (direction);
                
                for (int compindex = 0; compindex < 3; compindex ++) {
                        int value = (int) ambient_color.color_array[compindex] + 
                                    (int) ((float) sun_color.color_array[compindex] * factor);
                        color_iter->color_array[compindex] = (value < 255) ? value : 255;
                }
                color_iter ++;
            }    
        } else 
            for (index = 0; index < nodecount; index++) {
                 *color_iter = ambient_color;
                 color_iter ++;
            }    

}

*/
