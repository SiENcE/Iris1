//
// File: MapBlock3D.cpp
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

#include "renderer/3D/MapBlock3D.h"
#include "include.h"
#include "math.h"
#include "uotype.h"
#include "loaders/Map.h"
#include "renderer/TextureBuffer.h"
#include "renderer/SDLScreen.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/TileDataLoader.h"
#include "loaders/MultisLoader.h"
#include "Geometry.h"
#include "renderer/MapBuffer.h"




int GroundCoords[4][2] = { {0, 1}, {1, 1}, {1, 0}, {0, 0} };
float GroundTexCoords[4][2] =
  { {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0}, {0.0, 0.0} };

extern float water_phase;

int environment_table[8][2] =
  { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };


cMapblock3D::cMapblock3D( int blockx, int blocky ) : cMapblock( blockx, blocky )
{
	cullmode = 0;
	_generated = false;

	light_handler = new cMotiveBasedGroundLight( blockx, blocky, &ground_vertieces[0][0], (float *)&groundnormals );
}

cMapblock3D::~cMapblock3D()
{
	for ( unsigned int i = 0; i < m_vLights.size(); i++ )
	{
		SAFE_DELETE( m_vLights[i] );
	}

	SAFE_DELETE( light_handler );
}


void FilterWater (struct MulCell *cell, float *height, unsigned short *tile,
                  int *water)
{
  if (water)
    *water = 2048;

  if (height)
    *height = (float) cell->z;

  int id = cell->tileid;

  //This should be not hardcoded ! -> XML-File
  if ((id >= 76) && (id <= 112))    /* <=111 Filter water ground */
    id = 22;
  if (((id >= 168) && (id <= 171)) || ((id >= 310) && (id <= 311)))
      {                         /* Filter water itself */
        id = 22;
        if (water)
          *water = cell->z + 128 | 1024;
        if (height)
          *height = (float) (cell->z - 15);
      }

  if (tile)
    *tile = id;
}

void CreateObject (struct sStaticObject *object, unsigned int x,
                   unsigned int y, int z, unsigned int tileid,
                   cStaticModel * model)
{
  object->x = x % 8;
  object->y = y % 8;
  object->z = z;
  object->blockx = x / 8;
  object->blocky = y / 8;
  object->tileid = tileid;

  struct TileDataStaticEntry entry;
  if (pTileDataLoader.LoadEntry (tileid, &entry))
      {
        object->height = entry.height;
        object->tiledata_flags = entry.flags;
      }

  float *bounding_sphere = model->getBoundingSphere ();
/*  object->flags = model->modelflags;
  object->height = model->infos.height;
  object->tiledata_flags = model->infos.flags;*/
  object->sphere[0] = bounding_sphere[0] + object->x;
  object->sphere[1] = bounding_sphere[1] + object->y;
  object->sphere[2] = bounding_sphere[2] + object->z * 0.1f;
  object->sphere[3] = bounding_sphere[3];
  object->alpha = 255;
  object->fader = NULL;
}




