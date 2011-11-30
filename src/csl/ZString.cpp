/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZString.cpp
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

#ifndef ZC_BUILDING_ZBASE
#define ZC_BUILDING_ZBASE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <ZExcept.hpp>

// #include "../Fluid/mmgr.h"


using namespace std;
// uncomment next line if index check shall be ommitted
// #define ZSTRING_NO_INDEX_CHECK

// messages
static const char InvalidIndex[] = "Invalid string index";
static const char NullSearchString[] = "Search string must not be null";
static const char RefCount0[] = "String refcount decrementing below 0";

// other strings
static const char lowerChars[] = "äàáâãåçëèéêïìíîæöòóôøüùúûñÿý";
static const char upperChars[] = "ÄÀÁÂÃÅÇËÈÉÊÏÌÍÎÆÖÒÓÔØÜÙÚÛÑ¥Ý";

static char iNulString[] = "";

//////////////////////////////// ZStringBuffer ///////////////////////////////

ZExport0 ZStringBuffer::ZStringBuffer ():
iRefCount (1), iLogSize (0), iPhySize (0), iData (iNulString)
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::ZStringBuffer()");
}                               // ZStringBuffer

ZExport0 ZStringBuffer::ZStringBuffer (long aSize):iRefCount (1),
iLogSize (0), iPhySize (0), iData (iNulString)
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::ZStringBuffer(long aSize)");
  if (aSize)
    setSize (aSize);
}                               // ZStringBuffer

ZExport0 ZStringBuffer::ZStringBuffer (ZStringBuffer & aBuffer):iRefCount (1),
iLogSize (0), iPhySize (0), iData (0)
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::ZStringBuffer(ZStringBuffer& aBuffer)");
  setSize (aBuffer.iLogSize);
  memcpy (iData, (char *) aBuffer, aBuffer.iLogSize + 1);
}                               // ZStringBuffer

ZExport0 ZStringBuffer::~ZStringBuffer ()
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::~ZStringBuffer()");
  if (iPhySize)
    delete[]iData;
}                               // ~ZStringBuffer

ZExport (void) ZStringBuffer::removeRef ()
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::removeRef()");
  if (!iRefCount)
    ZTHROWEXC (RefCount0);
  iRefCount--;
}                               // removeRef

ZExport (void) ZStringBuffer::setSize (long aSize)
{
  ZFUNCTRACE_DEVELOP ("ZStringBuffer::setSize(long aSize)");
  if (!iData)
    iLogSize = iPhySize = 0;    // init new buffer
  if (aSize >= iPhySize)
      {
        // must allocate a new buffer
        long oldPhySize (iPhySize);
        iPhySize = (aSize + 16) & 0xfffffff0;   // round up to next block size
        char *newData = new char[iPhySize];     // allocate the new buffer
        if (iLogSize)
          memcpy (newData, iData, iLogSize);    // copy old contents
        if (iData && oldPhySize)
          delete[]iData;        // drop old buffer
        iData = newData;        // memorize new buffer location
      }                         // if
  iData[iLogSize = aSize] = 0;  // set terminating zero
}                               // setSize

////////////////////////////////// ZString ///////////////////////////////////

static ZStringBuffer *iNullBuffer = 0;

static ZStringBuffer *newStringBuffer (long aSize)
{
  if (aSize)
    return new ZStringBuffer (aSize);
  if (!iNullBuffer)
    iNullBuffer = new ZStringBuffer ();
  iNullBuffer->addRef ();
  return iNullBuffer;
}                               // newStringBuffer

#define privateBuffer() \
if (iBuffer->refs() > 1) { \
   iBuffer->removeRef(); \
   iBuffer = new ZStringBuffer(*iBuffer); \
}

ZExport0 ZString::ZString ():iBuffer (newStringBuffer (0))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString()");
}                               // ZString

ZExport0 ZString::ZString (const ZString & aString):iBuffer (aString.iBuffer)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(const ZString& aString)");
  iBuffer->addRef ();
}                               // ZStringBuffer

ZExport0 ZString::
ZString (const char *aText):iBuffer (newStringBuffer (strlen (aText)))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(const char* aText)");
  strcpy (*iBuffer, aText);
}                               // ZString

