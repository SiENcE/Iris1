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


#include "granny/grntype.h"
#include "granny/grannyfile.h"
#include "granny/GrannyTextures.h"
#include "Config.h"
#include "Logger.h"
#include <iostream>
#include <cassert>
#include <string>
#include "pathsearch.h" // by ghoulsblade




using namespace std;

// Utility Function
void calculateBoneRests (Bone * bone);


cGrannyFile::cGrannyFile ()
{
	m_texture = 0;
	m_initialized = false;
	m_texfilename = "";
	left_hand_bone = -1;
	right_hand_bone = -1;
	color_array = NULL;
	master_bone = -1;
}

cGrannyFile::~cGrannyFile ()
{
	// Destroy the Texture we're binding
	//delete texture;
	if (pGrannyTextureLoader)
		pGrannyTextureLoader->FreeTexture (m_texfilename);
	m_texture = NULL;
	if (color_array)
		free (color_array);
}

char *strlower(char *s) 
{ 
int offset = ((int)'A')-((int)'a'); 
for(int i=0; s[i]; i++) { 
if( (s[i]>='A') && (s[i]<='Z') ) 
s[i] -= offset; 
} 
return s; 
}

bool isMasterName(const char *str)
{
//	char *s = _strlwr(_strdup(str));
    char *s = strlower(strdup(str)); 
	bool ismaster = false;

	if (strstr(s, "master") || strstr(s, "mesh"))	
		ismaster = true;

	// free(s);

	return ismaster;
}

// assing bone name, find master bone 
//  - bone.id -> bone.ojbptr -> bone.obj -> textid -> textchunk -> bone.name
void cGrannyFile::initBone()
{
	if (!m_initialized) return;

	std::vector<Bone *> &bones = getBones().bones;
	std::vector<dword> &boneObjects = getTies().boneObjects;
	std::vector<dword> &boneObjPtrs = getTies().boneObjPtrs;

	dword lodNm = findString("__ObjectName");

	int temp_lhand_bone = -1, temp_rhand_bone = -1;
	master_bone = left_hand_bone = right_hand_bone = -1;

	for (unsigned int i = 0; i < bones.size(); i++) {
		Bone* bone = bones[i];
		// bonetie.boneObjPtr: connect bone.id to boneObj
		dword objptr = boneObjPtrs[bone->id];
		// bonetie.boneObjects: connect bone.id to textchunk(bone name)
		dword obj = boneObjects[objptr-1];
		// objects.getValue(obj-1,key); objects[obj]->getValue(key);
		dword textid = getValue(obj,lodNm);
		// assign bone name
		bone->name = findID(textid);

		const char *s = bone->name.c_str();
		if (isMasterName(s) && master_bone == -1)
			master_bone = bone->id;
		else if (_stricmp(s, "CP_Grasp_Rhand") == 0)
			right_hand_bone  = bone->id;
		else if (_stricmp(s, "CP_Grasp_Lhand") == 0)
			left_hand_bone  = bone->id;
		else if (_stricmp(s, "Bip01 L Hand") == 0)
			temp_lhand_bone = bone->id;
		else if (_stricmp(s, "Bip01 R Hand") == 0)
			temp_rhand_bone = bone->id;
	}

#if 1
	// td anim, some aos anim(id 73) has not cp_grasp_ bone, we use hane bone.
	if ( !Config::GetAOS() || left_hand_bone  == -1 ) 
		left_hand_bone  = temp_lhand_bone;
	if (!Config::GetAOS() || right_hand_bone == -1) 
		right_hand_bone = temp_rhand_bone;
#endif
}

int cGrannyFile::getBoneID(const char *name)
{
	if (!m_initialized) return -1;

	std::vector<Bone *> &bones = getBones().bones;

	if (isMasterName(name)) 
		return master_bone;

	for (unsigned int i = 0; i < bones.size(); i++) {
		Bone* bone = bones[i];
		if (_stricmp(bone->name.c_str(), name) == 0)
			return bone->id;
	}

	return -1;
}

