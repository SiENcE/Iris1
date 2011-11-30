//
// File: StaticModels.cpp
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



#include "loaders/StaticModels.h"




using namespace std;

char FILEID_GAMEMODEL[] = "UI3G";

extern bool CheckHeaderID (char *ID1, char *ID2);

#if defined(WIN32) && !defined(__MINGW32__) 
// Visual C pragma 
#pragma pack(push, 1) 
#endif

struct sGameModelFileHeader
{
  char Sign[4];
  Uint32 Length;
  Uint32 Version;
  Uint32 ModelID;
  Uint32 Flags;
  Uint32 NodeStart;
  Uint32 NodeCount;
  Uint32 FaceStart;
  Uint32 FaceCount;
  Uint32 FaceLightNodeStart;
  Uint32 FaceLightNodeCount;
  Uint32 PointLightNodeStart;
  Uint32 PointLightNodeCount;
  Uint32 ParallelLightNodeStart;
  Uint32 ParallelLightNodeCount;
  Uint32 LightSourceInfoStart;
  float bounding_sphere[4];
  Uint32 RasterStart;
  double LastChange;
  Uint32 EffectStart;
  Uint32 Reserved[8];
} STRUCT_PACKED;

struct sGameModelFileNode
{
  float Vector[3];
  float Normal[3];
} STRUCT_PACKED;

struct sGameModelPointLightNode
{
  float Vector[3];
  float Normal[3];
} STRUCT_PACKED;

struct sGameModelParallelLightNode
{
  float Normal[3];
} STRUCT_PACKED;

struct sGameModelFaceLightNode
{
  Uint32 NodeIndex;
  Uint32 PointLightNodeIndex;
  Uint32 ParallelLightNodeIndex;
} STRUCT_PACKED;

struct sGameModelFileFace
{
  Uint32 Nodes[3];
  Uint32 FaceLightNodes[3];
  float Normal[3];
  float TexCoords[3][2];
  Sint32 TextureIndex;
  Uint32 Flags;
} STRUCT_PACKED;

struct sGameModelLightSourceInfo
{
  bool ModelIsLightSource;
  bool Flickering;
  float X, Y, Z, Radius;
  Uint8 r, g, b;
  int flicker_min_delay;
  int flicker_max_delay;
  int flicker_amount;
  Uint32 Reserved[4];
} STRUCT_PACKED;

struct sGameModelParticleEffectInfo
{
  char EffectName[32];
  float X, Y, Z;
  Uint32 FRotateEffect;
  float FAngle1, FAngle2;
  Uint32 Reserved[6];
} STRUCT_PACKED;

#if defined(WIN32) && !defined(__MINGW32__) 
#pragma pack(pop) 
#endif

cStaticModelNode::cStaticModelNode (float position[3], float normal[3])
{
  for (int index = 0; index < 3; index++)
      {
        m_position[index] = position[index];
        m_normal[index] = normal[index];
      }
}


cStaticModelNode::~cStaticModelNode ()
{
}

void cStaticModelNode::CreateVertex (vertex * v)
{
  assert (v);
  v->x = m_position[0];
  v->y = m_position[1];
  v->z = m_position[2];
  v->color.colorRGB.r = 255;
  v->color.colorRGB.g = 255;
  v->color.colorRGB.b = 255;
  v->color.colorRGB.a = 255;
}

float cStaticModelNode::calc_light_factor (float light[3])
{
  float res =
    m_normal[0] * light[0] + m_normal[1] * light[1] + m_normal[2] * light[2];
  return (res > 0.0f) ? res : 0.0f;
}

float *cStaticModelNode::normal ()
{
  return m_normal;
}

float *cStaticModelNode::position ()
{
  return m_position;
}

cStaticModelPointLightNode::cStaticModelPointLightNode (float position[3],
                                                        float normal[3])
{
  for (int index = 0; index < 3; index++)
      {
        m_position[index] = position[index];
        m_normal[index] = normal[index];
      }
}