ZExport0 ZString::
ZString (const signed char
         *aText):iBuffer (newStringBuffer (strlen ((const char *) aText)))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(const signed char* aText)");
  strcpy (*iBuffer, (const char *) aText);
}                               // ZString

ZExport0 ZString::
ZString (const unsigned char
         *aText):iBuffer (newStringBuffer (strlen ((const char *) aText)))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(const unsigned char* aText)");
  strcpy (*iBuffer, (const char *) aText);
}                               // ZString

ZExport0 ZString::ZString (const void *aData,
                           long aSize):iBuffer (newStringBuffer (aSize))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(const void* aData, long aSize)");
  if (aData)
    memcpy (*iBuffer, aData, aSize);
}                               // ZString

ZExport0 ZString::ZString (char aChar):iBuffer (new ZStringBuffer (1))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(char aChar)");
  **iBuffer = aChar;
}                               // ZString

ZExport0 ZString::ZString (signed char aChar):iBuffer (new ZStringBuffer (1))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(signed char aChar)");
  **iBuffer = aChar;
}                               // ZString

ZExport0 ZString::
ZString (unsigned char aChar):iBuffer (new ZStringBuffer (1))
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(unsigned char aChar)");
  **iBuffer = aChar;
}                               // ZString

ZExport0 ZString::ZString (signed short aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(signed short aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%d", (signed) aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (unsigned short aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(unsigned short aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%u", (unsigned) aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (signed aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(signed aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%d", aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (unsigned aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(unsigned aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%u", aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (signed long aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(signed long aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%ld", aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (unsigned long aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(unsigned long aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%lu", aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::ZString (double aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::ZString(double aValue)");
  char buf[32];
  iBuffer = new ZStringBuffer (sprintf (buf, "%.*g", DBL_DIG, aValue));
  strcpy (*iBuffer, buf);
}                               // ZString

ZExport0 ZString::~ZString ()
{
  ZFUNCTRACE_DEVELOP ("ZString::~ZString()");
  iBuffer->removeRef ();
  if (iBuffer->refs () == 0)
    delete iBuffer;
}                               // ~ZString

ZExport0 ZString::operator  char *()
{
  ZFUNCTRACE_DEVELOP ("ZString::operator char*()");
  privateBuffer ();
  return *iBuffer;
}                               // operator char*

ZExport (char *) ZString::buffer ()
{
  ZFUNCTRACE_DEVELOP ("ZString::buffer()");
  privateBuffer ();
  return *iBuffer;
}                               // buffer

ZExport (char &) ZString::operator[](int aIndex)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](int aIndex)");
  privateBuffer ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (char &) ZString::operator[](unsigned aIndex)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](unsigned aIndex)");
  privateBuffer ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || (long) aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (char &) ZString::operator[](long aIndex)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](long aIndex)");
  privateBuffer ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (char &) ZString::operator[](unsigned long aIndex)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](unsigned long aIndex)");
  privateBuffer ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > (unsigned long) iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (const char &) ZString::operator[](int aIndex) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](int aIndex) const");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (const char &) ZString::operator[](unsigned aIndex) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](unsigned aIndex) const");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || (long) aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (const char &) ZString::operator[](long aIndex) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](long aIndex) const");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (const char &) ZString::operator[](unsigned long aIndex) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator[](unsigned long aIndex) const");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aIndex < 1 || aIndex > (unsigned long) iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  return (*iBuffer)[aIndex - 1];
}                               // operator[]

ZExport (int) ZString::asInt () const
{
  ZFUNCTRACE_DEVELOP ("ZString::asInt() const");
  return atoi (*iBuffer);
}                               // asInt

ZExport (long) ZString::asLong () const
{
  ZFUNCTRACE_DEVELOP ("ZString::asLong() const");
  return atol (*iBuffer);
}                               // asLong

ZExport (unsigned) ZString::asUnsigned () const
{
  ZFUNCTRACE_DEVELOP ("ZString::asUnsigned() const");
  return strtoul (*iBuffer, 0, 10);
}                               // asUnsigned

ZExport (unsigned long) ZString::asUnsignedLong () const
{
  ZFUNCTRACE_DEVELOP ("ZString::asUnsignedLong() const");
  return strtoul (*iBuffer, 0, 10);
}                               // asUnsignedLong

