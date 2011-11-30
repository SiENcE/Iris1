/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZSql.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Virtual base class for universal sql database interface
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.29  First implementation                        P.Koch, IBK
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

#include <ZTrace.hpp>
#include <ZDynLink.hpp>
#include <ZSql.hpp>
#include <string.h>

// #include "../Fluid/mmgr.h"

ZEXCLASSIMPLEMENT (ZSqlError, ZException);

class Dll:public ZBase
{
public:
  Dll (const ZString & aName, ZDynamicLinkLibrary * aDll,
       Dll * aPrev):iName (aName), iDll (aDll), iPrev (aPrev)
  {
  }                             // Dll

   ~Dll ()
  {
    delete iDll;
    if (iPrev)
      delete iPrev;
  }                             // ~Dll

  ZString iName;                // name of library
  ZDynamicLinkLibrary *iDll;    // library handle
  Dll *iPrev;                   // previous library
};                              // DLL

static Dll *dlls = 0;

///////////////////////////////// ZSqlLink /////////////////////////////////

ZExport (ZSqlLink *) ZSqlLink::connect (const char *aDatabase,    // db name (ORACLE, DB2, ...)
                                        const char *aConnection,  // connection/datastore name
                                        const char *aUsername,    // users name
                                        const char *aPassword,    // user password
                                        int aMaxCursor)     // cursor pool size
{
  ZFUNCTRACE_DEVELOP
    ("ZSqlLink::connect(const char*, const char*, const char*, const char*, int)");
#if ZC_WIN || ZC_OS2
#if ZC_IBM
  ZString dllName (ZString ("Z") + aDatabase + "I");
#endif
#if ZC_MICROSOFT
  ZString dllName (ZString ("Z") + aDatabase + "M");
#endif
#if ZC_BORLAND
  ZString dllName (ZString ("Z") + aDatabase + "B");
#endif
#if ZC_GNU
  ZString dllName (ZString ("Z") + aDatabase + "G");
#endif
  dllName.lowerCase ();
#endif // ZC_WIN || ZC_OS2
#if ZC_UNIXFAM
  ZString db (aDatabase);
  ZString dllName (ZString ("libZ")
                   + db.subString (1, 1).upperCase ()
                   + db.subString (2).lowerCase ()
                   + "G.so." + ZString (ZC_MAJOR_VERSION));
#endif // ZC_UNIXFAM

  // check if allready loaded
  Dll *d = dlls;
  while (d)
      {
        if (d->iName == dllName)
          break;
        d = d->iPrev;
      }                         // while

  if (!d)
      {
        ZDynamicLinkLibrary *dll =
          new ZDynamicLinkLibrary (dllName.constBuffer ());
        d = dlls = new Dll (dllName, dll, dlls);
      }                         // if
  ZSqlLink *(*connectTo) (const char *, const char *, const char *, int);
  connectTo = (ZSqlLink * (*)(const char *, const char *, const char *, int))
    d->iDll->procAddress (
#if ZC_WIN || ZC_OS2
#if ZC_IBM
                            "ZSqlConnectTo__FPCcN21i"
#endif
#if ZC_BORLAND
                            "@ZSqlConnectTo$qpxct1t1i"
#endif
#if ZC_MICROSOFT
                            "?ZSqlConnectTo@@YAPAVZSqlLink@@PBD00H@Z"
#endif
#if ZC_GNU
                            "ZSqlConnectTo__FPCcN21i"
#endif
#endif // ZC_WIN || ZC_OS2
#if ZC_UNIXFAM
                            "ZSqlConnectTo__FPCcN20i"
#endif // ZC_UNIXFAM
    );
  return (*connectTo) (aConnection, aUsername, aPassword, aMaxCursor);
}                               // connect

ZExport0 ZSqlLink::ZSqlLink (int aMaxCursor)
{
  ZFUNCTRACE_DEVELOP ("ZSqlLink::ZSqlLink(int aMaxCursor)");
  iMaxCursor = aMaxCursor;
  iCsrCount = 0;
  iCsrPool = new ZSqlCursor *[aMaxCursor];
}                               // ZSqlLink

