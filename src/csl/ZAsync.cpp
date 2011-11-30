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

#ifndef ZC_BUILDING_ZBASE
#define ZC_BUILDING_ZBASE
#endif

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <ZTrace.hpp>
#include <ZProcess.hpp>
#include <ZDate.hpp>
#include <ZAsync.hpp>

// #include "../Fluid/mmgr.h"

#define INSTREAMBUF        2048 // size of instream buffer

using namespace std;

ZEXCLASSIMPLEMENT (ZAsyncError, ZException);

#define ZTHROWASYNC(aMsg) { ZAsyncError exc(aMsg); ZTHROW(exc); }

///////////////////////////// ZAsyncDeviceInt (OS/2) /////////////////////////

#ifdef ZC_OS2

#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#define INCL_DOSERRORS
#include <os2.h>

#ifndef LINECONTROL
#include <bsedev.h>
#endif

static const char ErrorX[] = "Error ";



#define ZTHROWASYNCOS2(aMsg) ZTHROWASYNC(ErrorX+ZString(rc)+aMsg)

class ZAsyncDeviceInt:public ZBase
{
public:
  ZAsyncDeviceInt (ZAsyncDevice * aParent, const ZString & aDevName);
   ~ZAsyncDeviceInt ();
  void write (ZString aData);
  ZString read (long aMaxChars);
  void waitForOutput ();
  long inputChars ();
  void purgeInput ();
  void setReadTimeout (long aReadTimeout);
  void setBps (long aBps);
  void setBits (ZAsyncDevice::DataBits bits);
  void setParity (ZAsyncDevice::Parity par);
  void setStops (ZAsyncDevice::StopBits stp);
  void setFlow (ZAsyncDevice::FlowCtrl flow);
  long readTimeout ();
  long bps ();
    ZAsyncDevice::DataBits bits ();
    ZAsyncDevice::Parity parity ();
    ZAsyncDevice::StopBits stops ();
    ZAsyncDevice::FlowCtrl flow ();

private:
  void getLineCtl (LINECONTROL & aCtl);
  void setLineCtl (LINECONTROL & aCtl);
  void getDcbInfo (DCBINFO & aDcb);
  void setDcbInfo (DCBINFO & aDcb);
  void closeHandle ();
  void checkHandle ();

  ZAsyncDevice *iParent;        // parent instance
  HFILE iHandle;                // handle
  ZBoolean iIsOpen;             // device is open
  ZString iDevName;             // device name
    ZAsyncDevice::FlowCtrl iFlow;   // flow control
};                              // ZAsyncDeviceInt

ZAsyncDeviceInt::ZAsyncDeviceInt (ZAsyncDevice * aParent,
                                  const ZString & aDevName):iParent (aParent),
iIsOpen (zFalse), iDevName (aDevName), iFlow (ZAsyncDevice::flowNone)
{
}                               // ZAsyncDeviceInt

ZAsyncDeviceInt::~ZAsyncDeviceInt ()
{
  closeHandle ();
}                               // ZAsyncDeviceInt

void ZAsyncDeviceInt::getLineCtl (LINECONTROL & param)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::getLineCtl(LINECONTROL& param)");
  ULONG parLen = sizeof (param);
  APIRET rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_GETLINECTRL,
                           NULL, 0, NULL,
                           &param, sizeof (param), &parLen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" getting linectl");
}                               // getLineCtl

void ZAsyncDeviceInt::setLineCtl (LINECONTROL & param)
{
  ZFUNCTRACE_DEVELOP ("void ZAsyncDeviceInt::setLineCtl(LINECONTROL& param)");
  ULONG parLen = sizeof (param);
  APIRET rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_SETLINECTRL,
                           &param, sizeof (param), &parLen,
                           NULL, 0, NULL);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" setting linectl");
}                               // setLineCtl

void ZAsyncDeviceInt::getDcbInfo (DCBINFO & param)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::getDcbInfo(DCBINFO& param)");
  ULONG parLen = sizeof (param);
  APIRET rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_GETDCBINFO,
                           NULL, 0, NULL,
                           &param, parLen, &parLen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" getting dcb info");
}                               // getDcbInfo

void ZAsyncDeviceInt::setDcbInfo (DCBINFO & param)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setDcbInfo(DCBINFO& param)");
  ULONG parLen = sizeof (param);
  APIRET rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_SETDCBINFO,
                           &param, parLen, &parLen,
                           NULL, 0, NULL);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" setting dcb info");
}                               // setDcbInfo

void ZAsyncDeviceInt::closeHandle ()
{
  if (iIsOpen)
      {
        APIRET rc = DosClose (iHandle);
        if (rc != NO_ERROR)
          ZTHROWASYNCOS2 (" closing async-device");
        iIsOpen = zFalse;
      }                         // if
}                               // closeHandle

