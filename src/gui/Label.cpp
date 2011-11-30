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


#include "gui/Label.h"




Label::Label( int x, int y, const char *text, unsigned short hue, unsigned char font )
{
	SetPosition (x, y);

	element = 0;
	timeout = 0;
	creation_time = SDL_GetTicks();
	_text = (char *)malloc( strlen( text ) + 1);
	strcpy (_text, text);
	_hue = hue;
	_font = font;
	_align = ALIGN_LEFT;
	_style = 0;
	rgbcolor = false;
	_r = 0.0f;
	_g = 0.0f;
	_b = 0.0f;
	control_type = CONTROLTYPE_LABEL;
	_wrapped = false;
	SetFlag( GUMPFLAG_MOVABLE, 0 );
	refresh();
}


Label::~Label()
{
	if ( _text )
	{
		free( _text );
	}
	SAFE_DELETE( element );

	for ( unsigned int i = 0; i < wrapped_text.size(); i++ )
	{
		SAFE_DELETE( wrapped_text.at( i ) );
	}
}


void Label::setText( char *text )
{
	if ( _text )
	{
		free( _text );
	}

	_text = (char *)malloc( strlen( text ) + 1);
	strcpy( _text, text );
	refresh();
}

void Label::setHue (unsigned short hue)
{
  _hue = hue;
  refresh ();
}

void Label::setFont (unsigned char font)
{
  _font = font;
  refresh ();
}

void Label::refresh ()
{
	SAFE_DELETE( element );
	int hue = _hue;
	if ( rgbcolor )
	{
		hue = 0;
	}
	element = new cTextElement( _text, hue, _font, _style );
}

void Label::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);
  float fade = 1.0f;

  if (timeout)
      {
        float tick =
          (float) SDL_GetTicks () - ((float) creation_time +
                                     (float) timeout * 0.75f);
        if (tick > 0)
          fade = 1.0f - tick / ((float) timeout * 0.25f);
        if (fade < 0.0f)
          fade = 0.0f;
      }
  if (fade != 1.0f)
    glColor4f (1.0f, 1.0f, 1.0f, fade);
  if (rgbcolor)
    glColor4f (_r, _g, _b, 1.0f);
  switch (_align)
      {
      case ALIGN_LEFT:
        element->draw (x, y);
        break;
      case ALIGN_RIGHT:
        element->draw (x - element->width (), y);
        break;
      case ALIGN_CENTER:
        element->draw (x - element->width () / 2, y);
        break;
      }
  if (_wrapped)
      {
        for (unsigned int i = 0; i < wrapped_text.size (); i++)
            {                   //std::cout << "drawrap" << std::endl;
              switch (_align)
                  {
                  case ALIGN_LEFT:
                    wrapped_text.at (i)->draw (x,
                                               y +
                                               ((i +
                                                 1) *
                                                wrapped_text.at (i)->
                                                height ()));
                    break;
                  case ALIGN_RIGHT:
                    wrapped_text.at (i)->draw (x -
                                               wrapped_text.at (i)->width (),
                                               y +
                                               ((i +
                                                 1) *
                                                wrapped_text.at (i)->
                                                height ()));
                    break;
                  case ALIGN_CENTER:
                    wrapped_text.at (i)->draw (x -
                                               wrapped_text.at (i)->width () /
                                               2,
                                               y +
                                               ((i +
                                                 1) *
                                                wrapped_text.at (i)->
                                                height ()));
                    break;
                  }
            }
      }
  glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
}

cTextElement *Label::GetElement (void)
{
  return element;
}

unsigned int Label::GetTimeOut (void)
{
  return timeout;
}

void Label::SetTimeOut (unsigned int timeout)
{
  this->timeout = timeout;
}

bool Label::OutTimed (void)
{
  if (!timeout)
    return false;
  return SDL_GetTicks () > (creation_time + timeout);
}

void Label::setAlign (unsigned int align)
{
  switch (align)
      {
      case ALIGN_CENTER:
      case ALIGN_RIGHT:
        _align = align;
        break;
      default:
        _align = ALIGN_LEFT;
      }
}

int Label::labelWidth ()
{
  if (element)
    return element->width ();
  return 0;
}

int Label::labelHeight ()
{
  if (element)
    return element->height ();
  return 0;
}

void Label::Crop (int width, int height)
{
  int fontsize = element->width () / (int) strlen ((const char *) _text);

  int croppedsize = width / fontsize;
  if ((int) element->width () > width)
      {
        char *newtext = new char[croppedsize + 1];
        newtext[croppedsize] = 0;
        strncpy (newtext, (const char *) _text, (size_t) croppedsize);
        if(_text)
         free( _text);
        //_text = newtext;
        _text = (char *) malloc (croppedsize + 1);
        strcpy(_text, newtext);
        refresh ();
        delete[] newtext;
      }
}

void Label::SetRGBColor (int r, int g, int b)
{
  rgbcolor = true;
  _r = r / 255.0f;
  _g = g / 255.0f;
  _b = b / 255.0f;
}

void Label::SetStyle (int style)
{
  _style = style;
  refresh ();
}

int Label::Wrap (int width)
{

  _wrapped = true;
  int fontsize = element->width () / (int) strlen ((const char *) _text);


  int croppedsize = width / fontsize;

  int lines = element->width () / width;  // / (int)strlen((const char*)_text);
  std::string str = std::string( _text );

  cTextElement *wrap_elem;
  int hue = _hue;
  if (rgbcolor)
    hue = 0;
  Crop (width, 0);

  int pos = croppedsize;

  for (;;)
      {

        if (croppedsize > (int) str.size () - 1)
          break;
        int pos2;
        if ((int) str.size () - pos >= croppedsize)
          pos2 = croppedsize;
        else
          pos2 = std::string::npos;

        wrap_elem =
          new cTextElement (str.substr (pos, pos2).c_str (), hue, _font,
                            _style);
        wrapped_text.push_back (wrap_elem);

        pos += croppedsize;
        if (pos2 == (int) std::string::npos)
          break;
      }
  return lines;
}

bool Label::MouseIsOver (int x, int y)
{


  if ((x >= this->x) && (y >= this->y) && (x < this->x + this->labelWidth())
      && (y < this->y + this->labelHeight()))
      {
        //if(this->getType == CONTROLTYPE_LABEL)
         //return true;
        //return CheckPixel (x - this->x, y - this->y);
        return true;
      }
  return false;
}

