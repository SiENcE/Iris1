//
// File: GrannyLoader.cpp
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


#include "granny/GrannyLoader.h"
#include "granny/GrannyModelTD.h"
#include "granny/GrannyModelAOS.h"
#include "granny/grannyfile.h"
#include "granny/GrannyTextures.h"

#include "Config.h"
#include "Logger.h"
#include "xml.h"

#include <cassert>




using namespace std;

cGrannyLoader *pGrannyLoader = NULL;

std::string to_lower (std::string filename)
{
	for (unsigned int i = 0; i < filename.size (); i++)
	{                         // lowercase
		char ch = filename[i];
		if ((ch >= 'A') && (ch <= 'Z'))
			filename[i] = ch + 32;
	}
	return filename;
}

struct TypeData
{
	TypeData (const string & NewName, int NewBlock):name (NewName),
		blockid (NewBlock)
	{
	};
	string name;
	int blockid;
};

TypeData TypeInfo[] = {
	TypeData ("walkunarmed", 0),
	TypeData ("walkarmed", 1),
	TypeData ("rununarmed", 2),
	TypeData ("runarmed", 3),
	TypeData ("stand", 4),
	TypeData ("standforonehandedattack", 5),
	TypeData ("walkinattackpositiononehanded", 6),
	TypeData ("standfortwohandedattack", 7),
	TypeData ("walkinattackpositiontwohanded", 8),
	TypeData ("attackonehanded", 9),
	TypeData ("takehit", 10),
	TypeData ("fidget.3", 11),
	TypeData ("attack.unarmed.1", 12),
	TypeData ("attack.unarmed.2", 13),
	TypeData ("attack.twohanded.down", 14),
	TypeData ("attack.twohanded.wide", 15),
	TypeData ("attack.twohanded.jab", 16),
	TypeData ("walkinattackposition", 17),
	TypeData ("cast.directed", 18),
	TypeData ("cast.area/summon", 19),
	TypeData ("attack.bowandarrow", 20),
	TypeData ("attack.crossbow", 21),
	TypeData ("die.1", 22),
	TypeData ("die.2", 23),
	TypeData ("horseback.stand", 24),
	TypeData ("horseback.rideslow", 25),
	TypeData ("horseback.ridefast", 26),
	TypeData ("horseback.attack", 27),
	TypeData ("horseback.attack.bowandarrow", 28),
	TypeData ("horseback.attack.crossbow", 29),
	TypeData ("horseback.attack.2", 30),
	TypeData ("bow", 31),
	TypeData ("salute", 32),
	TypeData ("eat", 33),
	TypeData ("llama.stand", 34),
	TypeData ("llama.rideslow", 35),
	TypeData ("llama.ridefast", 36),
	TypeData ("llama.attack", 37),
	TypeData ("llama.attack.bowandarrow", 38),
	TypeData ("llama.attack.crossbow", 39),
	TypeData ("llama.attack.2", 40),
	TypeData ("ostard.stand", 41),
	TypeData ("ostard.rideslow", 42),
	TypeData ("ostard.ridefast", 43),
	TypeData ("ostard.attack", 44),
	TypeData ("ostard.attack.bowandarrow", 45),
	TypeData ("ostard.attack.crossbow", 46),
	TypeData ("ostard.attack.2", 47),
	TypeData ("kirin.stand", 48),
	TypeData ("kirin.rideslow", 49),
	TypeData ("kirin.ridefast", 50),
	TypeData ("kirin.attack", 51),
	TypeData ("kirin.attack.bowandarrow", 52),
	TypeData ("kirin.attack.crossbow", 53),
	TypeData ("kirin.attack.2", 54),
	TypeData ("giant_beetle.stand", 55),
	TypeData ("giant_beetle.rideslow", 56),
	TypeData ("giant_beetle.ridefast", 57),
	TypeData ("giant_beetle.attack", 58),
	TypeData ("giant_beetle.attack.bowandarrow", 59),
	TypeData ("giant_beetle.attack.crossbow", 60),
	TypeData ("giant_beetle.attack.2", 61),
	TypeData ("ridgeback.stand", 62),
	TypeData ("ridgeback.rideslow", 63),
	TypeData ("ridgeback.ridefast", 64),
	TypeData ("ridgeback.attack", 65),
	TypeData ("ridgeback.attack.bowandarrow", 66),
	TypeData ("ridgeback.attack.crossbow", 67),
	TypeData ("ridgeback.attack.2", 68),
	TypeData ("seahorse.stand", 69),
	TypeData ("seahorse.rideslow", 70),
	TypeData ("seahorse.ridefast", 71),
	TypeData ("seahorse.attack", 72),
	TypeData ("seahorse.attack.bowandarrow", 73),
	TypeData ("seahorse.attack.crossbow", 74),
	TypeData ("seahorse.attack.2", 75),
	TypeData ("sit", 76),
	TypeData ("applaud", 78),
	TypeData ("argue.1", 79),
	TypeData ("argue.2", 80),
	TypeData ("argue.3", 81),
	TypeData ("kiss", 82),
	TypeData ("bow.2", 83),
	TypeData ("bow.3", 84),
	TypeData ("coverears", 85),
	TypeData ("courtesy.1", 86),
	TypeData ("dance.folk.1", 87),
	TypeData ("dance.folk.2", 88),
	TypeData ("dance.3", 89),
	TypeData ("dance.4", 90),
	TypeData ("foldarms", 91),
	TypeData ("impatient", 92),
	TypeData ("lecture", 93),
	TypeData ("nod_yes", 94),
	TypeData ("point", 95),
	TypeData ("salute.1", 96),
	TypeData ("salute.2", 97),
	TypeData ("salute.3", 98),
	TypeData ("shake_no", 99),
	TypeData ("victory.1", 101),
	TypeData ("victory.2", 102),
	TypeData ("wave.1", 103),
	TypeData ("wave.2", 104),
	TypeData ("wave.long", 105),
	TypeData ("what", 106),
	TypeData ("fidget.sexy", 107),
	TypeData ("stand.sexy", 108),
	TypeData ("fidget.sinister", 109),
	TypeData ("stand.sinister", 110),
	TypeData ("fidget.drunk", 111),
	TypeData ("stand.drunk", 112),
	TypeData ("fidget.athletic", 113),
	TypeData ("stand.athletic", 114),
	TypeData ("fidget.aged", 115),
	TypeData ("stand.aged", 116),
	TypeData ("fidget.jammin", 117),
	TypeData ("stand.jammin", 118),
	TypeData ("", -1),
};