void ZAsyncDeviceInt::checkHandle ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::checkHandle()");

  if (iIsOpen)
    return;

  ULONG action;
  APIRET rc = DosOpen ((char *) iDevName, &iHandle, &action, 0L,
                       FILE_NORMAL, FILE_OPEN,
                       OPEN_ACCESS_READWRITE
                       | OPEN_SHARE_DENYREADWRITE
                       | OPEN_FLAGS_NOINHERIT | OPEN_FLAGS_FAIL_ON_ERROR,
                       (PEAOP2) NULL);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" opening async-device " + iDevName);

  // initialize dcb
  DCBINFO dcbInfo;
  getDcbInfo (dcbInfo);

  dcbInfo.usWriteTimeout = 100; // 1 sec
  dcbInfo.usReadTimeout = 100;  // 1 sec

  UCHAR CtlClrBits = MODE_DTR_HANDSHAKE |
    MODE_CTS_HANDSHAKE |
    MODE_DSR_HANDSHAKE | MODE_DCD_HANDSHAKE | MODE_DSR_SENSITIVITY;

  UCHAR CtlSetBits = MODE_DTR_CONTROL;
  if (iFlow == ZAsyncDevice::flowHard || iFlow == ZAsyncDevice::flowBoth)
    CtlSetBits |= MODE_CTS_HANDSHAKE | MODE_DSR_SENSITIVITY;

  dcbInfo.fbCtlHndShake &= ~CtlClrBits;
  dcbInfo.fbCtlHndShake |= CtlSetBits;

  UCHAR FlwClrBits = MODE_AUTO_TRANSMIT | MODE_AUTO_RECEIVE | MODE_ERROR_CHAR | MODE_NULL_STRIPPING | MODE_BREAK_CHAR | 0x20 |      // mode_auto_receive_fullduplex
    MODE_RTS_HANDSHAKE;

  UCHAR FlwSetBits = MODE_RTS_CONTROL;

  if (iFlow == ZAsyncDevice::flowSoft || iFlow == ZAsyncDevice::flowBoth)
    FlwSetBits |= MODE_AUTO_TRANSMIT | MODE_AUTO_RECEIVE;

  dcbInfo.fbFlowReplace &= ~FlwClrBits;
  dcbInfo.fbFlowReplace |= FlwSetBits;

  UCHAR TioClrBits = MODE_NO_WRITE_TIMEOUT | MODE_READ_TIMEOUT;
  UCHAR TioSetBits = MODE_WAIT_READ_TIMEOUT;
  dcbInfo.fbTimeout &= ~TioClrBits;
  dcbInfo.fbTimeout |= TioSetBits;

  dcbInfo.bErrorReplacementChar = dcbInfo.bBreakReplacementChar = 0;

  if (iFlow == ZAsyncDevice::flowSoft || iFlow == ZAsyncDevice::flowBoth)
      {
        dcbInfo.bXONChar = A_DC1;
        dcbInfo.bXOFFChar = A_DC3;
      }
  else
      {
        dcbInfo.bXONChar = dcbInfo.bXOFFChar = 0;
      }                         // if

  setDcbInfo (dcbInfo);

  // turn dtr & rts on
  MODEMSTATUS modemStatus;
  ULONG modemLen = sizeof (modemStatus);
  USHORT errWord;
  ULONG errLen = sizeof (errWord);

  modemStatus.fbModemOn = DTR_ON | RTS_ON;
  modemStatus.fbModemOff = ~modemStatus.fbModemOn;
  rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_SETMODEMCTRL,
                    &modemStatus, modemLen, &modemLen,
                    &errWord, errLen, &errLen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" setting modemctl");

  // start transmitting
  rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_STARTTRANSMIT,
                    NULL, 0, NULL, NULL, 0, NULL);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" starting transmit");

  // turn break off
  errLen = sizeof (errWord);
  rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_SETBREAKOFF,
                    NULL, 0, NULL, &errWord, errLen, &errLen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" turning break off");

  iParent->iInStreamPos = iParent->iInStream.size () + 1;
  iIsOpen = zTrue;
}                               // checkHandle

void ZAsyncDeviceInt::write (ZString aData)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::write(const ZString& aData)");
  checkHandle ();
  ULONG actual;
  APIRET rc = DosWrite (iHandle, (char *) aData, aData.size (), &actual);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" writing to async-device");
}                               // write

ZString ZAsyncDeviceInt::read (long aMaxChars)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::read(long aMaxChars)");
  checkHandle ();
  char *readbuf = new char[aMaxChars];
  ULONG actual;
  APIRET rc = DosRead (iHandle, (char *) readbuf, (ULONG) aMaxChars, &actual);
  if (rc != NO_ERROR)
      {
        delete[]readbuf;
        ZTHROWASYNCOS2 (" reading from async-device");
      }                         // if
  ZString str (readbuf, actual);
  delete[]readbuf;
  return str;
}                               // read

void ZAsyncDeviceInt::waitForOutput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::waitForOutput()");
  if (!iIsOpen)
    return;

  BYTE mask = WRITE_REQUEST_QUEUED |
    DATA_IN_TX_QUE |
    HARDWARE_TRANSMITTING |
    CHAR_READY_TO_SEND_IMM | WAITING_TO_SEND_XON | WAITING_TO_SEND_XOFF;

  for (;;)
      {
        BYTE status;
        ULONG parLen = sizeof (status);
        APIRET rc;
        rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_GETLINESTATUS,
                          NULL, 0, NULL, &status, parLen, &parLen);
        if (rc != NO_ERROR)
          ZTHROWASYNCOS2 (" getting linestatus");
        status &= mask;
        if (!status)
          break;
        ZCurrentThread::sleep (50);
      }                         // for
}                               // waitForOutput

long ZAsyncDeviceInt::inputChars ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::inputChars()");
  if (!iIsOpen)
    return 0;

  // check if any input available
  RXQUEUE rxq;
  ULONG rxqlen = sizeof (rxq);
  APIRET rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_GETINQUECOUNT,
                           NULL, 0, NULL,
                           &rxq, rxqlen, &rxqlen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" reading inqueue count");
  return rxq.cch;
}                               // inputChars

void ZAsyncDeviceInt::purgeInput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::purgeInput()");
  if (!iIsOpen)
    return;
  while (inputChars () > 0)
    read (1000);
}                               // purgeInput

void ZAsyncDeviceInt::setReadTimeout (long aReadTimeout)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setReadTimeout(long aReadTimeout)");

  checkHandle ();
  DCBINFO dcbInfo;
  getDcbInfo (dcbInfo);
  dcbInfo.usReadTimeout = aReadTimeout / 10;
  setDcbInfo (dcbInfo);
}                               // setReadTimeout

void ZAsyncDeviceInt::setBps (long aBps)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setBps(long aBps)");

#pragma pack(1)
  struct
  {
    ULONG bitrate;
    UCHAR fraction;
  } param;                      // function parameter
#pragma pack()

  APIRET rc;                    // return code
  ULONG parLen = sizeof (param);    // actual parameter length

  checkHandle ();
  param.bitrate = aBps;
  param.fraction = 0;
  rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_EXTSETBAUDRATE,
                    &param, sizeof (param), &parLen, NULL, 0, NULL);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" setting bps");
}                               // setBps

