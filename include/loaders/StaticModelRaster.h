//
// File: StaticModelRaster.h
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


#ifndef _STATICMODELRASTER_H_
#define _STATICMODELRASTER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"
#include "SDL/SDL.h"

#include <iostream>
#include <fstream>

// #include "../Fluid/mmgr.h"

class cStaticModelRaster {

    private:

        Uint8 * m_data;
        Sint32 m_size_x, m_size_y, m_size_z;
        Sint32 m_min_x, m_min_y, m_min_z;

    public:

        cStaticModelRaster ();
        ~cStaticModelRaster ();
        void LoadFromStream (std::ifstream * stream);
        Uint8 * data(); 
        Sint32 size_x ();
        Sint32 size_y ();
        Sint32 size_z ();
        Sint32 min_x ();
        Sint32 min_y ();
        Sint32 min_z ();
};


#endif //_STATICMODELRASTER_H_

