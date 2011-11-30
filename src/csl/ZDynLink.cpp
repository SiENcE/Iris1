/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZDynLink.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  ZDynamicLinkLibrary implements access to DLL or shared
 *                libraries respectively.
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

#include <iostream>

#include <ZDynLink.hpp>
#include <ZExcept.hpp>
#include <ZPlatfrm.hpp>

#if ZC_WIN
#define NO_STRICT
#include <windows.h>

#define THROWSYSERR(aMsg) ZTHROWEXC(ZString(aMsg)+": "+getWinError())

static ZString getWinError ()
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
#endif // ZC_WIN

#if ZC_OS2
#define INCL_DOSMODULEMGR
#include <os2.h>
#endif // ZC_OS2

#ifdef ZC_UNIXFAM
#include <dlfcn.h>
typedef void *HMODULE;
#endif // ZC_UNIXFAM

// #include "../Fluid/mmgr.h"

//////////////////////////////// ZDllModule ///////////////////////////////////

class ZDllModule:public ZBase
{
public:
  // constructors/destructor
  ZDllModule (const char *aName, ZDllModule * aNext);
   ~ZDllModule ();

  // queries
  ZString name () const
  {
    return iName;
  }
  ZString fullName ();

  // reference counting
  void addRef ()
  {
    iUseCount++;
  }
  void removeRef ()
  {
    iUseCount--;
  }
  unsigned long useCount () const
  {
    return iUseCount;
  }

  // opening and closing
  void open ();
  void close ();

  // list pointers
  ZDllModule *next () const
  {
    return iNext;
  }
  void setNext (ZDllModule * aNext)
  {
    iNext = aNext;
  }

private:
  friend class ZDynamicLinkLibrary;
  ZString iName;                // module name
  HMODULE iHandle;              // module handle
  unsigned long iUseCount;      // reference count
  ZBoolean iOpen;               // is opened
  ZDllModule *iNext;            // next in list chain
};                              // ZDllModule

ZDllModule::ZDllModule (const char *aName, ZDllModule * aNext):iName (aName),
iHandle (0), iUseCount (0), iOpen (zFalse), iNext (aNext)
{
  ZFUNCTRACE_DEVELOP
    ("ZDllModule::ZDllModule(const char* aName, ZDllModule* aNext)");
}                               // ZDllModule

ZDllModule::~ZDllModule ()
{
  ZFUNCTRACE_DEVELOP ("ZDllModule::~ZDllModule()");
  close ();
}                               // ZDllModule

ZString ZDllModule::fullName ()
{
  ZFUNCTRACE_DEVELOP ("ZDllModule::fullName()");
  open ();
#ifdef ZC_OS2
  char name[128];
  unsigned long rc = DosQueryModuleName (iHandle, sizeof (name), name);
  if (rc)
    ZTHROWEXC ("DosQueryModuleName error " + ZString (rc));
  return name;
#endif // ZC_OS2
#ifdef ZC_WIN
  char name[128];
  if (!GetModuleFileName (iHandle, name, sizeof (name)))
    THROWSYSERR ("GetModuleFileName");
  return name;
#endif // ZC_WIN
#ifdef ZC_UNIXFAM
  return iName;                 // n/a under linux
#endif // ZC_UNIXFAM

}                               // fullName