ZExport (double) ZString::asDouble () const
{
  ZFUNCTRACE_DEVELOP ("ZString::asDouble() const");
  return atof (*iBuffer);
}                               // asDouble

ZExport (long) ZString::indexOf (const ZString & aString, long aStartPos,
                                 ZBoolean aIgnoreCase) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::indexOf(const ZString& aString, long aStartPos, ZBoolean aIgnoreCase) const");
  if (aStartPos > iBuffer->size ())
    return 0;
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStartPos < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  long size (aString.iBuffer->size ());
  if (size == 0)
    ZTHROWEXC (NullSearchString);
  if (aIgnoreCase)
      {
        ZString str (ZString::upperCase (aString));
        for (long pos = aStartPos; pos + size - 1 <= iBuffer->size (); pos++)
          if (ZString::upperCase (subString (pos, size)) == str)
            return pos;
      }
  else
    for (long pos = aStartPos; pos + size - 1 <= iBuffer->size (); pos++)
      if (subString (pos, size) == aString)
        return pos;
  return 0;
}                               // indexOf

ZExport (long) ZString::indexOf (char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP ("ZString::indexOf(char aChar, long aStartPos) const");
  if (aStartPos > iBuffer->size ())
    return 0;
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStartPos < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  aStartPos--;
  char *pos =
    (char *) memchr (*iBuffer + aStartPos, aChar,
                     iBuffer->size () - aStartPos);
  if (pos)
    return (long) (pos - (char *) *iBuffer + 1);
  return 0;
}                               // indexOf

ZExport (long) ZString::indexOf (signed char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::indexOf(signed char aChar, long aStartPos) const");
  return indexOf ((char) aChar, aStartPos);
}                               // indexOf

ZExport (long) ZString::indexOf (unsigned char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::indexOf(unsigned char aChar, long aStartPos) const");
  return indexOf ((char) aChar, aStartPos);
}                               // indexOf

ZExport (long) ZString::lastIndexOf (const ZString & aString, long aStartPos,
                                     ZBoolean aIgnoreCase) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::lastIndexOf(const ZString& aString, long aStartPos, ZBoolean aIgnoreCase) const");
  if (!iBuffer->size ())
    return 0;
  if (aStartPos > iBuffer->size ())
    aStartPos = iBuffer->size ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStartPos < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  long size (aString.iBuffer->size ());
  if (size == 0)
    ZTHROWEXC (NullSearchString);
  if (aIgnoreCase)
      {
        ZString str (ZString::upperCase (aString));
        for (long pos = aStartPos - size + 1; pos >= 1; pos--)
          if (ZString::upperCase (subString (pos, size)) == str)
            return pos;
      }
  else
    for (long pos = aStartPos - size + 1; pos >= 1; pos--)
      if (subString (pos, size) == aString)
        return pos;
  return 0;
}                               // lastIndexOf

ZExport (long) ZString::lastIndexOf (char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::lastIndexOf(char aChar, long aStartPos) const");
  if (!iBuffer->size ())
    return 0;
  if (aStartPos > iBuffer->size ())
    aStartPos = iBuffer->size ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStartPos < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  const char *buf = *iBuffer;
  for (const char *pos = buf + aStartPos - 1; pos >= buf; pos--)
    if (*pos == aChar)
      return (long) (pos - buf) + 1;
  return 0;
}                               // lastIndexOf

ZExport (long) ZString::lastIndexOf (signed char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::lastIndexOf(signed char aChar, long aStartPos) const");
  return lastIndexOf ((char) aChar, aStartPos);
}                               // lastIndexOf

ZExport (long) ZString::lastIndexOf (unsigned char aChar, long aStartPos) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::lastIndexOf(unsigned char aChar, long aStartPos) const");
  return lastIndexOf ((char) aChar, aStartPos);
}                               // lastIndexOf

ZExport (long) ZString::occurrencesOf (char aChar) const
{
  ZFUNCTRACE_DEVELOP ("ZString::occurrencesOf(char aChar) const");
  long count (0);
  char *pos = *iBuffer;
  char *end = pos + iBuffer->size ();
  for (;;)
      {
        if (pos >= end)
          break;
        pos = (char *) memchr (pos, aChar, end - pos);
        if (pos)
            {
              count++;
              pos++;
            }
        else
          break;
      }                         // for
  return count;
}                               // occurrencesOf