void ZAsyncDeviceInt::setBits (ZAsyncDevice::DataBits bits)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setBits(ZAsyncDevice::DataBits bits)");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  param.bDataBits = bits;
  setLineCtl (param);
}                               // setBits

void ZAsyncDeviceInt::setParity (ZAsyncDevice::Parity par)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setParity(ZAsyncDevice::Parity par)");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  param.bParity = par;
  setLineCtl (param);
}                               // setParity

void ZAsyncDeviceInt::setStops (ZAsyncDevice::StopBits stp)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setStops(ZAsyncDevice::StopBits stp)");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  param.bStopBits = stp;
  setLineCtl (param);
}                               // setStops

void ZAsyncDeviceInt::setFlow (ZAsyncDevice::FlowCtrl flow)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setFlow(ZAsyncDevice::FlowCtrl flow)");
  if (flow != iFlow)
      {
        closeHandle ();
        iFlow = flow;
      }                         // if
}                               // setFlow

long ZAsyncDeviceInt::readTimeout ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::readTimeout()");
  checkHandle ();
  DCBINFO dcbInfo;
  getDcbInfo (dcbInfo);
  return dcbInfo.usReadTimeout;
}                               // readTimeout

long ZAsyncDeviceInt::bps ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bps()");

#pragma pack(1)
  struct
  {
    ULONG currBps;
    UCHAR currFrac;
    ULONG minBps;
    UCHAR minFrac;
    ULONG maxBps;
    UCHAR maxFrac;
  } param;                      // function parameter
#pragma pack()

  APIRET rc;                    // return code
  ULONG parLen = sizeof (param);    // actual parameter length

  checkHandle ();
  rc = DosDevIOCtl (iHandle, IOCTL_ASYNC, ASYNC_EXTGETBAUDRATE,
                    NULL, 0, NULL, &param, sizeof (param), &parLen);
  if (rc != NO_ERROR)
    ZTHROWASYNCOS2 (" setting bps");
  return param.currBps;
}                               // bps

ZAsyncDevice::DataBits ZAsyncDeviceInt::bits ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bits()");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  return (ZAsyncDevice::DataBits) param.bDataBits;
}                               // bits

ZAsyncDevice::Parity ZAsyncDeviceInt::parity ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::parity()");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  return (ZAsyncDevice::Parity) param.bParity;
}                               // parity

ZAsyncDevice::StopBits ZAsyncDeviceInt::stops ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::stops()");
  checkHandle ();
  LINECONTROL param;
  getLineCtl (param);
  return (ZAsyncDevice::StopBits) param.bStopBits;
}                               // stops

ZAsyncDevice::FlowCtrl ZAsyncDeviceInt::flow ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::flow()");
  return iFlow;
}                               // flow

#endif // ZC_OS2



///////////////////////////// ZAsyncDeviceInt (Windows) //////////////////////

#ifdef ZC_WIN

#include <windows.h>

#define ZTHROWASYNCWIN(aMsg) ZTHROWASYNC(ZString(aMsg)+": "+getWinError())

class ZAsyncDeviceInt:public ZBase
{
public:
  ZAsyncDeviceInt (ZAsyncDevice * aParent, const ZString & aDevName);
  ~ZAsyncDeviceInt ();
  void write (const ZString & aData);
  ZString read (long aMaxChars);
  void waitForOutput ();
  void purgeInput ();
  void setReadTimeout (long aReadTimeout);
  void setBps (long aBps);
  void setBits (ZAsyncDevice::DataBits bits);
  void setParity (ZAsyncDevice::Parity par);
  void setStops (ZAsyncDevice::StopBits stp);
  void setFlow (ZAsyncDevice::FlowCtrl flow);
  long readTimeout ();
  long bps ();
  ZAsyncDevice::DataBits bits ();
  ZAsyncDevice::Parity parity ();
  ZAsyncDevice::StopBits stops ();
  ZAsyncDevice::FlowCtrl flow ();

private:
  ZString getWinError ();
  void getDcb (DCB & aDcb);
  void setDcb (DCB & aDcb);
  void getCto (COMMTIMEOUTS & aCto);
  void setCto (COMMTIMEOUTS & aCto);
  void checkHandle ();
  void closeHandle ();
  ZBoolean checkError ();

  HANDLE iHandle;               // device handle
  long iReadTimeout;            // read timeout
  long iBps;                    // bits per second
  ZAsyncDevice::DataBits iDataBits; // data bits per word
  ZAsyncDevice::Parity iParity; // parity
  ZAsyncDevice::StopBits iStopBits; // # of stopbits
  ZAsyncDevice::FlowCtrl iFlow; // flow control
  ZBoolean iIsOpen;             // device is open
  ZString iDevName;             // device name
  ZAsyncDevice *iParent;        // parent instance
};                              // ZAsyncDeviceInt

ZAsyncDeviceInt::ZAsyncDeviceInt (ZAsyncDevice * aParent,
                                  const ZString &
                                  aDevName):iReadTimeout (1000), iBps (9600),
iDataBits (ZAsyncDevice::dataBits8), iParity (ZAsyncDevice::parityNone),
iStopBits (ZAsyncDevice::stopBits1), iFlow (ZAsyncDevice::flowNone),
iIsOpen (zFalse), iDevName (aDevName), iParent (aParent)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::ZAsyncDeviceInt(ZAsyncDevice* aParent, const ZString& aDevName)");
}                               // ZAsyncDeviceInt

ZAsyncDeviceInt::~ZAsyncDeviceInt ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::~ZAsyncDeviceInt()");
  if (iIsOpen)
    CloseHandle (iHandle);
}                               // ~ZAsyncDeviceInt

ZString ZAsyncDeviceInt::getWinError ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::getWinError()");

  DWORD errcode = GetLastError ();
  LPTSTR lpMsgBuf;
  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 errcode,
                 MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPTSTR) & lpMsgBuf, 0, NULL);
  ZString msg (lpMsgBuf);
  LocalFree (lpMsgBuf);
  return msg;
}                               // getWinError

