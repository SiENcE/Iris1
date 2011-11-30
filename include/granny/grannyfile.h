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

#ifndef _GRANNYFILE_H_
#define _GRANNYFILE_H_

#define _stricmp strcasecmp
#define _strdup strdup

#include <string>
#include "grnitemlist.h"
#include "grnmatrix.h"
#include "renderer/Texture.h"
#include "renderer/3D/CharacterLight.h"
#include "include.h"
#include <map>

// #include "../Fluid/mmgr.h"

class cDeformedArray
{
public:
	cDeformedArray(int size);
	virtual ~cDeformedArray();
	float * data();

	GrnMatrix matrix_right_hand;
	GrnMatrix matrix_left_hand;
private:
	float * m_data;
};

class cGrannyFile
{
public:
	cGrannyFile();
	virtual ~cGrannyFile();
	void load( std::string filename, std::string basepath );
	void addTime( float t );
	virtual void getSkeleton( Bone *bone, float & curTime );
	std::string getTextureName();
	Meshes &getMeshes();
	Bones &getBones();
	Textures &getTextures();
	BoneAnim &getBoneAnim(dword id);
	BoneTies &getTies();
	Animations &getAnimations();
	dword findString( std::string str );
	dword getValue(dword obj,dword key);
	std::string findID(dword id);
	dword findValue(dword key,dword value);
	void Render(cGrannyFile *animation,  float & curTime, cCharacterLight * character_light, float r, float g, float b, float alpha, bool is_corpse);

	void setID(int id) { m_id = id; }
	int id() { return m_id; }

	GrnMatrix matrix_right_hand;
	GrnMatrix matrix_left_hand;

	int left_hand_bone;
	int right_hand_bone;
	int master_bone;
	
	float * color_array;

	bool loadTexture( const char *basepath );

	// Returns the GL Texture Identifier
	GLuint getTexture() const
	{
		if( m_texture )
			return m_texture->GetGLTex();
		else
			return 0;
	}

	bool initialized () {
		return m_initialized;
	}

	virtual float length () {
		return 0.0f;
	}

	virtual cDeformedArray * getDeformed (int index) { return NULL; }
	virtual void addDeformed (cDeformedArray * deformed, int index) { delete deformed; }

	int getBoneID(const char *name);
protected:
	void initBone();
	void mainChunk();
	int getFrame (cGrannyFile * animation, float & curTime);
	cDeformedArray * createDeformed (cGrannyFile * animation, float time, std::list<Mesh>::iterator imesh);
	ItemList copyright,object,final;
	cGrannyStream *m_stream;
	Texture *m_texture; // Our main Texture
	bool m_initialized;
	std::string m_texfilename;
	int m_id;

};

class cGrannyAnimation : public cGrannyFile
{
public:
	cGrannyAnimation();
	virtual ~cGrannyAnimation();
	void Assign (cGrannyFile * model);
	dword * GetAnimBones(void);
	void getSkeleton( Bone *bone, float & curTime );
	virtual float length () {
		return m_length;
	}
	virtual cDeformedArray * getDeformed (int index);
	virtual void addDeformed (cDeformedArray * deformed, int index);
protected:
	std::vector<dword> m_animBones;
	float m_length;
	std::map <int, cDeformedArray *> m_cache;
};

#endif
