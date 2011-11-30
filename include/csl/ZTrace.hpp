/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZTrace.hpp
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

#ifndef _ZTRACE_
#define _ZTRACE_

#include <ZString.hpp>

class ZTrace : public ZBase
{
   public:
      ZBaseLink0
         ZTrace(
            const ZString& aFile,
            long aLine,
            const ZString& aName);

      ZBaseLink0 ~ZTrace();

      static ZBaseLink(void)
         writeMsg(
            const ZString& aFile,
            long aLine,
            const ZString& aMessage);

   private:
      enum Destination {
         none,
         stdError,
         stdOutput,
         toFile
      };

      static void
         write(
            const ZString& aFile,
            long aLine,
            const ZString& aMessage);

      static ZBoolean iInitialized;
      static ZBoolean iPrintFile;
      static ZBoolean iPrintLine;
      static Destination iDestination;
      static long iFuncLevel;
      static ZString iOutFile;

      ZString iName;
      ZString iFile;
      long iLine;
}; // ZTrace

#ifdef ZC_TRACE_ALL
   #define ZMODTRACE_ALL(module)   ZTrace trc_x_3_y_5(__FILE__,__LINE__,module)
   #define ZTRACE_ALL(message)     ZTrace::writeMsg(__FILE__,__LINE__,message)
   #ifndef ZC_TRACE_DEVELOP
     #define ZC_TRACE_DEVELOP
   #endif
#else
   #define ZMODTRACE_ALL(modname)
   #define ZTRACE_ALL(message)
#endif

#ifdef ZC_TRACE_DEVELOP
   #define ZMODTRACE_DEVELOP(module) ZTrace trc_x_4_y_6(__FILE__,__LINE__,module)
   #define ZTRACE_DEVELOP(message)   ZTrace::writeMsg(__FILE__,__LINE__,message)
   #ifndef ZC_TRACE_RUNTIME
     #define ZC_TRACE_RUNTIME
   #endif
#else
   #define ZMODTRACE_DEVELOP(module)
   #define ZTRACE_DEVELOP(message)
#endif

#ifdef ZC_TRACE_RUNTIME
   #define ZMODTRACE_RUNTIME(module) ZTrace trc_x_5_y_7(__FILE__,__LINE__,module)
   #define ZTRACE_RUNTIME(message)   ZTrace::writeMsg(__FILE__,__LINE__,message)
#else
   #define ZMODTRACE_RUNTIME(module)
   #define ZTRACE_RUNTIME(message)
#endif

#ifdef __FUNCTION__
  #define ZFUNCTRACE_ALL(function) ZMODTRACE_ALL(__FUNCTION__)
  #define ZFUNCTRACE_DEVELOP(function) ZMODTRACE_DEVELOP(__FUNCTION__)
  #define ZFUNCTRACE_RUNTIME(function) ZMODTRACE_RUNTIME(__FUNCTION__)
#else
  #define ZFUNCTRACE_ALL(function) ZMODTRACE_ALL(function)
  #define ZFUNCTRACE_DEVELOP(function) ZMODTRACE_DEVELOP(function)
  #define ZFUNCTRACE_RUNTIME(function) ZMODTRACE_RUNTIME(function)
#endif


#endif // _ZTRACE_