float *cStaticModelPointLightNode::position ()
{
  return m_position;
}

float *cStaticModelPointLightNode::normal ()
{
  return m_normal;
}

float cStaticModelPointLightNode::calc_light_factor (float light[3])
{
  float res =
    m_normal[0] * light[0] + m_normal[1] * light[1] + m_normal[2] * light[2];
  return (res > 0.0f) ? res : 0.0f;
}

cStaticModelFaceLightNode::cStaticModelFaceLightNode (Uint32 node_index,
                                                      Uint32
                                                      point_light_index,
                                                      Uint32
                                                      parallel_light_index)
{
  m_node_index = node_index;
  m_point_light_index = point_light_index;
  m_parallel_light_index = parallel_light_index;
}

Uint32 cStaticModelFaceLightNode::node_index ()
{
  return m_node_index;
}

Uint32 cStaticModelFaceLightNode::point_light_index ()
{
  return m_point_light_index;
}

Uint32 cStaticModelFaceLightNode::parallel_light_index ()
{
  return m_parallel_light_index;
}

cStaticModelFace::cStaticModelFace (Uint32 node_indices[3],
                                    cStaticModelNode * nodes[3],
                                    float normal[3], float texcoords[3][2],
                                    Uint32 flags)
{
  for (int index = 0; index < 3; index++)
      {
        m_nodes[index] = nodes[index];
        m_normal[index] = normal[index];
        m_texcoords[index][0] = texcoords[index][0];
        m_texcoords[index][1] = texcoords[index][1];
        m_node_indices[index] = node_indices[index];
      }
  m_texture = 0;
  m_gl_texture = 0;
  m_flags = flags;
}

cStaticModelFace::~cStaticModelFace ()
{
}

