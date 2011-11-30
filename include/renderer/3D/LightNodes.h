//
// File: LightNodes.h
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

/*
   This Module handles the static ambient lighting (see docs for more info :)
*/
 
#ifndef _LIGHTNODES_H_
#define _LIGHTNODES_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <vector>
#include <map>

#include "loaders/StaticModels.h"

// #include "../Fluid/mmgr.h"

struct sLightNodeNormal {
    float normal[3];
    struct sLightNodeNormal * next;
};

struct sEquivNodes;
struct sLightNode {
    float resulting_normal[3];
    struct sLightNodeNormal * children;
    struct sEquivNodes * equiv_nodes;
};

struct sEquivNodes {
    struct sLightNode * nodes[3];
};

class cLightNodeEnvironment; // node handlers "around" the current node handler
class cLightNodeHandler {
    private:
        std::map <Uint32, sLightNode *> nodemap;   // a map for checking multiple nodes
        std::vector <sLightNodeNormal *> normal_array_cache;  // a list of node arrays      
        sLightNodeNormal * m_current_normal_array;
        Uint32 m_normal_count;
        Uint32 m_normal_head;  // index for adding
    
        std::vector <sLightNode *> node_array_cache;  // a list of node arrays      
        sLightNode * m_current_node_array;
        Uint32 m_node_count;
        Uint32 m_node_head;  // index for adding
        
        std::vector <sEquivNodes *> equiv_array_cache;  // a list of node arrays      
        sEquivNodes * m_current_equiv_array;
        Uint32 m_equiv_count;
        Uint32 m_equiv_head;  // index for adding
        
        bool m_normals_changed;
        
        void AllocNewNormalArray ();
        void AllocNewNodeArray ();
        void AllocNewEquivArray ();
    public:        
        cLightNodeHandler ();
        ~cLightNodeHandler ();
        
        struct sLightNodeNormal * AddNormal (float * normal);
        struct sLightNode * AddNode (float * normal, float x, float y, float z);
        struct sEquivNodes * AddEquiv ();
        void AddEquivNodeToNode (sLightNode * node, sLightNode * equiv_node);
        void RemoveNodeFromEquivNode (sLightNode * node, sLightNode * equiv_node);
        
        // light_node_list: expects a list of model->nodecount () pointers 
        void AddModel (cStaticModel * model, float x, float y, float z, sLightNode * * light_node_list, cLightNodeEnvironment & environment); 
        
        // calculate normals
        void CalcNormals ();
        
        bool getNormalsChanged ();
        void setNormalsChanged (bool normals_changed);
};

 
#endif //_LIGHTNODES_H_
