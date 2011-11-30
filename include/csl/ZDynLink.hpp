/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZDynLink.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  ZDynamicLinkLibrary implements access to DLL or shared
 *                libraries respectively.
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

#ifndef _ZDYNLINK_
#define _ZDYNLINK_

#include <ZString.hpp>

class ZDllModule;

class ZDynamicLinkLibrary : public ZBase
{
   public:
      // constructors/destructor
      ZBaseLink0 ZDynamicLinkLibrary(const char* aFileName);
      ZBaseLink0 ZDynamicLinkLibrary(const ZDynamicLinkLibrary& aDll);
      ZBaseLink0 ~ZDynamicLinkLibrary();

      // assignment
      ZBaseLink(ZDynamicLinkLibrary&) operator=(const ZDynamicLinkLibrary& aDll);

      // entry information
      ZBaseLink(void*) procAddress(const char* aProcedureName);
      ZBaseLink(void*) procAddress(unsigned long aProcedureOrdinal);
      ZBaseLink(ZBoolean) isEntryPoint32Bit(const char* aProcedureName);
      ZBaseLink(ZBoolean) isEntryPoint32Bit(unsigned long aProcedureOrdinal);

      // other queries
      ZBaseLink(ZString) name() const;
      ZBaseLink(ZString) fullName() const;

   private:
      void dropModule();

      ZDllModule* iModule;
}; // ZDynamicLinkLibrary

#endif // _ZDYNLINK_