cStaticModel::cStaticModel( ifstream * stream, Uint32 length, cStaticTextureLoader *texture_loader )
{
	ASSERT( texture_loader );
	ASSERT( stream );

	unsigned int index;
	Uint32 stream_start = stream->tellg ();
	Uint32 stream_end = stream_start + length;

	sGameModelFileHeader header;

	particle_effect_info = NULL;
	light_source_info = NULL;

	this->texture_loader = texture_loader;


	if ( length < sizeof(header) )
	{
		THROWEXCEPTION ("invalid texture stream size (<headersize)");
	}

	stream->read( (char *)&header, sizeof(header) );
	header.Length = IRIS_SwapU32( header.Length );
	header.Version = IRIS_SwapU32( header.Version );
	header.ModelID = IRIS_SwapU32( header.ModelID );
	header.Flags = IRIS_SwapU32( header.Flags );
	header.NodeStart = IRIS_SwapU32( header.NodeStart );
	header.NodeCount = IRIS_SwapU32( header.NodeCount );
	header.FaceStart = IRIS_SwapU32( header.FaceStart );
	header.FaceCount = IRIS_SwapU32( header.FaceCount );
	header.FaceLightNodeStart = IRIS_SwapU32( header.FaceLightNodeStart );
	header.FaceLightNodeCount = IRIS_SwapU32( header.FaceLightNodeCount );
	header.PointLightNodeStart = IRIS_SwapU32( header.PointLightNodeStart );
	header.PointLightNodeCount = IRIS_SwapU32( header.PointLightNodeCount );
	header.ParallelLightNodeStart = IRIS_SwapU32( header.ParallelLightNodeStart );
	header.ParallelLightNodeCount = IRIS_SwapU32( header.ParallelLightNodeCount );
	header.LightSourceInfoStart = IRIS_SwapU32( header.LightSourceInfoStart );
	header.EffectStart = IRIS_SwapU32( header.EffectStart );

	for ( int ii = 0; ii < 4; ii++ )
	{
		header.bounding_sphere[ii] = IRIS_FloatFromLittle( header.bounding_sphere[ii] );
	}
	
	for ( int ii = 0; ii < 8; ii++ )
	{
		header.Reserved[ii] = IRIS_SwapU32( header.Reserved[ii] );
	}

	m_flags = header.Flags;

	/* Check some header information */
	if ( !CheckHeaderID( header.Sign, FILEID_GAMEMODEL ) )
	{
		THROWEXCEPTION( "invalid model stream" );
	}
	if ( header.Version != GAMEMODEL_VERSION )
	{
		THROWEXCEPTION( "model stream has an invalid version!" );
	}
	if ( length != header.Length )
	{
		THROWEXCEPTION( "invalid model stream size" );
	}

	if ( (header.NodeStart > stream_end) || (header.NodeStart + header.NodeCount * sizeof(sGameModelFileNode) > stream_end) )
	{
		THROWEXCEPTION( "invalid model stream" );
	}

	if ( (header.PointLightNodeStart > stream_end) || 
		(header.PointLightNodeStart + header.PointLightNodeCount * sizeof(sGameModelPointLightNode) > stream_end) )
	{
		THROWEXCEPTION( "invalid model stream" );
	}

	if ( (header.FaceLightNodeStart > stream_end) || 
	  (header.FaceLightNodeStart + header.FaceLightNodeCount * sizeof(sGameModelFaceLightNode) > stream_end) )
	{
		THROWEXCEPTION( "invalid model stream" );
	}

	if ( header.LightSourceInfoStart )
	{
		if ( (header.LightSourceInfoStart > stream_end) || (header.LightSourceInfoStart + sizeof(sGameModelLightSourceInfo) > stream_end) )
		{
			THROWEXCEPTION( "invalid model stream" );
		}

		sGameModelLightSourceInfo info;
		stream->seekg( header.LightSourceInfoStart + stream_start, std::ios::beg );
		stream->read ( (char *)&info, sizeof(info) );
		info.X = IRIS_FloatFromLittle( info.X );
		info.Y = IRIS_FloatFromLittle( info.Y );
		info.Z = IRIS_FloatFromLittle( info.Z );
		info.Radius = IRIS_FloatFromLittle( info.Radius );
		info.flicker_min_delay = IRIS_SwapI32( info.flicker_min_delay );
		info.flicker_max_delay = IRIS_SwapI32( info.flicker_max_delay );
		info.flicker_amount = IRIS_SwapI32( info.flicker_amount );

		for ( int ii = 0; ii < 4; ii++ )
		{
			info.Reserved[ii] = IRIS_SwapU32( info.Reserved[ii] );
		}

		if ( info.ModelIsLightSource )
		{
			sColor color;
			color.colorRGB.r = info.r;
			color.colorRGB.g = info.g;
			color.colorRGB.b = info.b;
			color.colorRGB.a = 255;
			light_source_info = new cStaticModelLightSourceInfo( info.Flickering, info.X, info.Y, info.Z, 
				info.Radius, color, info.flicker_min_delay, info.flicker_max_delay, info.flicker_amount );
		}
	}

	if ( header.EffectStart )
	{
		struct sGameModelParticleEffectInfo effectinfo;

		stream->seekg( header.EffectStart + stream_start, std::ios::beg );
		stream->read( (char *)&effectinfo, sizeof(effectinfo) );
		effectinfo.X = IRIS_FloatFromLittle( effectinfo.X );
		effectinfo.Y = IRIS_FloatFromLittle( effectinfo.Y );
		effectinfo.Z = IRIS_FloatFromLittle( effectinfo.Z );
		effectinfo.FAngle1 = IRIS_FloatFromLittle( effectinfo.FAngle1 );
		effectinfo.FAngle2 = IRIS_FloatFromLittle( effectinfo.FAngle2 );
		effectinfo.FRotateEffect = IRIS_SwapI32( effectinfo.FRotateEffect );
		char name[33];
		name[33] = 0;
		memcpy( name, &effectinfo.EffectName[0], 32 );
		particle_effect_info = new cStaticModelParticleEffectInfo( string( name ), effectinfo.X, effectinfo.Y, effectinfo.Z,
			effectinfo.FRotateEffect, effectinfo.FAngle1, effectinfo.FAngle2 );
	}

	m_modelid = header.ModelID;

	for ( index = 0; index < 4; index++ )
	{
		bounding_sphere[index] = header.bounding_sphere[index];
	}

	stream->seekg( header.NodeStart + stream_start, std::ios::beg );
	for ( index = 0; index < header.NodeCount; index++ )
	{
		sGameModelFileNode nodeentry;
		stream->read( (char *)&nodeentry, sizeof(nodeentry) );
		for ( int ii = 0; ii < 3; ii++ )
		{
			nodeentry.Vector[ii] = IRIS_FloatFromLittle( nodeentry.Vector[ii] );
			nodeentry.Normal[ii] = IRIS_FloatFromLittle( nodeentry.Normal[ii] );
		}
		m_nodes.push_back( new cStaticModelNode( nodeentry.Vector, nodeentry.Normal ) );
	}

	stream->seekg( header.FaceLightNodeStart + stream_start, std::ios::beg );
	for ( index = 0; index < header.FaceLightNodeCount; index++ )
	{
		sGameModelFaceLightNode nodeentry;
		stream->read( (char *)&nodeentry, sizeof(nodeentry) );
		nodeentry.NodeIndex = IRIS_SwapU32( nodeentry.NodeIndex );
		nodeentry.PointLightNodeIndex = IRIS_SwapU32( nodeentry.PointLightNodeIndex );
		nodeentry.ParallelLightNodeIndex = IRIS_SwapU32 ( nodeentry.ParallelLightNodeIndex );
		m_face_light_nodes.push_back( new cStaticModelFaceLightNode( nodeentry.NodeIndex, 
			nodeentry.PointLightNodeIndex, nodeentry.ParallelLightNodeIndex ) );
	}

	stream->seekg( header.PointLightNodeStart + stream_start, ios::beg );
	for ( index = 0; index < header.PointLightNodeCount; index++ )
	{
		sGameModelPointLightNode nodeentry;
		stream->read( (char *)&nodeentry, sizeof(nodeentry) );
		for ( int ii = 0; ii < 3; ii++ )
		{
			nodeentry.Vector[ii] = IRIS_FloatFromLittle( nodeentry.Vector[ii] );
			nodeentry.Normal[ii] = IRIS_FloatFromLittle( nodeentry.Normal[ii] );
		}
		m_point_light_nodes.push_back( new cStaticModelPointLightNode( nodeentry.Vector, nodeentry.Normal ) );
	}

	stream->seekg (header.FaceStart + stream_start, ios::beg);
	for ( index = 0; index < header.FaceCount; index++ )
	{
		int nodeindex;
		sGameModelFileFace faceentry;
		stream->read( (char *)&faceentry, sizeof(faceentry) );
		for ( int ii = 0; ii < 3; ii++ )
		{
			faceentry.Nodes[ii] = IRIS_SwapU32( faceentry.Nodes[ii] );
			faceentry.FaceLightNodes[ii] = IRIS_SwapU32( faceentry.FaceLightNodes[ii] );
			faceentry.Normal[ii] = IRIS_FloatFromLittle( faceentry.Normal[ii] );
			for ( int jj = 0; jj < 2; jj++ )
			{
				faceentry.TexCoords[ii][jj] = IRIS_FloatFromLittle( faceentry.TexCoords[ii][jj] );
			}
		}
		faceentry.TextureIndex = IRIS_SwapI32 (faceentry.TextureIndex);
		faceentry.Flags = IRIS_SwapU32 (faceentry.Flags);

		cStaticModelNode *model_nodes[3];

		for ( nodeindex = 0; nodeindex < 3; nodeindex++ )
		{
			if ( faceentry.Nodes[nodeindex] >= m_nodes.size() )
			{
				THROWEXCEPTION( "invalid model stream" );
			}
			model_nodes[nodeindex] = m_nodes[faceentry.Nodes[nodeindex]];
		}

		cStaticModelFace *face = new cStaticModelFace( faceentry.FaceLightNodes, model_nodes, 
			faceentry.Normal, faceentry.TexCoords, faceentry.Flags );

		for ( nodeindex = 0; nodeindex < 3; nodeindex++ )
		{
			if ( faceentry.FaceLightNodes[nodeindex] >= m_face_light_nodes.size() )
			{
				THROWEXCEPTION ("invalid model stream");
			}
			face->m_face_light_nodes[nodeindex] = m_face_light_nodes[faceentry.FaceLightNodes[nodeindex]];
		}

		if ( faceentry.TextureIndex >= 0 )
		{
			face->setTexture( faceentry.TextureIndex );
		}
		else
		{
			face->setTexture( -m_modelid );
		}
		m_faces.push_back( face );
	}

	header.RasterStart = IRIS_SwapU32( header.RasterStart );
	if ( header.RasterStart )
	{
		stream->seekg( header.RasterStart + stream_start, ios::beg );
		m_raster.LoadFromStream( stream );
	}

	CreateVertieces();
}