ZExport0 ZSqlLink::~ZSqlLink ()
{
  ZFUNCTRACE_DEVELOP ("ZSqlLink::~ZSqlLink()");
  for (int i = 0; i < iCsrCount; i++)
    delete iCsrPool[i];
  delete[]iCsrPool;
}                               // ~ZSqlLink

ZExport (ZString) ZSqlLink::literal (const char *s)
{
  ZFUNCTRACE_DEVELOP ("ZSqlLink::literal(const char *s)");
  ZString str ("'");
  while (*s)
      {
        if (*s == '\'')
          str += *s;
        str += *s++;
      }                         // while
  return str + "'";
}                               // literal

ZExport (int) ZSqlLink::getCursor (ZSqlCursor ** csr)
{
  ZFUNCTRACE_DEVELOP ("ZSqlLink::getCursor(ZSqlCursor **csr)");

  int ret, prio, curr;
  int old = -1;
  ZSqlCursor *c;

  // search for active cursor
  for (curr = 0; curr < iCsrCount; curr++)
      {
        c = iCsrPool[curr];
        if (csr == c->iOwner && *csr == c)
            {
              ret = c->newParseRequired () || c->iInv;
              goto gotcursor;
            }                   // if
        if (!c->iPrio)
          old = curr;           // remember least busy cursor
      }                         // for

  // check if new cursor may be opened
  if (iCsrCount < iMaxCursor)
      {
        iCsrPool[iCsrCount] = allocCursor ();
        curr = iCsrCount;
        iCsrPool[curr]->iPrio = iCsrCount++;
      }
  else
      {
        if (old < 0)
          ZTHROWEXC ("Internal error in ZSql.cpp");
        curr = old;
        iCsrPool[curr]->newParseRequired ();    // for DB2 cleanup!
      }                         // if

  // save owner signature and return SqlCursor ptr
  ret = 1;
  c = *csr = iCsrPool[curr];
  c->iOwner = csr;

gotcursor:
  c->iInv = zFalse;
  // mark this cursor most busy
  if (c->iPrio < iCsrCount - 1)
      {
        prio = c->iPrio;
        c->iPrio = iCsrCount;
        for (curr = 0; curr < iCsrCount; curr++)
            {
              c = iCsrPool[curr];
              if (c->iPrio >= prio)
                c->iPrio--;
            }                   // for
      }                         // if
  return ret;
}                               // getCursor

ZExport (ZSqlLink &) ZSqlLink::invCursor (ZSqlCursor ** csr)
{
  ZFUNCTRACE_DEVELOP ("ZSqlLink::invCursor(ZSqlCursor **csr)");
  for (int curr = 0; curr < iCsrCount; curr++)
      {
        ZSqlCursor *c = iCsrPool[curr];
        if (csr == c->iOwner && *csr == c)
            {
              c->iInv = zTrue;
              break;
            }                   // if
      }                         // for
  return *this;
}                               // ZSqlLink::invCursor

///////////////////////////////// ZSqlCursor /////////////////////////////////

ZExport (int) ZSqlCursor::newParseRequired ()
{
  ZFUNCTRACE_DEVELOP ("ZSqlCursor::newParseRequired()");
  return 0;
}                               // newParseRequired

