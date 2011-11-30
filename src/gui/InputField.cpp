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

#include "gui/InputField.h"




InputField::InputField ( int x, int y, unsigned int width, unsigned int height, const char *text, 
						unsigned short hue, unsigned char font, char passwordChar )
{
	OnKeyPress( NULL );
	this->x = x;
	this->y = y;
	this->_passwordChar = passwordChar;
	_width = width;

	if ( height != 0 )
	{
		_height = height;
	}
	else
	{
		const stFont *pFont = FontManager::GetInstance()->GetMulFont( font );

		if ( pFont )
		{
			_height = pFont->maxHeight;
		}
	}

	// 2 * _height
	data = new unsigned int[_height * 2];

	for ( int i = 0; i < _height * 2; ++i )
	{
		data[i] = 0x7FFFFFFF;
	}

	caret = new Texture();
	caret->LoadFromData( data, 2, _height, 32, GL_NEAREST );

	_caretPos = _text.length();
	xCropOffset = 0;

	_hue = hue;
	_font = font;
	tElement = 0;
	generated = false;

	SetFlags( GUMPFLAG_FOCUSABLE );

	control_type = CONTROLTYPE_INPUTFIELD;

	IgnoreCursorKeys = false;

	setText( (char *)text );
}


InputField::~InputField()
{
	SAFE_DELETE( tElement );
	SAFE_DELETE( caret );
	SAFE_DELETE_ARRAY( data );
}


void InputField::setText( char *text )
{
	if ( text )
	{
		_text = text;
	}
	else
	{
		_text = "";
	}

	SAFE_DELETE( tElement );

	if ( _passwordChar )
	{
		// Create a masked text
		char *pText = new char[_text.length() + 1];

		pText[_text.length()] = 0;
		memset( pText, _passwordChar, _text.length() );
		tElement = new cTextElement( pText, _hue, _font );

		SAFE_DELETE_ARRAY( pText );
	}
	else
	{
		tElement = new cTextElement( _text.c_str(), _hue, _font );
	}

	_caretPos = _text.length();

	recalcXCrop();
}

void InputField::regenerate ()
{
  delete tElement;

  if (_passwordChar)
      {
        // Create a masked text
        char *pText = new char[_text.length () + 1];

        pText[_text.length ()] = 0;
        memset (pText, _passwordChar, _text.length ());
        tElement = new cTextElement (pText, _hue, _font);
        SAFE_DELETE_ARRAY( pText );
      }
  else
      {
        tElement = new cTextElement (_text.c_str (), _hue, _font);
      }


  const stFont *font = FontManager::GetInstance()->GetMulFont( _font );

  if (!font)
    return;

//  unsigned int innerWidth = _width;
//  unsigned int innerHeight = font->maxHeight;

  recalcXCrop ();

  generated = true;
}


void InputField::recalcXCrop ()
{
  if (_caretPos == 0)
      {
        caretX = x;
        xCropOffset = 0;
        return;
      }
  // This has to be called whenever the caret position changes
  // It recalculates the inner X offset for the InputField, 
  // so the caret remains always visible to the user.

  // Relative Position of the Caret inside the text-texture
  // For passwords we need to calc the size for our password char and we're done
  unsigned int relX;

  if (_passwordChar)
      {
        /*const stFont *pFont = FontLoader->getFont( _font );

           if( !pFont )
           return;

           relX = _caretPos * pFont->chars[ _passwordChar ].width; */

        char *pText = new char[_text.length () + 1];

        pText[_text.length ()] = 0;
        memset (pText, _passwordChar, _text.length ());
        relX = pTextManager->getTextWidth (pText, _font) - 1;
        SAFE_DELETE_ARRAY( pText );
      }
  else
      {
        relX =
          pTextManager->getTextWidth (_text.substr (0, _caretPos).c_str (),
                                      _font) - 1;
      }

  unsigned int innerWidth = _width;

  if (tElement->width () <= innerWidth)
      {
        xCropOffset = 0;
        caretX = x + relX;
        return;
      }
  // Caret is left of our clip-box
  if (relX < xCropOffset)
      {
        // We need to move the "crop-window" to the left
        xCropOffset -= xCropOffset - relX - 1;
      }
  // It's "in" our clipbox (or right of it)
  else if (relX >= innerWidth)
      {
        unsigned int tempOffset = (relX + 1) - innerWidth;

        // If we are moving from right to left but we're still visible
        // there's no need to change xCropOffset
        // -----------------------------------------------------------
        // There's another problem we need to take care of:
        // When someone deletes text from right-to-left
        // then there is an "empty" space right of the caret
        // This needs to be fixed as gl will tile the texture
        // which leeds to an unwanted gfx glitch.
        if (tElement->width () - xCropOffset < innerWidth
            || tempOffset > xCropOffset || relX - xCropOffset - 1 < 0)
          xCropOffset = tempOffset;
      }

  caretX = x + relX - xCropOffset;
}


