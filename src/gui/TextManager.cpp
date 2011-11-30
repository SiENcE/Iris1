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


// Iris Includes
#include "gui/TextManager.h"
#include "loaders/HueLoader.h"
#include "renderer/Texture.h"
#include "renderer/SDLScreen.h"
#include "Config.h"
#include "irisgl.h"
#include <cassert>
#include "Logger.h"




// Library Includes
#include <vector>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//extern SDLScreen *SDLscreen;

// Private Data Members of cTextElement
struct stTextItem
{
  unsigned int width, height;
  unsigned int id;

  // GL Texture identifier
  Texture *texture;
};

/*!
	This method is for (re-)setting the text (and creating a texture, width, height)
*/
void cTextElement::setText( const char *text, unsigned short hueId, unsigned short fontId )
{
	assert( SDLScreen::GetInstance() );
	if ( p )
	{
		SAFE_DELETE( p->texture );
        SAFE_DELETE( p );
		p = NULL;
	}

	TTF_Font *ttf_font = FontManager::GetInstance()->GetTTFFont( fontId );
	
	if ( ttf_font )
	{
		if ( !hueId )
		{
			hueId = 0;
		}
		setTextTTF( text, hueId, ttf_font );
		//if(_style)
		TTF_SetFontStyle( ttf_font, _style );
		return;
	}

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int hChar = 0;
	unsigned int i = 0;

	// Calculate the Width of the whole text first
	const stFont *font = FontManager::GetInstance()->GetMulFont( fontId );

	// Simply don't render
	if ( !font )
	{
		return;
	}

	const stHue *hue = 0;

	if ( hueId != 0 )
	{
		hue = pHueLoader.getHue( hueId );
	}

	while ( text[i] != 0 )
	{
		unsigned char c = (unsigned char)text[i++];

		if ( c < 32 )
		{
			continue;
		}

		c -= 32;                // First 32 characters are not included

		width += font->chars[c].width;
		hChar = max( hChar, font->chars[c].height );
	}

	_width = width;
	_height = hChar;

	// Width and Height need to be a power of 2
	height = max( 64, hChar );     // Highest Char / Image Height

	// Make sure that height and width are powers of two so we don't get scaled here (min: 64)
	i = 64;

	while ( i < width )
	{
		i *= 2;
	}

	width = i;

	i = 64;

	while ( i < height )
	{
		i *= 2;
	}

	height = i;

	unsigned int *pixels = new unsigned int[width * height];  // RGBA (32 bit)
	unsigned int datalength = width * height * 4;

	memset( pixels, 0, datalength );   // All Transparent

	unsigned int xOffset = 0;

	i = 0;

	// Now copy each character over
	while ( text[i] != 0 )
	{
		unsigned char c = (unsigned char)text[i++];

		if ( c < 32 )
		{
			continue;
		}

		c -= 32;                // First 32 characters are not included

		// Calculate the Y offset
		unsigned int yOffset = hChar - font->chars[c].height;

		// Start at curY and curX and copy the pixels over.
		// Fonts are encoded Top To Bottom, Left To Right
		for ( unsigned int x = 0; x < font->chars[c].width; ++x )
		{
			for ( unsigned int y = 0; y < font->chars[c].height; ++y )
			{
				unsigned int color = font->chars[c].pixels[y * font->chars[c].width + x];

				// Alpha
				if ( color == 0 )
				{
					continue;
				}

				if ( hue )
				{
					color = hue->colors[font->chars[c].redmask[y * font->chars[c].width + x]];
				}

				pixels[(yOffset + y) * width + (xOffset + x)] = color;
			}
		}
		xOffset += font->chars[c].width;
	}

	Texture *texture = new Texture();
	texture->LoadFromData( pixels, width, height, 32, GL_LINEAR );

	SAFE_DELETE_ARRAY( pixels );

	stTextItem *item = new stTextItem();

	item->texture = texture;
	item->width = width;
	item->height = height;

	p = item;
}

/*
	This method encapsulates the creation of a text element.
*/
cTextElement::cTextElement (const char *text, unsigned short hueId,
                            unsigned short fontId, int style)
{
  p = 0;
  _width = 0;
  _style = style;
  setText (text, hueId, fontId);
}