cStaticModel::~cStaticModel()
{
	SAFE_DELETE( light_source_info );
	SAFE_DELETE( particle_effect_info );

	unsigned int index;
	for ( index = 0; index < m_nodes.size(); index++ )
	{
		SAFE_DELETE( m_nodes[index] );
	}
	
	for ( index = 0; index < m_face_light_nodes.size(); index++ )
	{
		SAFE_DELETE( m_face_light_nodes[index] );
	}

	for ( index = 0; index < m_point_light_nodes.size(); index++ )
	{
		SAFE_DELETE( m_point_light_nodes[index] );
	}
	for ( index = 0; index < m_faces.size(); index++ )
	{
		SAFE_DELETE( m_faces[index] );
	}
	
	m_nodes.clear();
	m_face_light_nodes.clear();
	m_point_light_nodes.clear();
	m_faces.clear();

	free( m_vertieces );
	m_vertieces = NULL;
}

void cStaticModel::CreateVertieces ()
{
  m_vertieces =
    (vertex *) malloc (sizeof (struct vertex) * m_face_light_nodes.size ());
  memset (m_vertieces, 0,
          sizeof (struct vertex) * m_face_light_nodes.size ());

  for (unsigned int index = 0; index < m_face_light_nodes.size (); index++)
      {
        assert (m_face_light_nodes[index]->m_node_index < m_nodes.size ());
        //printf("%i\n", m_face_light_nodes[index]->m_node_index);
        m_nodes[m_face_light_nodes[index]->m_node_index]->
          CreateVertex (&m_vertieces[index]);
      }
}