struct AOSBodyData
{
	AOSBodyData (const string & NewName, int NewID):name (NewName), id (NewID)
	{
	};
	string name;
	int id;
};

AOSBodyData AOSBodyInfo[] = {
	AOSBodyData ("EARS", 1),
	AOSBodyData ("FACE", 2),
	AOSBodyData ("FEET", 3),
	AOSBodyData ("HANDS", 4),
	AOSBodyData ("HEAD", 5),
	AOSBodyData ("LLEGS", 6),
	AOSBodyData ("NECK", 7),
	AOSBodyData ("TORSOUP", 8),
	AOSBodyData ("TORSO", 9),
	AOSBodyData ("UARMS", 10),
	AOSBodyData ("ULEGS", 11),
	AOSBodyData ("FARMS", 12),
	AOSBodyData ("PELVIS", 13),
	AOSBodyData ("", 0),
};

class cAnimSet
{
public:
	std::map<int, std::string> anim_names;
	int id;
	std::string defaultanim;
};

cGrannyLoader::cGrannyLoader( string filename, string mulpath )
{
	this->mulpath = mulpath;
	tick = 0;

	XML::Parser parser;
	XML::Node *granny, *document;

	try
	{
		parser.loadData( filename );
		document = parser.parseDocument();

		granny = document->findNode( "GRANNY" );

		if ( !granny )
		{
			throw "Couldn't find granny node.";
		}
	}
	catch (...)
	{
		Logger::WriteLine( "Couldn't load Granny Definitions" );
		return;
	}

	XML::Node *char_node, *set_node, *value;

	if ( (value = granny->findNode( "TEXTUREPATH" ) ) )
	{
		tex_basepath = mulpath + value->asString();
	}

	assert( !pGrannyTextureLoader );
	pGrannyTextureLoader = new cGrannyTextureLoader( tex_basepath );

	std::map<int, cAnimSet *> animsets;
	std::map<int, cAnimSet *>::iterator iter;
	std::map<int, string>::iterator name_iter;


	int idx = 0;
	while ( (set_node = granny->findNode("ANIMSET", idx)) )
	{
		cAnimSet *animset = new cAnimSet();
		value = set_node->findNode( "ID" );
		animset->id = (value != NULL) ? value->asInteger() : 0;
		if ( ( value = set_node->findNode( "DEFAULTANIM" )) )
		{
			animset->defaultanim = mulpath + to_lower( value->asString() );
		}

		int idx2 = 0;
		XML::Node *anim_node;
		while ( (anim_node = set_node->findNode( "ANIMATION", idx2 )) )
		{
			std::string animtype = "", filename = "";
			if ( (value = anim_node->findNode( "TYPE" )) )
			{
				animtype = value->asString();
			}
			if ( (value = anim_node->findNode ("FILE")) )
			{
				filename = mulpath + to_lower( value->asString() );
			}

			int anim_typeid = -1;
			for ( int i = 0; TypeInfo[i].blockid != -1; i++ )
			{
				const TypeData &Data = TypeInfo[i];
				if ( Data.name == animtype )
				{
					anim_typeid = Data.blockid;
				}
			}

			if ( anim_typeid != -1 )
			{
				animset->anim_names.insert( std::make_pair( anim_typeid, filename ) );
			}

			idx2++;
		}

		//animsets.erase(animset->id);
		animsets.insert( std::make_pair( animset->id, animset ) );

		idx++;
	}

	idx = 0;
	while ((char_node = granny->findNode ("CHARACTER", idx)))
	{
		std::string filename = "", default_anim = "", prefix = "";
		int left_hand_bone = -1;
		int right_hand_bone = -1;
		int animset = -1;
		int hand = HAND_NONE;

		value = char_node->findNode ("ID");
		Uint32 id = (value != NULL) ? value->asInteger () : 0;
		if ((value = char_node->findNode ("FILE")))
			filename = mulpath + to_lower (value->asString ());
		if ((value = char_node->findNode ("ANIMSET")))
			animset = value->asInteger ();
		if ((value = char_node->findNode ("PREFIX")))
			prefix = to_lower (value->asString ());

		if ((value = char_node->findNode ("LEFT_HAND_BONE")))
		{
			left_hand_bone = value->asInteger ();
			hand = HAND_OWNER;
		}
		if ((value = char_node->findNode ("RIGHT_HAND_BONE")))
		{
			right_hand_bone = value->asInteger ();
			hand = HAND_OWNER;
		}

		if ((value = char_node->findNode ("HAND")))
		{
			if (value->asString () == "left")
				hand = HAND_LEFT;
			if (value->asString () == "right")
				hand = HAND_RIGHT;
		}

		Uint16 assign = 0;
		if ((value = char_node->findNode ("ASSIGN")))
			assign = value->asInteger ();

		//SiENcE: if female id-1000, to show female clothes
		// (i dont know why it needs a subtract of 1000!, ids are correct)
		//if (assign == 401) id=id-1000;

		//HARKON: recevied clothes id is equals male/female(runuo 1.0). but weapon isn't equals.
		// examples, ganny_aos.xml male kilt id is 455, female kilt id is 1455. server only sends 455.
		if (assign == 401 && hand == HAND_NONE) id -= 1000;

		id |= assign << 16;      
		iter = animsets.find (animset);

		if (id && (iter != animsets.end ()))
		{
			assert (iter->second);
			cGrannyModel *model = new cGrannyModelTD( filename, tex_basepath, iter->second->defaultanim, prefix );
			model->SetHandBones( left_hand_bone, right_hand_bone );
			model->SetHand( hand );
			model->SetAnimset( animset );
			for ( name_iter=iter->second->anim_names.begin(); name_iter != iter->second->anim_names.end(); name_iter++ )
			{
				model->AddAnimation( name_iter->first, name_iter->second );
			}

			//models.erase(id);
			if ( models.find( id ) != models.end() )
			{
				//Logger::WriteLine("Warning: duplicated model id : %d", id);
				SAFE_DELETE( models.find( id )->second );
				models.erase( id );
			}
			models.insert( make_pair( id, model ) );
		}
		else
		{
			Logger::WriteLine ("Warning: Invalid Granny Definition");
		}

		idx++;
	}

	idx = 0;
	while ((char_node = granny->findNode ("CHARACTER_AOS", idx)))
	{
		std::string filename = "", default_anim = "";
		int animset = -1;

		int left_hand_bone = -1;
		int right_hand_bone = -1;
		int hand = HAND_NONE;

		value = char_node->findNode ("ID");
		Uint32 id = (value != NULL) ? value->asInteger () : 0;
		if ((value = char_node->findNode ("ANIMSET")))
			animset = value->asInteger ();

		if ((value = char_node->findNode ("LEFT_HAND_BONE")))
		{
			left_hand_bone = value->asInteger ();
			hand = HAND_OWNER;
		}
		if ((value = char_node->findNode ("RIGHT_HAND_BONE")))
		{
			right_hand_bone = value->asInteger ();
			hand = HAND_OWNER;
		}

		Uint16 assign = 0;
		if ((value = char_node->findNode ("ASSIGN")))
			assign = value->asInteger ();
		id |= assign << 16;

		iter = animsets.find (animset);

		if ( id && ( iter != animsets.end() ) )
		{
			assert (iter->second);
			cGrannyModelAOS *model = new cGrannyModelAOS (tex_basepath, iter->second->defaultanim);

			model->SetHandBones (left_hand_bone, right_hand_bone);
			model->SetHand (hand);

			model->SetAnimset (animset);
			for (int i = 0; AOSBodyInfo[i].id != 0; i++)
			{
				const AOSBodyData &Data = AOSBodyInfo[i];
				XML::Node *submodel = char_node->findNode (Data.name);
				if ( submodel )
				{
					model->AddModel( i, mulpath + to_lower( submodel->asString() ) );
				}
			}

			for ( name_iter = iter->second->anim_names.begin(); name_iter != iter->second->anim_names.end(); name_iter++ )
			{
				model->AddAnimation( name_iter->first, name_iter->second );
			}

			//models.erase(id);
			if ( models.find( id ) != models.end() )
			{
				//Logger::WriteLine("Warning: duplicated model id : %d\n", id);
				SAFE_DELETE( models.find( id )->second );
				models.erase( id );
			}
			models.insert( std::make_pair( id, model ) );

		}
		else
		{
			Logger::WriteLine ("Warning: Invalid Granny AOS Definition");
		}

		idx++;
	}

	for ( iter = animsets.begin(); iter != animsets.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	animsets.clear();

	//delete document;
}

cGrannyLoader::~cGrannyLoader()
{
	std::map<Uint32, cGrannyModel *>::iterator iter;
	for ( iter = models.begin(); iter != models.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	models.clear();

	assert( pGrannyTextureLoader );
	SAFE_DELETE( pGrannyTextureLoader );
}

void cGrannyLoader::Clear()
{
}



void cGrannyLoader::Render (Uint32 id, Uint32 type, float &curtime,
							cCharacterLight * character_light, float r,
							float g, float b, float alpha,
							GrnMatrix * left_matrix, GrnMatrix * right_matrix,
							bool is_corpse)
{
	std::map < Uint32, cGrannyModel * >::iterator iter = models.find (id);

	tick++;

	if (iter != models.end ())
	{
		assert (iter->second);

		iter->second->Render (type, tick, curtime, left_matrix, right_matrix,
			character_light, r, g, b, alpha, is_corpse);
	}

}

void cGrannyLoader::Render( Uint32 id, Uint32 type, float &curtime, cCharacterLight *character_light, float r,
							float g, float b, float alpha, std::vector<int> bodyparts, GrnMatrix *left_matrix,
							GrnMatrix *right_matrix, bool is_corpse )
{
	std::map<Uint32, cGrannyModel *>::iterator iter = models.find( id );

	tick++;

	if ( iter != models.end() )
	{
		assert( iter->second );
		if ( ( Config::GetAOS() ) && ( id == 400 || id == 401 ) )
		{
			cGrannyModelAOS *model = dynamic_cast<cGrannyModelAOS *>( iter->second );
			model->setBodyParts( bodyparts );
			model->Render( type, tick, curtime, left_matrix, right_matrix, character_light, r, g, b, alpha, is_corpse );
		}
	}

}

int cGrannyLoader::GetAnimset (Uint32 id)
{
	std::map < Uint32, cGrannyModel * >::iterator iter = models.find (id);

	//tick ++;

	if (iter != models.end ())
	{
		assert (iter->second);
		return iter->second->animset ();
	}
	return 0;
}

bool cGrannyLoader::isFound (Uint32 id)
{
	std::map < Uint32, cGrannyModel * >::iterator iter = models.find (id);
	if (iter != models.end ())
		return true;
	return false;
}
