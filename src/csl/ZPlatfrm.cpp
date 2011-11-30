/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZPlatfrm.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Information about the platform the program is running on.
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

#ifndef ZC_BUILDING_ZBASE
#define ZC_BUILDING_ZBASE
#endif

#include <ZPlatfrm.hpp>
#include <ZExcept.hpp>

#ifdef ZC_WIN
#include <windows.h>

#if ZC_MICROSOFT
#if _MSC_VER <= 1100
#define ZC_OSVERSIONINFOEX_MISSING
#endif
#endif

#if ZC_GNU
//      #define ZC_OSVERSIONINFOEX_MISSING
#endif

#ifdef ZC_OSVERSIONINFOEX_MISSING
typedef struct _OSVERSIONINFOEXA
{
  DWORD dwOSVersionInfoSize;
  DWORD dwMajorVersion;
  DWORD dwMinorVersion;
  DWORD dwBuildNumber;
  DWORD dwPlatformId;
  CHAR szCSDVersion[128];       // Maintenance string for PSS usage
  WORD wServicePackMajor;
  WORD wServicePackMinor;
  WORD wSuiteMask;
  BYTE wProductType;
  BYTE wReserved;
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;

typedef struct _OSVERSIONINFOEXW
{
  DWORD dwOSVersionInfoSize;
  DWORD dwMajorVersion;
  DWORD dwMinorVersion;
  DWORD dwBuildNumber;
  DWORD dwPlatformId;
  WCHAR szCSDVersion[128];      // Maintenance string for PSS usage
  WORD wServicePackMajor;
  WORD wServicePackMinor;
  WORD wSuiteMask;
  BYTE wProductType;
  BYTE wReserved;
} OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW,
  RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;

#ifdef UNICODE
typedef OSVERSIONINFOEXW OSVERSIONINFOEX;
typedef POSVERSIONINFOEXW POSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXW LPOSVERSIONINFOEX;
#else
typedef OSVERSIONINFOEXA OSVERSIONINFOEX;
typedef POSVERSIONINFOEXA POSVERSIONINFOEX;
typedef LPOSVERSIONINFOEXA LPOSVERSIONINFOEX;
#endif // UNICODE

#undef ZC_OSVERSIONINFOEX_MISSING
#endif // ZC_OSVERSIONINFOEX_MISSING

#ifndef VER_NT_SERVER
#define VER_NT_SERVER 0x0000003
#endif // VER_NT_SERVER

#endif // ZC_WIN

// #include "../Fluid/mmgr.h"

int ZPlatform::iNumProcessors = 0;

ZBoolean ZPlatform::iOS2 = zFalse;
ZBoolean ZPlatform::iLinux = zFalse;
ZBoolean ZPlatform::iFreeBSD = zFalse;
ZBoolean ZPlatform::iNetBSD = zFalse;
ZBoolean ZPlatform::iAIX = zFalse;
ZBoolean ZPlatform::iSolaris = zFalse;
ZBoolean ZPlatform::iUnix = zFalse;
ZBoolean ZPlatform::iWin95 = zFalse;
ZBoolean ZPlatform::iWin98 = zFalse;
ZBoolean ZPlatform::iWinME = zFalse;
ZBoolean ZPlatform::iWinNT3 = zFalse;
ZBoolean ZPlatform::iWinNT4 = zFalse;
ZBoolean ZPlatform::iWin2000 = zFalse;
ZBoolean ZPlatform::iWinXP = zFalse;
ZBoolean ZPlatform::iWinServer = zFalse;

void ZPlatform::init ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::init()");
  if (iNumProcessors == 0)
      {
#ifdef ZC_WIN
        SYSTEM_INFO sysInfo;
        GetSystemInfo (&sysInfo);
        iNumProcessors = sysInfo.dwNumberOfProcessors;

        OSVERSIONINFOEX osvi;
        BOOL bOsVersionInfoEx;

        // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
        // If that fails, try using the OSVERSIONINFO structure.
        ZeroMemory (&osvi, sizeof (OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOEX);

        bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) & osvi);
        if (!bOsVersionInfoEx)
            {
              // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
              osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
              if (!GetVersionEx ((OSVERSIONINFO *) & osvi))
                ZTHROWEXC ("GetVersionEx error " + ZString (GetLastError ()));
            }                   // if

        switch (osvi.dwPlatformId)
            {
            case VER_PLATFORM_WIN32_NT:
              // Test for the product.
              if (osvi.dwMajorVersion <= 3)
                iWinNT3 = zTrue;
              else if (osvi.dwMajorVersion == 4)
                iWinNT4 = zTrue;
              else if (osvi.dwMajorVersion == 5)
                  {
                    if (osvi.dwMinorVersion == 0)
                      iWin2000 = zTrue;
                    else
                      iWinXP = zTrue;
                  }             // if

              // Test for server
              if (bOsVersionInfoEx)
                  {
                    if (osvi.wProductType == VER_NT_SERVER)
                      iWinServer = zTrue;
                  }
              else
                  {
                    HKEY hKey;
                    char szProductType[80];
                    DWORD dwBufLen;

                    RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                                  "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
                                  0, KEY_QUERY_VALUE, &hKey);
                    RegQueryValueEx (hKey, "ProductType", NULL, NULL,
                                     (LPBYTE) szProductType, &dwBufLen);
                    RegCloseKey (hKey);
                    if (ZString (szProductType) == "SERVERNT")
                      iWinServer = zTrue;
                  }             // if
              break;

            case VER_PLATFORM_WIN32_WINDOWS:
              if (osvi.dwMajorVersion > 4)
                iWinME = zTrue; // or whatever will follow...
              else if (osvi.dwMajorVersion == 4)
                  {
                    if (osvi.dwMinorVersion >= 90)
                      iWinME = zTrue;
                    else if (osvi.dwMinorVersion >= 10)
                      iWin98 = zTrue;
                    else
                      iWin95 = zTrue;
                  }             // if
              break;

            default:;
            }                   // switch