void ZDllModule::open ()
{
  ZFUNCTRACE_DEVELOP ("ZDllModule::open()");
  if (!iOpen)
      {
#if ZC_OS2
        char nameBuf[128];
        unsigned long rc =
          DosLoadModule (nameBuf, sizeof (nameBuf), iName, &iHandle);
        if (rc != 0 || iHandle == 0)
          ZTHROWEXC ("DosLoadModule error " + ZString (rc) + " on " + iName);
#endif // ZC_OS2
#if ZC_WIN
        // Suppress the error message box that appears when the dll load fails
        //      unsigned int errMode(SetErrorMode(SEM_FAILCRITICALERRORS));
#ifdef ZC_GNU
        iHandle = LoadLibrary (iName.constBuffer ());
#else
        iHandle = LoadLibrary (iName);
#endif
        if (iHandle == 0)
            {
              unsigned long rc = GetLastError ();
              if (rc)
                  {
//            SetErrorMode(errMode);
                    THROWSYSERR ("LoadLibrary(" + iName + ")");
                  }             // if
            }                   // if
//      SetErrorMode(errMode);
#endif // ZC_WIN
#if ZC_UNIXFAM
        dlerror ();             // clear last error
        iHandle = dlopen (iName.constBuffer (), RTLD_NOW);
        if (iHandle == 0)
          ZTHROWEXC (dlerror ());
#endif // ZC_UNIXFAM
        iOpen = zTrue;
      }                         // if
}                               // open

void ZDllModule::close ()
{
  ZFUNCTRACE_DEVELOP ("ZDllModule::close()");
  if (iOpen)
      {
#if ZC_OS2
        unsigned long rc = DosFreeModule (iHandle);
        if (rc)
          ZTHROWEXC ("DosFreeModule error " + ZString (rc) + " on " + iName);
#endif // ZC_OS2
#if ZC_WIN
        if (!FreeLibrary (iHandle) && !ZPlatform::isWin98 ())
          THROWSYSERR ("FreeLibrary(" + iName + ")");
#endif // ZC_WIN
#if ZC_UNIXFAM
        dlclose (iHandle);
#endif // ZC_UNIXFAM
        iHandle = 0;
        iOpen = zFalse;
      }                         // if
}                               // close

static ZDllModule *iModuleList = 0; // root of module list

//////////////////////////// ZDynamicLinkLibrary /////////////////////////////

ZExport0 ZDynamicLinkLibrary::
ZDynamicLinkLibrary (const char *aName):iModule (0)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::ZDynamicLinkLibrary(const char* aName)");

  // search if allready in list
  for (ZDllModule * m = iModuleList; m; m = m->next ())
    if (m->name () == aName)
        {
          iModule = m;
          break;
        }                       // if

  // if not, prepend to list
  if (iModule == 0)
    iModule = iModuleList = new ZDllModule (aName, iModuleList);

  iModule->addRef ();
}                               // ZDynamicLinkLibrary

ZExport0 ZDynamicLinkLibrary::
ZDynamicLinkLibrary (const ZDynamicLinkLibrary & aDll):iModule (aDll.iModule)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::ZDynamicLinkLibrary(const ZDynamicLinkLibrary& aDll)");
  iModule->addRef ();
}                               // ZDynamicLinkLibrary

ZExport0 ZDynamicLinkLibrary::~ZDynamicLinkLibrary ()
{
  ZFUNCTRACE_DEVELOP ("ZDynamicLinkLibrary::~ZDynamicLinkLibrary()");
  dropModule ();
}                               // ~ZDynamicLinkLibrary

ZExport (ZDynamicLinkLibrary &) ZDynamicLinkLibrary::
operator= (const ZDynamicLinkLibrary & aDll)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::operator=(const ZDynamicLinkLibrary& aDll)");
  if (this != &aDll)
      {
        dropModule ();
        iModule = aDll.iModule;
        iModule->addRef ();
      }                         // if
  return *this;
}                               // operator =

ZExport (void *) ZDynamicLinkLibrary::procAddress (const char *aProcName)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::procAddress(const char* aProcName)");
  iModule->open ();
#if ZC_OS2
  int (_System * procAddr) ();
  unsigned long rc =
    DosQueryProcAddr (iModule->iHandle, 0, aProcName, &procAddr);
  if (rc)
    ZTHROWEXC ("DosQueryProcAddr error " + ZString (rc) + " on " + aProcName);
#endif
#if ZC_WIN
  FARPROC procAddr = GetProcAddress (iModule->iHandle, aProcName);
  if (!procAddr)
    THROWSYSERR ("GetProcAddress(" + ZString (aProcName) + ")");
