//
// File: MotiveBasedLight.cpp
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
#include "renderer/3D/MotiveBasedLight.h"
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

cLightMotive::cLightMotive (float x, float y, float z, cStaticModel * model,
                            cLight3D * light)
{
  assert (model);
  assert (light);
  m_light = light;

  m_nodecount = (model->point_light_nodes ())->size ();

  m_factor_array = (float *) malloc (m_nodecount * sizeof (float));

  ApplyLight (x, y, z, model);
}

void cLightMotive::ApplyLight (float x, float y, float z,
                               cStaticModel * model)
{
  float *fac_iter = m_factor_array;

  float light_x = m_light->x () - x;
  float light_y = m_light->y () - y;
  float light_z = m_light->z () - z;
  float radius_square = m_light->radius () * m_light->radius ();

  tStaticPointLightNodes *nodes = model->point_light_nodes ();

  vertex *v = model->vertieces ();
  for (int nodeindex = 0; nodeindex < m_nodecount; nodeindex++)
      {
        cStaticModelPointLightNode *node = (*nodes)[nodeindex];
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

              *fac_iter = light_fac * node->calc_light_factor (vec_P);
            }
        else
          *fac_iter = 0.0f;
        fac_iter++;
        v++;
      }


}

cLightMotive::~cLightMotive ()
{
  free (m_factor_array);
  m_factor_array = NULL;
}

cLight3D *cLightMotive::light ()
{
  return m_light;
}

float *cLightMotive::factor_array ()
{
  return m_factor_array;
}




cMotiveBasedLight::cMotiveBasedLight( float x, float y, float z, int blockx, int blocky, cStaticModel *model )
{
	assert( model );
	m_x = blockx * 8 + x;
	m_y = blocky * 8 + y;
	m_z = z;
	m_model = model;

	int nodecount = (model->nodes())->size();
	ambient_color_array = (sColor *)malloc( nodecount * sizeof(sColor));
	memset( ambient_color_array, 0xff, nodecount * sizeof(sColor) );
}


cMotiveBasedLight::~cMotiveBasedLight()
{
	free( ambient_color_array );
	ambient_color_array = NULL;

	std::list<cLightMotive *>::iterator iter;
	for ( iter = motives.begin(); iter != motives.end(); iter++ )
	{
		SAFE_DELETE( *iter );
	}

	motives.clear();
}


void cMotiveBasedLight::AddLight (cLight3D * light)
{
  assert (light);
  cLightMotive *motive = new cLightMotive (m_x, m_y, m_z, m_model, light);
  motives.push_back (motive);
}

void cMotiveBasedLight::RemoveLight (cLight3D * light)
{
  list < cLightMotive * >::iterator iter;
  for (iter = motives.begin (); iter != motives.end ();)
      {
        list < cLightMotive * >::iterator next_iter = iter;
        next_iter++;
        if ((*iter)->light () == light)
            {
              delete *iter;
              motives.erase (iter);
            }
        iter = next_iter;
      }
}

void cMotiveBasedLight::PrepareModelForRendering ()
{
  tStaticFaceLightNodes *face_light_nodes = m_model->face_light_nodes ();
  unsigned int colorindex, nodeindex;


  int *colors = (int *) malloc (face_light_nodes->size () * sizeof (int) * 3);
  int *color_iter = colors;

  for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex++)     // Apply Ambient Light
    for (colorindex = 0; colorindex < 3; colorindex++)
        {
          *color_iter =
            ambient_color_array[(*face_light_nodes)[nodeindex]->
                                node_index ()].color_array[colorindex];
          color_iter++;
        }


  list < cLightMotive * >::iterator iter;
  for (iter = motives.begin (); iter != motives.end (); iter++)
      {
        cLightMotive *motive = *iter;

        cLight3D *light = motive->light ();
        sColor light_color = light->color ();

        color_iter = colors;
        float *factor_array = motive->factor_array ();

        if (light->white_light ())
          for (nodeindex = 0; nodeindex < face_light_nodes->size ();
               nodeindex++)
              {                 // Apply White Light
                float *factor_iter =
                  factor_array +
                  (*face_light_nodes)[nodeindex]->point_light_index ();
                int value = (int) ((*factor_iter) * light_color.colorRGB.r);  // Take red value as intensity
                (*color_iter) += value;
                color_iter++;   // r
                (*color_iter) += value;
                color_iter++;   // g
                (*color_iter) += value;
                color_iter++;   // b
              }
        else
            {
              for (nodeindex = 0; nodeindex < face_light_nodes->size ();
                   nodeindex++)
                  {             // Apply Colored Light
                    float *factor_iter =
                      factor_array +
                      (*face_light_nodes)[nodeindex]->point_light_index ();
                    for (colorindex = 0; colorindex < 3; colorindex++)
                        {
                          (*color_iter) +=
                            (int) ((*factor_iter) *
                                   (float) light_color.
                                   color_array[colorindex]);
                          color_iter++;
                        }

                  }
            }
      }


  vertex *v = m_model->vertieces ();
  color_iter = colors;
  for (nodeindex = 0; nodeindex < face_light_nodes->size (); nodeindex++)
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