ZExport (ZString) ZString::subString (long aStart, long aCount, char aPadChar) const
{
  ZFUNCTRACE_DEVELOP
    ("ZString::subString(long aStart, long aCount, char aPadChar) const");
  long size (iBuffer->size ());
  if (aStart > size)
    return ZString ();
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStart < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  aStart--;
  long avail (size - aStart);
  if (aCount < 0)
    aCount = avail;
  if (aStart == 0 && aCount == size)
    return ZString (*this);
  ZString str (*iBuffer + aStart, aCount);
  if (aCount > avail)
      {
        char *s = *str.iBuffer + avail;
        for (long cnt = aCount - avail; cnt--; *s++ = aPadChar);
      }                         // if
  return str;
}                               // subString

ZExport (ZBoolean) ZString::isPrintable () const
{
  ZFUNCTRACE_DEVELOP ("ZString::isPrintable() const");
  const char *s = *iBuffer;
  for (long i = iBuffer->size (); i > 0; i--)
      {
        if (*s >= 0 && *s < ' ')
          return zFalse;
        s++;
      }                         // for
  return zTrue;
}                               // isPrintable

static int bufcmp (const ZString & aString1, const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("bufcmp(const ZString& aString1, const ZString& aString2)");
  long s1 (aString1.size ());
  long s2 (aString2.size ());
  long s;
  if (s1 < s2)
    s = s1;
  else
    s = s2;
  int x (memcmp ((const char *) aString1, (const char *) aString2, s));
  if (x == 0)
    return s1 - s2;
  else
    return x;
}                               // bufcmp

ZExport (ZBoolean) operator== (const ZString & aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator==(const ZString& aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) == 0;
}                               // operator ==

ZExport (ZBoolean) operator== (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator==(const ZString& aString1, const char* aString2)");
  return strcmp (aString1, aString2) == 0;
}                               // operator ==

ZExport (ZBoolean) operator== (const char *&aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator==(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) == 0;
}                               // operator ==

ZExport (ZBoolean) operator!= (const ZString & aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator!=(const ZString &aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) != 0;
}                               // operator !=

ZExport (ZBoolean) operator!= (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator!=(const ZString &aString1, const char* aString2)");
  return strcmp (aString1, aString2) != 0;
}                               // operator !=

ZExport (ZBoolean) operator!= (const char *&aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator!=(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) != 0;
}                               // operator !=

ZExport (ZBoolean) operator< (const ZString & aString1,
                              const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<(const ZString &aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) < 0;
}                               // operator <

ZExport (ZBoolean) operator< (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<(const ZString &aString1, const char* aString2)");
  return strcmp (aString1, aString2) < 0;
}                               // operator <

ZExport (ZBoolean) operator< (const char *&aString1, const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) < 0;
}                               // operator <

ZExport (ZBoolean) operator<= (const ZString & aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<=(const ZString &aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) <= 0;
}                               // operator <=

ZExport (ZBoolean) operator<= (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<=(const ZString &aString1, const char* aString2)");
  return strcmp (aString1, aString2) <= 0;
}                               // operator <=

ZExport (ZBoolean) operator<= (const char *&aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator<=(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) <= 0;
}                               // operator <=

ZExport (ZBoolean) operator> (const ZString & aString1,
                              const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>(const ZString &aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) > 0;
}                               // operator >

ZExport (ZBoolean) operator> (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>(const ZString &aString1, const char* aString2)");
  return strcmp (aString1, aString2) > 0;
}                               // operator >

ZExport (ZBoolean) operator> (const char *&aString1, const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) > 0;
}                               // operator >

ZExport (ZBoolean) operator>= (const ZString & aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>=(const ZString &aString1, const ZString& aString2)");
  return bufcmp (aString1, aString2) >= 0;
}                               // operator >=

ZExport (ZBoolean) operator>= (const ZString & aString1, const char *aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>=(const ZString &aString1, const char* aString2)");
  return strcmp (aString1, aString2) >= 0;
}                               // operator >=