void ZAsyncDeviceInt::getDcb (DCB & dcb)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::getDcb(DCB& dcb)");

  dcb.DCBlength = sizeof (DCB);
  if (!GetCommState (iHandle, &dcb))
    ZTHROWASYNCWIN ("GetCommState");
}                               // getDcb

void ZAsyncDeviceInt::setDcb (DCB & dcb)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setDcb(DCB& dcb)");
  if (!SetCommState (iHandle, &dcb))
    ZTHROWASYNCWIN ("SetCommState");
}                               // setDcb

void ZAsyncDeviceInt::getCto (COMMTIMEOUTS & cto)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::getCto(COMMTIMEOUTS& cto)");
  if (!GetCommTimeouts (iHandle, &cto))
    ZTHROWASYNCWIN ("GetCommTimeouts");
}                               // getCto

void ZAsyncDeviceInt::setCto (COMMTIMEOUTS & cto)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setCto(COMMTIMEOUTS& cto)");
  if (!SetCommTimeouts (iHandle, &cto))
    ZTHROWASYNCWIN ("SetCommTimeouts");
}                               // setCto

void ZAsyncDeviceInt::checkHandle ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::checkHandle()");
  if (iIsOpen)
    return;
  int errCount (0);
  while (!iIsOpen)
      {
        try
        {
          iHandle = INVALID_HANDLE_VALUE;
          iHandle = CreateFile ((char *) iDevName,
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
          if (iHandle == INVALID_HANDLE_VALUE)
            ZTHROWASYNCWIN ("CreateFile");

          if (!SetupComm (iHandle, 4096, 4096))
            ZTHROWASYNCWIN ("SetupComm");

          // set timeouts
          COMMTIMEOUTS cto;
          cto.ReadIntervalTimeout = 0;
          cto.ReadTotalTimeoutMultiplier = 0;
          cto.ReadTotalTimeoutConstant = 50;
          cto.WriteTotalTimeoutMultiplier = 0;
          cto.WriteTotalTimeoutConstant = 0;

          // initialize dcb
          DCB dcb;
          getDcb (dcb);
          dcb.BaudRate = iBps;
          dcb.fBinary = TRUE;
          dcb.fOutxCtsFlow = (iFlow == ZAsyncDevice::flowHard
                              || iFlow == ZAsyncDevice::flowBoth);
          dcb.fOutxDsrFlow = FALSE;
          dcb.fDtrControl = DTR_CONTROL_ENABLE;
          dcb.fDsrSensitivity = FALSE;
          dcb.fTXContinueOnXoff = TRUE;
          dcb.fOutX =
            dcb.fInX = (iFlow == ZAsyncDevice::flowSoft
                        || iFlow == ZAsyncDevice::flowBoth);
          dcb.fErrorChar = FALSE;
          dcb.fNull = FALSE;
          dcb.fRtsControl = RTS_CONTROL_ENABLE;
          dcb.fAbortOnError = FALSE;
          dcb.XonLim = 16;
          dcb.XoffLim = 16;
          dcb.ByteSize = iDataBits;
          dcb.Parity = iParity;
          dcb.StopBits = iStopBits;
          dcb.XonChar = A_DC1;
          dcb.XoffChar = A_DC3;

          checkError ();
          setCto (cto);
          checkError ();
          setDcb (dcb);
          checkError ();

          iParent->iInStreamPos = iParent->iInStream.size () + 1;
          iIsOpen = zTrue;
        }                       // try
        catch (ZException exc)
        {
          ZTRACE_DEVELOP ("com open error");
          if (iHandle != INVALID_HANDLE_VALUE)
            CloseHandle (iHandle);
          if (++errCount == 10)
            throw;
        }                       // catch
      }                         // while
}                               // checkHandle

ZBoolean ZAsyncDeviceInt::checkError ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::checkError()");
  if (!iIsOpen)
    return zFalse;

  DWORD errors;                 // error flags
  COMSTAT status;               // comm status

  if (!ClearCommError (iHandle, &errors, &status))
    ZTHROWASYNCWIN ("ClearCommError");
  if (errors & CE_DNS)
    ZTHROWASYNC (iDevName + " device not selected");
  if (errors & CE_FRAME)
    ZTHROWASYNC (iDevName + " framing error");
  if (errors & CE_IOE)
    ZTHROWASYNC (iDevName + " I/O error detected");
  if (errors & CE_MODE)
    ZTHROWASYNC (iDevName + " invalid mode requested");
  if (errors & CE_OOP)
    ZTHROWASYNC (iDevName + " out of paper");
  if (errors & CE_OVERRUN)
    ZTHROWASYNC (iDevName + " char buffer overrun");
  if (errors & CE_PTO)
    ZTHROWASYNC (iDevName + " parallel port timeout");
  if (errors & CE_RXOVER)
    ZTHROWASYNC (iDevName + " input buffer overflow");
  if (errors & CE_RXPARITY)
    ZTHROWASYNC (iDevName + " parity error");
  if (errors & CE_TXFULL)
    ZTHROWASYNC (iDevName + " output buffer full");
  if (status.fEof)
    ZTHROWASYNC (iDevName + " EOF received");
  return zTrue;
}                               // checkError

void ZAsyncDeviceInt::closeHandle ()
{
  if (iIsOpen)
      {
        if (!CloseHandle (iHandle))
          ZTHROWASYNCWIN ("CloseHandle");
        iIsOpen = zFalse;
      }                         // if
}                               // closeHandle

void ZAsyncDeviceInt::write (const ZString & aData)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::write(const ZString& aData)");
  checkHandle ();
  DWORD actual;
  BOOL rc =
    WriteFile (iHandle, (const char *) aData, aData.size (), &actual, NULL);
  if (!rc)
    ZTHROWASYNCWIN ("WriteFile");
  checkError ();
}                               // write

