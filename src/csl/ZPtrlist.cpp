/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZPtrlist.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  Pointer list class as generic base class for any kind
 *                of list.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.06.12  First implementation                        P.Koch, IBK
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

#include <ZExcept.hpp>

// #include "../Fluid/mmgr.h"

ZExport0 ZPointerlist::ZPointerlist ():iCount (0), iSize (0), iList (0)
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::ZPointerlist()");
}                               // ZPointerlist

ZExport0 ZPointerlist::
ZPointerlist (const ZPointerlist & aPointerlist):iCount (0), iSize (0),
iList (0)
{
  ZFUNCTRACE_DEVELOP
    ("ZPointerlist::ZPointerlist(const ZPointerlist& aPointerist)");
  *this = aPointerlist;
}                               // ZPointerlist

ZExport0 ZPointerlist::~ZPointerlist ()
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::~ZPointerlist()");
  drop ();
  if (iList)
    delete[]iList;
}                               // ~ZPointerlist

ZExport (ZPointerlist &) ZPointerlist::operator= (const ZPointerlist & aList)
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::operator=(const ZPointerlist& aList)");
  drop ();
  *this += aList;
  return *this;
}                               // operator=

ZExport (ZPointerlist &) ZPointerlist::operator+= (const ZPointerlist & aList)
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::operator+=(const ZPointerlist& aList)");
  for (long i = 0; i < aList.count (); i++)
    addAsLast (copy (aList[i]));
  return *this;
}                               // operator+=

ZExport (void *) ZPointerlist::operator[](long aIndex) const
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::operator[](long aIndex) const");
  ZTRACE_DEVELOP ("aIndex " + ZString (aIndex));
  if (aIndex >= iCount || aIndex < 0)
    ZTHROWEXC ("Invalid index");
  return iList[aIndex];
}                               // operator[]

ZExport (ZPointerlist &) ZPointerlist::removeFromPos (long aIndex)
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::removeFromPos(long aIndex)");

  if (aIndex >= iCount || aIndex < 0)
    ZTHROWEXC ("Invalid index");
  del (iList[aIndex]);
  for (long i = aIndex + 1; i < iCount; i++)
    iList[i - 1] = iList[i];
  iCount--;
  return *this;
}                               // ZPointerlist::removeFromPos

ZExport (ZPointerlist &) ZPointerlist::addAtPos (long aIndex, void *aPointer)
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::addAtPos(long aIndex, void* aPointer");

  if (aIndex > iCount || aIndex < 0)
    ZTHROWEXC ("Invalid index");

  if (iCount == iSize)
      {
        iSize = iSize ? iSize + iSize : 10;
        ZTRACE_DEVELOP ("iSize " + ZString (iSize));
        void **newList = new void *[iSize];
        long i = 0;
        while (i < aIndex)
            {
              newList[i] = iList[i];
              i++;
            }                   // while
        newList[i++] = aPointer;
        while (i <= iCount)
            {
              newList[i] = iList[i - 1];
              i++;
            }                   // while
        if (iList)
          delete[]iList;
        iList = newList;
      }
  else
      {
        for (long i = iCount; i > aIndex; i--)
          iList[i] = iList[i - 1];
        iList[aIndex] = aPointer;
      }                         // if
  iCount++;
  return *this;
}                               // addAtPos

ZExport (ZPointerlist &) ZPointerlist::drop ()
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::drop()");
  for (long i = 0; i < iCount; i++)
    del (iList[i]);
  iCount = 0;
  return *this;
}                               // ZPointerlist::drop

ZExport (long) ZPointerlist::find (const void *aPointer) const
{
  ZFUNCTRACE_DEVELOP ("ZPointerlist::find(const void* aPointer) const");
  for (long i = 0; i < iCount; i++)
    if (equal (aPointer, iList[i]))
      return i;
  return -1;
}                               // ZPointerlist::find