#define DEFSHIFTLEFTNUM(VarType) \
ZExport(ZSqlCursor&) ZSqlCursor::operator<<(VarType val)\
{\
    ZFUNCTRACE_DEVELOP("ZSqlCursor::operator<<(VarType val)");\
    bindValue(val);\
    return *this;\
}
DEFSHIFTLEFTNUM (const ZSqlInt8 &)
DEFSHIFTLEFTNUM (const ZSqlInt16 &)
DEFSHIFTLEFTNUM (const ZSqlInt32 &)
DEFSHIFTLEFTNUM (const ZSqlFloat32 &)
DEFSHIFTLEFTNUM (const ZSqlFloat64 &) DEFSHIFTLEFTNUM (const ZSqlChar &)
#define DEFSHIFTLEFTBASIC(VarType,DestType) \
ZExport(ZSqlCursor&) ZSqlCursor::operator<<(VarType val)\
{\
    ZFUNCTRACE_DEVELOP("ZSqlCursor::operator<<(VarType val)");\
    bindValue(DestType(val));\
    return *this;\
}
DEFSHIFTLEFTBASIC (char, ZSqlInt8)
DEFSHIFTLEFTBASIC (signed char, ZSqlInt8)
DEFSHIFTLEFTBASIC (unsigned char, ZSqlInt8)
DEFSHIFTLEFTBASIC (short, ZSqlInt16)
DEFSHIFTLEFTBASIC (unsigned short, ZSqlInt16)
DEFSHIFTLEFTBASIC (long, ZSqlInt32)
DEFSHIFTLEFTBASIC (unsigned long, ZSqlInt32)
DEFSHIFTLEFTBASIC (int, ZSqlInt32)
DEFSHIFTLEFTBASIC (unsigned int, ZSqlInt32)
DEFSHIFTLEFTBASIC (float, ZSqlFloat32)
DEFSHIFTLEFTBASIC (double, ZSqlFloat64)
DEFSHIFTLEFTBASIC (char *, ZSqlChar)
DEFSHIFTLEFTBASIC (signed char *, ZSqlChar)
DEFSHIFTLEFTBASIC (unsigned char *, ZSqlChar)
//DEFSHIFTLEFTBASIC(const ZString&,ZSqlChar)
ZExport (ZSqlCursor &) ZSqlCursor::operator<< (const ZString & val)
{
  ZFUNCTRACE_DEVELOP ("ZSqlCursor::operator<<(const ZString& val)");
  bindValue (ZSqlChar ((const char *) val));
  return *this;
}                               // operator<<


#define DEFSHIFTRIGHTSQL(VarType) \
ZExport(ZSqlCursor&) ZSqlCursor::operator>>(VarType& val)\
{\
    ZFUNCTRACE_DEVELOP("ZSqlCursor::operator>>(VarType& val)");\
    fetchValue(val);\
    return *this;\
}
DEFSHIFTRIGHTSQL (ZSqlInt8)
DEFSHIFTRIGHTSQL (ZSqlInt16)
DEFSHIFTRIGHTSQL (ZSqlInt32)
DEFSHIFTRIGHTSQL (ZSqlFloat32)
DEFSHIFTRIGHTSQL (ZSqlFloat64) DEFSHIFTRIGHTSQL (ZSqlChar)
#define DEFSHIFTRIGHTNUM(VarType,DestType) \
ZExport(ZSqlCursor&) ZSqlCursor::operator>>(VarType& val)\
{\
    ZFUNCTRACE_DEVELOP("ZSqlCursor::operator>>(VarType& val)");\
    DestType dst;\
    fetchValue(dst);\
    val=dst;\
    return *this;\
}
DEFSHIFTRIGHTNUM (char, ZSqlInt8)
DEFSHIFTRIGHTNUM (signed char, ZSqlInt8)
DEFSHIFTRIGHTNUM (unsigned char, ZSqlInt8)
DEFSHIFTRIGHTNUM (short, ZSqlInt16)
DEFSHIFTRIGHTNUM (unsigned short, ZSqlInt16)
DEFSHIFTRIGHTNUM (long, ZSqlInt32)
DEFSHIFTRIGHTNUM (unsigned long, ZSqlInt32)
DEFSHIFTRIGHTNUM (int, ZSqlInt32)
DEFSHIFTRIGHTNUM (unsigned int, ZSqlInt32)
DEFSHIFTRIGHTNUM (float, ZSqlFloat32) DEFSHIFTRIGHTNUM (double, ZSqlFloat64)
#define DEFSHIFTRIGHTCHAR(VarType) \
ZExport(ZSqlCursor&) ZSqlCursor::operator>>(VarType *val)\
{\
    ZFUNCTRACE_DEVELOP("ZSqlCursor::operator>>(VarType *val)");\
    ZSqlChar str;\
    fetchValue(str);\
    strcpy((char*)val,str.constBuffer());\
    return *this;\
}
DEFSHIFTRIGHTCHAR (char)
DEFSHIFTRIGHTCHAR (signed char)
DEFSHIFTRIGHTCHAR (unsigned char)
ZExport (ZSqlCursor &) ZSqlCursor::operator>> (ZString & val)
{
  ZFUNCTRACE_DEVELOP ("ZSqlCursor::operator>>(ZString& val)");
  ZSqlChar str;
  fetchValue (str);
  val = ZString ((char *) str);
  return *this;
}                               // ZSqlCursor& operator>>(ZString&)