ZString ZAsyncDeviceInt::read (long aMaxChars)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::read(long aMaxChars)");
  checkHandle ();
  char *readbuf = new char[aMaxChars];
  DWORD actual (0);
  long wtime (0);
  do
      {
        if (checkError ())
            {                   // any input chars waiting?
              BOOL rc = ReadFile (iHandle, readbuf, aMaxChars, &actual, NULL);
              if (!rc)
                  {
                    closeHandle ();
                    checkHandle ();
                    continue;
                  }             // if
              if (actual)
                break;
            }                   // if
        if (!iReadTimeout)
          continue;
        wtime += 50;
      }
  while (wtime < iReadTimeout);
  ZString str (readbuf, actual);
  delete[]readbuf;
  return str;
}                               // read

void ZAsyncDeviceInt::waitForOutput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::waitForOutput()");
  if (!iIsOpen)
    return;

  if (!FlushFileBuffers (iHandle))
    ZTHROWASYNCWIN ("FlushFileBuffers");
  checkError ();
}                               // waitForOutput

void ZAsyncDeviceInt::purgeInput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::purgeInput()");
  if (!iIsOpen)
    return;
  if (!PurgeComm (iHandle, PURGE_RXABORT | PURGE_RXCLEAR))
    ZTHROWASYNCWIN ("PurgeComm");
  checkError ();
}                               // purgeInput

void ZAsyncDeviceInt::setReadTimeout (long aReadTimeout)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setReadTimeout(long aReadTimeout)");
  iReadTimeout = aReadTimeout;
}                               // setReadTimeout

void ZAsyncDeviceInt::setBps (long aBps)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setBps(long aBps)");
  if (aBps != iBps)
      {
        closeHandle ();
        iBps = aBps;
      }                         // if
}                               // setBps

void ZAsyncDeviceInt::setBits (ZAsyncDevice::DataBits bits)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setBits(ZAsyncDevice::DataBits bits)");
  if (bits != iDataBits)
      {
        closeHandle ();
        iDataBits = bits;
      }                         // if
}                               // setBits

void ZAsyncDeviceInt::setParity (ZAsyncDevice::Parity par)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setParity(ZAsyncDevice::Parity par)");
  if (par != iParity)
      {
        closeHandle ();
        iParity = par;
      }                         // if
}                               // setParity

void ZAsyncDeviceInt::setStops (ZAsyncDevice::StopBits stp)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setStops(ZAsyncDevice::StopBits stp)");
  if (stp != iStopBits)
      {
        closeHandle ();
        iStopBits = stp;
      }                         // if
}                               // setStops

void ZAsyncDeviceInt::setFlow (ZAsyncDevice::FlowCtrl flow)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setFlow(ZAsyncDevice::FlowCtrl flow)");
  if (flow != iFlow)
      {
        closeHandle ();
        iFlow = flow;
      }                         // if
}                               // setFlow

long ZAsyncDeviceInt::readTimeout ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::readTimeout()");
  return iReadTimeout;
}                               // readTimeout

long ZAsyncDeviceInt::bps ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bps()");
  return iBps;
}                               // bps

ZAsyncDevice::DataBits ZAsyncDeviceInt::bits ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bits()");
  return iDataBits;
}                               // bits

ZAsyncDevice::Parity ZAsyncDeviceInt::parity ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::parity()");
  return iParity;
}                               // parity

ZAsyncDevice::StopBits ZAsyncDeviceInt::stops ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::stops()");
  return iStopBits;
}                               // stops

ZAsyncDevice::FlowCtrl ZAsyncDeviceInt::flow ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::flow()");
  return iFlow;
}                               // flow

#endif // ZC_WIN



///////////////////////////// ZAsyncDeviceInt (LINUX) /////////////////////////

#if ZC_UNIXFAM

#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char ErrorX[] = "Error ";

#define ZTHROWSYSERR(aMsg) ZTHROWASYNC(ErrorX+ZString(errno)+aMsg)

class ZAsyncDeviceInt:public ZBase
{
public:
  ZAsyncDeviceInt (ZAsyncDevice * aParent, const ZString & aDevName);
   ~ZAsyncDeviceInt ();
  void write (ZString aData);
  ZString read (long aMaxChars);
  void waitForOutput ();
  void purgeInput ();
  void setReadTimeout (long aReadTimeout);
  void setBps (long aBps);
  void setBits (ZAsyncDevice::DataBits bits);
  void setParity (ZAsyncDevice::Parity par);
  void setStops (ZAsyncDevice::StopBits stp);
  void setFlow (ZAsyncDevice::FlowCtrl flow);
  long readTimeout ();
  long bps ();
  ZAsyncDevice::DataBits bits ();
  ZAsyncDevice::Parity parity ();
  ZAsyncDevice::StopBits stops ();
  ZAsyncDevice::FlowCtrl flow ();

private:
  void closeHandle ();
  void checkHandle ();

  ZAsyncDevice *iParent;        // parent instance
  int iHandle;                  // file handle
  ZBoolean iIsOpen;             // device is open
  ZString iDevName;             // device name
  long iReadTimeout;            // read timeout
  long iBps;                    // bits per second
  ZAsyncDevice::DataBits iDataBits; // data bits per word
  ZAsyncDevice::Parity iParity; // parity
  ZAsyncDevice::StopBits iStopBits; // # of stopbits
  ZAsyncDevice::FlowCtrl iFlow; // flow control
};                              // ZAsyncDeviceInt

ZAsyncDeviceInt::ZAsyncDeviceInt (ZAsyncDevice * aParent,
                                  const ZString & aDevName):iParent (aParent),
iIsOpen (zFalse), iDevName (aDevName), iReadTimeout (1000), iBps (9600),
iDataBits (ZAsyncDevice::dataBits8), iParity (ZAsyncDevice::parityNone),
iStopBits (ZAsyncDevice::stopBits1), iFlow (ZAsyncDevice::flowNone)
{
}                               // ZAsyncDeviceInt

ZAsyncDeviceInt::~ZAsyncDeviceInt ()
{
  closeHandle ();
}                               // ZAsyncDeviceInt

void ZAsyncDeviceInt::closeHandle ()
{
  if (iIsOpen)
      {
        if (close (iHandle))
          ZTHROWSYSERR (" closing async-device");
        iIsOpen = zFalse;
      }                         // if
}                               // closeHandle

