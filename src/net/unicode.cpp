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


//#include "glob.h"
//#include "stdafx.h"
#ifdef  WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include "net/unicode.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>




cUnicode::cUnicode (char *str, int len /* = 0 */ )
{
  memset (m_charBuf, 0, sizeof (m_charBuf));
  memset (m_unicodeBuf, 0, sizeof (m_unicodeBuf));

  if (!len)
    strcpy (m_charBuf, str);
  else
    strncpy (m_charBuf, str, len);

  CharToUnicode ();
}

cUnicode::cUnicode (NCHAR * str, int len /* = 0 */ )
{
  memset (m_charBuf, 0, sizeof (m_charBuf));
  memset (m_unicodeBuf, 0, sizeof (m_unicodeBuf));

  UnicodeCpy (str, len);

  UnicodeToChar ();
}

cUnicode::~cUnicode ()
{
}

void cUnicode::UnicodeCpy (NCHAR * str, int len /* = 0 */ )
{
  NCHAR *n = m_unicodeBuf;

  if (!len)
    while (*str)
      *(n++) = *(str++);
  else
    while (*str && len)
        {
          *(n++) = *(str++);
          len--;
        }

  *n = 0;
}

void cUnicode::CharToUnicode ()
{
  char *c = m_charBuf;
  NCHAR *u = m_unicodeBuf;

  m_charLen = 0;
  m_unicodeLen = 0;

  while (*c)
      {
        *(u++) = (Uint16) (*(c++));

        m_charLen++;
        m_unicodeLen += 2;
      }

  *u = 0;
  m_charLen++;
  m_unicodeLen += 2;
}

void cUnicode::UnicodeToChar ()
{
  char *c = m_charBuf;
  NCHAR *u = m_unicodeBuf;

  m_charLen = 0;
  m_unicodeLen = 0;

  while (*u)
      {
        *(c++) = (*(u++)) & 0xFF;

        m_charLen++;
        m_unicodeLen += 2;
      }

  *c = '\0';

  m_charLen++;
  m_unicodeLen += 2;
}
