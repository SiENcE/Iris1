//
// File: ImageArt.cpp
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

#include "gui/ImageArt.h"




ImageArt::ImageArt() : m_iTextId( 0 ), m_iHue( 0 )
{
	Control::Control();
}


ImageArt::ImageArt( int iX, int iY, int iTextId ) : m_iHue( 0 ), m_iTextId( iTextId )
{
	Control::Control();
	SetPosition( iX, iY );
	control_type = CONTROLTYPE_IMAGEART;
}


ImageArt::ImageArt( int iX, int iY, int iTextId, int iFlags ) : m_iTextId( iTextId )
{
	Control::Control();
	SetPosition( iX, iY );
	SetFlags( iFlags );
	control_type = CONTROLTYPE_IMAGEART;
}


ImageArt::~ImageArt()
{
	for ( unsigned int i = 0; i < m_vTextures.size(); i++ )
	{
		SAFE_DELETE( m_vTextures[i] );
	}
}


void ImageArt::SetTexID( int iTextId )
{
	m_iTextId = iTextId;
}


void ImageArt::SetHue( int iHue )
{
	m_iHue = iHue;
}


int ImageArt::GetTexID() const
{
	return m_iTextId;
}


int ImageArt::GetHue() const
{
	return m_iHue;
}


void ImageArt::Draw( GumpHandler *kGumps )
{
	Control::Draw( kGumps );

	m_kTexture = TextureBuffer::GetInstance()->GetArtTexture( m_iTextId + 16384 );

	if ( m_kTexture )
	{
		m_vTextures.push_back( m_kTexture );
		if ( !GetWidth() || !GetHeight() )
		{
			SetSize( m_kTexture->GetRealWidth(), m_kTexture->GetRealHeight() );
		}
		// DrawRect(10, 10, texture->GetWidth(), texture->GetHeight(), texture, 250, true);
		// DrawRect(GetX(), GetY(), GetWidth(), GetHeight(), texture, GetAlpha(), true);
		if ( m_iHue != 0 )
		{
			DrawRectHued( GetX(), GetY(), m_kTexture->GetWidth(), m_kTexture->GetHeight(), 
				m_kTexture, m_iHue, GetAlpha(), true );
		}
		else
		{
			DrawRect( GetX(), GetY(), m_kTexture->GetWidth(), m_kTexture->GetHeight(), m_kTexture, GetAlpha(), true );
		}
	}
}