#else
#if ZC_OS2
        iOS2 = zTrue;
#elif ZC_LINUX
        iLinux = zTrue;
#elif ZC_FREEBSD
        iFreeBSD = zTrue;
#elif ZC_NETBSD
        iNetBSD = zTrue;
#elif ZC_AIX
        iAIX = zTrue;
#elif ZC_SOLARIS
        iSolaris = zTrue;
#elif ZC_UNIX
        iUnix = zTrue;
#endif
        iNumProcessors = 1;
#endif
      }                         // if
}                               // init

ZExport (int) ZPlatform::numProcessors ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::numProcessors()");
  init ();
  return iNumProcessors;
}                               // numProcessors

ZExport (ZBoolean) ZPlatform::isOS2 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isOS2()");
  init ();
  return iOS2;
}                               // isOS2

ZExport (ZBoolean) ZPlatform::isLinux ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isLinux()");
  init ();
  return iLinux;
}                               // isLinux

ZExport (ZBoolean) ZPlatform::isBSD ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isBSD()");
  init ();
  return iFreeBSD || iNetBSD;
}                               // isBSD

ZExport (ZBoolean) ZPlatform::isFreeBSD ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isFreeBSD()");
  init ();
  return iFreeBSD;
}                               // isFreeBSD

ZExport (ZBoolean) ZPlatform::isNetBSD ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isNetBSD()");
  init ();
  return iNetBSD;
}                               // isNetBSD

ZExport (ZBoolean) ZPlatform::isAIX ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isAIX()");
  init ();
  return iAIX;
}                               // isAIX

ZExport (ZBoolean) ZPlatform::isSolaris ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isSolaris()");
  init ();
  return iSolaris;
}                               // isOS2

ZExport (ZBoolean) ZPlatform::isUnix ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isUnix()");
  init ();
  return iUnix;
}                               // isUnix

ZExport (ZBoolean) ZPlatform::isUnixFamily ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isUnixFamily()");
  init ();
  return iLinux || iFreeBSD || iNetBSD || iAIX || iSolaris || iUnix;
}                               // isUnixFamily

ZExport (ZBoolean) ZPlatform::isWin95 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWin95()");
  init ();
  return iWin95;
}                               // isWin95

ZExport (ZBoolean) ZPlatform::isWin98 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWin98()");
  init ();
  return iWin98;
}                               // isWin98

ZExport (ZBoolean) ZPlatform::isWinME ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinME()");
  init ();
  return iWinME;
}                               // isWinME

ZExport (ZBoolean) ZPlatform::isWinNT3 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinNT3()");
  init ();
  return iWinNT3;
}                               // isWinNT3

ZExport (ZBoolean) ZPlatform::isWinNT4 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinNT4()");
  init ();
  return iWinNT4;
}                               // isWinNT4

ZExport (ZBoolean) ZPlatform::isWin2000 ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWin2000()");
  init ();
  return iWin2000;
}                               // isWin2000

ZExport (ZBoolean) ZPlatform::isWinXP ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinXP()");
  init ();
  return iWinXP;
}                               // isWinXP

ZExport (ZBoolean) ZPlatform::isWinServer ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinServer()");
  init ();
  return iWinServer;
}                               // isWinServer

ZExport (ZBoolean) ZPlatform::isWin95Family ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWin95Family()");
  init ();
  return iWin95 || iWin98 || iWinME;
}                               // isWin95Family

ZExport (ZBoolean) ZPlatform::isWinNTFamily ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWinNTFamily()");
  init ();
  return iWinNT3 || iWinNT4 || iWin2000 || iWinXP;
}                               // isWinNTFamily

ZExport (ZBoolean) ZPlatform::isWindows ()
{
  ZFUNCTRACE_DEVELOP ("ZPlatform::isWindows()");
  init ();
  return iWin95 || iWin98 || iWinME || iWinNT3 || iWinNT4 || iWin2000
    || iWinXP;
}                               // isWindows
