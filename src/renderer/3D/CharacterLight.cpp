//
// File: CharacterLight.cpp
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


#include "renderer/3D/CharacterLight.h"
#include "renderer/3D/Light3D.h"
#include "Exception.h"
#include "Geometry.h"
#include <cassert>
#include <math.h>




using namespace std;

float char_normals[6][3] =
  { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f,
                                                                  0.0f},
  {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} };


cCharacterLight::cCharacterLight (int size_x, int size_y, int size_z)
{
  assert (size_x > 0);
  assert (size_y > 0);
  assert (size_z > 0);

  m_size_x = size_x;
  m_size_y = size_y;
  m_size_z = size_z;

  int mem = size_x * size_y * size_z * 6 * sizeof (sColor); // allocate color lookup table
  color_array = (struct sColor *) malloc (mem);
  if (!color_array)
    THROWEXCEPTION ("out of memory");
  memset (color_array, 0, mem);
}

cCharacterLight::~cCharacterLight ()
{
  if (color_array)
    free (color_array);
}

void cCharacterLight::Generate (float char_x, float char_y, float char_z, int blockx, int blocky, sColor ambient_color, sColor sun_color, float direction[3], float *invmatrix)     // Generates lookup table
{
  assert (invmatrix);
  assert (direction);

  sColor *color = color_array;

  int index, colorindex, compindex;

  float normals[6][3];

  sColor base_colors[6];        // calc basic colors
  for (colorindex = 0; colorindex < 6; colorindex++)
      {
        float *normal = normals[colorindex];
        Vec3TransformRotation (char_normals[colorindex], invmatrix, normal);

        float light_fac = normal[0] * direction[0] +
          normal[1] * direction[1] + normal[2] * direction[2];

        for (index = 0; index < 3; index++)
            {
              int value =
                ambient_color.color_array[index] +
                (int) ((float) sun_color.color_array[index] *
                       ((light_fac > 0.0f) ? light_fac : 0.0f));
              base_colors[colorindex].color_array[index] =
                (value < 255) ? value : 255;
            }
      }


  float center_x = (m_size_x - 1) / 2.0f;
  float center_y = (m_size_y - 1) / 2.0f;

  for (int z = 0; z < m_size_z; z++)
    for (int y = 0; y < m_size_y; y++)
      for (int x = 0; x < m_size_x; x++)
          {
            float temp_pos[3], position[3];
            temp_pos[0] = (x - center_x) * 0.25f;
            temp_pos[1] = (y - center_y) * 0.25f;
            temp_pos[2] = z * 0.25f;
            Vec3TransformCoord (temp_pos, invmatrix, position);
            position[0] = 0.0f;
            position[0] = 0.0f;
            position[0] = 0.0f;

            position[2] += 1.0f;    // Z must be in the center


            std::list < cLight3D * >static_light_list =
              pLightManager.static_light_list ();
            std::list < cLight3D * >::iterator light_iter;

            int temp_colors[6][3];
            for (colorindex = 0; colorindex < 6; colorindex++)    // for every normal do
              for (compindex = 0; compindex < 3; compindex++)
                temp_colors[colorindex][compindex] =
                  base_colors[colorindex].color_array[compindex];

            for (light_iter = static_light_list.begin ();
                 light_iter != static_light_list.end (); light_iter++)
              if ((*light_iter)->light_in_block (blockx, blocky))
                  {
                    cLight3D *light = *light_iter;
//                        printf("%.2f %.2f %.2f\n", position[0] - char_x, position[1] - char_y, position[2] - char_z);
                    float light_x =
                      light->x () - char_x - blockx * 8 - 0.5f - position[0];
                    float light_y =
                      light->y () - char_y - blocky * 8 - 0.5f - position[1];
                    float light_z =
                      light->z () - char_z + 10.0f - position[2];
                    float radius_square = light->radius () * light->radius ();
                    sColor light_color = light->color ();

                    float vec_P[3];
                    vec_P[0] = position[0] - light_x;
                    vec_P[1] = position[1] - light_y;
                    vec_P[2] = position[2] - (light_z * 0.1f);

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

                          for (colorindex = 0; colorindex < 6; colorindex++)
                              { // for every normal do
                                float *normal = normals[colorindex];
                                float scalar_product =
                                  vec_P[0] * normal[0] +
                                  vec_P[1] * normal[1] + vec_P[2] * normal[2];
                                float factor =
                                  light_fac * (scalar_product >
                                               0.0f ? scalar_product : 0.0f);
                                if (factor > 0.01f)
                                  for (compindex = 0; compindex < 3;
                                       compindex++)
                                    temp_colors[colorindex][compindex] +=
                                      (int) ((float) light_color.
                                             color_array[compindex] * factor);
                              }
                        }
                  }

            for (colorindex = 0; colorindex < 6; colorindex++)
                {               // for every normal do
                  for (compindex = 0; compindex < 3; compindex++)
                    color->color_array[compindex] =
                      (temp_colors[colorindex][compindex] <
                       255 ? temp_colors[colorindex][compindex] : 255);
//                    printf ("Color %i: (%i/%i/%i)\n", colorindex, color->colorRGB.r, color->colorRGB.g, color->colorRGB.b);

                  color++;
                }


          }

}

void cCharacterLight::CalcColor (int x, int y, int z, float *normal, float *result) // normal and result must be arrays
{
  assert (normal);
  assert (result);
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  if (z < 0)
    z = 0;
  if (x >= m_size_x)
    x = m_size_x;
  if (y >= m_size_y)
    y = m_size_y;
  if (z >= m_size_z)
    z = m_size_z;

  float factor[3];
  int i;

  for (i = 0; i < 3; i++)
    factor[i] = normal[i] * normal[i];

  sColor *color;
  int index = x + y * m_size_x + z * m_size_x * m_size_y;
  int result_color[3];

  if (normal[2] >= 0.0f)
      {
        color = color_array + index + 0;
        for (i = 0; i < 3; i++)
          result_color[i] = (int) (color->color_array[i] * factor[2]);
      }
  else
      {
        color = color_array + index + 1;
        for (i = 0; i < 3; i++)
          result_color[i] = (int) (color->color_array[i] * factor[2]);
      }

  if (normal[1] >= 0.0f)
      {
        color = color_array + index + 2;
        for (i = 0; i < 3; i++)
          result_color[i] += (int) (color->color_array[i] * factor[1]);
      }
  else
      {
        color = color_array + index + 3;
        for (i = 0; i < 3; i++)
          result_color[i] += (int) (color->color_array[i] * factor[1]);
      }

  if (normal[0] >= 0.0f)
      {
        color = color_array + index + 4;
        for (i = 0; i < 3; i++)
          result_color[i] += (int) (color->color_array[i] * factor[0]);
      }
  else
      {
        color = color_array + index + 5;
        for (i = 0; i < 3; i++)
          result_color[i] += (int) (color->color_array[i] * factor[0]);
      }

  for (i = 0; i < 3; i++)
    result[i] = (float) result_color[i] / 255.0f;
}