bool cMapblock3D::Generate( cLightNodeEnvironment &environment )
{
//  float light_vector[3] =
//    { 0.57735026918963, 0.57735026918963, 0.57735026918963 };
    m_light_direction[0] = 1.0f;
    m_light_direction[1] = 0.0f;
    m_light_direction[2] = 0.0f;

	if ( _generated )
	{
		return false;
	}
	m_ambient_light_color.colorRGB.r = 20;
	m_ambient_light_color.colorRGB.g = 10;
	m_ambient_light_color.colorRGB.b = 10;
	m_ambient_light_color.colorRGB.a = 255;
	m_sun_light_color.colorRGB.r = 240;
	m_sun_light_color.colorRGB.g = 200;
	m_sun_light_color.colorRGB.b = 120;
	m_sun_light_color.colorRGB.a = 255;
/*	ambient_color.colorRGB.r = 80;
	ambient_color.colorRGB.g = 90;
	ambient_color.colorRGB.b = 90;
	sun_color.colorRGB.r = 400;
	sun_color.colorRGB.g = 400;
	sun_color.colorRGB.b = 420;*/

	shader_matrix.setBlock( m_blockx, m_blocky );

	cStaticModel *model;
	float heightmap[10][10];
	float tilemap[4][3];
	//  float groundnormals[9][9][3];

	_generated = true;

//	light_vector[2] *= 10.0f;
//	NormalizeVector( light_vector );


/*
	cNodeList * env_nodes [8];
	for (int i = 0; i < 8; i++) {
		if(environment[i])
			env_nodes[i] = environment[i]->nodelist();
		else
			env_nodes[i] = NULL;
	}
*/

	// bool flag;

	if ( !pMapLoader )
	{
		return false;
	}

	memset( watermap, 0, 64 * 4 );
	memset( ground_vertieces, 0, 9 * 9 * sizeof(vertex) );

	struct MulBlock block;

	pMapLoader->LoadMapBlock (m_blockx, m_blocky, &block);

	int x, y;

	for ( y = 0; y < 8; y++ )
		for ( x = 0; x < 8; x++ )
			FilterWater( &block.cells[y][x], &heightmap[y][x], &groundids[y][x], &watermap[y][x] );

	pMapLoader->LoadMapBlock( m_blockx + 1, m_blocky, &block );
	for ( x = 0; x < 2; x++ )
		for ( y = 0; y < 8; y++ )
			FilterWater( &block.cells[y][x], &heightmap[y][8 + x], NULL, NULL );

	pMapLoader->LoadMapBlock( m_blockx, m_blocky + 1, &block );
	for ( y = 0; y < 2; y++ )
		for ( x = 0; x < 8; x++ )
			FilterWater( &block.cells[y][x], &heightmap[8 + y][x], NULL, NULL );

	pMapLoader->LoadMapBlock( m_blockx + 1, m_blocky + 1, &block );
	for ( y = 0; y < 2; y++ )
		for ( x = 0; x < 2; x++ )
			FilterWater( &block.cells[y][x], &heightmap[8 + y][8 + x], NULL, NULL );

	int i;
	int vertex;

	float zmin = 10000.0f;
	float zmax = -10000.0f;


	/* Calculate Ground Quads */
	for ( y = 0; y < 9; y++ )
		for ( x = 0; x < 9; x++ )
		{
			if ( (float)heightmap[y][x] > zmax )
			{
				zmax = heightmap[y][x];
			}
			if ( (float)heightmap[y][x] < zmin )
			{
				zmin = heightmap[y][x];
			}

			for ( vertex = 0; vertex < 4; vertex++ )
			{
				tilemap[vertex][0] = (float)GroundCoords[vertex][0] + (float)x;
				tilemap[vertex][1] = (float)GroundCoords[vertex][1] + (float)y;
				tilemap[vertex][2] = heightmap[y + GroundCoords[vertex][1]][x + GroundCoords[vertex][0]] * 0.1f;
			}

			groundmap[y][x] = (signed char) (heightmap[y][x]);

			CalcNormal( tilemap[2], tilemap[1], tilemap[0], groundnormals[y][x] );
			NormalizeVector( groundnormals[y][x] );

			float *gnormal = groundnormals[y][x];
			float light_fac = gnormal[0] * m_light_direction[0] + gnormal[1] *
				m_light_direction[1] + gnormal[2] * m_light_direction[2];

			ground_vertieces[y][x].x = (float)x;
			ground_vertieces[y][x].y = (float)y;
			ground_vertieces[y][x].z = heightmap[y][x]*0.1f;
			ground_vertieces[y][x].u = (float)x;
			ground_vertieces[y][x].v = (float)y;

			for ( int i = 0; i < 3; i++ )
			{
				int value =	m_ambient_light_color.color_array[i] +
					(int)( (float) m_sun_light_color.color_array[i] * ((light_fac > 0.0f) ? light_fac : 0.0f) );
				ground_vertieces[y][x].color.color_array[i] = (value < 255) ? value : 255;
				// printf("%i\n", value);
			}

			ground_vertieces[y][x].color.colorRGB.r = 255;
			ground_vertieces[y][x].color.colorRGB.g = 128;
			ground_vertieces[y][x].color.colorRGB.b = 128;

			ground_vertieces[y][x].color.colorRGB.a = 255;
		}

		zmin *= 0.1f;
		zmax *= 0.1f;

		sphere[0] = 4.0f;
		sphere[1] = 4.0f;
		sphere[2] = (zmin + zmax) * 0.5f;
		sphere[3] = sqrt( (zmin - sphere[2]) * (zmin - sphere[2]) + 16.0f + 16.0f );

		int staticcount = 0;
		struct staticinfo *statics = pMapLoader->LoadStatics( m_blockx, m_blocky, staticcount );
		struct staticinfo *statics_p = statics;
		x = m_blockx * 8;
		y = m_blocky * 8;

		for ( i = 0; i < staticcount; i++ )
		{
            //This should be also not hardcoded ! -> XML-File
			if ( ((statics_p->TileID >= 6038) && (statics_p->TileID <= 6066)) ||
                 ((statics_p->TileID >=13422) && (statics_p->TileID <=13445)) ||
                 ((statics_p->TileID >=13460) && (statics_p->TileID <=13483)) ||
                 ((statics_p->TileID >=13496) && (statics_p->TileID <=13514)))
			{                   // Filter out water
				watermap[(int)statics_p->y][(int) statics_p->x] = (int) statics_p->z + 128 | 1024;
			}
			else
            {
				model = StaticModelLoader::GetInstance()->getModel( statics_p->TileID );
				if ( model )
				{
					struct sStaticObject *object = objects.Add();
					object->hue = statics_p->Hue;
					CreateObject( object, x + statics_p->x, y + statics_p->y, statics_p->z, statics_p->TileID, model );

					if ( model->flag( MODELFLAG_TILEABLE ) )
					{
						object->light = new cMotiveBasedLight_Tile( object->x, object->y, object->z, m_blockx, m_blocky, model, environment );
					}
					else
					{
						object->light =	new cMotiveBasedLight_Entity( object->x, object->y, object->z, m_blockx, m_blocky, model );
					}
					m_vLights.push_back( object->light );

					for ( int new_x = -1; new_x <= 1; new_x++ )
						for ( int new_y = -1; new_y <= 1; new_y++ )
						{
							cMapblock *block = pMapbufferHandler.buffer()->Get (m_blockx + new_x, m_blocky + new_y);
							if ( block )
							{
								((cMapblock3D *)block)->GetShaderMatrix()->AddModel (object->x - new_x * 8, object->y - new_y * 8,
									object->z, model );
							}
						}

						//nodehandler.AddModel(model, object->x, object->y, object->z);
						//                  if (model->modelflags & MODELFLAG_TILEABLE) {
						//model->AddToNodes(statics_p->x, statics_p->y, statics_p->z, &nodes, &nodequads, object, env_nodes);
						//                  }
				}
            }
			statics_p++;
		}

		free( statics );

		shader_matrix.AddGround( &groundmap[0][0] );

		nodehandler.CalcNormals();

		std::list<cLight3D *>static_light_list = pLightManager.static_light_list();
		std::list<cLight3D *>::iterator light_iter;
		for ( unsigned int i = 0; i < objects.count(); i++ )
		{
			struct sStaticObject *object = objects.Get (i);
			//object->light->CalcAmbientLight (ambient_color, sun_color, light_direction);

			for (light_iter = static_light_list.begin ();
				light_iter != static_light_list.end (); light_iter++)
				if ((*light_iter)->light_in_block (m_blockx, m_blocky))
				{
					object->light->AddLight (*light_iter);
				}
		}

		for (light_iter = static_light_list.begin ();
			light_iter != static_light_list.end (); light_iter++)
			if ((*light_iter)->light_in_block (m_blockx, m_blocky))
				light_handler->AddLight (*light_iter);

  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
		model = StaticModelLoader::GetInstance()->getModel (object->tileid);
        if (model)
          if (model->GetLightSourceInfo ())
            pLightManager.AddDefinedStaticLightSource (object->x +
                                                        m_blockx * 8,
                                                        object->y +
                                                        m_blocky * 8,
                                                        object->z,
                                                        model->
                                                        GetLightSourceInfo
                                                        ());
      }

  /*nodes.CalcNormals();
     nodes.CalcLight(light_vector);
     GenerateStaticLight (); */