ZExport (ZBoolean) operator>= (const char *&aString1,
                               const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator>=(const char* &aString1, const ZString& aString2)");
  return strcmp (aString1, aString2) >= 0;
}                               // operator >=

ZExport (ZString &) ZString::operator= (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator=(const ZString& aString)");
  if (iBuffer != aString.iBuffer)
      {
        iBuffer->removeRef ();
        if (iBuffer->refs () == 0)
          delete iBuffer;
        iBuffer = aString.iBuffer;
        iBuffer->addRef ();
      }                         // if
  return *this;
}                               // operator =

ZExport (ZString &) ZString::operator= (const char *aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator=(const char* aString)");
  privateBuffer ();
  iBuffer->setSize (strlen (aString));
  strcpy (*iBuffer, aString);
  return *this;
}                               // operator =

ZExport (ZString) ZString::operator+ (const ZString & aString) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+(const ZString& aString) const");
  ZString str (*this);
  str += aString;
  return str;
}                               // operator +

ZExport (ZString) ZString::operator+ (const char *aString) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+(const char* aString) const");
  ZString str (*this);
  str += aString;
  return str;
}                               // operator +

ZExport (ZString) ZString::operator+ (char aChar) const
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+(char aChar) const");
  ZString str (*this);
  str += aChar;
  return str;
}                               // operator +

ZExport (ZString) operator+ (const char *aString1, const ZString & aString2)
{
  ZFUNCTRACE_DEVELOP
    ("operator+(const char* aString1, const ZString& aString2)");
  ZString str (aString1);
  str += aString2;
  return str;
}                               // operator +

ZExport (ZString) operator+ (char aChar, const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("operator+(char aChar, const ZString& aString)");
  ZString str (aChar);
  str += aString;
  return str;
}                               // operator +

ZExport (ZString &) ZString::operator+= (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+=(const ZString& aString)");
  if (aString.size () > 0)
      {
        long oldSize (size ());
        privateBuffer ();
        iBuffer->setSize (oldSize + aString.size ());
        memcpy (*iBuffer + oldSize, (const char *) aString, aString.size ());
      }                         // if
  return *this;
}                               // operator +=

ZExport (ZString &) ZString::operator+= (const char *aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+=(const char* aString)");
  int s (strlen (aString));
  if (s > 0)
      {
        long oldSize (size ());
        privateBuffer ();
        iBuffer->setSize (oldSize + s);
        memcpy (*iBuffer + oldSize, aString, s);
      }                         // if
  return *this;
}                               // operator +=

ZExport (ZString &) ZString::operator+= (char aChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::operator+=(char aChar)");
  privateBuffer ();
  iBuffer->setSize (size () + 1);
  (*iBuffer)[size () - 1] = aChar;
  return *this;
}                               // operator +=

ZExport (ZString &) ZString::lowerCase ()
{
  ZFUNCTRACE_DEVELOP ("ZString::lowerCase()");
  privateBuffer ();
  char *s = *iBuffer;
  for (long n = iBuffer->size (); n; n--, s++)
    if (*s >= 'A' && *s <= 'Z')
      *s += ('a' - 'A');
    else
        {
          const char *pos = strchr (upperChars, *s);
          if (pos)
            *s = lowerChars[pos - upperChars];
        }                       // if
  return *this;
}                               // lowerCase

ZExport (ZString &) ZString::upperCase ()
{
  ZFUNCTRACE_DEVELOP ("ZString::upperCase()");
  privateBuffer ();
  char *s = *iBuffer;
  for (long n = iBuffer->size (); n; n--, s++)
    if (*s >= 'a' && *s <= 'z')
      *s -= ('a' - 'A');
    else
        {
          const char *pos = strchr (lowerChars, *s);
          if (pos)
            *s = upperChars[pos - lowerChars];
        }                       // if
  return *this;
}                               // upperCase

ZExport (ZString) ZString::lowerCase (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::lowerCase(const ZString& aString)");
  ZString str (aString);
  return str.lowerCase ();
}                               // lowerCase

ZExport (ZString) ZString::upperCase (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::upperCase(const ZString& aString)");
  ZString str (aString);
  return str.upperCase ();
}                               // upperCase

