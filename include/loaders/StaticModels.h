//
// File: StaticModels.h
// Created by:  Alexander Oster - tensor@ultima-iris.de
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

#ifndef _STATICMODELS_H_
#define _STATICMODELS_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "Common.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <map>
#include <vector>

#include "SDL/SDL.h"

#include "irisgl.h"
#include "Logger.h"
#include "Exception.h"
#include "Geometry.h"
#include "uotype.h"
#include "StaticModelRaster.h"
#include "StaticTextureLoader.h"
#include "iris_endian.h"
#include "renderer/TextureBuffer.h"
#include "renderer/3D/SceneMaker.h"
#include "loaders/StaticTextureLoader.h"

// #include "../Fluid/mmgr.h"

#define FACEFLAG_BACKFACE 1


class cStaticModel;

class cStaticModelLightSourceInfo
{
private:
	bool m_flickering;
	float m_x, m_y, m_z, m_radius;
	sColor m_color;
	int m_flicker_min_delay;
	int m_flicker_max_delay;
	int m_flicker_amount;
public:
	cStaticModelLightSourceInfo( bool flickering, float x, float y, float z, float radius, sColor color, int flicker_min_delay, int flicker_max_delay, int flicker_amount );
	bool flickering();
	float x();
	float y();
	float z();
	float radius();
	sColor color();
	int flicker_min_delay();
	int flicker_max_delay();
	int flicker_amount();
};

class cStaticModelParticleEffectInfo {
    private:
        std::string m_name;
        float m_x, m_y, m_z;
        bool m_rotate;

        float m_angle1, m_angle2;
    public:
        cStaticModelParticleEffectInfo (std::string name, float x, float y, float z, bool rotate, float angle1, float angle2);
        std::string name ();
        float x ();
        float y ();
        float z ();
        float angle1 ();
        float angle2 ();
        bool rotate ();
};

class cStaticModelNode {
    friend class cStaticModel;
    private:
       float m_position[3];
       float m_normal[3];
    public:
       cStaticModelNode (float position[3], float normal[3]);
       ~cStaticModelNode ();
       
       void CreateVertex (vertex * v);
       float calc_light_factor (float light[3]);
       float * normal ();
       float * position ();
};

class cStaticModelPointLightNode {
    friend class cStaticModel;
    private:
       float m_position[3];
       float m_normal[3];
    public:
       cStaticModelPointLightNode (float position[3], float normal[3]);
       float * normal ();
       float * position ();
       float calc_light_factor (float light[3]);
};

class cStaticModelFaceLightNode {
    friend class cStaticModel;
    private:
        Uint32 m_node_index;
        Uint32 m_point_light_index;
        Uint32 m_parallel_light_index;
    public:
       cStaticModelFaceLightNode (Uint32 node_index, Uint32 point_light_index, Uint32 parallel_light_index);
       Uint32 node_index ();
       Uint32 point_light_index ();
       Uint32 parallel_light_index ();
};

typedef std::vector <cStaticModelNode *> tStaticNodes;
typedef std::vector <cStaticModelPointLightNode *> tStaticPointLightNodes;
typedef std::vector <cStaticModelFaceLightNode *> tStaticFaceLightNodes;

class cStaticModelFace {
    friend class cStaticModel;
    private:
       cStaticModelNode * m_nodes[3];
       cStaticModelFaceLightNode * m_face_light_nodes[3];
       Uint32 m_node_indices[3];
       float m_normal[3];
       float m_texcoords[3][2];
       Uint32 m_gl_texture;
       int m_texture;
       Uint32 m_flags;
    public:
       cStaticModelFace (Uint32 node_indices[3], cStaticModelNode * nodes[3], float normal[3], float texcoords[3][2], Uint32 flags);
       ~cStaticModelFace ();
       
       void setTexture (int texture) { m_texture = texture; }
       Uint32 texture () { return m_texture; }
};


class cStaticModel
{  
public:
	cStaticModel( std::ifstream *stream, Uint32 length, cStaticTextureLoader *texture_loader );
	~cStaticModel();

	Uint32 modelid() { return m_modelid; }

	void Render( float x, float y, float z, Uint8 alpha );
	float *getBoundingSphere() { return bounding_sphere; }
	tStaticNodes *nodes() { return &m_nodes; }
	tStaticPointLightNodes *point_light_nodes() { return &m_point_light_nodes; }
	tStaticFaceLightNodes *face_light_nodes() { return &m_face_light_nodes; }
	vertex * vertieces() { return m_vertieces; }
	bool flag( Uint32 flag_mask ) { return m_flags & flag_mask; }

	cStaticModelLightSourceInfo *GetLightSourceInfo();
	cStaticModelParticleEffectInfo *GetParticleEffectInfo();
	cStaticModelRaster *raster() { return &m_raster; }
	bool CheckRay( float vecOrigin[3], float vecDir[3], float deltax, float deltay, float deltaz, float &lambda );

private:
	Uint32 m_modelid;
	tStaticNodes m_nodes;
	tStaticFaceLightNodes m_face_light_nodes;
	tStaticPointLightNodes m_point_light_nodes;
	std::vector <cStaticModelFace *> m_faces;
	Uint32 m_flags;

	vertex *m_vertieces;
	cStaticModelLightSourceInfo *light_source_info;
	cStaticModelParticleEffectInfo *particle_effect_info;

	cStaticModelRaster m_raster;
	cStaticTextureLoader *texture_loader;

	float bounding_sphere[4];

	void CreateVertieces();
};

#endif //_STATICMODELS_H_
