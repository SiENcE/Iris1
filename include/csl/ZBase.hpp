/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZBase.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  ZBase is the base class all other classes are derived from.
 *                It implements the elementary class Boolean.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.20  First implementation                        P.Koch, IBK
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

#ifndef _ZBASE_
#define _ZBASE_

#include <ZBase.h>

#ifndef ZC_BUILDING_ZBASE
  #ifdef ZC_IBM
    #pragma library("ZBaseI.lib")
  #endif
  #ifdef ZC_MICROSOFT
    #pragma comment(lib,"ZBaseM.lib")
  #endif
  #ifdef ZC_BORLAND
    #pragma comment(lib,"ZBaseB.lib")
  #endif
  #define ZBaseLink0 ZImport0
  #define ZBaseLink(type) ZImport(type)
#else
  #define ZBaseLink0 ZExport0
  #define ZBaseLink(type) ZExport(type)
#endif

typedef int ZBoolean;

enum ZBooleanConstants { zFalse = 0, zTrue = 1 };

class ZBase
{
   public:
      // declare other constants
      enum Constants {
         maxLong = 0x7fffffff,
         maxUlong = 0xffffffff
      }; // Constants

}; // ZBase

#endif // _ZBASE_
