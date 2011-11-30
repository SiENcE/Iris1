//
// File: DynamicObjects.cpp
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


#include "renderer/DynamicObjects.h"

#include "include.h"
#include "Config.h"
#include "loaders/TileDataBuffer.h"
#include "renderer/3D/Light3D.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/MultisLoader.h"
#include "renderer/particles/ParticleEngine.h"




cDynamicObject::cDynamicObject()
{
  id = 0;
  x = 0;
  y = 0;
  z = 0;
  model = 0;
  itemcount = 0;
  incrcounter = 0;
  direction = 0;
  dye = 0;
  flag = 0;
  clip = 0;
  type = 0;
  parent = 0;
  alpha = 255;
  fader = NULL;
  m_motive = NULL;
  m_light_source = NULL;
  m_recalc_ambient_light = true;
  m_particle_effect_handle = 0;
}


cDynamicObject::~cDynamicObject()
{
  if (m_light_source) {
    pLightManager.UnRegisterLight (m_light_source);
  }
  if (m_particle_effect_handle)
    pParticleEngine.RemoveEffect (m_particle_effect_handle);
}

void cDynamicObject::setMotive (cMotiveBasedLight * motive)
{
	m_motive = motive;
}

cMotiveBasedLight *cDynamicObject::motive()
{
	return m_motive;
}

void cDynamicObject::setLightSource (cLight3D * light_source)
{
  m_light_source = light_source;
}

cLight3D *cDynamicObject::light_source ()
{
  return m_light_source;
}

void cDynamicObject::SetRecalcAmbientLightFlag ()
{
  m_recalc_ambient_light = true;
}

bool cDynamicObject::RecalcAmbientLightFlag ()
{
  return m_recalc_ambient_light;
}

void cDynamicObject::SetParticleEffectHandle (Uint32 handle)
{
  m_particle_effect_handle = handle;
}


Uint32 cDynamicObject::GetParticleEffectHandle ()
{
  return m_particle_effect_handle;
}


cDynamicObjectList pDynamicObjectList;


cDynamicObjectList::cDynamicObjectList() : m_roof_z( ROOF_NONE ), callback_OnAdd( NULL ), callback_OnDelete( NULL )
{
}


cDynamicObjectList::~cDynamicObjectList()
{
	Clear();
}


void cDynamicObjectList::Clear ()
{
	for ( unsigned int i = 0; i < vLights.size(); i++ )
	{
		SAFE_DELETE( vLights[i] );
	}
	vLights.clear();

	dynamiclist_t::iterator iter;
	for ( iter = dynamiclist.begin(); iter != dynamiclist.end(); iter++ )
	{
		SAFE_DELETE( iter->second );
	}
	dynamiclist.clear();
}


cDynamicObject *cDynamicObjectList::Add( Uint32 id )
{
	dynamiclist_t::iterator iter;
	iter = dynamiclist.find( id );


	if ( iter != dynamiclist.end() )
	{
		if ( callback_OnDelete && ( iter->second->type == DYNAMICTYPE_WORLD ) )
		{
			callback_OnDelete (iter->second);
		}
		SAFE_DELETE( iter->second );
		dynamiclist.erase( iter );
	}

	cDynamicObject *result = new cDynamicObject();
	if ( ( m_roof_z != ROOF_NONE ) && ( result->z >= m_roof_z ) )
	{
		result->alpha = Config::GetRoofFadeAlpha();
	}

	dynamiclist.insert( std::make_pair( id, result ) );
	result->id = id;

	return result;
}


cDynamicObject *cDynamicObjectList::Get( unsigned int id )
{
	dynamiclist_t::iterator iter;
	iter = dynamiclist.find( id );

	if ( iter != dynamiclist.end() )
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}


void cDynamicObjectList::Delete( unsigned int id )
{
	cDynamicObject *object = Get( id );
	if ( object )
	{
		if ( callback_OnDelete && ( object->type == DYNAMICTYPE_WORLD ) )
		{
			callback_OnDelete( object );
		}
        SAFE_DELETE( object );
		dynamiclist.erase( id );
	}
}


int cDynamicObjectList::UpdateFader( int min_z, Uint8 alpha, cFader *fader, bool below )
{
	int count = 0;
	dynamiclist_t::iterator iter;
	bool ok;
	for ( iter = dynamiclist.begin(); iter != dynamiclist.end(); iter++ )
	{
		ok = ( ( (iter->second->z >= min_z) && (!below)) ||
			( (iter->second->z < min_z) && (below) ) );
		if ( ok && (iter->second->alpha == alpha) )
		{
			iter->second->fader = fader;
			count++;
		}
	}
	return count;
}


void cDynamicObjectList::ResetFader (cFader * fader)
{
	dynamiclist_t::iterator iter;
	for ( iter = dynamiclist.begin(); iter != dynamiclist.end(); iter++ )
	{
		if ( iter->second->fader )
		{
			iter->second->alpha = (int)(iter->second->fader->value() + 0.5f);
		}
		if ( iter->second->fader == fader )
		{
			iter->second->fader = NULL;
		}
	}
}


int cDynamicObjectList::GetRoofHeight( int x, int y, int z )
{
	int roof = ROOF_NONE;

	dynamiclist_t::iterator iter;
	cDynamicObject *object;
	for ( iter = dynamiclist.begin(); iter != dynamiclist.end(); iter++ )
	{
		object = iter->second;
		if ( !object )
		{
			continue;
		}

		if ( object->model>=MULTI_ID && pMultisLoader->MultiExists( object->model ) )
		{
			stMultiList* MultiList = pMultisLoader->GetMulti( object->model );
			MultiIter elem = MultiList->multiParts.begin();
			for ( ; elem != MultiList->multiParts.end(); elem++ )
			{
				stMultiPart multi = *elem;
				int mz = object->z + multi.z;
				if ( object->x + multi.x == x && object->y + multi.y == y && mz > z )
				{
					if ( mz < roof )
					{
						roof = mz;
					}
				}
			}
			continue;
		}

		if ( (object->x == x) && (object->y == y) && (object->z > z) )
		{
			if ( object->z < roof )
			{
				roof = object->z;
			}
		}
	}

	return roof;
}