void cGrannyFile::load (std::string filename, std::string basepath)
{
	if (color_array)
		free (color_array);
	color_array = NULL;

	std::fstream * file =
		new fstream (filename.c_str (), ios::in | ios::binary);

	// ghoulsblade's patch : try searching for the path with ignore case
	if (!file->is_open ()) {
		std::string realpath = rob_pathsearch(filename);
		if (realpath.size() > 0) {
			SAFE_DELETE( file ); 
			file = new fstream (realpath.c_str (), ios::in | ios::binary);
		}
	}
		
	if (!file->is_open ())
	{
		Logger::WriteLine ("Error: File not found: " + filename);
		SAFE_DELETE( file ); // ghoulsblade : fixed MEMORY LEAK, file was not deleted
        file=0;
		return;
	}


	m_stream = new cGrannyStream (file, filename);
	file->close ();
	delete file;

	glPushMatrix ();
	glLoadIdentity ();

	m_stream->seekg (0x40);         // skip header (Could be FileType magic)

	dword chunk = m_stream->readDword ();
	switch (chunk)
	{
	case 0xCA5E0000:
		mainChunk ();
		break;
	default:
		hex (cerr);
		cerr << "Unknown main chunk: " << chunk << endl;
		exit (1);
	}

	if (getTextureName () != "")
		loadTexture (basepath.c_str ());

	std::vector<Bone *> &bones = getBones().bones;
	if( bones.size() > 0 )
	{
		calculateBoneRests( bones[0] );
	}


	delete m_stream;
	m_stream = NULL;
	m_initialized = true;

	Meshes & meshes = getMeshes ();
	list < Mesh >::iterator imesh;
	imesh = meshes.meshes.begin ();

	if (imesh != meshes.meshes.end ())
	{
		color_array =
			(float *) malloc (imesh->points.size () * 3 * sizeof (float));
	}
	
	initBone();

	glPopMatrix ();
}

void cGrannyFile::addTime (float t)
{
}

string cGrannyFile::getTextureName ()
{
	return object.getTextureName ();
}

Meshes & cGrannyFile::getMeshes ()
{
	return object.getMeshes ();
}

Bones & cGrannyFile::getBones ()
{
	return object.getBones ();
}

BoneTies & cGrannyFile::getTies ()
{
	return object.getTies ();
}

Textures & cGrannyFile::getTextures ()
{
	return object.getTextures ();
}

BoneAnim & cGrannyFile::getBoneAnim (dword id)
{
	return object.getBoneAnim (id);
}

Animations & cGrannyFile::getAnimations ()
{
	return object.getAnimations ();
}

dword cGrannyFile::findString (string str)
{
	return object.findString (str);
}

dword cGrannyFile::getValue (dword obj, dword key)
{
	return object.getValue (obj, key);
}

dword cGrannyFile::findValue (dword key, dword value)
{
	return object.findValue (key, value);
}

std::string cGrannyFile::findID (dword id)
{
	return object.findID (id);
}

void cGrannyFile::mainChunk ()
{
	dword children = m_stream->readDword ();

	for (int i = 0; i < 6; ++i)
		m_stream->readDword ();       // CRC?

	for (dword child = 0; child < children; ++child)
	{
		// Chunk Header
		dword chunk = m_stream->readDword ();

		switch (chunk)
		{
			// Final Chunk (End-of-File?)
		case 0xCA5E0101:
			final.load (m_stream);
			break;

			// Copyright Chunk
		case 0xCA5E0102:
			copyright.load (m_stream);
			break;

			// Object Chunk
		case 0xCA5E0103:
			object.load (m_stream);
			break;

			//SiENcE:  Chunk is new to ML/SE Models maybe Texture: case 0xca5e0100

		default:
			hex (cerr);
			cerr << "Unknown GRN Chunk: " << chunk << endl;
			exit (1);
		}
	}
}

bool cGrannyFile::loadTexture (const char *basepath)
{
	std::string filename;

	if (m_texture && pGrannyTextureLoader)
		pGrannyTextureLoader->FreeTexture (m_texfilename);

	m_texture = NULL;
	m_texfilename = getTextureName ();

	if (pGrannyTextureLoader)
		m_texture = pGrannyTextureLoader->LoadTexture(m_texfilename);

	return (m_texture);
}

