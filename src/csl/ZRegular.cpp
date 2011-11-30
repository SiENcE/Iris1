/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZRegular.cpp
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

#ifndef ZC_BUILDING_ZBASE
#define ZC_BUILDING_ZBASE
#endif

#include <ZRegular.hpp>
#include <ZExcept.hpp>

#if ZC_IBM
#include <regex.h>
#elif ZC_UNIXFAM
#include <sys/types.h>
#include <regex.h>
#define __REG_SUBEXP_MAX 9
#else
#include <ZRegEx.h>
#endif

// #include "../Fluid/mmgr.h"

ZExport0 ZRegularExpression::ZRegularExpression (const ZString & aExpression,
                                                 int aOpenFlags)
{
  ZFUNCTRACE_DEVELOP
    ("ZRegularExpression::ZRegularExpression(const ZString& aExpression, int aOpenFlags)");
  iHandle = new regex_t;
  int cflags (0);
  if (aOpenFlags & openExtended)
    cflags |= REG_EXTENDED;
  if (aOpenFlags & openIgnorecase)
    cflags |= REG_ICASE;
  if (aOpenFlags & openNewline)
    cflags |= REG_NEWLINE;
  if (aOpenFlags & openNosubreps)
    cflags |= REG_NOSUB;
  int err = regcomp ((regex_t *) iHandle, aExpression.constBuffer (), cflags);
  if (err)
      {
        char msg[80];
        regerror (err, (regex_t *) iHandle, msg, sizeof (msg));
        regfree ((regex_t *) iHandle);
        delete (regex_t *) iHandle;
        iHandle = 0;
        ZTHROWEXC (msg);
      }                         // if
}                               // ZRegularExpression

ZExport0 ZRegularExpression::~ZRegularExpression ()
{
  ZFUNCTRACE_DEVELOP ("ZRegularExpression::~ZRegularExpression()");
  if (iHandle)
      {
        regfree ((regex_t *) iHandle);
        delete (regex_t *) iHandle;
      }                         // if
}                               // ~ZRegularExpression

ZExport (int) ZRegularExpression::match (const ZString & aString,
                                         int aMaxMatches,
                                         long *aStartpos,
                                         long *aLength, int aMatchFlags)
{
  ZFUNCTRACE_DEVELOP
    ("ZRegularExpression::match(const ZString&, int, long*, long*, int)");
  if (!iHandle)
    ZTHROWEXC ("Invalid regular expression!");

  // check nmatch
  if (aMaxMatches < 0)
    ZTHROWEXC ("match count >= 0");
  if (aMaxMatches > __REG_SUBEXP_MAX)
    ZTHROWEXC ("match count must be <= " + ZString (__REG_SUBEXP_MAX));
  regmatch_t rm[__REG_SUBEXP_MAX];

  int eflags (0);
  if (aMatchFlags & matchNotBol)
    eflags |= REG_NOTBOL;
  if (aMatchFlags & matchNotEol)
    eflags |= REG_NOTEOL;

  // do the matching
  int err = regexec ((regex_t *) iHandle, aString, aMaxMatches, rm, eflags);

  if (err == REG_NOMATCH)
    return 0;

  if (err)
      {
        char msg[80];
        regerror (err, (regex_t *) iHandle, msg, sizeof (msg));
        ZTHROWEXC (msg);
      }                         // if

  // get the results
  for (int x = 0; x < aMaxMatches; x++)
      {
        if (rm[x].rm_so < 0)
          return x;
        aStartpos[x] = rm[x].rm_so + 1;
        aLength[x] = rm[x].rm_eo - rm[x].rm_so;
      }                         // for
  return aMaxMatches;
}                               // match
