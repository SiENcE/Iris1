/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZAsync.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Asynchronous serial I/O support
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

#ifndef _ZASYNC_
#define _ZASYNC_

#include <ZExcept.hpp>
#include <ZString.hpp>
#include <fstream>

//// #include "../Fluid/mmgr.h"


#define         A_NUL           0           // Nul code
#define         A_SOH           1           // Start of header
#define         A_STX           2           // Start of text
#define         A_ETX           3           // End of text
#define         A_EOT           4           // End of transmission
#define         A_ENQ           5           // Enquire
#define         A_ACK           6           // Acknowledge
#define         A_BEL           7           // Bell
#define         A_BS            8           // Backspace
#define         A_HT            9           // Horizontal tabulator
#define         A_LF            10          // Line feed
#define         A_VT            11          // Vertical tabulator
#define         A_FF            12          // Form feed
#define         A_CR            13          // Carriage return
#define         A_SO            14          // Shift out
#define         A_SI            15          // Shift in
#define         A_DLE           16          // Data link escape
#define         A_DC1           17          // Device control 1 (xon)
#define         A_DC2           18          // Device control 2
#define         A_DC3           19          // Device control 3 (xoff)
#define         A_DC4           20          // Device control 4
#define         A_NAK           21          // Not acknowledge
#define         A_SYN           22          // Synchronize
#define         A_ETB           23          // End of transmission block
#define         A_CAN           24          // Cancel
#define         A_EM            25          // End of media
#define         A_SUB           26          // Substitute
#define         A_ESC           27          // Escape
#define         A_FS            28          // Frame separator
#define         A_GS            29          // Group separator
#define         A_RS            30          // Record separator
#define         A_US            31          // Unit separator
#define         A_DEL           127         // Delete

class ZAsyncDeviceInt;  // operating system specific stuff

class ZAsyncDevice : public ZBase
{
   public:
      enum DataBits {
         dataBits5         = 5,
         dataBits6         = 6,
         dataBits7         = 7,
         dataBits8         = 8
      };

      enum Parity {
         parityNone        = 0,
         parityOdd         = 1,
         parityEven        = 2,
         parityMark        = 3,
         paritySpace       = 4
      };

      enum StopBits {
         stopBits1         = 0,
         stopBits1x5       = 1,
         stopBits2         = 2
      };

      enum FlowCtrl {
         flowNone          = 0,
         flowHard          = 1,
         flowSoft          = 2,
         flowBoth          = 3
      };

      ZBaseLink0
         ZAsyncDevice(                   // open device
            const ZString& aDevName,
            const ZString& aLogName=ZString());

      ZBaseLink0
         ~ZAsyncDevice();                // close device

      ZBaseLink(ZString)
         read(                           // read from device
            long aMaxChars = 1);

      ZBaseLink(ZAsyncDevice&)
         write(                          // write to device
            const ZString& data);

      ZBaseLink(ZBoolean)
         readChar(                       // buffered char read
            char& aChar);

      ZBaseLink(ZAsyncDevice&)
         waitForOutput();                // wait for output to complete

      ZBaseLink(ZAsyncDevice&)
         purgeInput();                   // purge input buffers

      ZBaseLink(long)
         inputChars();                   // get # of chars in input queue

      ZBaseLink(ZAsyncDevice&)
         setReadTimeout(                 // set read timeout
            long aTime);                    // timeout in millisecs

      ZBaseLink(ZAsyncDevice&)
         setBps(                         // set bits per second
            long aBps);

      ZBaseLink(ZAsyncDevice&)
         setBits(                        // set bits per word
            DataBits aBits);

      ZBaseLink(ZAsyncDevice&)
         setParity(                      // set parity
            Parity aParity);

      ZBaseLink(ZAsyncDevice&)
         setStops(                       // set stopbits
            StopBits aStopBits);

      ZBaseLink(ZAsyncDevice&)
         setFlow(                        // set flow control
            FlowCtrl aFlow);

      ZBaseLink(long)
         readTimeout();                  // query read timeout in millisecs

      ZBaseLink(long)
         bps();                          // query bits per second

      ZBaseLink(DataBits)
         bits();                         // query bits per word

      ZBaseLink(Parity)
         parity();                       // query parity

      ZBaseLink(StopBits)
         stops();                        // query stopbits

      ZBaseLink(FlowCtrl)
         flow();                         // query flow control

      // stream output operators

      ZBaseLink(ZAsyncDevice&) operator<<(char aChar);
      ZBaseLink(ZAsyncDevice&) operator<<(signed char aChar);
      ZBaseLink(ZAsyncDevice&) operator<<(unsigned char aChar);
      ZBaseLink(ZAsyncDevice&) operator<<(const char* aString);
      ZBaseLink(ZAsyncDevice&) operator<<(const signed char* aString);
      ZBaseLink(ZAsyncDevice&) operator<<(const unsigned char* aString);
      ZBaseLink(ZAsyncDevice&) operator<<(const ZString& aString);

      // stream input operators

      ZBaseLink(ZAsyncDevice&) operator>>(char& aChar);
      ZBaseLink(ZAsyncDevice&) operator>>(signed char& aChar);
      ZBaseLink(ZAsyncDevice&) operator>>(unsigned char& aChar);
      ZBaseLink(ZAsyncDevice&) operator>>(char* aString);
      ZBaseLink(ZAsyncDevice&) operator>>(signed char* aString);
      ZBaseLink(ZAsyncDevice&) operator>>(unsigned char* aString);
      ZBaseLink(ZAsyncDevice&) operator>>(ZString& aString);

   private:
      friend class ZAsyncDeviceInt;

      ZString     iInStream;             // input stream buffer
      long        iInStreamPos;          // buffer pointer
      std::ofstream    iLogFile;              // logfile
      ZBoolean    iLogMode;              // log mode
      ZAsyncDeviceInt* iOp;              // operating system specific stuff
}; // ZAsyncDevice

ZEXCLASSDECLARE(ZBaseLink,ZAsyncError,ZException);

#endif  // _ZASYNC_
