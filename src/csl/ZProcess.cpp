/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZProcess.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Process and thread related operations
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.18  First implementation                        P.Koch, IBK
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

#include <ZProcess.hpp>
#include <ZTrace.hpp>

#ifdef ZC_WIN
#include <windows.h>
#endif

#ifdef ZC_OS2
#define INCL_DOSPROCESS
#include <os2.h>
#endif

#ifdef ZC_UNIXFAM
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#endif

// #include "../Fluid/mmgr.h"

ZExport (void) ZCurrentThread::sleep (unsigned long aMilliSecs)
{
  ZFUNCTRACE_DEVELOP ("ZCurrentThread::sleep(unsigned long aMilliSecs)");
#if ZC_WIN
  Sleep (aMilliSecs);
#elif ZC_OS2
  DosSleep (aMilliSecs);
#elif ZC_UNIXFAM
  struct timeval timeout;
  timeout.tv_sec = aMilliSecs / 1000;
  timeout.tv_usec = (aMilliSecs % 1000) * 1000;
  select (0, NULL, NULL, NULL, &timeout);
#else
  Not yet implemented.
#endif
}                               // sleep
