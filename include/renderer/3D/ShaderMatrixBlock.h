//
// File: ShaderMatrixBlock.h
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
  This file contains a bitmask which is used for shading the terrain and statics
  see lighting docs for more info
*/
 
#ifndef _SHADERMATRIXBLOCK_H_
#define _SHADERMATRIXBLOCK_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

#include <vector>
#include "renderer/StaticObjects.h"
#include "loaders/StaticModels.h"

// #include "../Fluid/mmgr.h"


class cShaderMatrixBlock {
    private:
        Uint8 bitmask[52*16*16]; // (52 * 16 * 16) for one block
        int m_block_x;
        int m_block_y;
    public:
        cShaderMatrixBlock ();
        ~cShaderMatrixBlock ();
        
        void setBlock (int x, int y);
        
        void AddModel (int x, int y, int z, cStaticModel * model);
        void AddGround (signed char * groundmap); // expects a [9][9]
        void CheckRay (float x, float y, float z, float light_dir[3], float & intensity);
        void DoRender (float move_x, float move_y);
};

 
#endif //_SHADERMATRIXBLOCK_H_
