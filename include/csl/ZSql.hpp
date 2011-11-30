/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZSql.hpp
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

#ifndef _ZSQL_
#define _ZSQL_

#include <ZExcept.hpp>

#define ZDECLSQLNUMVAR(name, type, zero) \
class name : public ZBase\
{\
   public:\
      name() : iValue(zero), iNull(zTrue) {}\
\
      name(const name& aVar) : iValue(aVar.iValue), iNull(aVar.iNull) {}\
\
      name(type aValue, ZBoolean aNull = zFalse)\
         : iValue(aNull ? zero : aValue), iNull(aNull) {}\
\
      name& operator=(const name& aVar)\
         { iValue = aVar.iValue; iNull = aVar.iNull; return *this; }\
\
      operator type () const { return iValue; }\
\
      ZBoolean operator==(const name& aVar)\
         { return aVar.iValue==iValue && aVar.iNull==iNull; }\
\
      ZBoolean operator!=(const name& aVar)\
         { return aVar.iValue!=iValue || aVar.iNull!=iNull; }\
\
      ZBoolean isNull() const { return iNull; }\
\
      name& setNull(ZBoolean aNull = zTrue)\
         { iNull = aNull; if (iNull) iValue = zero; return *this; }\
\
   private:\
      type iValue;\
      ZBoolean iNull;\
};

ZDECLSQLNUMVAR(ZSqlInt8, signed char, 0)
ZDECLSQLNUMVAR(ZSqlInt16, short, 0)
ZDECLSQLNUMVAR(ZSqlInt32, long, 0)
ZDECLSQLNUMVAR(ZSqlFloat32, float, 0.0)
ZDECLSQLNUMVAR(ZSqlFloat64, double, 0.0)

class ZSqlChar : public ZBase
{
   public:
      ZSqlChar() : iNull(zTrue) {}

      ZSqlChar(const ZSqlChar& aVar) :
         iValue(aVar.iValue),
         iNull(aVar.iNull)
         {}

      ZSqlChar(const ZString& aValue, ZBoolean aNull = zFalse) :
         iValue(aValue),
         iNull(aNull)
      {
         if (iNull) iValue = "";
      }

      ZSqlChar(const char* aValue, ZBoolean aNull = zFalse) :
         iValue(aNull ? "" : aValue),
         iNull(aNull)
         {}

      ZSqlChar(const signed char* aValue, ZBoolean aNull = zFalse) :
         iValue(aNull ? "" : (const char*)aValue),
         iNull(aNull)
         {}

      ZSqlChar(const unsigned char* aValue, ZBoolean aNull = zFalse) :
         iValue(aNull ? "" : (const char*)aValue),
         iNull(aNull)
         {}

      operator const ZString& () const { return iValue; }

      operator ZString () { return iValue; }

      operator char* () { return iValue.buffer(); }

      operator const char* () const { return iValue.constBuffer(); }

      char* buffer() { return iValue.buffer(); }

      const char* constBuffer() const { return iValue.constBuffer(); }

      ZSqlChar& operator=(const ZSqlChar& aVar)
         { iValue = aVar.iValue; iNull = aVar.iNull; return *this; }

      ZBoolean operator==(const ZSqlChar& aVar) const
         { return aVar.iValue==iValue && aVar.iNull==iNull; }

      ZBoolean operator!=(const ZSqlChar& aVar) const
         { return aVar.iValue!=iValue || aVar.iNull!=iNull; }

      ZBoolean isNull() const { return iNull; }

      ZSqlChar& setNull(ZBoolean aNull = zTrue)
         { iNull = aNull; if (iNull) iValue = ""; return *this; }

   private:
      ZString iValue;
      ZBoolean iNull;
}; // ZSqlChar

#define ZSqlRaw ZSqlChar

class ZSqlCursor : public ZBase
{
   public:
      enum VarType {                     // variable types
         varTypeInt8,                       //  8 bit int   (char)
         varTypeInt16,                      // 16 bit int   (short int)
         varTypeInt32,                      // 32 bit int   (long int)
         varTypeFloat32,                    // 32 bit fp    (float)
         varTypeFloat64,                    // 64 bit fp    (double)
         varTypeChar,                       // xlated char  (char* or ZString)
         varTypeRaw                         // raw char     (char* or ZString)
      };

      virtual ZSqlCursor&
         parse(                          // parse statement
            const char *,                   // sql statement
            short int=1,                    // bind array size
            short int=1,                    // select array size
            long int=2000 ) = 0;            // max.select size for long types

      virtual ZSqlCursor&
         bind(                           // bind vars
            const char *,                   // placeholder name
            VarType=varTypeInt32,           // c++ variable type
            long int=0 ) = 0;               // max.size for Char & Raw

      virtual ZSqlCursor&
         define(                         // define vars
            const char *,                   // select column name
            VarType=varTypeInt32,           // c++ variable type
            long int=0 ) = 0;               // max.size for String & Raw

      virtual ZSqlCursor&
         execute() = 0;                  // execute dml statement

      virtual int
         fetch() = 0;                    // fetch next row

      virtual unsigned long
         rowsProcessed() = 0;            // get # of processed rows

      virtual int
         selectColumnCount() = 0;        // get # of select columns

      virtual char*
         selectColumnName(int) = 0;      // get name of select column

      virtual VarType
         selectColumnType(int) = 0;      // get type of select column

      virtual int
         selectColumnSize(int) = 0;      // get size of select column