//  nodequads.ReduceQuads();
  //nodequads.Sort();
  m_recalc_ambient_light = true;    // Make sure, that ambient lighting is done

  light_handler->CalcAmbientLight (m_ambient_light_color, m_sun_light_color, m_light_direction,
                                   &shader_matrix);

  // Generate Water
  for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++)
      if (watermap[y][x] & 1024)
          {
            watermap[y][x] &= 1023;
            for (int d = 0; d < 8; d++)
                {
                  int new_x = x + environment_table[d][0];
                  int new_y = y + environment_table[d][1];

                  if ((new_x >= 0) && (new_x < 8) && (new_y >= 0)
                      && (new_y < 8))
                    if (watermap[new_y][new_x] & 2048)
                      watermap[new_y][new_x] = watermap[y][x];
                }
          }

  return true;
}

int tile_coords[4][2] = { {0, 0}, {0, 1}, {1, 1}, {1, 0} };


void cMapblock3D::Render (int x, int y, bool do_culling, float move_x,
                          float move_y)
{
  assert (light_handler);
  _in_use = true;

  glEnable (GL_CULL_FACE);
  flag_cullbackfaces = true;


  Texture *texture = NULL;
  float mx = x * 8.0f;
  float my = y * 8.0f;


  if (do_culling)
      {
        float d = CheckSphereNearPlane (sphere, mx, my);

        if (d > 0.0f)
          cullmode = 0;
        if (d == 0.0f)
          cullmode = 1;
        if (d < 0.0f)
          cullmode = 2;
      }

  if (cullmode == 0)
      {


        if (nodehandler.getNormalsChanged () || m_recalc_ambient_light)
            {
              for (unsigned int i = 0; i < objects.count (); i++)
                  {
                    struct sStaticObject *object = objects.Get (i);
                    object->light->CalcAmbientLight (m_ambient_light_color, m_sun_light_color,
                                                     m_light_direction);
                  }
              light_handler->CalcAmbientLight (m_ambient_light_color, m_sun_light_color,
                                               m_light_direction,
                                               &shader_matrix);
              m_recalc_ambient_light = false;
            }


        light_handler->PrepareForRendering ();
        for (int tx = 0; tx < 8; tx++)
          for (int ty = 0; ty < 8; ty++)
              {
                int texid = 0;
                  texture = TextureBuffer::GetInstance()->GetGroundTexture (groundids[ty][tx]);
                if (texture)
                  texid = texture->GetGLTex ();
                add_quad_to_vertex_buffer (&ground_vertieces[ty][tx],
                                           &ground_vertieces[ty + 1][tx],
                                           &ground_vertieces[ty + 1][tx + 1],
                                           &ground_vertieces[ty][tx + 1],
                                           texid, move_x, move_y, 0.0f, 255);
              }

      }




  if (cullmode != 2)
      {
        if (do_culling)
          CreateFrustum (mx, my);


        for (unsigned int i = 0; i < objects.count (); i++)
            {
              struct sStaticObject *object = objects.Get (i);
              if (object->fader)

                  {
                    int act_alpha = (int) (object->fader->value () + 0.5f);
                    if (act_alpha >= 255)
                      act_alpha = 255;
                    if (act_alpha <= 0)
                      act_alpha = 0;
                    object->alpha = act_alpha;
                  }

              cStaticModel *model =
				  StaticModelLoader::GetInstance()->getModel (object->tileid);
              if ((model) && (!object->clip))
                  {
                    object->light->PrepareModelForRendering ();
                    model->Render (object->x + move_x, object->y + move_y,
                                   object->z * 0.1f, object->alpha);
                  }

            }

      }


}