void InputField::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);
  if (!generated)
    regenerate ();

  if (!tElement)
    return;

  // Three levels of Height (First Row = y, Second Row = y + HeightOfBackground(), Third Row + y + HeightOfBackground() + HeightOfFont())
  const stFont *font = FontManager::GetInstance()->GetMulFont( _font );

  if (!font)
    return;

  // Do we draw the outer "Border" ?
  // Relevant for our border here are just the corners (positioning)
  unsigned int yFontOffset = _height - tElement->height ();

  tElement->draw (x, y + yFontOffset, _width, 0, xCropOffset);

  // Draw the Caret (y = secondRow, height = inner_height, x = secondCol + ( TextLen (0, caretPos) - xOffset ) )
  if (GetFocus ())
      {
        // Drawn Damn Rect
        DrawRect (caretX, y + 1, 2, _height - 2, caret);
      }
}

int InputField::HandleMessage (gui_message * msg)
{
  if (!tElement)
    return false;

  int tempCaretPos = _caretPos;

  if ((msg->type == MESSAGE_KEYPRESSED) && (focus))
      {
        // Is the key a "normal" character, then append it
        char key = (char) msg->keypressed.key;

        if (key >= 32 && key <= 126)
            {
              char iText[2] = { 0, };

              iText[0] = key;

              _text.insert (_caretPos++, iText);
              setText ((char *) _text.c_str ());
              _caretPos = tempCaretPos + 1;
              recalcXCrop ();
              return true;
            }
        else if ((msg->keypressed.key == SDLK_LEFT) && !IgnoreCursorKeys)
            {
              if (_caretPos > 0)
                  {
                    _caretPos--;
                    recalcXCrop ();
                  }
              return true;
            }
        else if ((msg->keypressed.key == SDLK_RIGHT) && !IgnoreCursorKeys)
            {
              if (_caretPos < _text.length ())
                  {
                    _caretPos++;
                    recalcXCrop ();
                  }
              return true;
            }
        else if (key == SDLK_BACKSPACE)
            {
              // Delete the character before the caret
              if (_caretPos > 0)
                  {
                    _text.erase ((_text.begin () + (_caretPos - 1)));
                    setText ((char *) _text.c_str ());
                    _caretPos = tempCaretPos - 1;
                    recalcXCrop ();
                  }
              return true;
            }
        else if (key == SDLK_DELETE)
            {
              //Doesn't Work
            }

        if (callback_OnKeyPress)
            {
              callback_OnKeyPress (this, msg->keypressed.key);
              return true;
            }
      }

//  bool msgT = msg->type == MESSAGE_MOUSEDOWN;

  return Control::HandleMessage (msg);
}

bool InputField::MouseIsOver (int x, int y)
{
  return (x >= this->x && x <= this->x + this->_width && y >= this->y
          && y <= this->y + this->_height);
}

void InputField::
OnKeyPress (int (*callback) (Control * sender, unsigned short key))
{
  callback_OnKeyPress = callback;
}

void InputField::SetIgnoreCursorKeys (bool IgnoreCursorKeys)
{
  this->IgnoreCursorKeys = IgnoreCursorKeys;
}


void InputField::DoOnKeyPress (int key)
{
  if (callback_OnKeyPress)
    callback_OnKeyPress (this, key);
}
