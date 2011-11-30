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

#ifndef _ZPTRLIST_
#define _ZPTRLIST_

#include <ZBase.hpp>

class ZPointerlist : public ZBase
{
   public:
      // default constructor
      ZBaseLink0 ZPointerlist();

      // copy constructor
      ZBaseLink0 ZPointerlist(const ZPointerlist& aPointerlist);

      // destructor
      virtual ZBaseLink0 ~ZPointerlist();

      // assignment and concatination
      ZBaseLink(ZPointerlist&) operator=(const ZPointerlist& aPointerlist);

      ZBaseLink(ZPointerlist&) operator+=(const ZPointerlist& aPointerlist);

      // subscribing
      ZBaseLink(void*) operator[](long aIndex) const;

      // list manipulation
      ZBaseLink(ZPointerlist&) removeFromPos(long aIndex);

      ZBaseLink(ZPointerlist&) addAtPos(long aIndex, void* aPointer);

      ZPointerlist& addAsFirst(void* aPointer)
         { return addAtPos(0, aPointer); }

      ZPointerlist& addAsLast(void* aPointer)
         { return addAtPos(iCount, aPointer); }

      // clear list
      ZBaseLink(ZPointerlist&) drop();

      // query list size
      long count() const { return iCount; }

      // find an element
      ZBaseLink(long) find(const void* aPointer) const;

   protected:
      virtual void* copy(void* aPointer) const
         { return aPointer; }

      virtual void del(void* aPointer)
         {}

      virtual ZBoolean equal(const void* aPointer1, const void* aPointer2) const
         { return aPointer1 == aPointer2; }

   private:

      long     iCount;                   // # of elements
      long     iSize;                    // current array size
      void**   iList;                    // array of void*
}; // ZPointerlist

#define ZLISTCLASSDECLARE(name, element) \
\
class name : public ZBase \
{ \
   public: \
      name() {}\
\
      name(const element& aElement) \
         { iList.addAsLast(new element(aElement)); } \
\
      name& operator=(const name& aList) \
         { iList = aList.iList; return *this; } \
\
      name& operator+=(const name& aList) \
         { iList += aList.iList; return *this; } \
\
      name& operator+=(const element& aElement) \
         { iList.addAsLast(new element(aElement)); return *this; } \
\
      name& operator+=(element* aElement) \
         { iList.addAsLast(aElement); return *this; } \
\
      element& operator[](long aIndex) const \
         { return *(element*)iList[aIndex]; } \
\
      name& removeFromPos(long aIndex) \
         { iList.removeFromPos(aIndex); return *this; } \
\
      name& addAtPos(long aIndex, const element& aElement) \
         { iList.addAtPos(aIndex, new element(aElement)); return *this; } \
\
      name& addAtPos(long aIndex, element* aElement) \
         { iList.addAtPos(aIndex, aElement); return *this; } \
\
      name& addAsFirst(const element& aElement) \
         { iList.addAsFirst(new element(aElement)); return *this; } \
\
      name& addAsFirst(element* aElement) \
         { iList.addAsFirst(aElement); return *this; } \
\
      name& addAsLast(const element& aElement) \
         { iList.addAsLast(new element(aElement)); return *this; } \
\
      name& addAsLast(element* aElement) \
         { iList.addAsLast(aElement); return *this; } \
\
      name& drop() \
         { iList.drop(); return *this; } \
\
      long count() const \
         { return iList.count(); } \
\
      long find(const element& aElement) const \
         { return iList.find(&aElement); } \
\
   private: \
      class List : public ZPointerlist \
      { \
         private: \
            void* copy(void* aPointer) const \
               { return new element(*(element*)(aPointer)); } \
\
            void del(void* aPointer) \
               { delete (element*)aPointer; } \
\
            ZBoolean equal(const void* aPointer1, const void* aPointer2) const \
               { return *(element*)aPointer1 == *(element*)aPointer2 ; } \
      }; \
\
      List iList; \
};

#endif // _PTRLIST_