void cMapblock3D::CreateFrustum (float x, float y)
{
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
            {
              if (object->alpha > 0)
                object->clip = (!SphereInFrustum (object->sphere, x, y));
              else
                object->clip = true;
            }
      }
}

void cMapblock3D::RenderWater (int dx, int dy, bool do_culling)
{
  if (cullmode == 2)
    return;

  float mx = dx * 8.0;
  float my = dy * 8.0;

  glDisable (GL_ALPHA_TEST);
  glNormal3f (0.0, 0.0, 1.0);
  glBegin (GL_QUADS);
  glDisable (GL_LIGHTING);
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
      if (!(watermap[y][x] & 2048))
          {
            float sphere[4];

            sphere[0] = (float) x + 0.5f;

            sphere[1] = (float) y + 0.5f;
            sphere[2] = (float) ((watermap[y][x] & 511) - 128) * 0.1f;
            sphere[3] = 1.0f;

            if (do_culling)
                {
                  if (SphereInFrustum (sphere, mx, my))
                    watermap[y][x] |= 4096;
                  else
                    watermap[y][x] &= 4095;
                }

            if (watermap[y][x] & 4096)
                {
                  for (int vertex = 0; vertex < 4; vertex++)
                      {
                        float u = (x + GroundCoords[vertex][0]) * 0.125;
                        float v = (y + GroundCoords[vertex][1]) * 0.125;

                        glTexCoord2f (u, v);
                        glVertex3f (x + GroundCoords[vertex][0],
                                    y + GroundCoords[vertex][1],
                                    (((watermap[y][x] & 511) - 128 +
                                      water_sin_table[(int)
                                                      (water_phase *
                                                       32.0) & 31][((x +
                                                                     GroundCoords
                                                                     [vertex]
                                                                     [0]) +
                                                                    (y +
                                                                     GroundCoords
                                                                     [vertex]
                                                                     [1])) &
                                                                   7])) *
                                    0.1f);
                      }
                }
          }
  glEnd ();
  glEnable (GL_ALPHA_TEST);
}






