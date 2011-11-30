/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZFile.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  File handling utilities
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.06.18  First implementation                        P.Koch, IBK
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

#include <ZFile.hpp>
#include <ZTrace.hpp>
#include <stdlib.h>
#if ZC_UNIXFAM
#include <unistd.h>
#endif
#if ZC_WIN || ZC_OS2
#include <io.h>
#endif

// #include "../Fluid/mmgr.h"

ZExport (ZString) ZFile::stripExtension (const ZString & aFileName,
                                         ZString & aExtension)
{
  ZFUNCTRACE_DEVELOP
    ("stripExtension(const ZString& aFileName, const ZString& aExtension)");
  long pos = aFileName.lastIndexOf ('.');
  if (pos > aFileName.lastIndexOf (ZC_PATHSEPARATOR))
      {
        aExtension = aFileName.subString (pos);
        return aFileName.subString (1, pos - 1);
      }                         // if
  aExtension = "";
  return aFileName;
}                               // stripExtension

ZExport (ZString) ZFile::stripExtension (const ZString & aFileName)
{
  ZFUNCTRACE_DEVELOP ("stripExtension(const ZString& aFileName)");
  ZString ext;
  return stripExtension (aFileName, ext);
}                               // stripExtension

ZExport (ZString) ZFile::stripPath (const ZString & aFileName,
                                    ZString & aPath)
{
  ZFUNCTRACE_DEVELOP
    ("stripPath(const ZString& aFileName, const ZString& aPath)");
  long pos = aFileName.lastIndexOf (ZC_PATHSEPARATOR);
  if (pos)
      {
        aPath = aFileName.subString (1, pos);
        return aFileName.subString (pos + 1);
      }                         // if
  aPath = "";
  return aFileName;
}                               // stripPath

ZExport (ZString) ZFile::stripPath (const ZString & aFileName)
{
  ZFUNCTRACE_DEVELOP ("stripPath(const ZString& aFileName)");
  ZString path;
  return stripPath (aFileName, path);
}                               // stripPath

ZExport (ZString) ZFile::addExtension (const ZString & aFileName,
                                       const ZString & aExtension)
{
  ZFUNCTRACE_DEVELOP
    ("addExtension(const ZString& aFileName, const ZString& aExtension)");
  ZString ext;
  ZString file = stripExtension (aFileName, ext);
  if (ext.size ())
    return aFileName;           // has allready an extension
  return file + aExtension;
}                               // addExtension

ZExport (ZString) ZFile::fullPath (const ZString & aFileName)
{
  ZFUNCTRACE_DEVELOP ("fullPath(const ZString& aFileName)");
#if ZC_WIN || ZC_OS2
  char path[2048];
#if ZC_GNU
  _fullpath (path, aFileName.constBuffer (), sizeof (path));
#else
  ZString file (aFileName);
  _fullpath (path, file, sizeof (path));
#endif
  return ZString (path);
#endif
#if ZC_UNIXFAM
  long len (aFileName.length ());
  if (len >= 1 && aFileName[1] == ZC_PATHSEPARATOR)
    return aFileName;
  char cwd[2048];
  getcwd (cwd, sizeof (cwd));
  return ZString (cwd).stripTrailing (ZC_PATHSEPARATOR) + ZC_PATHSEPARATOR +
    aFileName;
#endif
}                               // fullPath

ZExport (ZString) ZFile::locateFile (const ZString & aFileName,
                                     const ZString & aEnvVar)
{
  ZFUNCTRACE_DEVELOP
    ("locateFile(const ZString& aFileName, const ZString& aEnvVar)");

  // try from current path
  if (access (aFileName, 0) == 0)
    return fullPath (aFileName);

  // don't try further if path enclosed
  if (aFileName.indexOf (ZC_PATHSEPARATOR))
    return ZString ();

  // try each path in colon separated env var
  const char *e = getenv (aEnvVar.constBuffer ());
  while (e && *e)
      {
        ZString buf;
#if ZC_WIN || ZC_OS2
        while (*e && *e != ';')
          buf += *e++;
#endif
#if ZC_UNIXFAM
        while (*e && *e != ':')
          buf += *e++;
#endif
        if (buf.size ())
            {
              if (buf[buf.size ()] != ZC_PATHSEPARATOR)
                buf += ZC_PATHSEPARATOR;
              buf += aFileName;
              if (access (buf.constBuffer (), 0) == 0)
                return fullPath (buf);
            }                   // if
        if (*e)
          e++;
      }                         // while
  return ZString ();
}                               // locateFile
