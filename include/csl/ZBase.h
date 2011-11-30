/* Copyright (c) 1998-2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZBase.h
 * Application :  IBK Open Class Library
 * Purpose     :  ZBase definitions
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.20  First implementation                        P.Koch, IBK
 * 2001.10.27  Add FreeBSD and NetBSD                      P.Koch, IBK
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

#ifndef _ZBASEDEFS_
#define _ZBASEDEFS_

//// #include "../Fluid/mmgr.h"


#define ZC_MAJOR_VERSION 4
#define ZC_MINOR_VERSION 3

/* compiler determination */

#if defined(__IBMC__) || defined(__IBMCPP__)
   #define ZC_IBM 1          /* IBM VisualAge C++ */
   #define ZC_COMPILER "IBM"
#endif /* IBM */

#ifdef _MSC_VER
   #define ZC_MICROSOFT 1    /* Microsoft Visual C++ */
   #define ZC_COMPILER "MICROSOFT"
#endif /* MICROSOFT */

#ifdef __BORLANDC__
   #define ZC_BORLAND 1      /* Borland C++ */
   #define ZC_COMPILER "BORLAND"
#endif /* BORLAND */

#ifdef __GNUC__
   #define ZC_GNU 1          /* GNU gcc/g++ */
   #define ZC_COMPILER "GNU"
#endif /* GNU */

/* platform determination */

#if ZC_IBM
   #ifdef __OS2__
      #define ZC_OS2 1
      #define ZC_PATHSEPARATOR '\\'
   #endif
   #ifdef __WINDOWS__
      #define ZC_WIN 1
      #define ZC_PATHSEPARATOR '\\'
   #endif
#endif

#if ZC_MICROSOFT
   #define ZC_WIN 1
   #define ZC_PATHSEPARATOR '\\'
#endif

#if ZC_BORLAND
   #define ZC_WIN 1
   #define ZC_PATHSEPARATOR '\\'
#endif

#if ZC_GNU
   #if defined(__WIN32__) || defined(__CYGWIN32__)
      #define ZC_WIN 1
      #define ZC_PATHSEPARATOR '\\'
   #else
      #if defined(__linux__)
         #define ZC_LINUX 1
      #elif defined(__NetBSD__)
         #define ZC_NETBSD 1
         #define ZC_BSD 1
      #elif defined(__FreeBSD__)
         #define ZC_FREEBSD 1
         #define ZC_BSD 1
      #elif defined(sparc) && defined(sun) && defined(unix)
         #define ZC_SOLARIS 1
      #else
         #define ZC_UNIX 1
      #endif
      #define ZC_UNIXFAM 1
      #define ZC_PATHSEPARATOR '/'
      #define _MAX_PATH 2048
   #endif
#endif

/* import/export macros */

#if ZC_IBM
  #define ZImport0
  #define ZExport0 _Export
  #define ZImport(type) type
  #define ZExport(type) type _Export

  #if ZC_OS2
    #define ZFuncptrAPI * _System
    #define ZImportAPI(type) type _System
    #define ZExportAPI(type) type _System
  #endif /* ZC_OS2 */

  #ifdef ZC_WIN
    #define ZFuncptrAPI * __stdcall
    #define ZImportAPI(type) type __stdcall
    #define ZExportAPI(type) type __stdcall _Export
  #endif /* ZC_WIN */
#endif /* ZC_IBM */

#if ZC_MICROSOFT
  #define ZImport0
  #define ZExport0 
  #define ZImport(type) type
  #define ZExport(type) type
  #define ZFuncptrAPI *
  #define ZImportAPI(type) type 
  #define ZExportAPI(type) type
#endif /* ZC_MICROSOFT */

#if ZC_BORLAND
  #define ZImport0
  #define ZExport0 __declspec(dllexport)
  #define ZImport(type) type
  #define ZExport(type) __declspec(dllexport) type
  #define ZFuncptrAPI __stdcall *
  #define ZImportAPI(type) type __stdcall
  #define ZExportAPI(type) __declspec(dllexport) type __stdcall
#endif /* ZC_BORLAND */

#if ZC_GNU

  //#if ZC_UNIXFAM
    #define ZImport0
    #define ZExport0
    #define ZImport(type) type
    #define ZExport(type) type
    #define ZFuncptrAPI *
    #define ZImportAPI(type) type
    #define ZExportAPI(type) type
 // #endif /* ZC_UNIXFAM */
#endif /* ZC_GNU */

#endif /* _ZBASEDEFS_ */