cTextElement::~cTextElement ()
{
  if (p)
    delete p->texture;

  delete p;
}

void cTextElement::draw (int x, int y, unsigned int width,
                         unsigned int height, int xOffset)
{
  if (!p || !p->texture)
    return;

  // xOffset is used to wrap the text "into" the boundary box defined by width and height
  // The Text is NOT stretched but cropped to fit into the box
  if (!width)
    width = p->texture->GetWidth ();

  if (!height)
    height = p->texture->GetHeight ();

  // Build a float value to represent the offset.

  glBindTexture (GL_TEXTURE_2D, p->texture->GetGLTex ());
  glBegin (GL_QUADS);

  int y1 = Config::GetHeight() - 1 - (y);
  int y2 = Config::GetHeight() - 1 - (y + height);

  // NOTE: _width is the *real* width of the visible characters
  // while p->width is the width of the texture (including alpha parts)

  // If we don't need to crop then don't do any processing
  if (width >= _width)
    width = _width;


//  unsigned int xAlpha = p->width - _width;

  int x1 = x;
  int x2 = x + width;

  // Normal: 0.0
  //float u1 = (float)xOffset / (float)p->texture->GetWidth();
  float u1 = 0.0;

  // Width should be the really visible width
  if (width < _width)
    u1 = (float) (xOffset) / (float) p->width;  // This gets too large yet

  // Normal: 1.0
  float u2 = 1.0 + (u1 - (float) (p->width - width) / (float) p->width);

  //float u2 = (float)width / (float)p->texture->GetWidth() - u1;

  // Normal: 0.0
  float v1 = 0.0;

  // Normal: 1.0
  float v2 = (float) height / (float) p->texture->GetHeight ();

  glTexCoord2f (u1, v1);
  glVertex2i (x1, y1);

  glTexCoord2f (u1, v2);
  glVertex2i (x1, y2);

  glTexCoord2f (u2, v2);
  glVertex2i (x2, y2);

  glTexCoord2f (u2, v1);
  glVertex2i (x2, y1);

  glEnd ();
}

Texture *cTextElement::texture ()
{
  if (!p)
    return NULL;
  return p->texture;
}

struct TextManagerPrivate
{
  std::vector < cFixedTextElement * >fixedText;
};

// Initialize our private implementation
cTextManager::cTextManager ()
{
  p = new TextManagerPrivate;
}

// Destroy our private implementation
cTextManager::~cTextManager ()
{
  // Free all TextItems on Screen
  std::vector < cFixedTextElement * >::iterator it;

  for (it = p->fixedText.begin (); it != p->fixedText.end (); ++it)
    delete (*it);

  delete p;
}

/*!
	Draw all items registered in this Manager
*/
void cTextManager::draw ()
{
  // Draw fixed text items.
  for (unsigned int i = 0; i < p->fixedText.size (); ++i)
      {
        cFixedTextElement *item = p->fixedText[i];

        item->draw ();
      }
}

unsigned char cTextManager::getTextHeight (const char *text,
                                           unsigned short fontId)
{
	TTF_Font *ttf_font = FontManager::GetInstance()->GetTTFFont (fontId);
  if (ttf_font)
      {
        return getTextHeightTTF (text, ttf_font);
      }

  // Calculate the Width of the whole text first
  const stFont *font = FontManager::GetInstance()->GetMulFont( fontId );

  // Simply don't render
  if (!font)
    return 0;

  unsigned int i = 0;
  unsigned char hChar = 0;

  while (text[i] != 0)
      {
        unsigned char c = text[i++];

        if (c < 32)
          continue;

        c -= 32;                // First 32 characters are not included

        hChar = max (hChar, font->chars[c].height);
        i++;
      }

  return hChar;
}

unsigned int cTextManager::getTextWidth (const char *text,
                                         unsigned short fontId)
{

	TTF_Font *ttf_font = FontManager::GetInstance()->GetTTFFont (fontId);
  if (ttf_font)
      {
        return getTextWidthTTF (text, ttf_font);
      }

  // Calculate the Width of the whole text first
  const stFont *font = FontManager::GetInstance()->GetMulFont( fontId );

  // Simply don't render
  if (!font)
    return 0;

  unsigned int i = 0;
  unsigned int width = 0;

  while (text[i] != 0)
      {
        unsigned char c = text[i++];

        if (c < 32)
          continue;

        c -= 32;                // First 32 characters are not included

        width += font->chars[c].width;
      }

  return width;
}

