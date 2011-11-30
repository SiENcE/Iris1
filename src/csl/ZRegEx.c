/* Copyright (c) 2001-2002 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZRegEx.c
 * Application :  IBK Open Class Library
 * Purpose     :  Regular expressions for compilers lacking the standard
 *                functions.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.06.04  First implemention                          P.Koch, IBK
 * 2002.05.26  Release 4.4.0                               P.Koch, IBK
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

#include <ZBase.h>
#include <ZRegex.h>

ZExportAPI(int)
   ZRegComp(                       /* compile regular expression */
      regex_t* aRegExMem,             /* address of regex_t structure */
      const char* aRegEx,             /* regular expression */
      int aCflags)                    /* cflags */
{
   return regcomp(aRegExMem, aRegEx, aCflags);
} // ZRegComp

ZExportAPI(int)
   ZRegExec(                       /*  compare text with compiler regex */
      const regex_t* aRegExMem,       /* address of regex_t structure */
      const char* aText,              /* text to search */
      unsigned aMatchMax,             /* max. # of subexpression matches */
      regmatch_t* aMatches,           /* array of regmatch_t */
      int aEflags)                    /* eflags */
{
   return regexec(aRegExMem, aText, aMatchMax, aMatches, aEflags);
} // ZRegExec

ZExportAPI(unsigned)
   ZRegError(                      /* get regex error message */
      int aErrno,                     /* errorcode in question */
      const regex_t* aRegExMem,       /* address of regex_t structure */
      char* aMessage,                 /* buffer for message */
      unsigned aMessageSize)          /* size of buffer for message */
{
   return regerror(aErrno, aRegExMem, aMessage, aMessageSize);
} // ZRegError

ZExportAPI(void)
   ZRegFree(                       /* free memory allocated in regex_t */
      regex_t* aRegExMem)             /* address of regex_t structure */
{
   regfree(aRegExMem);
} // ZRegFree
