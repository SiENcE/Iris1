//
// File: SceneMaker.cpp
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
#include "renderer/3D/SceneMaker.h"
#include "Logger.h"
#include "Exception.h"
#include "include.h"
#include "Config.h"
#include "uotype.h"
#include "Geometry.h"
#include <vector>
#include <cassert>
#include <map>




using namespace std;

vertex *vertex_buffer = NULL;

int vertex_buffer_size = 10000;
int vertex_buffer_pos = 0;

struct triangle
{
  int start;
  int num;
  int tex;
  int transp;
};

typedef vector < struct triangle >triangle_vec;

triangle_vec triangle_buffer;
triangle_vec transparent_buffer;

int act_tex = -1;

void init_vertex_buffer ()
{
  if (vertex_buffer)
    THROWEXCEPTION ("double init of vertex buffer");
  vertex_buffer = (vertex *) malloc (sizeof (vertex) * (vertex_buffer_size + 9));   // 9 security buffer
  if (!vertex_buffer)
    THROWEXCEPTION ("vertex buffer out of memory");

  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);
  glEnableClientState (GL_COLOR_ARRAY);
  glVertexPointer (3, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].x);
  glTexCoordPointer (2, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].u);
  glColorPointer (4, GL_UNSIGNED_BYTE, sizeof (vertex),
                  &vertex_buffer[0].color);
}

void resize_vertex_buffer ()
{
  if (!vertex_buffer)
    THROWEXCEPTION ("vertex buffer fatal error");
  vertex_buffer_size *= 2;
  vertex_buffer = (vertex *) realloc (vertex_buffer, sizeof (vertex) * (vertex_buffer_size + 9));     // 9 security buffer
  if (!vertex_buffer)
    THROWEXCEPTION ("vertex buffer out of memory");
  glVertexPointer (3, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].x);
  glTexCoordPointer (2, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].u);
  glColorPointer (4, GL_UNSIGNED_BYTE, sizeof (vertex),
                  &vertex_buffer[0].color);
}

void free_vertex_buffer ()
{
  flush_vertex_buffer ();

  free (vertex_buffer);
  vertex_buffer = NULL;
}

void flush_vertex_buffer ()
{
  triangle_buffer.clear ();
  transparent_buffer.clear ();

  vertex_buffer_pos = 0;
  act_tex = -1;
}

void add_triangle_to_vertex_buffer (vertex * v1, vertex * v2, vertex * v3,
                                    int tex, float move_x, float move_y,
                                    float move_z, int alpha)
{

  triangle_vec *triangles =
    (alpha != 255) ? &transparent_buffer : &triangle_buffer;

  triangle act_triangle;

  act_triangle.start = vertex_buffer_pos;
  act_triangle.num = 3;
  act_triangle.tex = tex;
  act_triangle.transp = alpha != 255;
  triangles->push_back (act_triangle);

  vertex & vec_1 = vertex_buffer[vertex_buffer_pos++];
  vec_1 = *v1;
  vec_1.x += move_x;
  vec_1.y += move_y;
  vec_1.z += move_z;
  vec_1.color.colorRGB.a = alpha;

  vertex & vec_2 = vertex_buffer[vertex_buffer_pos++];
  vec_2 = *v2;
  vec_2.x += move_x;
  vec_2.y += move_y;
  vec_2.z += move_z;
  vec_2.color.colorRGB.a = alpha;

  vertex & vec_3 = vertex_buffer[vertex_buffer_pos++];
  vec_3 = *v3;
  vec_3.x += move_x;
  vec_3.y += move_y;
  vec_3.z += move_z;
  vec_3.color.colorRGB.a = alpha;

  if (vertex_buffer_pos >= vertex_buffer_size)
    resize_vertex_buffer ();

}

void add_quad_to_vertex_buffer (vertex * v1, vertex * v2, vertex * v3,
                                vertex * v4, int tex, float move_x,
                                float move_y, float move_z, int alpha)
{
  triangle_vec *triangles =
    (alpha != 255) ? &transparent_buffer : &triangle_buffer;

  triangle act_triangle;

  act_triangle.start = vertex_buffer_pos;
  act_triangle.num = 4;
  act_triangle.tex = tex;
  act_triangle.transp = alpha != 255;
  triangles->push_back (act_triangle);

  vertex & vec_1 = vertex_buffer[vertex_buffer_pos++];
  vec_1 = *v1;
  vec_1.x += move_x;
  vec_1.y += move_y;
  vec_1.z += move_z;
  vec_1.color.colorRGB.a = alpha;

  vertex & vec_2 = vertex_buffer[vertex_buffer_pos++];
  vec_2 = *v2;
  vec_2.x += move_x;
  vec_2.y += move_y;
  vec_2.z += move_z;
  vec_2.color.colorRGB.a = alpha;

  vertex & vec_3 = vertex_buffer[vertex_buffer_pos++];
  vec_3 = *v4;
  vec_3.x += move_x;
  vec_3.y += move_y;
  vec_3.z += move_z;
  vec_3.color.colorRGB.a = alpha;

//   vertex_buffer[vertex_buffer_pos++] = vec_3;

  vertex & vec_4 = vertex_buffer[vertex_buffer_pos++];
  vec_4 = *v3;
  vec_4.x += move_x;
  vec_4.y += move_y;
  vec_4.z += move_z;
  vec_4.color.colorRGB.a = alpha;

  //vertex_buffer[vertex_buffer_pos++] = vec_1;

  if (vertex_buffer_pos >= vertex_buffer_size)
    resize_vertex_buffer ();

}


void render_vertex_buffer ()
{
  glVertexPointer (3, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].x);
  glTexCoordPointer (2, GL_FLOAT, sizeof (vertex), &vertex_buffer[0].u);
  glColorPointer (4, GL_UNSIGNED_BYTE, sizeof (vertex),
                  &vertex_buffer[0].color);

  int act_tex = -1;
  for (unsigned int i = 0; i < triangle_buffer.size (); i++)
      {
        triangle & act_triangle = triangle_buffer[i];
        if (act_triangle.tex != act_tex)
            {
              act_tex = act_triangle.tex;
              glBindTexture (GL_TEXTURE_2D, act_tex);
            }
        glDrawArrays (GL_TRIANGLE_STRIP, act_triangle.start,
                      act_triangle.num);

      }
}

void render_vertex_buffer_transparent ()
{
  glDisable (GL_ALPHA_TEST);

  for (unsigned int i = 0; i < transparent_buffer.size (); i++)
      {
        triangle & act_triangle = transparent_buffer[i];
        if (act_triangle.tex != act_tex)
            {
              act_tex = act_triangle.tex;
              glBindTexture (GL_TEXTURE_2D, act_tex);
            }
        glDrawArrays (GL_TRIANGLE_STRIP, act_triangle.start,
                      act_triangle.num);
      }

  glEnable (GL_ALPHA_TEST);

//   printf("Rendered %i polygons (%i with alpha channel)\n", polycount + transcount, transcount);
}