void calculateBoneRests (Bone * bone)
{
	GrnMatrix matrix;
	matrix.setTransform(bone->quaternion, bone->translate);
	glMultMatrixf(matrix.matrix);

	glGetFloatv (GL_MODELVIEW_MATRIX, matrix.matrix);

	bone->matrix = matrix;
	bone->matrix.invert ();
	bone->curMatrix = matrix;
	bone->curMatrix *= bone->matrix;


	vector < Bone * >::iterator ibone;
	for (ibone = bone->children.begin ();
		ibone != bone->children.end (); ibone++)
	{
		glPushMatrix ();
		calculateBoneRests (*ibone);
		glPopMatrix ();
	}
}


int cGrannyFile::getFrame (cGrannyFile * animation, float &curTime)
{
	if (animation)
	{
		if (curTime >= animation->length ())
			curTime = 0.0f;
		if (animation->length () > 0.0f)
			return (int) (curTime / animation->length () * 25.0f);
	}
	return 0;
}

cDeformedArray *cGrannyFile::createDeformed (cGrannyFile * animation,
											 float time,
											 list < Mesh >::iterator imesh)
{
	cDeformedArray *deformed = NULL;

	Bones & bones = getBones ();
	BoneTies & boneTies = getTies ();

	glPushMatrix ();
	glLoadIdentity ();
	if (animation)
		animation->getSkeleton (bones.bones[0], time);
	glPopMatrix ();

	vector < BoneWeight >::iterator iwt;
	dword pnt = 0;
	if (imesh->weights.size () > 0)
	{
		deformed = new cDeformedArray (imesh->weights.size ());
		float *data = deformed->data ();

		for (iwt = imesh->weights.begin (); iwt != imesh->weights.end ();
			++iwt, ++pnt)
		{
			data[0] = 0;  
			data[1] = 0;  
			data[2] = 0;
			for (unsigned int wt = 0; wt < iwt->numWeights; ++wt)
			{
				Point post;
				float weight = iwt->weights[wt];
				dword bone = boneTies.boneties[iwt->bones[wt]]->bone;
				post = bones.bones[bone]->curMatrix * imesh->points[pnt];
				data[0]+=(post.points[0]*weight);
				data[1]+=(post.points[1]*weight);
				data[2]+=(post.points[2]*weight); 
			}
			data += 3;
		}
	}
	else if (imesh->points.size () > 0)
	{
		deformed = new cDeformedArray (imesh->points.size ());
		float *data = deformed->data ();

		for (unsigned int i = 0; i < imesh->points.size (); i++)
		{
			Point post;
			float weight = 1.0f;
			post = bones.bones[1]->curMatrix * imesh->points[i];
			*data = post.points[0]*weight; data++;
			*data = post.points[1]*weight; data++;
			*data = post.points[2]*weight; data++;
		}
	}

	if (animation)
	{
		deformed->matrix_left_hand = animation->matrix_left_hand;
		deformed->matrix_right_hand = animation->matrix_right_hand;
	}

	return deformed;
}