void cStaticModel::Render (float x, float y, float z, Uint8 alpha)
{
  int i;
  assert (texture_loader);

  for (unsigned int index = 0; index < m_faces.size (); index++)
      {
        cStaticModelFace *face = m_faces[index];
        if (!face->m_gl_texture)
            {                   // Load texture if not cached
              if (face->m_texture >= 0)
                face->m_gl_texture =
                  texture_loader->GetTexture (face->m_texture)->
                  GetGLTex ();
              else
                face->m_gl_texture =
                  TextureBuffer::GetInstance()->GetArtTexture (-face->m_texture +
                                                 16384)->GetGLTex (); // Not good, can cause texture errors
            }

        vertex *verts[3];
        for (i = 0; i < 3; i++)
            {
              verts[i] = &m_vertieces[face->m_node_indices[i]];
              verts[i]->u = face->m_texcoords[i][0];
              verts[i]->v = face->m_texcoords[i][1];
            }
        add_triangle_to_vertex_buffer (verts[0], verts[1], verts[2],
                                       face->m_gl_texture, x, y, z, alpha);
        if (face->m_flags & FACEFLAG_BACKFACE)
          add_triangle_to_vertex_buffer (verts[2], verts[1], verts[0],
                                         face->m_gl_texture, x, y, z, alpha);
      }
}

