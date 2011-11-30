//
// File: LightNodes.cpp
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

#include "renderer/3D/LightNodes.h"
#include "renderer/3D/LightNodeEnvironment.h"
#include "Exception.h"
#include "Geometry.h"
#include <cassert>
#include <math.h>




using namespace std;

#define LIGHTNODE_ARRAY_SIZE 32
#define NODEEQUIV_ARRAY_SIZE 8

cLightNodeHandler::cLightNodeHandler ()
{
  m_normal_count = 0;           // Initialize count
  m_normal_head = LIGHTNODE_ARRAY_SIZE;   // fake a full array
  m_current_normal_array = NULL;

  m_node_count = 0;             // Initialize count
  m_node_head = LIGHTNODE_ARRAY_SIZE;     // fake a full array
  m_current_node_array = NULL;

  m_equiv_count = 0;            // Initialize count
  m_equiv_head = NODEEQUIV_ARRAY_SIZE;    // fake a full array
  m_current_equiv_array = NULL;

  m_normals_changed = false;
}

cLightNodeHandler::~cLightNodeHandler ()
{
  unsigned int index, node_index;
  sLightNode *node;

  for (index = 0; index < node_array_cache.size (); index++)
      {
        node = node_array_cache[index];
        // calculate count of nodes in an array
        // the last array contains m_node_head nodes, the other arrays are full
        unsigned int size =
          (index ==
           node_array_cache.size () - 1) ? m_node_head : LIGHTNODE_ARRAY_SIZE;
        for (node_index = 0; node_index < size; node_index++)
            {
              if (node->equiv_nodes)
                for (int i = 0; i < 3; i++)
                  RemoveNodeFromEquivNode (node->equiv_nodes->nodes[i], node);
              node->equiv_nodes = NULL;
              node++;
            }
      }


  // Free all arrays
  for (index = 0; index < normal_array_cache.size (); index++)
    free (normal_array_cache[index]);
  for (index = 0; index < node_array_cache.size (); index++)
    free (node_array_cache[index]);
  for (index = 0; index < equiv_array_cache.size (); index++)
    free (equiv_array_cache[index]);
}

struct sLightNode *cLightNodeHandler::AddNode (float *normal, float x,
                                               float y, float z)
{
  // Calc a Uint32 ID out of the position                

  int rx = (int) ((x * 20.0f) + 512.5f);
  if ((rx < 0) || (rx > 2047))
    rx = 0;

  int ry = (int) ((y * 20.0f) + 512.5f);
  if ((ry < 0) || (ry > 2047))
    ry = 0;

  int rz = (int) ((z * 4.0f) + 512.5f);
  if ((rz < 0) || (rz > 1023))
    rz = 0;

  Uint32 id = ((Uint32) rx) << 21 | ((Uint32) ry) << 10 | ((Uint32) rz);

  struct sLightNode *result;
  map < Uint32, sLightNode * >::iterator iter = nodemap.find (id);

  if (iter == nodemap.end ())
      {                         // if not found, alloc a new node
        if (m_node_head == LIGHTNODE_ARRAY_SIZE)      // if current array is full
          AllocNewNodeArray (); // alloc a new array
        assert (m_current_node_array);
        result = m_current_node_array + m_node_head;
        assert (result);
        m_node_count++;
        m_node_head++;
        result->children = NULL;
        result->equiv_nodes = NULL;

        nodemap.insert (make_pair (id, result));
      }
  else
      {
        result = iter->second;
      }
  assert (result);

  if (normal)
      {
        struct sLightNodeNormal *normal_entry = AddNormal (normal);     // Create new normal entry
        normal_entry->next = result->children;  // and Add to a linked list
        result->children = normal_entry;
      }

  assert (result);
  return result;

}

struct sLightNodeNormal *cLightNodeHandler::AddNormal (float *normal)
{
  // allocate new node
  assert (normal);
  assert (m_normal_head <= LIGHTNODE_ARRAY_SIZE);

  if (m_normal_head == LIGHTNODE_ARRAY_SIZE)    // if current array is full
    AllocNewNormalArray ();     // alloc a new array

  assert (m_current_normal_array);
  sLightNodeNormal *result = m_current_normal_array + m_normal_head;
  memcpy (result->normal, normal, sizeof (float) * 3);
  result->next = NULL;

  m_normal_count++;
  m_normal_head++;