void cDynamicObjectList::OnAdd (void (*callback) (cDynamicObject * object))
{
  callback_OnAdd = callback;
}

void cDynamicObjectList::OnDelete (void (*callback) (cDynamicObject * object))
{
  callback_OnDelete = callback;
}


cDynamicObject *cDynamicObjectList::AddCharEquip (Uint32 id, Uint16 model,
                                                  Uint16 dye, Uint32 parent)



{
  cDynamicObject *result = Add (id);
  assert (result);
  result->model = model;
  result->type = DYNAMICTYPE_EQUIPMENT;
  result->dye = dye;
  result->parent = parent;
  return result;
}

cDynamicObject *cDynamicObjectList::AddContainerContent (Uint32 id,
                                                         Uint16 model,
                                                         Uint16 dye,
                                                         Uint32 parent, int x,
                                                         int y, int itemcount)
{
  cDynamicObject *result = Add (id);
  assert (result);
  result->id = id;
  result->type = DYNAMICTYPE_CONTAINER;
  result->model = model;
  result->itemcount = itemcount;
  result->x = x;
  result->y = y;
  result->parent = parent;
  result->dye = dye;
  return result;
}

cDynamicObject *cDynamicObjectList::AddWorldItem( Uint32 id, Uint16 model, Uint16 dye, int x, int y, int z,
												  int itemcount, int incrcounter, int direction, int flag )
{
    //-----------------------------------------------------------------------------------------
    //Tensor: old method was simply a:
    //	cDynamicObject *result  = Add( id );
    //-----------------------------------------------------------------------------------------

	//Tensor: what did we do here? Imho not the best idea, since we don't cleanup anything...
    //-----------------------------------------------------------------------------------------
	cDynamicObject *result = Get( id );
	// much better with that check!
	if ( !( result && result->x == x && result->y == y && result->z == z ) )
	{
        //printf("[WorldItem-ID  (%d)]\n", id);
		result = Add( id );
    //-----------------------------------------------------------------------------------------
        assert( result );
        result->id = id;
        result->type = DYNAMICTYPE_WORLD;
        result->model = model;

        // SiENcE: just for debugging
        // char msg[128];
        // sprintf (msg, "ModelID: %i\n", model);
        // printf(msg);

        result->x = x;
        result->y = y;
        result->z = z;
        result->itemcount = itemcount;
        result->incrcounter = incrcounter;
        result->dye = dye;
        result->direction = direction;
        result->flag = flag;

        cStaticModel *static_model = StaticModelLoader::GetInstance()->getModel( model );
        if ( static_model )
        {
            int blockx = x / 8;
            int blocky = y / 8;

            cMotiveBasedLight *light = new cMotiveBasedLight_Entity( (float)(x % 8), (float)(y % 8), (float)z, blockx, blocky, static_model );
            vLights.push_back( light );
            result->setMotive( light );
            if ( static_model->GetLightSourceInfo() )
            {
                result->setLightSource( pLightManager.AddDefinedStaticLightSource( x, y, z,static_model->GetLightSourceInfo() ) );
            }

            std::list<cLight3D *> static_light_list = pLightManager.static_light_list();
            std::list<cLight3D *>::iterator light_iter;
            for ( light_iter = static_light_list.begin(); light_iter != static_light_list.end(); light_iter++ )
            {
                if ( (*light_iter)->light_in_block( blockx, blocky ) )
                {
                    light->AddLight( *light_iter );
                }
            }
            //much better to init. effects here...effects should be independent from lightlist
            if ( static_model->GetParticleEffectInfo() )
            {
                result->SetParticleEffectHandle( pParticleEngine.AddEffect( static_model->GetParticleEffectInfo(),
                    (float)x, (float)y - 0.8, z * 0.1f ) );
                    //-0.8 is a hack for p.effects
            }
        }

        if ( callback_OnAdd )
        {
            callback_OnAdd( result );
        }
	}
	return result;
}


void cDynamicObjectList::CalcAmbientLight (sColor ambient_color,
                                           sColor sun_color,
                                           float direction[3])
{
  dynamiclist_t::iterator iter;
  for (iter = dynamiclist.begin (); iter != dynamiclist.end (); iter++)
      {
        cDynamicObject *object = iter->second;
        if (object->motive ())
          object->motive ()->CalcAmbientLight (ambient_color, sun_color,
                                               direction);
      }

}

void cDynamicObjectList::SetRecalcAmbientLightFlag ()
{
  dynamiclist_t::iterator iter;
  for (iter = dynamiclist.begin (); iter != dynamiclist.end (); iter++)
    iter->second->SetRecalcAmbientLightFlag ();
}

void cDynamicObjectList::AddLight (cLight3D * light)
{
  assert (light);
  dynamiclist_t::iterator iter;
  for (iter = dynamiclist.begin (); iter != dynamiclist.end (); iter++)
      {
        cDynamicObject *object = iter->second;
        if (object->motive ())
          object->motive ()->AddLight (light);
      }

}

void cDynamicObjectList::RemoveLight (cLight3D * light)
{
  assert (light);
  dynamiclist_t::iterator iter;
  for (iter = dynamiclist.begin (); iter != dynamiclist.end (); iter++)
      {
        cDynamicObject *object = iter->second;
        if (object->motive ())
          object->motive ()->RemoveLight (light);
      }
}
