/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZString.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  ZString is the string type. Besides of text it is also
 *                capable of handling binary data.
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

#ifndef _ZSTRING_
#define _ZSTRING_

#include <iostream>
#include <fstream>
#include <ZBase.hpp>
#include "Common.h"

class ZStringBuffer : public ZBase
{
   public:
      // default constructor
      ZBaseLink0 ZStringBuffer();

      // constructor with size specification
      ZBaseLink0 ZStringBuffer(long aSize);

      // copy constructor
      ZBaseLink0 ZStringBuffer(ZStringBuffer& aBuffer);

      // destructor
      ZBaseLink0 ~ZStringBuffer();

      // add a reference
      void addRef() { iRefCount++; }

      // remove a reference
      ZBaseLink(void) removeRef();

      // get # of references
      long refs() const { return iRefCount; }

      // get buffer logical size
      long size() const { return iLogSize; }

      // rezize buffer
      ZBaseLink(void) setSize(long aSize);

      // data buffer access
      operator char*() const { return iData; }
      char* data() const { return iData; }

   private:
      long iRefCount;           // # of references
      long iLogSize;            // logical buffer size
      long iPhySize;            // physical buffer size
      char* iData;              // data buffer
}; // ZStringBuffer


class ZString : public ZBase
{
   public:
      // default constructor
      ZBaseLink0 ZString();

      // copy constructor
      ZBaseLink0 ZString(const ZString& aString);

      // constructor with given text
      ZBaseLink0 ZString(const char *aText);
      ZBaseLink0 ZString(const signed char *aText);
      ZBaseLink0 ZString(const unsigned char *aText);

      // constructor with given binary data
      ZBaseLink0 ZString(const void *aData, long aSize);

      // constructor with given char
      ZBaseLink0 ZString(char aChar);
      ZBaseLink0 ZString(signed char aChar);
      ZBaseLink0 ZString(unsigned char aChar);

      // construct from integer value
      ZBaseLink0 ZString(signed short aValue);
      ZBaseLink0 ZString(unsigned short aValue);
      ZBaseLink0 ZString(signed aValue);
      ZBaseLink0 ZString(unsigned aValue);
      ZBaseLink0 ZString(signed long aValue);
      ZBaseLink0 ZString(unsigned long aValue);

      // construct from floating value
      ZBaseLink0 ZString(double aValue);

      // destructor
      ZBaseLink0 ~ZString();

      // get size
      long size() const { return iBuffer->size(); }
      long length() const { return iBuffer->size(); }

      // data buffer access
      ZBaseLink0 operator char*();
      operator const char*() const { return *iBuffer; };
      ZBaseLink(char*) buffer();
      const char* constBuffer() const { return *iBuffer; };

      ZBaseLink(char&) operator[](int aIndex);
      ZBaseLink(char&) operator[](unsigned aIndex);
      ZBaseLink(char&) operator[](long aIndex);
      ZBaseLink(char&) operator[](unsigned long aIndex);

      ZBaseLink(const char&) operator[](int aIndex) const;
      ZBaseLink(const char&) operator[](unsigned aIndex) const;
      ZBaseLink(const char&) operator[](long aIndex) const;
      ZBaseLink(const char&) operator[](unsigned long aIndex) const;

      // numeric queries
      ZBaseLink(int) asInt() const;
      ZBaseLink(long) asLong() const;
      ZBaseLink(unsigned) asUnsigned() const;
      ZBaseLink(unsigned long) asUnsignedLong() const;
      ZBaseLink(double) asDouble() const;

      // other queries & testing
      ZBaseLink(long)
         indexOf(
            const ZString& aString,
            long aStartPos = 1,
            ZBoolean aIgnoreCase = zFalse) const;

      ZBaseLink(long)
         indexOf(
            char aChar,
            long aStartPos = 1) const;

      ZBaseLink(long)
         indexOf(
            signed char aChar,
            long aStartPos = 1) const;

      ZBaseLink(long)
         indexOf(
            unsigned char aChar,
            long aStartPos = 1) const;

      ZBaseLink(long)
         lastIndexOf(
            const ZString& aString,
            long aStartPos = maxLong,
            ZBoolean aIgnoreCase = zFalse) const;

      ZBaseLink(long)
         lastIndexOf(
            char aChar,
            long aStartPos = maxLong) const;

      ZBaseLink(long)
         lastIndexOf(
            signed char aChar,
            long aStartPos = maxLong) const;

      ZBaseLink(long)
         lastIndexOf(
            unsigned char aChar,
            long aStartPos = maxLong) const;

