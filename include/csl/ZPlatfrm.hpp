/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZPlatfrm.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  Information about the platform the program is running on.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.20  First implementation                        P.Koch, IBK
 * 2001.07.11  Add OS/2 and Unixish platforms              P.Koch, IBK
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

#ifndef _ZPLATFRM_
#define _ZPLATFRM_

#include <ZBase.hpp>

class ZPlatform : public ZBase {
   public:
      static ZBaseLink(ZBoolean) isOS2();
      static ZBaseLink(ZBoolean) isLinux();
      static ZBaseLink(ZBoolean) isBSD();
      static ZBaseLink(ZBoolean) isFreeBSD();
      static ZBaseLink(ZBoolean) isNetBSD();
      static ZBaseLink(ZBoolean) isAIX();
      static ZBaseLink(ZBoolean) isSolaris();
      static ZBaseLink(ZBoolean) isUnix();
      static ZBaseLink(ZBoolean) isUnixFamily();
      static ZBaseLink(ZBoolean) isWin95();
      static ZBaseLink(ZBoolean) isWin98();
      static ZBaseLink(ZBoolean) isWinME();
      static ZBaseLink(ZBoolean) isWinNT3();
      static ZBaseLink(ZBoolean) isWinNT4();
      static ZBaseLink(ZBoolean) isWin2000();
      static ZBaseLink(ZBoolean) isWinXP();
      static ZBaseLink(ZBoolean) isWinServer();
      static ZBaseLink(ZBoolean) isWin95Family();
      static ZBaseLink(ZBoolean) isWinNTFamily();
      static ZBaseLink(ZBoolean) isWindows();

      static ZBaseLink(int) numProcessors();

   private:
      ZPlatform();
      ZPlatform(const ZPlatform& aPlatform);
      ZPlatform& operator=(const ZPlatform&);
      static void init();

      static ZBoolean iOS2;
      static ZBoolean iLinux;
      static ZBoolean iFreeBSD;
      static ZBoolean iNetBSD;
      static ZBoolean iAIX;
      static ZBoolean iSolaris;
      static ZBoolean iUnix;
      static ZBoolean iWin95;
      static ZBoolean iWin98;
      static ZBoolean iWinME;
      static ZBoolean iWinNT3;
      static ZBoolean iWinNT4;
      static ZBoolean iWin2000;
      static ZBoolean iWinXP;
      static ZBoolean iWinServer;

      static int iNumProcessors;
};

#endif // _ZPLATFRM_