ZExport (ZString &) ZString::center (long aLength, char aPadChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::center(long aLength, char aPadChar)");
  if (aLength > iBuffer->size ())
      {
        long fill = aLength - iBuffer->size ();
        long leading = fill / 2;
        long trailing = fill - leading;
        if (leading)
          rightJustify (iBuffer->size () + leading, aPadChar);
        if (trailing)
            {
              leftJustify (iBuffer->size () + trailing, aPadChar);
            }                   // if
      }                         // if
  return *this;
}                               // center

ZExport (ZString &) ZString::leftJustify (long aLength, char aPadChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::leftJustify(long aLength, char aPadChar)");
  if (aLength < iBuffer->size ())
    *this = subString (1, aLength);
  else if (aLength > iBuffer->size ())
      {
        privateBuffer ();
        long cnt (aLength - iBuffer->size ());
        iBuffer->setSize (aLength);
        char *s = *iBuffer + iBuffer->size () - 1;
        while (cnt--)
          *s-- = aPadChar;
      }                         // if
  return *this;
}                               // leftJustify

ZExport (ZString &) ZString::rightJustify (long aLength, char aPadChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::rightJustify(long aLength, char aPadChar)");
  if (aLength < iBuffer->size ())
    *this = subString (iBuffer->size () - aLength + 1);
  else if (aLength > iBuffer->size ())
      {
        long cnt (aLength - iBuffer->size ());
        ZString str (0, cnt);
        char *s = str;
        while (cnt--)
          *s++ = aPadChar;
        *this = str + *this;
      }                         // if
  return *this;
}                               // rightJustify

ZExport (ZString &) ZString::strip (char aChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::strip(char aChar)");
  stripTrailing (aChar);
  stripLeading (aChar);
  return *this;
}                               // strip

ZExport (ZString &) ZString::stripLeading (char aChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::stripLeading(char aChar)");
  long start (0);
  while (start < iBuffer->size () && (*iBuffer)[start] == aChar)
    start++;
  if (start < iBuffer->size ())
    *this = subString (start + 1);
  else
    *this = "";
  return *this;
}                               // stripLeading

ZExport (ZString &) ZString::stripTrailing (char aChar)
{
  ZFUNCTRACE_DEVELOP ("ZString::stripTrailing(char aChar)");
  long cnt (iBuffer->size ());
  while (cnt > 0 && (*iBuffer)[cnt - 1] == aChar)
    cnt--;
  if (cnt < iBuffer->size ())
      {
        privateBuffer ();
        iBuffer->setSize (cnt);
      }                         // if
  return *this;
}                               // stripTrailing

ZExport (ZString &) ZString::change (const ZString & aFrom,
                                     const ZString & aTo,
                                     long aStart,
                                     long aMaxChanges, ZBoolean aIgnoreCase)
{
  ZFUNCTRACE_DEVELOP
    ("ZString::change(const ZString&, const ZString&, long, long, ZBoolean)");
  if (!iBuffer->size ())
    return *this;
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aStart < 1 || aStart > iBuffer->size ())
    ZTHROWEXC (InvalidIndex);
#endif
  long pos (aStart);
  long fromSize (aFrom.iBuffer->size ());
  long toSize (aTo.iBuffer->size ());
  for (long cnt = 0; cnt < aMaxChanges; cnt++)
      {
        pos = indexOf (aFrom, pos, aIgnoreCase);
        if (!pos)
          break;
        *this = subString (1, pos - 1) + aTo + subString (pos + fromSize);
        pos += toSize;
      }                         // for
  return *this;
}                               // change

ZExport (long) ZString::wordCount () const
{
  ZFUNCTRACE_DEVELOP ("ZString::wordCount() const");
  const char *s = *iBuffer;
  long rest (iBuffer->size ());
  long cnt (0);
  while (rest)
      {
        while (rest && *s <= ' ')
            {
              s++;
              rest--;
            }
        if (rest)
            {
              cnt++;
              while (rest && *s > ' ')
                  {
                    s++;
                    rest--;
                  }
            }                   // if
      }                         // while
  return cnt;
}                               // wordCount