      ZBaseLink(long)
         occurrencesOf(
            char aChar) const;

      ZBaseLink(ZBoolean)
         isPrintable() const;

      ZBaseLink(ZString)
         subString(
            long aStart,
            long aCount = -1,
            char aPadChar = ' ') const;

      // compares
      friend ZBaseLink(ZBoolean)
         operator == (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator == (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator == (
            const char* &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator != (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator != (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator != (
            const char* &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator <  (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator <  (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator <  (
            const char* &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator <= (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator <= (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator <= (
            const char* &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator >  (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator >  (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator >  (
            const char* &aString1,
            const ZString &aString2);

      friend ZBaseLink(ZBoolean)
         operator >= (
            const ZString &aString1,
            const ZString& aString2);

      friend ZBaseLink(ZBoolean)
         operator >= (
            const ZString &aString1,
            const char* aString2);

      friend ZBaseLink(ZBoolean)
         operator >= (
            const char* &aString1,
            const ZString& aString2);

      // assignment and concatination
      ZBaseLink(ZString&) operator = (const ZString& aString);
      ZBaseLink(ZString&) operator = (const char* aString);

      ZBaseLink(ZString&) operator += (const ZString& aString);
      ZBaseLink(ZString&) operator += (const char* aString);
      ZBaseLink(ZString&) operator += (char aChar);

      ZBaseLink(ZString) operator + (const ZString& aString) const;
      ZBaseLink(ZString) operator + (const char* aString) const;
      ZBaseLink(ZString) operator + (char aChar) const;

      friend ZBaseLink(ZString)
         operator + (
            const char* aString1,
            const ZString& aString2);

      friend ZBaseLink(ZString)
         operator + (
            char aChar,
            const ZString& aString2);

      // manipulations
      ZBaseLink(ZString&) lowerCase();
      ZBaseLink(ZString&) upperCase();

      static ZBaseLink(ZString)
         lowerCase(
            const ZString& aString);

      static ZBaseLink(ZString)
         upperCase(
            const ZString& aString);

      ZBaseLink(ZString&)
         center(
            long aLength,
            char aPadChar = ' ');

      ZBaseLink(ZString&)
         leftJustify(
            long aLength,
            char aPadChar = ' ');

      ZBaseLink(ZString&)
         rightJustify(
            long aLength,
            char aPadChar = ' ');

      ZBaseLink(ZString&)
         strip(
            char aChar = ' ');

      ZBaseLink(ZString&)
         stripLeading(
            char aChar = ' ');

      ZBaseLink(ZString&)
         stripTrailing(
            char aChar = ' ');

      ZBaseLink(ZString&)
         change(
            const ZString &aFrom,
            const ZString &aTo,
            long aStart = 1,
            long aMaxChanges = maxLong,
            ZBoolean aIgnoreCase = zFalse);

      // word operations
      ZBaseLink(long) wordCount() const;

      ZString word(long aIndex) const
      {
         return words(aIndex,1);
      } // word

      ZBaseLink(ZString)
         words(
            long aFirst,
            long aCount = maxLong) const;

      static ZBaseLink(ZString)
         removeWords(
            const ZString& aString,
            long aFirst,
            long aCount = maxLong);

      ZBaseLink(ZString&)
         removeWords(
            long aFirst,
            long aCount = maxLong);

      // conversions
      ZBaseLink(ZString&) imp();
      ZBaseLink(ZString&) exp();
      static ZBaseLink(ZString) imp(const ZString& aString);
      static ZBaseLink(ZString) exp(const ZString& aString);

      static ZBaseLink(ZString) hex(char aValue);
      static ZBaseLink(ZString) hex(signed char aValue);
      static ZBaseLink(ZString) hex(unsigned char aValue);
      static ZBaseLink(ZString) hex(signed aValue);
      static ZBaseLink(ZString) hex(unsigned aValue);
      static ZBaseLink(ZString) hex(signed long aValue);
      static ZBaseLink(ZString) hex(unsigned long aValue);
      static ZBaseLink(ZString) hex(void* aPointer);

      // stream i/o
      friend ZBaseLink(std::ostream&) operator << (std::ostream &aStream, const ZString& aString);
      friend ZBaseLink(std::istream&) operator >> (std::istream &aStream, ZString &aString);
      static ZBaseLink(ZString) lineFrom(std::istream &aStream, char aDelim = '\n');

   private:
      ZStringBuffer *iBuffer;            // string buffer
}; // ZString

#endif // _ZSTRING_
