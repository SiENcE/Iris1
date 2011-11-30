//
// File: SceneMaker.h
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

#ifndef _SCENEMAKER_H_
#define _SCENEMAKER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"
#include "SDL/SDL.h"

#include <iostream>
#include "uotype.h"

// #include "../Fluid/mmgr.h"

struct vertex { 
    float u, v, x, y, z; 
    sColor color;
};

extern void init_vertex_buffer ();
extern void free_vertex_buffer ();
extern void vertex_buffer_add (float * v);

extern void add_triangle_to_vertex_buffer (vertex * v1, vertex * v2, vertex * v3, int tex, float move_x, float move_y, float move_z, int alpha);
extern void add_quad_to_vertex_buffer (vertex * v1, vertex * v2, vertex * v3, vertex * v4, int tex, float move_x, float move_y, float move_z, int alpha);

extern void flush_vertex_buffer ();
extern void render_vertex_buffer ();
extern void render_vertex_buffer_transparent ();

#endif //_SCENEMAKER_H_
