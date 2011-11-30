/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZRegEx.h
 * Application :  IBK Open Class Library
 * Purpose     :  Regular expressions for compilers lacking the standard
 *                functions.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.06.04  First implemention                          P.Koch, IBK
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

#ifndef _ZREGEX_
#define _ZREGEX_



#ifdef ZC_IBM
  #pragma library("ZRegExI.lib")
#endif
#ifdef ZC_MICROSOFT
  //#pragma comment(lib,"ZRegExM.lib")
#endif
#ifdef ZC_BORLAND
  #pragma comment(lib,"ZRegExB.lib")
#endif
#ifdef ZC_GNU
  // must explicitly link with "ZRegExG.lib"
#endif

#include <ZBase.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __REG_SUBEXP_MAX 9         /* Maximum # of subexpressions */

/* regcomp() cflags */

#define REG_EXTENDED    0x0001     /* Use Extended RE syntax rules */
#define REG_ICASE       0x0002     /* Ignore case in match */
#define REG_NEWLINE     0x0004     /* Convert <backslash><n> to <newline> */
#define REG_NOSUB       0x0008     /* regexec() not report subexpressions */

/* regexec() eflags */

#define REG_NOTBOL      0x0100     /* First character not start of line */
#define REG_NOTEOL      0x0200     /* Last character not end of line */


/* Regular Expression error codes */

#define REG_NOMATCH        1       /* RE pattern not found */
#define REG_BADPAT         2       /* Invalid Regular Expression */
#define REG_ECOLLATE       3       /* Invalid collating element */
#define REG_ECTYPE         4       /* Invalid character class */
#define REG_EESCAPE        5       /* Last character is \ */
#define REG_ESUBREG        6       /* Invalid number in \digit */
#define REG_EBRACK         7       /* [] imbalance */
#define REG_EPAREN         8       /* \( \) or () imbalance */
#define REG_EBRACE         9       /* \{ \} or { } imbalance */
#define REG_BADBR         10       /* Invalid \{ \} range exp */
#define REG_ERANGE        11       /* Invalid range exp endpoint */
#define REG_ESPACE        12       /* Out of memory */
#define REG_BADRPT        13       /* ?*+ not preceded by valid RE */
#define REG_ECHAR         14       /* invalid multibyte character */
#define REG_EBOL          15       /* ^ anchor and not BOL */
#define REG_EEOL          16       /* $ anchor and not EOL */

typedef struct {                   /* regcomp() data saved for regexec() */
   unsigned int    re_nsub;           /* # of subexpressions in RE pattern */
   void*           re_comp;           /* compiled RE; freed by regfree() */
   int             re_cflags;         /* saved cflags for regexec() */
   unsigned int    re_erroff;         /* RE pattern error offset */
   unsigned int    re_len;            /* # wchar_t chars in compiled pattern */
   unsigned short  re_ucoll[2];       /* min/max unique collating values */
   void*           re_lsub[__REG_SUBEXP_MAX+1]; /* start subexp */
   void*           re_esub[__REG_SUBEXP_MAX+1]; /* end subexp */
   unsigned char   re_map[256];       /* map of valid pattern characters */
   short           re_shift;          /* Saved shift state */
   short           re_dbcs;           /* May start with DBCS character */
} regex_t;

typedef struct {                   /* substring locations - from regexec() */
   long            rm_so;             /* offset of substring */
   long            rm_eo;             /* offset of first char after substring */
   short           rm_ss;             /* Shift state at start of substring */
   short           rm_es;             /* Shift state at end of substring */
} regmatch_t;


/* Regular Expression function prototypes */

#define regcomp ZRegComp
ZImportAPI(int)
   regcomp(                        /* compile regular expression */
      regex_t *,                      /* address of regex_t structure */
      const char *,                   /* regular expression */
      int);                           /* cflags */

#define regexec ZRegExec
ZImportAPI(int)
   regexec(                        /*  compare text with compiler regex */
      const regex_t *,                /* address of regex_t structure */
      const char *,                   /* text to search */
      unsigned,                       /* max. # of subexpression matches */
      regmatch_t *,                   /* array of regmatch_t */
      int);                           /* eflags */

#define regerror ZRegError
ZImportAPI(unsigned)
   regerror(                       /* get regex error message */
      int,                            /* errorcode in question */
      const regex_t *,                /* address of regex_t structure */
      char *,                         /* buffer for message */
      unsigned);                      /* size of buffer for message */

#define regfree  ZRegFree
ZImportAPI(void)
   regfree(                        /* free memory allocated in regex_t */
      regex_t *);                     /* address of regex_t structure */

#ifdef __cplusplus
}
#endif
#endif // _REGEX_