cMotiveBasedLight_Entity::cMotiveBasedLight_Entity (float x, float y, float z,
                                                    int blockx, int blocky,
                                                    cStaticModel *
                                                    model):cMotiveBasedLight
  (x, y, z, blockx, blocky, model)
{
}

void cMotiveBasedLight_Entity::CalcAmbientLight (sColor ambient_color,
                                                 sColor sun_color,
                                                 float direction[3])
{
  assert (m_model);
  int index;

  sColor *color_iter = ambient_color_array;
  tStaticNodes *nodes = m_model->nodes ();

  int nodecount = nodes->size ();

  if (direction)
      {
        for (index = 0; index < nodecount; index++)
            {
              cStaticModelNode *node = (*nodes)[index];
              float factor = node->calc_light_factor (direction);

              for (int compindex = 0; compindex < 3; compindex++)
                  {
                    int value = (int) ambient_color.color_array[compindex] +
                      (int) ((float) sun_color.color_array[compindex] *
                             factor);
                    color_iter->color_array[compindex] =
                      (value < 255) ? value : 255;
                  }
              color_iter++;
            }
      }
  else
    for (index = 0; index < nodecount; index++)
        {
          *color_iter = ambient_color;
          color_iter++;
        }

}


cMotiveBasedLight_Tile::cMotiveBasedLight_Tile( float x, float y, float z, int blockx, int blocky, cStaticModel *model, 
					cLightNodeEnvironment &node_environment ) : cMotiveBasedLight( x, y, z, blockx, blocky, model )
{
	assert( model );
	int nodecount = (model->nodes())->size();
	light_node_list = (sLightNode **)malloc( nodecount * sizeof(sLightNode *));

	cLightNodeHandler *node_handler = node_environment.get( 0, 0 );  // get center handler
	node_handler->AddModel( model, x, y, z, light_node_list, node_environment );
}


cMotiveBasedLight_Tile::~cMotiveBasedLight_Tile()
{
	free( light_node_list );
}

void cMotiveBasedLight_Tile::CalcAmbientLight (sColor ambient_color,
                                               sColor sun_color,
                                               float direction[3])
{
  assert (m_model);
  int index;

  sColor *color_iter = ambient_color_array;
  tStaticNodes *nodes = m_model->nodes ();

  int nodecount = nodes->size ();

  sLightNode **light_node = light_node_list;

  if (direction)
      {
        for (index = 0; index < nodecount; index++)
            {
              float *normal = (*light_node)->resulting_normal;

              float factor =
                normal[0] * direction[0] + normal[1] * direction[1] +
                normal[2] * direction[2];
              factor = (factor > 0.0f) ? factor : 0.0f;

              for (int compindex = 0; compindex < 3; compindex++)
                  {
                    int value = (int) ambient_color.color_array[compindex] +
                      (int) ((float) sun_color.color_array[compindex] *
                             factor);
                    color_iter->color_array[compindex] =
                      (value < 255) ? value : 255;

                  }
              color_iter++;
              light_node++;
            }
      }
  else
    for (index = 0; index < nodecount; index++)
        {
          *color_iter = ambient_color;
          color_iter++;
        }

}