      virtual int
         selectColumnScale(int) = 0;     // get scale of select column

      virtual char*
         selectColumnDescription(int)    // get description of select column
         = 0;

      // bind value operators

      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlInt8&);
      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlInt16&);
      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlInt32&);
      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlFloat32&);
      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlFloat64&);
      ZBaseLink(ZSqlCursor&) operator<<(const ZSqlChar&);

      ZBaseLink(ZSqlCursor&) operator<<(char);
      ZBaseLink(ZSqlCursor&) operator<<(signed char);
      ZBaseLink(ZSqlCursor&) operator<<(unsigned char);
      ZBaseLink(ZSqlCursor&) operator<<(short);
      ZBaseLink(ZSqlCursor&) operator<<(unsigned short);
      ZBaseLink(ZSqlCursor&) operator<<(long);
      ZBaseLink(ZSqlCursor&) operator<<(unsigned long);
      ZBaseLink(ZSqlCursor&) operator<<(int);
      ZBaseLink(ZSqlCursor&) operator<<(unsigned int);
      ZBaseLink(ZSqlCursor&) operator<<(float);
      ZBaseLink(ZSqlCursor&) operator<<(double);
      ZBaseLink(ZSqlCursor&) operator<<(char *);
      ZBaseLink(ZSqlCursor&) operator<<(signed char *);
      ZBaseLink(ZSqlCursor&) operator<<(unsigned char *);
      ZBaseLink(ZSqlCursor&) operator<<(const ZString&);

      // fetch value operators
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlInt8&);
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlInt16&);
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlInt32&);
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlFloat32&);
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlFloat64&);
      ZBaseLink(ZSqlCursor&) operator>>(ZSqlChar&);

      ZBaseLink(ZSqlCursor&) operator>>(char&);
      ZBaseLink(ZSqlCursor&) operator>>(signed char&);
      ZBaseLink(ZSqlCursor&) operator>>(unsigned char&);
      ZBaseLink(ZSqlCursor&) operator>>(short&);
      ZBaseLink(ZSqlCursor&) operator>>(unsigned short&);
      ZBaseLink(ZSqlCursor&) operator>>(long&);
      ZBaseLink(ZSqlCursor&) operator>>(unsigned long&);
      ZBaseLink(ZSqlCursor&) operator>>(int&);
      ZBaseLink(ZSqlCursor&) operator>>(unsigned int&);
      ZBaseLink(ZSqlCursor&) operator>>(float&);
      ZBaseLink(ZSqlCursor&) operator>>(double&);
      ZBaseLink(ZSqlCursor&) operator>>(char *);
      ZBaseLink(ZSqlCursor&) operator>>(signed char *);
      ZBaseLink(ZSqlCursor&) operator>>(unsigned char *);
      ZBaseLink(ZSqlCursor&) operator>>(ZString&);

   private:
      friend class ZSqlLink;

      virtual ZBaseLink(int) newParseRequired();

      virtual void bindValue(const ZSqlInt8&) = 0;
      virtual void bindValue(const ZSqlInt16&) = 0;
      virtual void bindValue(const ZSqlInt32&) = 0;
      virtual void bindValue(const ZSqlFloat32&) = 0;
      virtual void bindValue(const ZSqlFloat64&) = 0;
      virtual void bindValue(const ZSqlChar&) = 0;

      virtual void fetchValue(ZSqlInt8&) = 0;
      virtual void fetchValue(ZSqlInt16&) = 0;
      virtual void fetchValue(ZSqlInt32&) = 0;
      virtual void fetchValue(ZSqlFloat32&) = 0;
      virtual void fetchValue(ZSqlFloat64&) = 0;
      virtual void fetchValue(ZSqlChar&) = 0;

      ZSqlCursor  **iOwner;              // current owner
      short int   iPrio;                 // priority for recycling
      ZBoolean    iInv;                  // cursor invalidated
}; // ZSqlCursor

class ZSqlLink : public ZBase
{
   public:
      static ZBaseLink(ZSqlLink*)
         connect(                        // connect and create link
            const char* aDatabase,          // db name (ORACLE, DB2, ...)
            const char* aConnection = "",   // connection/datastore name
            const char* aUsername   = "",   // users name
            const char* aPassword   = "",   // users password
            int aMaxCursor          = 16);  // cursor pool size

      virtual ZBaseLink0 ~ZSqlLink();    // destructor (logoff)

      static ZBaseLink(ZString)
         literal(                        // prepare literal string
            const char* aString);           // string to prepare

      ZBaseLink(int)
         getCursor(                      // get cursor from pool
            ZSqlCursor** aCsr);             // address of csr ptr

      virtual ZBaseLink(ZSqlLink&)
         invCursor(                      // force reparse on next getCursor
            ZSqlCursor** aCsr);             // address of csr ptr

      virtual ZSqlLink&
         commit() = 0;                   // commit work

      virtual ZSqlLink&
         rollback() = 0;                 // rollback work

      virtual ZString
         dataBaseName() const = 0;       // tell database type

   protected:
      virtual ZSqlCursor* allocCursor() = 0; // allocate a cursor

      ZBaseLink0 ZSqlLink(int aMaxCursor); // constructor (logon)

      ZSqlCursor** iCsrPool;             // cursor pool array
      int iMaxCursor;                    // max. pool size
      int iCsrCount;                     // currently allocated cursors
}; // ZSqlLink

ZEXCLASSDECLARE(ZBaseLink, ZSqlError,ZException)

#endif // _ZSQL_