sStaticObject *cMapblock3D::CheckRay (float vecOrigin[3], float vecDir[3],
                                      float &distance, float deltax,
                                      float deltay, int max_z)
{

  float act_dist, lambda, act_lambda, lam2;
  sStaticObject *result = NULL;
  act_lambda = 1000000.0f;

  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        if (object)
          if ((!object->clip) && (object->alpha == 255)
              && (object->z <= max_z))
              {
                float sphere[3];
                sphere[0] = object->sphere[0] + deltax;
                sphere[1] = object->sphere[1] + deltay;
                sphere[2] = object->sphere[2] * 0.1f;
                act_dist =
                  GetSquaredDistRayPoint (vecOrigin, vecDir, sphere, lam2);
                if (lam2 > 0) {
//                  if (act_dist < (object->sphere[3] * object->sphere[3]))
                      {
                        cStaticModel *model =
							StaticModelLoader::GetInstance()->getModel (object->tileid);
                        if (model)
                            {
                              if (model->
                                  CheckRay (vecOrigin, vecDir,
                                            object->x + deltax,
                                            object->y + deltay, object->z,
                                            lambda))
                                  {

                                    if (lambda < act_lambda)
                                        {
                                          act_lambda = lambda;
                                          result = object;
                                        }
                                  }
                            }
                      }

                      }
              }
      }
  if (result)
    distance = act_lambda;

  return result;
}


Uint8 cMapblock3D::CheckRayOnGround (float vecOrigin[3], float vecDir[3],
                                     float &distance, float deltax,
                                     float deltay, int &height)
{
  if (cullmode == 2)
    return 255;

  float lambda, act_lambda;
  int result = 255;
  act_lambda = 1000000.0f;

  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
        {
//          int id = (x + y * 8) * 4 * 3;
          float vecs[4][3];
          for (int i = 0; i < 4; i++)
              {
                vertex *v =
                  &ground_vertieces[y + GroundCoords[i][0]][x +
                                                            GroundCoords[i]
                                                            [1]];
                vecs[i][0] = v->x + deltax;
                vecs[i][1] = v->y + deltay;
                vecs[i][2] = v->z;
              }

          if (IntersectTriangleWithRay
              (vecOrigin, vecDir, vecs[0], vecs[1], vecs[2], lambda))
            if (lambda < act_lambda)
                {
                  act_lambda = lambda;
                  result = x + y * 8;
                  height = groundmap[y][x];
                }

          if (IntersectTriangleWithRay
              (vecOrigin, vecDir, vecs[0], vecs[2], vecs[3], lambda))
            if (lambda < act_lambda)
                {
                  act_lambda = lambda;
                  result = x + y * 8;
                  height = groundmap[y][x];
                }
        }
  distance = act_lambda;
  return result;
}


void cMapblock3D::AddLight (cLight3D * light)
{
  assert (light);
  light_handler->AddLight (light);
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        object->light->AddLight (light);
      }

}

void cMapblock3D::RemoveLight (cLight3D * light)
{
  assert (light);
  light_handler->RemoveLight (light);
  for (unsigned int i = 0; i < objects.count (); i++)
      {
        struct sStaticObject *object = objects.Get (i);
        object->light->RemoveLight (light);
      }
}

void cMapblock3D::SetRecalcAmbientLightFlag ()
{
  m_recalc_ambient_light = true;
}

void cMapblock3D::AddMultiObject(Uint32 id, Uint16 tileid, Uint16 dye, int x, int y, int z)
{

	cStaticModel *model = StaticModelLoader::GetInstance()->getModel (tileid);
  if (!model) return;
  struct sStaticObject *object = objects.Add ();
  object->hue = dye;
  CreateObject (object, x, y, z, tileid, model);


//  int blockx = x / 8;
//  int blocky = y / 8;

  object->light = new cMotiveBasedLight_Entity (object->x, object->y,
                                                object->z, m_blockx,
                                                m_blocky, model);
  for (int new_x = -1; new_x <= 1; new_x++)
    for (int new_y = -1; new_y <= 1; new_y++)
    {
        cMapblock *block = pMapbufferHandler.buffer()->Get (m_blockx + new_x, m_blocky + new_y);
        if (!block) continue;
        ((cMapblock3D *) block)->GetShaderMatrix ()->AddModel (object->x - new_x * 8,
                                                               object->y - new_y * 8,
                                                               object->z, model);
    }
}

void cMapblock3D::set_light_color (sColor ambient_color, sColor sun_color)
{
     this->m_ambient_light_color = ambient_color;
     this->m_sun_light_color = sun_color;
     m_recalc_ambient_light = true;
}

void cMapblock3D::set_light_direction (float light_direction[3])
{
     m_light_direction[0] = light_direction[0];
     m_light_direction[1] = light_direction[1];
     m_light_direction[2] = light_direction[2];
     m_recalc_ambient_light = true;
}
