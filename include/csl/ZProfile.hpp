/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZProfile.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  Profile database management
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.31  First implementation                        P.Koch, IBK
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

#ifndef _ZPROFILE_
#define _ZPROFILE_

#include <ZStrlist.hpp>

class ZProfile : public ZBase
{
   public:
      // constructors / destructor
      ZBaseLink0 ZProfile(const ZString& aRoot, const ZString& aPath=ZString());
      ZBaseLink0 ZProfile(const ZProfile& aProfile);
      ZBaseLink0 ~ZProfile();

      // special profiles
      static ZBaseLink(ZProfile) systemProfile(const ZString& aPath=ZString());
      static ZBaseLink(ZProfile) userProfile(const ZString& aPath=ZString());

      // copy
      ZBaseLink(ZProfile) &operator=(const ZProfile& aProfile);

      ZString path() const { return iPath; }
      ZBaseLink(ZProfile&) setPath(const ZString& aPath);

      ZBaseLink(ZProfile&) getKeys(ZStringlist& aList);
      ZBaseLink(ZProfile&) getValues(ZStringlist& aList);

      // testing
      enum ValueType {
         Other   = 0,
         String  = 1,
         Binary  = 2,
         Integer = 3,
         Auto    = 4
      };

      ZBaseLink(int) valueType(const ZString& aValueName = ZString());
      ZBaseLink(ZBoolean) valueExists(const ZString& aValueName = ZString());
      ZBaseLink(ZBoolean) keyExists(const ZString& aKeyName = ZString());

      // reading and writing
      ZBaseLink(ZString) value(const ZString& aValueName = ZString());
      ZBaseLink(long) longValue(const ZString& aValueName = ZString());
      ZBaseLink(ZProfile&)
         setValue(const ZString& aData,
            const ZString& aValueKey = ZString(),
            int aType = Auto);
      ZBaseLink(ZProfile&) setValue(long aData, const ZString& aValueKey = ZString());
      ZBaseLink(ZProfile&) deleteValue(const ZString& aValueName = ZString());
      ZBaseLink(ZProfile&) deleteKey(const ZString& aKeyName);

   private:
#ifdef ZC_WIN
      void openPath(ZBoolean aCreate);
      void closePath();
      void openRoot(ZBoolean aCreate);
#endif
#ifdef ZC_OS2
      void openRoot();
      ZString getOs2Error() const;
#endif
      void closeRoot();

      ZString  iRoot;                    // root string
      ZString  iPath;                    // path string
#ifdef ZC_WIN
      void*    iRootHandle;              // root handle
      void*    iPathHandle;              // path handle
      ZBoolean iRootReadOnly;            // root is opened readonly
      ZBoolean iPathReadOnly;            // path is opened readonly
#endif
#ifdef ZC_OS2
      unsigned long iRootHandle;         // root handle
      unsigned long iHab;                // anchor handle
#endif
}; // ZProfile

#endif // _ZPROFILE_