void cGrannyFile::Render (cGrannyFile * animation, float &curTime,
						  cCharacterLight * character_light, float r, float g,
						  float b, float alpha, bool is_corpse)
{
	if (!initialized ())
		return;

	cDeformedArray *deformed = NULL;
	int frame;
	if (is_corpse)
		frame = 24;
	else
		frame = getFrame (animation, curTime);
	//int frame = getFrame(animation, curTime);

	float time = 0.0f;

	if (animation)
	{
		time = ((float) frame) / 25.0f * animation->length ();
		deformed = animation->getDeformed (frame);
	}


	Meshes & meshes = getMeshes ();
	//Bones &bones = getBones();
	Textures & textures = getTextures ();
	//BoneTies &boneTies = getTies();
	list < Mesh >::iterator imesh;
	if (meshes.meshes.size () != 1)
	{
		//             printf("Warning: More than one Granny Mesh is not supported!\n");
		//return;
	}

	imesh = meshes.meshes.begin ();


	if (!deformed)
	{
		deformed = createDeformed (animation, time, imesh);
		if (animation)
			animation->addDeformed (deformed, frame);
	}
	else
	{
		if (animation)
		{
			animation->matrix_left_hand = deformed->matrix_left_hand;
			animation->matrix_right_hand = deformed->matrix_right_hand;
		}

	}



	// we would need to recalculate normals here.. screw that dude.
	// I doubt OSI does (although since I have yet to actually run UO:TD,
	// I don't know for sure).

	if (deformed)
	{

		float *data = deformed->data ();

		if (imesh->normals.size () && color_array
			&& (imesh->points.size () <= imesh->normals.size ()))
		{
			for (unsigned int i = 0; i < imesh->points.size (); i++)
			{
				//                    float *vertex = data + (i * 3);

				/*  depreciated
				int v_x = (int) (vertex[0] * 2.0f + 1.0f);
				int v_y = (int) (vertex[1] * 2.0f + 1.0f);
				int v_z = (int) (vertex[2] * 1.0f); */


				Point & p = imesh->normals[i];
				character_light->CalcColor (0, 0, 0, p.points,
					color_array + i * 3);
			}
		}
		if (alpha < 1.0f)
			glDisable (GL_ALPHA_TEST);
		dword poly = 0;
		vector < gPolygon >::iterator polygon;
		//
		glBindTexture (GL_TEXTURE_2D, getTexture ());
		glBegin (GL_TRIANGLES);

		assert (character_light);

		for (polygon = imesh->polygons.begin ();
			polygon != imesh->polygons.end (); polygon++, poly++)
		{
			for (int i = 0; i < 3; ++i)
			{

				// Do we have texture-map information?
				if (imesh->textureMap.size ())
				{
					Point & p =
						imesh->textureMap[textures.polygons[poly].
						nodes[i + 1]];
					glTexCoord2fv (p.points);
				}

				float *vertex = data + (polygon->nodes[i] * 3);

				if (color_array)
				{
					float *color = color_array + polygon->nodes[i] * 3;
					//glColor3f(color[0] * r, color[1] * g, color[2] * b);

					if (alpha < 1.0f)
						glColor4f (color[0] * r, color[1] * g,
						color[2] * b, alpha);
					else
						glColor3f (color[0] * r, color[1] * g,
						color[2] * b);


					//                                    printf("%.2f %.2f %.2f %.2f\n", p.points[0], p.points[1], p.points[2], p.points[0]*p.points[0]+ p.points[1]* p.points[1]+ p.points[2]* p.points[2]);
					//glNormal3fv( p.points);
				}
				//printf("%i %i %i\n", v_x, v_y, v_z);
				//
				//printf("%.2f %.2f %.2f\n", *vertex, *(vertex + 1), *(vertex + 2));

				glVertex3fv (vertex);

			}
		}
		if (alpha < 1.0f)
			glEnable (GL_ALPHA_TEST);
		glEnd ();

		//glBindTexture( GL_TEXTURE_2D, 0 );

		if (!animation)
			delete deformed;
	}
}

void cGrannyFile::getSkeleton (Bone * bone, float &curTime)
{
}

cGrannyAnimation::cGrannyAnimation ()
{
	m_length = 0.0f;
}

cGrannyAnimation::~cGrannyAnimation ()
{
	std::map <int, cDeformedArray *>::iterator iter;
	for (iter = m_cache.begin(); iter != m_cache.end(); iter++)
		delete iter->second;
	m_cache.clear();

}


void cGrannyAnimation::Assign (cGrannyFile * model)
{
	if (!initialized() || !model || !model->initialized())
		return;

	left_hand_bone = model->left_hand_bone;
	right_hand_bone = model->right_hand_bone;

	BoneTies &boneTies = getTies();
	BoneTies &boneLodTies = model->getTies();
	Animations &anims = getAnimations();
	std::vector<BoneAnim *> &animBones = anims.bones;

	dword anmNm = findString("__ObjectName");
	dword lodNm = model->findString("__ObjectName");

	m_animBones.assign(model->getBones().bones.size(), (dword)-1);

	// for each animation bone
	for (unsigned int i = 0; i < animBones.size(); i++)
	{
		dword boneId = 0;
		dword animId = animBones[i]->id; // objptr
		dword id = boneTies.boneObjects[ animId - 1 ]; // object
		dword textId = getValue( id, anmNm );
		std::string boneStr = findID( textId );

		boneId = model->getBoneID(boneStr.c_str());

		// model 의 bone.id 와 연결된 BoneAnim 색인을 저장
		if (boneId != (dword)-1)
			m_animBones[ boneId ] = i;
	}

	m_length = object.getAnimLength();
}