#endif
#if ZC_UNIXFAM
  dlerror ();                   // clear error code
  void *procAddr = dlsym (iModule->iHandle, aProcName);
  const char *err = dlerror ();
  if (err != 0)
    ZTHROWEXC (err);
#endif
  return (void *) procAddr;
}                               // procAddress

ZExport (void *) ZDynamicLinkLibrary::procAddress (unsigned long aProcOrdinal)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary :: procAddress(unsigned long aProcOrdinal)");
  iModule->open ();
#if ZC_OS2
  int (_System * procAddr) ();
  unsigned long rc =
    DosQueryProcAddr (iModule->iHandle, aProcOrdinal, 0, &procAddr);
  if (rc)
    ZTHROWEXC ("DosQueryProcAddr error " + ZString (rc) + " on #" +
               ZString (aProcOrdinal));
#endif
#if ZC_WIN
  long ordinal (MAKELONG (LOWORD (aProcOrdinal), 0));
  FARPROC procAddr = GetProcAddress (iModule->iHandle, (char *) ordinal);
  if (!procAddr)
    THROWSYSERR ("GetProcAddress (" + ZString (aProcOrdinal) + ")");
#endif
#if ZC_UNIXFAM
  dlerror ();                   // clear error code
  void *procAddr =
    dlsym (iModule->iHandle,
           ZString ("@" + ZString (aProcOrdinal)).constBuffer ());
  const char *err = dlerror ();
  if (err != 0)
    ZTHROWEXC (err);
#endif
  return (void *) procAddr;
}                               // procAddress

ZExport (ZBoolean) ZDynamicLinkLibrary::
isEntryPoint32Bit (const char *aProcName)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::isEntryPoint32Bit(const char *aProcName)");
#if ZC_OS2
  iModule->open ();
  unsigned long result (1);
  unsigned long rc =
    DosQueryProcType (iModule->iHandle, 0, aProcName, &result);
  if (rc)
    ZTHROWEXC ("DosQueryProcType error " + ZString (rc) + " on " + aProcName);
  return (ZBoolean) result;
#else
  return zTrue;
#endif
}                               // isEntryPoint32Bit

ZExport (ZBoolean) ZDynamicLinkLibrary::
isEntryPoint32Bit (unsigned long aProcOrdinal)
{
  ZFUNCTRACE_DEVELOP
    ("ZDynamicLinkLibrary::isEntryPoint32Bit(unsigned long aProcOrdinal)");
#if ZC_OS2
  iModule->open ();
  unsigned long result (1);
  unsigned long rc =
    DosQueryProcType (iModule->iHandle, aProcOrdinal, 0, &result);
  if (rc)
    ZTHROWEXC ("DosQueryProcType error " + ZString (rc) + " on #" +
               ZString (aProcOrdinal));
  return (ZBoolean) result;
#else
  return zTrue;
#endif
}                               // isEntryPoint32Bit

ZExport (ZString) ZDynamicLinkLibrary::name () const
{
  ZFUNCTRACE_DEVELOP ("ZDynamicLinkLibrary::name() const");
  return iModule->name ();
}                               // name

ZExport (ZString) ZDynamicLinkLibrary::fullName () const
{
  ZFUNCTRACE_DEVELOP ("ZDynamicLinkLibrary::fullName() const");
  return iModule->fullName ();
}                               // fullName

void ZDynamicLinkLibrary::dropModule ()
{
  ZFUNCTRACE_DEVELOP ("ZDynamicLinkLibrary::dropModule()");
  iModule->removeRef ();
  if (iModule->useCount () == 0)
      {
        if (iModuleList == iModule)
          iModuleList = iModule->next ();
        else
            {
              ZDllModule *p = iModuleList;
              for (ZDllModule * m = p->next (); m; p = m, m = m->next ())
                if (m == iModule)
                    {
                      p->setNext (m->next ());
                      break;
                    }           // if
            }                   // if
        delete iModule;
      }                         // if
}                               // dropModule