  return result;
}

struct sEquivNodes *cLightNodeHandler::AddEquiv ()
{
  assert (m_equiv_head <= NODEEQUIV_ARRAY_SIZE);
  if (m_equiv_head == NODEEQUIV_ARRAY_SIZE)     // if current array is full
    AllocNewEquivArray ();      // alloc a new array

  assert (m_current_equiv_array);
  sEquivNodes *result = m_current_equiv_array + m_equiv_head;
  m_equiv_count++;
  m_equiv_head++;

  return result;
}

void cLightNodeHandler::AddModel (cStaticModel * model, float x, float y,
                                  float z, sLightNode * *light_node_list,
                                  cLightNodeEnvironment & environment)
{
  assert (model);
  assert (light_node_list);
  tStaticNodes *nodes = model->nodes ();
  for (unsigned int index = 0; index < nodes->size (); index++)
      {
        float *normal = (*nodes)[index]->normal ();
        float *position = (*nodes)[index]->position ();
        float pos_x = position[0] + x;
        float pos_y = position[1] + y;
        float pos_z = position[2] * 10.0f + z;

        struct sLightNode *node = AddNode (normal, pos_x, pos_y, pos_z);
        if (!node)
          return;

        assert (node);
        //if (!node->equiv_node) {
        int dx = (pos_x < 0.00f) ? -1 : (pos_x >= 8.00f) ? 1 : 0;
        int dy = (pos_y < 0.00f) ? -1 : (pos_y >= 8.00f) ? 1 : 0;
        if (dx || dy)
            {                   // Node is outside of block
              float new_x = pos_x - (float) (dx * 8);
              float new_y = pos_y - (float) (dy * 8);

              //if (dx && dy)
              //printf("New: %.2f/%.2f Old: %.2f/%.2f | %d %d\n", new_x, new_y, pos_x, pos_y, dx, dy);

              cLightNodeHandler *env_handler = environment.get (dx, dy);
              env_handler->setNormalsChanged (true);

              sLightNode *equiv_node =
                env_handler->AddNode (NULL, new_x, new_y, pos_z);

              assert (env_handler != this);

              if (!node->equiv_nodes)
                node->equiv_nodes = AddEquiv ();

              if (!equiv_node->equiv_nodes)
                equiv_node->equiv_nodes = env_handler->AddEquiv ();

              assert (node->equiv_nodes);
              assert (equiv_node->equiv_nodes);

              AddEquivNodeToNode (node, equiv_node);
              AddEquivNodeToNode (equiv_node, node);

              for (int i = 0; i < 3; i++)
                  {
                    AddEquivNodeToNode (node,
                                        equiv_node->equiv_nodes->nodes[i]);
                    AddEquivNodeToNode (equiv_node->equiv_nodes->nodes[i],
                                        node);
                    AddEquivNodeToNode (equiv_node,
                                        node->equiv_nodes->nodes[i]);
                    AddEquivNodeToNode (node->equiv_nodes->nodes[i],
                                        equiv_node);
                  }

/*                for (int i = 0; i < 3; i++)
                        AddEquivNodeToNode(equiv_node, node->equiv_nodes->nodes[i]);*/


/*                    
                
                for (int i = 0; i < 3; i++) {
                    if (node->equiv_nodes->nodes[i] == equiv_node) 
                            break;
                    if (node->equiv_nodes->nodes[i] == NULL) {
                            node->equiv_nodes->nodes[i] = equiv_node;                                
                            break;
                    }
                }*/


              /*assert ((node->equiv_node == equiv_node) || !node->equiv_node);

                 node->equiv_node = equiv_node;
                 node->equiv_node->equiv_node = node; */
            }
        //} 

        light_node_list[index] = node;
      }
}

void cLightNodeHandler::AddEquivNodeToNode (sLightNode * node,
                                            sLightNode * equiv_node)
{
  if (!node || !equiv_node || (node == equiv_node))
    return;


  assert (node->equiv_nodes);

  for (int i = 0; i < 3; i++)
    if (node->equiv_nodes->nodes[i] == equiv_node)
      return;

  for (int i = 0; i < 3; i++)
    if (node->equiv_nodes->nodes[i] == NULL)
        {
          node->equiv_nodes->nodes[i] = equiv_node;
          return;
        }
  assert (false);
}


