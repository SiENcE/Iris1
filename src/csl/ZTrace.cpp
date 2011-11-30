/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZTrace.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Program flow tracing
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.27  First implementation                        P.Koch, IBK
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

#include <ZTrace.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

// #include "../Fluid/mmgr.h"

using namespace std;

ZBoolean ZTrace::iInitialized = zFalse;
ZBoolean ZTrace::iPrintFile = zFalse;
ZBoolean ZTrace::iPrintLine = zFalse;

ZTrace::Destination ZTrace::iDestination = ZTrace::none;

long ZTrace::iFuncLevel = 0;

ZString ZTrace::iOutFile;

ZExport0 ZTrace::ZTrace(
   const ZString& aFile,
   long aLine,
   const ZString& aName) :
   iName(aName),
   iFile(aFile),
   iLine(aLine)
{
   write(iFile, iLine, "+"+iName);
   iFuncLevel++;
} // ZTrace

ZExport0 ZTrace::~ZTrace()
{
   if (iFuncLevel > 0) iFuncLevel--;
   write(iFile, iLine, "-"+iName);
} // ~Trace

ZExport(void) ZTrace::writeMsg(
   const ZString& aFile,
   long aLine,
   const ZString& aMessage)
{
   write(aFile, aLine, ZString(">")+aMessage);
} // writeMsg

void ZTrace::write(
   const ZString& aFile,
   long aLine,
   const ZString& aMessage)
{
   if (!iInitialized) {
      char* info = getenv("ZTRACE");
      if (info) {
         int mode(3);
         ZString dest(info);
         long pos = dest.indexOf(',');
         if (pos) {
            mode = dest.subString(pos+1).strip().asInt();
            dest = dest.subString(1,pos-1);
         } // if
         dest.strip();
         if (ZString::lowerCase(dest) == "stderr")
            iDestination = stdError;
         else
            if (ZString::lowerCase(dest) == "stdout")
               iDestination = stdOutput;
            else
               if (dest.size()) {
                  iDestination = toFile;
                  iOutFile = dest;
               } // if
         if (mode & 1) iPrintFile = zTrue;
         if (mode & 2) iPrintLine = zTrue;
      } // if
      iInitialized = zTrue;
   } // if

   ZString prefix;
   if (iPrintFile)
      prefix += aFile.subString(aFile.lastIndexOf(ZC_PATHSEPARATOR)+1);
   if (iPrintLine)
      prefix += "("+ZString(aLine)+")";
   if (prefix.size())
      prefix += ' ';

   ZString space;
   if (iFuncLevel > 0)
      space.leftJustify(iFuncLevel);

   ZString output(prefix+space+aMessage);

   switch (iDestination) {
      case stdError:
         cerr << output << endl;
         break;
      case stdOutput:
         cout << output << endl;
         break;
      case toFile: {
         ofstream trc(iOutFile.constBuffer(), ios::app);
         trc << output << endl;
         break;
      }
      default:;
   } // switch
} // write
