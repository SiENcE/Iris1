/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZDate.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  Class with date and time (timestamp).
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

#ifndef _ZDATE_
#define _ZDATE_

#include <ZString.hpp>

class ZDateTime : public ZBase
{
   public:

      // default constructor
      ZBaseLink0 ZDateTime();

      // create from known date
      ZBaseLink0
         ZDateTime(
            int aYear,
            int aMonth  = 1,
            int aDay    = 1,
            int aHour   = 0,
            int aMinute = 0,
            int aSecond = 0);

      // copy constructor
      ZBaseLink0 ZDateTime(const ZDateTime& aDateTime);

      // destructor
      ZBaseLink0 ~ZDateTime();

      // element set
      ZBaseLink(ZDateTime&) setYear(int aYear);
      ZBaseLink(ZDateTime&) setMonth(int aMonth);
      ZBaseLink(ZDateTime&) setDay(int aDay);
      ZBaseLink(ZDateTime&) setHour(int aHour);
      ZBaseLink(ZDateTime&) setMinute(int aMinute);
      ZBaseLink(ZDateTime&) setSecond(int aSecond);

      // element queries
      int year() const { return iYear; }
      int month() const { return iMonth; }
      int day() const { return iDay; }
      int hour() const { return iHour; }
      int minute() const { return iMinute; }
      int second() const { return iSecond; }

      // other queries
      enum Weekdays {
         monday    = 1,
         tuesday   = 2,
         wednesday = 3,
         thursday  = 4,
         friday    = 5,
         saturday  = 6,
         sunday    = 7
      }; // Weekdays

      ZBaseLink(int) dayOfWeek() const;             // day of the week (1 = monday)
      ZBaseLink(int) dayOfYear() const;             // day of the year (newyearsday = 1)

      // format models
      enum Formats {
         iso  = 0,
         euro = 1,
         us   = 2
      }; // Formats

      static ZBaseLink(int) format();               // query default format
      static ZBaseLink(void) setFormat(int aFormat);// set default format

      // formatted date
      ZBaseLink(ZString) asDate() const;            // default format date
      ZBaseLink(ZString) asIsoDate() const;         // ISO date yyyy.mm.dd
      ZBaseLink(ZString) asEuroDate() const;        // european date dd.mm.yyyy
      ZBaseLink(ZString) asUsDate() const;          // us date mm/dd/yyyy

      // formatted time
      ZBaseLink(ZString) asTime() const;            // default format time
      ZBaseLink(ZString) asIsoTime() const;         // ISO time hh.mm.ss
      ZBaseLink(ZString) asEuroTime() const;        // european time hh:mm:ss
      ZBaseLink(ZString) asUsTime() const;          // us time hh.mm.ss

      // date math
      ZBaseLink(long) operator-(const ZDateTime& aDateTime) const; // diff secs
      ZBaseLink(ZDateTime) operator-(long aSeconds) const;
      ZBaseLink(ZDateTime) operator+(long aSeconds) const;
      ZBaseLink(ZDateTime&) operator-=(long aSeconds);
      ZBaseLink(ZDateTime&) operator+=(long aSeconds);
      static long minuteSeconds(long aMinutes = 1) { return aMinutes*60; }
      static long hourSeconds(long aHours = 1) { return aHours*3600; }
      static long daySeconds(long aDays = 1) { return aDays*86400; }

   private:
      long asSeconds() const;
      void fromSeconds(long aSeconds);

      static int iFormat;                // default format

      unsigned short iYear;              // year
      unsigned char iMonth;              // month  1 - 12
      unsigned char iDay;                // day    1 - 31
      unsigned char iHour;               // hour   0 - 23
      unsigned char iMinute;             // minute 0 - 59
      unsigned char iSecond;             // second 0 - 59
}; // ZDateTime

#endif // _ZDATE_