void cLightNodeHandler::RemoveNodeFromEquivNode (sLightNode * node,
                                                 sLightNode * equiv_node)
{
  if (!node || !equiv_node || (node == equiv_node))
    return;

  assert (node->equiv_nodes);

  for (int i = 0; i < 3; i++)
    if (node->equiv_nodes->nodes[i] == equiv_node)
      node->equiv_nodes->nodes[i] = NULL;
}

void cLightNodeHandler::AllocNewNormalArray ()
{
  m_normal_head = 0;
  m_current_normal_array =
    (struct sLightNodeNormal *) malloc (LIGHTNODE_ARRAY_SIZE *sizeof (struct sLightNodeNormal));
  memset (m_current_normal_array, 0,
          LIGHTNODE_ARRAY_SIZE * sizeof (struct sLightNodeNormal));
  normal_array_cache.push_back (m_current_normal_array);
}


void cLightNodeHandler::AllocNewNodeArray ()
{
  m_node_head = 0;
  m_current_node_array =
    (struct sLightNode *) malloc (LIGHTNODE_ARRAY_SIZE *sizeof (struct sLightNode));
  memset (m_current_node_array, 0,
          LIGHTNODE_ARRAY_SIZE * sizeof (struct sLightNode));
  node_array_cache.push_back (m_current_node_array);
}

void cLightNodeHandler::AllocNewEquivArray ()
{
  m_equiv_head = 0;
  m_current_equiv_array =
    (struct sEquivNodes *) malloc (NODEEQUIV_ARRAY_SIZE *sizeof (struct sEquivNodes));
  memset (m_current_equiv_array, 0,
          NODEEQUIV_ARRAY_SIZE * sizeof (struct sEquivNodes));
  equiv_array_cache.push_back (m_current_equiv_array);
}



void cLightNodeHandler::CalcNormals ()
{
  int i, equiv_index;
  sLightNode *node = NULL;
  int cache_index = 0;


  int curindex = LIGHTNODE_ARRAY_SIZE;
  for (unsigned int index = 0; index < m_node_count; index++)
      {
        if (curindex == LIGHTNODE_ARRAY_SIZE)
            {
              node = node_array_cache[cache_index];
              cache_index++;
              curindex = 0;
            }


        float normal[3];
        for (i = 0; i < 3; i++)
          normal[i] = 0.0f;

        sLightNodeNormal *normal_entry = node->children;
        while (normal_entry)
            {
              for (i = 0; i < 3; i++)
                normal[i] += normal_entry->normal[i];
              normal_entry = normal_entry->next;
            }

        if (node->equiv_nodes)
          for (equiv_index = 0; equiv_index < 3; equiv_index++)
            if (node->equiv_nodes->nodes[equiv_index])
                {
                  sLightNodeNormal *normal_entry =
                    node->equiv_nodes->nodes[equiv_index]->children;
                  while (normal_entry)
                      {
                        for (i = 0; i < 3; i++)
                          normal[i] += normal_entry->normal[i];
                        normal_entry = normal_entry->next;
                      }
                }

        if (normal[0] || normal[1] || normal[2])
            {
              float fac =
                1.0f / sqrt (normal[0] * normal[0] + normal[1] * normal[1] +
                             normal[2] * normal[2]);
              for (i = 0; i < 3; i++)
                node->resulting_normal[i] = normal[i] * fac;
            }
        else
            {
              if (node->children)
                  {
                    for (i = 0; i < 3; i++)
                      node->resulting_normal[i] = node->children->normal[i];
                  }
              else
                  {
                    for (i = 0; i < 3; i++)
                      node->resulting_normal[i] = 0.0f;
                  }
            }

        if (node->equiv_nodes)
          for (equiv_index = 0; equiv_index < 3; equiv_index++)
              {
                sLightNode *equiv_node =
                  node->equiv_nodes->nodes[equiv_index];
                if (equiv_node)
                  for (i = 0; i < 3; i++)
                    equiv_node->resulting_normal[i] =
                      node->resulting_normal[i];
              }

        node++;
        curindex++;
      }
}

bool cLightNodeHandler::getNormalsChanged ()
{
  bool result = m_normals_changed;
  m_normals_changed = false;
  return result;
}

void cLightNodeHandler::setNormalsChanged (bool normals_changed)
{
  m_normals_changed = normals_changed;
}