void ZAsyncDeviceInt::checkHandle ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::checkHandle()");

  if (iIsOpen)
    return;

  iHandle = open (iDevName.constBuffer (), O_RDWR | O_NOCTTY);
  if (iHandle < 0)
    ZTHROWSYSERR (" opening async-device " + iDevName);

  // initialize
  struct termios tio;
  memset (&tio, 0, sizeof (tio));

  tio.c_iflag = IGNBRK;
  if (iParity != ZAsyncDevice::parityNone)
    tio.c_iflag |= INPCK;
  if (iDataBits < ZAsyncDevice::dataBits8)
    tio.c_iflag |= ISTRIP;
  if (iFlow == ZAsyncDevice::flowSoft || iFlow == ZAsyncDevice::flowBoth)
    tio.c_iflag |= IXON | IXOFF;

  switch (iBps)
      {
      case 50:
        tio.c_cflag = B50;
        break;
      case 75:
        tio.c_cflag = B75;
        break;
      case 110:
        tio.c_cflag = B110;
        break;
      case 134:
        tio.c_cflag = B134;
        break;
      case 150:
        tio.c_cflag = B150;
        break;
      case 200:
        tio.c_cflag = B200;
        break;
      case 300:
        tio.c_cflag = B300;
        break;
      case 600:
        tio.c_cflag = B600;
        break;
      case 1200:
        tio.c_cflag = B1200;
        break;
      case 1800:
        tio.c_cflag = B1800;
        break;
      case 2400:
        tio.c_cflag = B2400;
        break;
      case 4800:
        tio.c_cflag = B4800;
        break;
      case 9600:
        tio.c_cflag = B9600;
        break;
      case 19200:
        tio.c_cflag = B19200;
        break;
      case 38400:
        tio.c_cflag = B38400;
        break;
      case 57600:
        tio.c_cflag = B57600;
        break;
      case 115200:
        tio.c_cflag = B115200;
        break;
      case 230400:
        tio.c_cflag = B230400;
        break;
#ifdef B460800
      case 460800:
        tio.c_cflag = B460800;
        break;
      case 500000:
        tio.c_cflag = B500000;
        break;
      case 576000:
        tio.c_cflag = B576000;
        break;
      case 921600:
        tio.c_cflag = B921600;
        break;
      case 1000000:
        tio.c_cflag = B1000000;
        break;
      case 1152000:
        tio.c_cflag = B1152000;
        break;
      case 1500000:
        tio.c_cflag = B1500000;
        break;
      case 2000000:
        tio.c_cflag = B2000000;
        break;
      case 2500000:
        tio.c_cflag = B2500000;
        break;
      case 3000000:
        tio.c_cflag = B3000000;
        break;
      case 3500000:
        tio.c_cflag = B3500000;
        break;
      case 4000000:
        tio.c_cflag = B4000000;
        break;
#endif
      default:
        ZTHROWASYNC ("invalid baud rate");
      }                         // switch

  switch (iDataBits)
      {
      case ZAsyncDevice::dataBits5:
        tio.c_cflag |= CS5;
        break;
      case ZAsyncDevice::dataBits6:
        tio.c_cflag |= CS6;
        break;
      case ZAsyncDevice::dataBits7:
        tio.c_cflag |= CS7;
        break;
      default:
        tio.c_cflag |= CS8;
        break;
      }                         // switch

  if (iStopBits != ZAsyncDevice::stopBits1)
    tio.c_cflag |= CSTOPB;

  tio.c_cflag |= CREAD | HUPCL;

  switch (iParity)
      {
      case ZAsyncDevice::parityEven:
        tio.c_cflag |= PARENB;
        break;
      case ZAsyncDevice::parityOdd:
        tio.c_cflag |= PARENB | PARODD;
        break;
      case ZAsyncDevice::parityMark:
      case ZAsyncDevice::paritySpace:
        ZTHROWASYNC ("mark/space parity is not supported");
      default:;
      }                         // switch

  if (iFlow == ZAsyncDevice::flowHard || iFlow == ZAsyncDevice::flowBoth)
    tio.c_cflag |= CRTSCTS;
  else
    tio.c_cflag |= CLOCAL;

  tio.c_cc[VMIN] = 1;

  tcflush (iHandle, TCIOFLUSH);
  tcsetattr (iHandle, TCSANOW, &tio);

  iParent->iInStreamPos = iParent->iInStream.size () + 1;
  iIsOpen = zTrue;
}                               // checkHandle

void ZAsyncDeviceInt::write (ZString aData)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::write(const ZString& aData)");
  checkHandle ();

  if (::write (iHandle, aData.constBuffer (), aData.size ()) < 0)
    ZTHROWSYSERR (" writing to async-device");
}                               // write

ZString ZAsyncDeviceInt::read (long aMaxChars)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::read(long aMaxChars)");
  checkHandle ();
  if (iReadTimeout)
      {
        fd_set readset;
        FD_ZERO (&readset);
        FD_SET (iHandle, &readset);
        struct timeval timeout;
        timeout.tv_sec = iReadTimeout / 1000;
        timeout.tv_usec = (iReadTimeout % 1000) * 1000;
        int ret = select (iHandle + 1, &readset, NULL, NULL, &timeout);
        if (ret < 0)
          ZTHROWSYSERR (" waiting for input on async-device");
        if (!FD_ISSET (iHandle, &readset))
          return ZString ();
      }                         // if
  char *readbuf = new char[aMaxChars];
  long actual =::read (iHandle, readbuf, aMaxChars);
  if (actual < 0)
      {
        delete[]readbuf;
        ZTHROWSYSERR (" reading from async-device");
      }                         // if
  ZString str (readbuf, actual);
  delete[]readbuf;
  return str;
}                               // read

void ZAsyncDeviceInt::waitForOutput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::waitForOutput()");
  if (!iIsOpen)
    return;
  tcdrain (iHandle);
}                               // waitForOutput