bool cStaticModel::CheckRay (float vecOrigin[3], float vecDir[3],
                             float deltax, float deltay, float deltaz,
                             float &lambda)
{

  float vecs[3][3];
  float act_lambda;
  bool result = false;
  lambda = 100000000.0f;
  for (unsigned int index = 0; index < m_faces.size (); index++)
      {
        cStaticModelFace *face = m_faces[index];
        for (int j = 0; j < 3; j++)
            {
              vertex *v = &m_vertieces[face->m_node_indices[j]];
              vecs[j][0] = v->x + deltax;
              vecs[j][1] = v->y + deltay;
              vecs[j][2] = v->z + deltaz * 0.1f;
            }

        if (IntersectTriangleWithRay
            (vecOrigin, vecDir, vecs[0], vecs[1], vecs[2], act_lambda))
            {
              result = true;
              if (act_lambda < lambda)
                lambda = act_lambda;
            }
      }

  return result;

}


cStaticModelLightSourceInfo *cStaticModel::GetLightSourceInfo ()
{
  return light_source_info;
}

cStaticModelParticleEffectInfo *cStaticModel::GetParticleEffectInfo ()
{
  return particle_effect_info;
}


cStaticModelLightSourceInfo::cStaticModelLightSourceInfo (bool flickering,
                                                          float x, float y,
                                                          float z,
                                                          float radius,
                                                          sColor color,
                                                          int
                                                          flicker_min_delay,
                                                          int
                                                          flicker_max_delay,
                                                          int flicker_amount)
{
  m_flickering = flickering;
  m_x = x;
  m_y = y;
  m_z = z;

  if (radius < 0.1f)
    THROWEXCEPTION ("invalid light radius");
  m_radius = radius;
  m_color = color;

  if (flicker_min_delay < 10)
    THROWEXCEPTION ("invalid flicker min delay");
  if (flicker_max_delay < flicker_min_delay + 10)
    THROWEXCEPTION ("invalid flicker max delay");
  if ((flicker_amount < 1) || (flicker_amount > 100))
    THROWEXCEPTION ("invalid flicker amount");

  m_flicker_min_delay = flicker_min_delay;
  m_flicker_max_delay = flicker_max_delay;
  m_flicker_amount = flicker_amount;
}

bool cStaticModelLightSourceInfo::flickering ()
{
  return m_flickering;
}

float cStaticModelLightSourceInfo::x ()
{
  return m_x;
}

float cStaticModelLightSourceInfo::y ()
{
  return m_y;
}

float cStaticModelLightSourceInfo::z ()
{
  return m_z;
}

float cStaticModelLightSourceInfo::radius ()
{
  return m_radius;
}

sColor cStaticModelLightSourceInfo::color ()
{
  return m_color;
}

int cStaticModelLightSourceInfo::flicker_min_delay ()
{
  return m_flicker_min_delay;
}

int cStaticModelLightSourceInfo::flicker_max_delay ()
{
  return m_flicker_max_delay;
}

int cStaticModelLightSourceInfo::flicker_amount ()
{
  return m_flicker_amount;
}


cStaticModelParticleEffectInfo::cStaticModelParticleEffectInfo (std::
                                                                string name,
                                                                float x,
                                                                float y,
                                                                float z,
                                                                bool rotate,
                                                                float angle1,
                                                                float angle2)
{
  m_name = name;
  m_x = x;
  m_y = y;
  m_z = z;
  m_angle1 = angle1;
  m_angle2 = angle2;
  m_rotate = rotate;
}


std::string cStaticModelParticleEffectInfo::name ()
{
  return m_name;
}


float cStaticModelParticleEffectInfo::x ()
{
  return m_x;
}


float cStaticModelParticleEffectInfo::y ()
{
  return m_y;
}


float cStaticModelParticleEffectInfo::z ()
{
  return m_z;
}


float cStaticModelParticleEffectInfo::angle1 ()
{
  return m_angle1;
}


float cStaticModelParticleEffectInfo::angle2 ()
{
  return m_angle2;
}


bool cStaticModelParticleEffectInfo::rotate ()
{
  return m_rotate;
}
