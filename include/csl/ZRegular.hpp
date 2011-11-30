/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZRegular.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  Regular expression searching
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.30  First implementation                        P.Koch, IBK
 *
 * OPEN SOURCE LICENSE
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to IBK at info@ibk-software.ch.
 */

#ifndef _ZREGULAR_
#define _ZREGULAR_

#include <ZString.hpp>

class ZRegularExpression : public ZBase
{

   public:
      enum OpenFlags {
         openExtended   = 0x01,          // use extended syntax
         openIgnorecase = 0x02,          // ignore case in match
         openNewline    = 0x04,          // convert \n to newline
         openNosubreps  = 0x08           // ignore # of subexpressions
      }; // OpenFlags

      ZBaseLink0
         ZRegularExpression(             // constructor
            const ZString& aExpression,     // the regular expression
            int aOpenFlags = 0);            // open flags

      ZBaseLink0
         ~ZRegularExpression();          // destructor

      enum Constants {
         maxMatches     = 9              // max. subexpressions per match
      }; // Constants

      enum MatchFlags {
         matchNotBol    = 0x100,         // first char is not begin of line
         matchNotEol    = 0x200          // last char is not end of line
      }; // MatchFlags

      ZBaseLink(int)
         match(                          // match expression in a string
            const ZString& aString,         // string to search
            int aMaxMatches,                // # of matches to find
            long* aStartpos,                // startpos of each subexpression
            long* aLength,                  // length of each matching subexpr.
            int aMatchFlags = 0);           // matching flags for string

   private:
      void* iHandle;                     // handle for internal data
}; // ZRegularExpression

#endif // _ZREGULAR_