void ZAsyncDeviceInt::purgeInput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::purgeInput()");
  if (!iIsOpen)
    return;
  tcflush (iHandle, TCIFLUSH);
}                               // purgeInput

void ZAsyncDeviceInt::setReadTimeout (long aReadTimeout)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setReadTimeout(long aReadTimeout)");
  iReadTimeout = aReadTimeout;
}                               // setReadTimeout

void ZAsyncDeviceInt::setBps (long aBps)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setBps(long aBps)");
  if (aBps != iBps)
      {
        closeHandle ();
        iBps = aBps;
      }                         // if
}                               // setBps

void ZAsyncDeviceInt::setBits (ZAsyncDevice::DataBits bits)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setBits(ZAsyncDevice::DataBits bits)");
  if (bits != iDataBits)
      {
        closeHandle ();
        iDataBits = bits;
      }                         // if
}                               // setBits

void ZAsyncDeviceInt::setParity (ZAsyncDevice::Parity par)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::setParity(ZAsyncDevice::Parity par)");
  if (par != iParity)
      {
        closeHandle ();
        iParity = par;
      }                         // if
}                               // setParity

void ZAsyncDeviceInt::setStops (ZAsyncDevice::StopBits stp)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setStops(ZAsyncDevice::StopBits stp)");
  if (stp != iStopBits)
      {
        closeHandle ();
        iStopBits = stp;
      }                         // if
}                               // setStops

void ZAsyncDeviceInt::setFlow (ZAsyncDevice::FlowCtrl flow)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDeviceInt::setFlow(ZAsyncDevice::FlowCtrl flow)");
  if (flow != iFlow)
      {
        closeHandle ();
        iFlow = flow;
      }                         // if
}                               // setFlow

long ZAsyncDeviceInt::readTimeout ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::readTimeout()");
  return iReadTimeout;
}                               // readTimeout

long ZAsyncDeviceInt::bps ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bps()");
  return iBps;
}                               // bps

ZAsyncDevice::DataBits ZAsyncDeviceInt::bits ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::bits()");
  return iDataBits;
}                               // bits

ZAsyncDevice::Parity ZAsyncDeviceInt::parity ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::parity()");
  return iParity;
}                               // parity

ZAsyncDevice::StopBits ZAsyncDeviceInt::stops ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::stops()");
  return iStopBits;
}                               // stops

ZAsyncDevice::FlowCtrl ZAsyncDeviceInt::flow ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDeviceInt::flow()");
  return iFlow;
}                               // flow

#endif // ZC_UNIXFAM



///////////////////////////////// ZAsyncDevice ///////////////////////////////

ZExport0 ZAsyncDevice::ZAsyncDevice (const ZString & aDevName,
                                     const ZString & aLogName)
{
  ZFUNCTRACE_DEVELOP
    ("ZAsyncDevice::ZAsyncDevice(const ZString& aDevName, const ZString& aLogName)");
  if (aLogName.length ())
      {
        iLogFile.open (aLogName, ios::out | ios::app);
        iLogMode = zTrue;
      }
  else
    iLogMode = zFalse;
  iOp = new ZAsyncDeviceInt (this, aDevName);
  iInStreamPos = iInStream.size () + 1;
}                               // ZAsyncDevice

ZExport0 ZAsyncDevice::~ZAsyncDevice ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::~ZAsyncDevice()");
  delete iOp;
}                               // ~ZAsyncDevice

ZExport (ZAsyncDevice &) ZAsyncDevice::write (const ZString & aData)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::write(const ZString& aData)");
  iOp->write (aData);
  if (iLogMode && aData.size ())
      {
        ZString ls (ZString::exp (aData));
        ZBoolean firstLine (zTrue);
        while (ls.length ())
            {
              if (firstLine)
                  {
                    iLogFile << ZDateTime ().asTime () << " send ";
                    firstLine = zFalse;
                  }
              else
                iLogFile << "            + ";
              if (ls.length () > 60)
                  {
                    iLogFile << ls.subString (1, 60) << endl;
                    ls = ls.subString (61);
                  }
              else
                  {
                    iLogFile << ls << endl;
                    ls = "";
                  }             // if
            }                   // while
      }                         // if
  return *this;
}                               // write

ZExport (ZString) ZAsyncDevice::read (long aMaxChars)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::read(long aMaxChars)");

  // if chars in streambuf return 'em first
  long cnt = iInStream.size () - iInStreamPos + 1;
  if (cnt > 0)
      {
        if (cnt > aMaxChars)
          cnt = aMaxChars;
        ZString str (iInStream.subString (iInStreamPos, cnt));
        iInStreamPos += cnt;
        return str;
      }                         // if

  // now do physical read
  ZString str (iOp->read (aMaxChars));
  if (iLogMode && str.size ())
      {
        ZString ls (ZString::exp (str));
        ZBoolean firstLine (zTrue);
        while (ls.length ())
            {
              if (firstLine)
                  {
                    iLogFile << ZDateTime ().asTime () << " recv ";
                    firstLine = zFalse;
                  }
              else
                iLogFile << "            + ";
              if (ls.length () > 60)
                  {
                    iLogFile << ls.subString (1, 60) << endl;
                    ls = ls.subString (61);
                  }
              else
                  {
                    iLogFile << ls << endl;
                    ls = "";
                  }             // if
            }                   // while
      }                         // if
  return str;
}                               // read

ZExport (ZAsyncDevice &) ZAsyncDevice::waitForOutput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::waitForOutput()");
  iOp->waitForOutput ();
  return *this;
}                               // waitForOutput

ZExport (ZAsyncDevice &) ZAsyncDevice::purgeInput ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::purgeInput()");
  iInStreamPos = iInStream.size () + 1;
  iOp->purgeInput ();
  return *this;
}                               // purgeInput