void cTextManager::addFixedText (cFixedTextElement * element)
{
  p->fixedText.push_back (element);
}

void cTextManager::removeFixedText (cFixedTextElement * element)
{
  std::vector < cFixedTextElement * >::iterator it;

  for (it = p->fixedText.begin (); it != p->fixedText.end (); ++it)
      {
        if (*it == element)
            {
              p->fixedText.erase (it);
              return;
            }
      }
}



static int power_of_two (int input)
{
  int value = 1;

  while (value < input)
      {
        value <<= 1;
      }
  return value;
}

void cTextElement::setTextTTF (const char *text, unsigned short hue,
                               TTF_Font * font)
{
  if (!font)
    return;

  SDL_Color color;

  const struct stHue *sthue = NULL;
  if (hue != 0)
    sthue = pHueLoader.getHue ( hue );

  if (sthue)
      {
        Uint32 col32 = sthue->colors[31];
        color.r = col32 & 0xFF;
        color.g = (col32 >> 8) & 0xFF;
        color.b = (col32 >> 16) & 0xFF;
      }
  else
      {
        color.r = 0xFF;
        color.g = 0xFF;
        color.b = 0xFF;
      }
  color.unused = 0;


  SDL_Surface *surface = TTF_RenderText_Blended (font, text, color);

  if (!surface)
      {
        return;
      }

  Texture *texture = new Texture;

  int w, h;
  SDL_Surface *image;
  SDL_Rect area;
  Uint32 saved_flags;
  Uint8 saved_alpha;

  /* Use the surface width and height expanded to powers of 2 */
  int real_width = surface->w;
  int real_height = surface->h;
  w = power_of_two (surface->w);
  h = power_of_two (surface->h);

  _width = real_width;
  _height = real_height;

  image = SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, 32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
                                0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#else
                                0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#endif
    );
  if (image == NULL)
      {
        return;
      }

  /* Save the alpha blending attributes */
  saved_flags = surface->flags & (SDL_SRCALPHA | SDL_RLEACCELOK);
  saved_alpha = surface->format->alpha;
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
      {
        SDL_SetAlpha (surface, 0, 0);
      }

  /* Copy the surface into the GL texture image */
  area.x = 0;
  area.y = 0;
  area.w = surface->w;
  area.h = surface->h;
  SDL_BlitSurface (surface, &area, image, &area);

  /* Restore the alpha blending attributes */
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
      {
        SDL_SetAlpha (surface, saved_flags, saved_alpha);
      }

  texture->LoadFromSurface (image);
  /* Create an OpenGL texture for the image */
  SDL_FreeSurface (image);      /* No longer needed */
  SDL_FreeSurface (surface);    /* No longer needed */

/*	SDL_Surface * surface = TTF_RenderText_Blended(font, text, color);
	if (surface) {
		

  texture->LoadFromSurface(surface);
		SDL_FreeSurface(surface);

*/
  stTextItem *item = new stTextItem;

  item->texture = texture;
  item->width = w;
  item->height = h;

  p = item;
}

unsigned char cTextManager::getTextHeightTTF (const char *text,
                                              TTF_Font * font)
{
  SDL_Color color;
  color.r = 0xff;
  color.g = 0xff;
  color.b = 0xff;

  SDL_Surface *surface = TTF_RenderText_Blended (font, text, color);

  if (!surface)
    return 0;
  Uint16 h = surface->h;

  SDL_FreeSurface (surface);

  return h;
}

unsigned int cTextManager::getTextWidthTTF (const char *text, TTF_Font * font)
{
  SDL_Color color;
  color.r = 0xff;
  color.g = 0xff;
  color.b = 0xff;
  SDL_Surface *surface = TTF_RenderText_Blended (font, text, color);

  if (!surface)
    return 0;
  Uint16 w = surface->w;

  SDL_FreeSurface (surface);

  return w;
}



cTextManager *pTextManager = NULL;