ZExport (ZString) ZString::words (long aFirst, long aCount) const
{
  ZFUNCTRACE_DEVELOP ("ZString::words(long aFirst, long aCount) const");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aFirst < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  const char *s = *iBuffer;
  long rest (iBuffer->size ());
  long nth (0);
  long cnt (0);
  ZString str;
  while (rest && cnt < aCount)
      {
        while (rest && *s <= ' ')
            {
              s++;
              rest--;
            }
        if (rest)
          if (++nth >= aFirst)
              {
                if (str.size ())
                  str += ' ';
                while (rest && *s > ' ')
                    {
                      str += *s++;
                      rest--;
                    }
                cnt++;
              }
          else
            while (rest && *s > ' ')
                {
                  s++;
                  rest--;
                }
      }                         // while
  return str;
}                               // words

ZExport (ZString &) ZString::removeWords (long aFirst, long aCount)
{
  ZFUNCTRACE_DEVELOP ("ZString::removeWords(long aFirst, long aCount)");
  *this = removeWords (*this, aFirst, aCount);
  return *this;
}                               // removeWords

ZExport (ZString) ZString::removeWords (const ZString & aString, long aFirst,
                                        long aCount)
{
  ZFUNCTRACE_DEVELOP
    ("ZString::removeWords(const ZString& aString, long aFirst, long aCount)");
#ifndef ZSTRING_NO_INDEX_CHECK
  if (aFirst < 1)
    ZTHROWEXC (InvalidIndex);
#endif
  const char *s = *aString.iBuffer;
  const char *s0 = s;
  const char *start = s;
  long rest (aString.iBuffer->size ());
  long nth (0);
  long cnt (0);
  ZString str;
  while (rest && cnt < aCount)
      {
        while (rest && *s <= ' ')
            {
              s++;
              rest--;
            }
        if (rest)
            {
              if (++nth == aFirst)
                start = s;
              while (rest && *s > ' ')
                  {
                    s++;
                    rest--;
                  }
              if (nth >= aFirst)
                if (++cnt == aCount)
                    {
                      while (rest && *s <= ' ')
                          {
                            s++;
                            rest--;
                          }
                      return aString.subString (1, start - s0)
                        + aString.subString (1 + s - s0);
                    }           // if
            }                   // if
      }                         // while
  if (cnt)
    return aString.subString (1, start - s0);
  return aString;
}                               // removeWords

ZExport (ZString &) ZString::imp ()
{
  ZFUNCTRACE_DEVELOP ("ZString::imp()");
  *this = imp (*this);
  return *this;
}                               // imp

ZExport (ZString &) ZString::exp ()
{
  ZFUNCTRACE_DEVELOP ("ZString::exp()");
  *this = exp (*this);
  return *this;
}                               // exp

static int digit (char ch)
{
  ZFUNCTRACE_DEVELOP ("digit(char ch)");
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'A' && ch <= 'Z')
    return ch - 'A' + 10;
  return ch - 'a' + 10;
}                               // digit

ZExport (ZString) ZString::imp (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::imp(const ZString& aString)");
  static const ZString hexDigits ("0123456789abcdefABCDEF");
  const char *s = aString;
  ZString str;
  while (*s)
      {
        if (*s == '\\')
            {
              s++;
              switch (*s)
                  {
                  case 0:
                    break;
                  case 'e':
                    str += '\33';
                    s++;
                    break;
                  case 't':
                    str += '\t';
                    s++;
                    break;
                  case 'r':
                    str += '\r';
                    s++;
                    break;
                  case 'n':
                    str += '\n';
                    s++;
                    break;
                  case 'f':
                    str += '\f';
                    s++;
                    break;
                  case 'x':
                    {
                      s++;
                      short x = 0, n = 0;
                      while (hexDigits.indexOf (*s) && n < 2)
                          {
                            x = x * 16 + digit (*s++);
                            n++;
                          }     // while
                      str += char (x);
                      break;
                    }
                  default:
                    if (*s >= '0' && *s <= '7')
                        {
                          short x = 0, n = 0;
                          while (*s >= '0' && *s <= '7' && n < 3)
                              {
                                x = x * 8 + digit (*s++);
                                n++;
                              } // while
                          str += char (x);
                        }
                    else
                        {
                          str += char (*s);
                          s++;
                        }       // if
                  }             // switch
            }
        else
            {
              str += char (*s);
              s++;
            }                   // if
      }                         // while
  return str;
}                               // imp