ZExport (long) ZAsyncDevice::inputChars ()
{
  long cnt = iInStream.size () - iInStreamPos + 1;
  if (cnt)
    return cnt;
#if ZC_WIN || ZC_UNIXFAM
  long rto (iOp->readTimeout ());
  try
  {
    iOp->setReadTimeout (1);
    iInStream = read (INSTREAMBUF);
    iInStreamPos = 1;
    iOp->setReadTimeout (rto);
    cnt = iInStream.size () - iInStreamPos + 1;
  }                             // try
  catch (const ZException & exc)
  {
    iOp->setReadTimeout (rto);
    throw;
  }                             // catch
  return cnt;
#endif
#ifdef ZC_OS2
  return iOp->inputChars ();
#endif
}                               // inputChars

ZExport (ZAsyncDevice &) ZAsyncDevice::setReadTimeout (long aReadTimeout)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setReadTimeout(long aReadTimeout)");
  iOp->setReadTimeout (aReadTimeout);
  return *this;
}                               // setReadTimeout

ZExport (ZAsyncDevice &) ZAsyncDevice::setBps (long aBps)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setBps(long aBps)");
  iOp->setBps (aBps);
  return *this;
}                               // setBps

ZExport (ZAsyncDevice &) ZAsyncDevice::setBits (DataBits bits)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setBits(DataBits bits)");
  iOp->setBits (bits);
  return *this;
}                               // setBits

ZExport (ZAsyncDevice &) ZAsyncDevice::setParity (Parity par)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setParity(Parity par)");
  iOp->setParity (par);
  return *this;
}                               // setParity

ZExport (ZAsyncDevice &) ZAsyncDevice::setStops (StopBits stp)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setStops(StopBits stp)");
  iOp->setStops (stp);
  return *this;
}                               // setStops

ZExport (ZAsyncDevice &) ZAsyncDevice::setFlow (FlowCtrl flow)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::setFlow(FlowCtrl flow)");
  iOp->setFlow (flow);
  return *this;
}                               // setFlow

ZExport (long) ZAsyncDevice::readTimeout ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::readTimeout()");
  return iOp->readTimeout ();
}                               // readTimeout

ZExport (long) ZAsyncDevice::bps ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::bps()");
  return iOp->bps ();
}                               // bps

ZExport (ZAsyncDevice::DataBits) ZAsyncDevice::bits ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::bits()");
  return iOp->bits ();
}                               // bits

ZExport (ZAsyncDevice::Parity) ZAsyncDevice::parity ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::parity()");
  return iOp->parity ();
}                               // parity

ZExport (ZAsyncDevice::StopBits) ZAsyncDevice::stops ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::stops()");
  return iOp->stops ();
}                               // stops

ZExport (ZAsyncDevice::FlowCtrl) ZAsyncDevice::flow ()
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::flow()");
  return iOp->flow ();
}                               // flow

#define DEFSHIFTLEFTCHAR(VarType) \
ZExport(ZAsyncDevice&) ZAsyncDevice::operator<<(VarType val)\
{\
    ZFUNCTRACE_DEVELOP("ZAsyncDevice::operator<<(VarType val)");\
    write(ZString(val));\
    return *this;\
}
DEFSHIFTLEFTCHAR (char)
DEFSHIFTLEFTCHAR (signed char)
DEFSHIFTLEFTCHAR (unsigned char)
DEFSHIFTLEFTCHAR (const char *)
DEFSHIFTLEFTCHAR (const signed char *)
DEFSHIFTLEFTCHAR (const unsigned char *)
ZExport (ZAsyncDevice &) ZAsyncDevice::operator<< (const ZString & val)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::operator<<(const ZString& val)");
  write (val);
  return *this;
}                               // operator<<(ZString val)

#define DEFSHIFTRIGHTCHAR(VarType) \
ZExport(ZAsyncDevice&) ZAsyncDevice::operator>>(VarType &val)\
{\
   ZFUNCTRACE_DEVELOP("ZAsyncDevice::operator>>(VarType &val)");\
   if (iInStreamPos>iInStream.size()) {\
       iInStream = read(INSTREAMBUF);\
       iInStreamPos = 1;\
   }\
   if (iInStreamPos<=iInStream.size())\
      val = iInStream[iInStreamPos++];\
   else\
      val = 0;\
    return *this;\
}
DEFSHIFTRIGHTCHAR (char)
DEFSHIFTRIGHTCHAR (signed char) DEFSHIFTRIGHTCHAR (unsigned char)
#define DEFSHIFTRIGHTSTR(VarType) \
ZExport(ZAsyncDevice&) ZAsyncDevice::operator>>(VarType *val)\
{\
   ZFUNCTRACE_DEVELOP("ZAsyncDevice::operator>>(VarType *val)");\
   if (iInStreamPos>iInStream.size()) {\
       iInStream = read(INSTREAMBUF);\
       iInStreamPos = 1;\
   }\
   if (iInStreamPos<=iInStream.size()) {\
      strcpy((char*)val, (char*)&iInStream[iInStreamPos]);\
      iInStreamPos += strlen((char*)val)+1;\
   } else\
      *val = 0;\
    return *this;\
}
DEFSHIFTRIGHTSTR (char)
DEFSHIFTRIGHTSTR (signed char)
DEFSHIFTRIGHTSTR (unsigned char)
ZExport (ZAsyncDevice &) ZAsyncDevice::operator>> (ZString & val)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::operator>>(ZString &val)");
  if (iInStreamPos > iInStream.size ())
      {
        val = read (INSTREAMBUF);
        return *this;
      }
  if (iInStreamPos <= iInStream.size ())
      {
        val = iInStream.subString (iInStreamPos);
        iInStreamPos += iInStream.size () + 1;
      }
  else
    val = "";
  return *this;
}                               // operator>>(ZString &val)

ZExport (ZBoolean) ZAsyncDevice::readChar (char &ch)
{
  ZFUNCTRACE_DEVELOP ("ZAsyncDevice::readChar(char& ch)");
  if (iInStreamPos > iInStream.size ())
      {
        iInStream = read (INSTREAMBUF);
        iInStreamPos = 1;
      }
  if (iInStreamPos <= iInStream.size ())
      {
        ch = iInStream[iInStreamPos++];
        return zTrue;
      }                         // if
  return zFalse;
}                               // readChar
