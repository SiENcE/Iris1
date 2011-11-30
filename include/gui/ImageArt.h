//
// File: ImageArt.h
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

#ifndef _IMAGEART_H_
#define _IMAGEART_H_

#include "Control.h"
#include "renderer/TextureBuffer.h"

// #include "../Fluid/mmgr.h"


class ImageArt : public Control
{
public:
	 ImageArt();
	 ImageArt( int iX, int iY, int iTextId );
	 ImageArt( int iX, int iY, int iTextId, int iFlags );
	 ~ImageArt();

	 void SetTexID( int iTextId );
	 void SetHue( int iHue );
	 
	 int GetTexID() const;
	 int GetHue() const;

	 virtual void Draw( GumpHandler *kGumps );

private:
	Texture *m_kTexture;
	int m_iTextId;
	int m_iHue;
	std::vector<Texture *> m_vTextures;
};

#endif	// _IMAGEART_H_