void cGrannyAnimation::getSkeleton (Bone * bone, float &curTime)
{
	float X, Y, Z, x, y, z, w;
	dword rid=0,mid=0;
	GrnMatrix matrix;

	if (m_animBones.empty() || !bone)
		return;

#if 0
	if ((left_hand_bone != -1) && ((int) bone->id == left_hand_bone))
	{
		glPushMatrix ();
		glRotatef (-135.0f, 1.0f, 0.0f, 0.0f);
		glRotatef (-45.0f, 0.0f, 1.0f, 0.0f);
		//glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix_left_hand.matrix);
		glPopMatrix ();
	}

	if ((right_hand_bone != -1) && ((int) bone->id == right_hand_bone))
	{
		glPushMatrix ();
		glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef (135.0f, 0.0f, 1.0f, 0.0f);
		//glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix_right_hand.matrix);
		glPopMatrix ();
	}
#endif

	rid = mid = 0;
	if ((m_animBones[bone->id] == (dword)-1))
	{
		return;
	}
	else
	{
		BoneAnim & boneAnim = getBoneAnim (m_animBones[bone->id]);
		vector < float >::iterator movi, roti;

		// Problem here, i suppose that some animations are looped back/forward
		if (curTime > boneAnim.length)
		{
			curTime = 0.0f;
		}

		for (roti = boneAnim.quaternionTimeline.begin ();
			roti != boneAnim.quaternionTimeline.end () && (*roti) < curTime;
			roti++, rid++);

		for (movi = boneAnim.translateTimeline.begin ();
			movi != boneAnim.translateTimeline.end () && (*movi) < curTime;
			movi++, mid++);

		Point t=boneAnim.translates[ mid ],q=boneAnim.quaternions[ rid ];

		X = t.points[ 0 ];
		Y = t.points[ 1 ];
		Z = t.points[ 2 ];

		// Interpolate when between Keyframes
		if (curTime != (*movi))
		{
			float F1 = *movi;
			movi++;
			if (movi != boneAnim.translateTimeline.end ())
			{
				float F2 = *movi;
				float x2 = boneAnim.translates[mid + 1].points[0];
				float y2 = boneAnim.translates[mid + 1].points[1];
				float z2 = boneAnim.translates[mid + 1].points[2];
				float tm = ( curTime - F1 ) / ( F2 - F1 );
				t.points[0] += tm * ( x2 - X );
				t.points[1] += tm * ( y2 - Y );
				t.points[2] += tm * ( z2 - Z );
			}
		}

		matrix.setTransform(q, t);

		glMultMatrixf (matrix.matrix);
		glGetFloatv (GL_MODELVIEW_MATRIX, matrix.matrix);
		bone->curMatrix = matrix;
		bone->curMatrix *= bone->matrix;

		if ((left_hand_bone  != -1) && ((int) bone->id == left_hand_bone))
			matrix_left_hand = matrix;

		if ((right_hand_bone != -1) && ((int) bone->id == right_hand_bone))
			matrix_right_hand = matrix;
	}



	vector < Bone * >::iterator ibone;
	for( ibone = bone->children.begin(); ibone != bone->children.end(); ++ibone )
	{
		glPushMatrix ();
		getSkeleton (*ibone, curTime);
		glPopMatrix ();
	}
}

cDeformedArray *cGrannyAnimation::getDeformed (int index)
{
	std::map < int, cDeformedArray * >::iterator iter = m_cache.find (index);
	if (iter != m_cache.end ())
		return iter->second;
	return NULL;
}

void cGrannyAnimation::addDeformed (cDeformedArray * deformed, int index)
{
	std::map < int, cDeformedArray * >::iterator iter = m_cache.find (index);
	if (iter != m_cache.end ())
	{
		delete iter->second;
		m_cache.erase (index);
	}

	m_cache.insert (make_pair (index, deformed));

}

cDeformedArray::cDeformedArray (int size)
{
	assert (size > 0);
	m_data = new float[size * 3];
}


cDeformedArray::~cDeformedArray ()
{
	delete [] m_data;
}


float *cDeformedArray::data ()
{
	return m_data;
}