ZExport (ZString) ZString::exp (const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("ZString::exp(const ZString& aString)");
  const char *s = aString;
  long p (aString.length ());
  ZString str;
  while (p--)
      {
        switch (*s)
            {
            case '\\':
              str += "\\\\";
              break;
            case '\33':
              str += "\\e";
              break;
            case '\t':
              str += "\\t";
              break;
            case '\r':
              str += "\\r";
              break;
            case '\n':
              str += "\\n";
              break;
            case '\f':
              str += "\\f";
              break;
            case '\'':
              str += "\\'";
              break;
            case '"':
              str += "\\\"";
              break;
            default:
              if (*s < ' ')
                  {
                    char buf[6];
                    if (*(s + 1) >= '0' && *(s + 1) <= '7')
                      sprintf (buf, "\\%03o", *s);
                    else
                      sprintf (buf, "\\%o", *s);
                    str += buf;
                  }
              else
                str += *s;
            }                   // switch
        s++;
      }                         // while
  return str;
}                               // exp

ZExport (ZString) ZString::hex (char aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(char aValue)");
  char buf[4];
  sprintf (buf, "%02X", (int) aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (signed char aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(signed char aValue)");
  char buf[4];
  sprintf (buf, "%02X", (int) aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (unsigned char aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(unsigned char aValue)");
  char buf[4];
  sprintf (buf, "%02X", (int) aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (signed aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(signed aValue)");
  char buf[6];
  sprintf (buf, "%04X", aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (unsigned aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(unsigned aValue)");
  char buf[6];
  sprintf (buf, "%04X", aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (signed long aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(signed long aValue)");
  char buf[10];
  sprintf (buf, "%08lX", aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (unsigned long aValue)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(unsigned long aValue)");
  char buf[10];
  sprintf (buf, "%08lX", aValue);
  return buf;
}                               // hex

ZExport (ZString) ZString::hex (void *aPointer)
{
  ZFUNCTRACE_DEVELOP ("ZString::hex(void* aPointer)");
  char buf[10];
  sprintf (buf, "%08lX", (unsigned long) aPointer);
  return buf;
}                               // hex

ZExport (ostream &) operator<< (ostream & aStream, const ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("operator<<(ostream &aStream, const ZString& aString)");
  aStream << (const char *) aString;
  return aStream;
}                               // operator <<

static ZBoolean isspace (char aChar)
{
  ZFUNCTRACE_DEVELOP ("isspace(char aChar)");
  return aChar == ' ' || aChar == '\t' || aChar == '\r' || aChar == '\n';
}                               // isspace

ZExport (istream &) operator>> (istream & aStream, ZString & aString)
{
  ZFUNCTRACE_DEVELOP ("operator>>(istream &aStream, ZString &aString)");

  aString = "";                 // faster than ZString() because aString's buffer is reused

  char ch;

  // skip spaces
  for (;;)
      {
        aStream.get (ch);
        if (aStream.eof ())
          return aStream;
        if (!isspace (ch))
          break;
      }                         // for
  aString += ch;

  // anx max limits set by user?
  int limit (aStream.width (0));

  // read string until space, limit or eof
  for (int n = 1; n != limit; n++)
      {
        aStream.get (ch);
        if (aStream.eof () || isspace (ch))
          break;
        aString += ch;
      }                         // for

  return aStream;
}                               // operator >>

ZExport (ZString) ZString::lineFrom (istream & aStream, char aDelim)
{
  ZFUNCTRACE_DEVELOP ("ZString::lineFrom(istream &aStream, char aDelim)");
  ZString buf (0, 128);
  long bytes = 0;
  char c;
  for (aStream.get (c); c != aDelim && !aStream.fail (); aStream.get (c))
      {
        if (bytes == buf.size ())
          buf += ZString (0, bytes);
        (*buf.iBuffer)[bytes++] = c;
      }                         // for
  buf.iBuffer->setSize (bytes);
  return buf;
}                               // lineFrom
