/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZResrce.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Resource locking classes:
 *                ZPrivateResource:
 *                  Resource for serialization within current process.
 *                ZSharedResource:
 *                  Resource for serialization of multiple processes.
 *                ZResourceLock:
 *                  Utility lock class to manage lock/unlock in a block of
 *                  code in conjunction with ZPrivateResource or
 *                  ZSharedResource.
 *                ZResource:
 *                  Virtual class ZPrivateResource and ZSharedResource are
 *                  derived of. Not intended for application use.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.21  First implementation                        P.Koch, IBK
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

#include <ZResrce.hpp>
#include <ZPlatfrm.hpp>
#include <ZExcept.hpp>

#ifdef ZC_WIN
#include <windows.h>
#endif

// #include "../Fluid/mmgr.h"

static const char TimedOut[] = "Timed out";

#define throwTimeout()\
{\
   ZResourceLockTimeout exc(TimedOut);\
   ZTHROW(exc);\
}

#ifdef ZC_OS2

#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>

static const char WaitForSingleObjectError[] = "WaitForSingleObject error ";
static const char DosCreateMutexSemError[] = "DosCreateMutexSem error ";
static const char DosRequestMutexSemError[] = "DosRequestMutexSem error ";
static const char DosReleaseMutexSemError[] = "DosReleaseMutexSem error ";
static const char DosOpenMutexSemError[] = "DosOpenMutexSem error ";
static const char DosCloseMutexSemError[] = "DosCloseMutexSem error ";

#define throwErr(aMsg) ZTHROWEXC(aMsg+ZString(rc))

#endif

#ifdef ZC_WIN

static const char WaitForSingleObjectError[] = "WaitForSingleObject error ";
static const char OpenMutexError[] = "OpenMutex error ";
static const char CreateMutexError[] = "CreateMutex error ";
static const char ReleaseMutexError[] = "ReleaseMutex error ";
static const char CloseHandleError[] = "CloseHandle error ";

#define throwErr(aMsg) ZTHROWEXC(aMsg+ZString(GetLastError()))

#endif

///////////////////////////// ZResource Implemention /////////////////////////

ZResource::ZResource ():
iReslockGate (0), iRefCount (0)
{
  ZFUNCTRACE_DEVELOP ("ZResource::ZResource()");
}                               // ZResource

ZResource::~ZResource ()
{
  ZFUNCTRACE_DEVELOP ("ZResource::~ZResource()");
  if (iReslockGate)
    delete iReslockGate;
}                               // ~ZResource

ZExport (ZResource &) ZResource::lock (long aTimeOut)
{
  ZFUNCTRACE_DEVELOP ("ZResource::lock(long aTimeOut)");
  if (iReslockGate == 0)
    iReslockGate = new ZResourceLock (this, aTimeOut);
  else
    iReslockGate->setLock (aTimeOut);
  return *this;
}                               // lock

ZExport (ZResource &) ZResource::unlock ()
{
  ZFUNCTRACE_DEVELOP ("ZResource::unlock()");
  if (iReslockGate != 0)
    iReslockGate->clearLock ();
  return *this;
}                               // unlock

/////////////////////////// ZResourceLock Implemention ///////////////////////

ZResourceLock::ZResourceLock (ZResource * aResource, long aTimeOut):
iResource (aResource)
{
  ZFUNCTRACE_DEVELOP
    ("ZResourceLock::ZResourceLock(ZResource* aResource, long aTimeOut)");
  setLock (aTimeOut);
}                               // ZResourceLock

ZResourceLock::~ZResourceLock ()
{
  ZFUNCTRACE_DEVELOP ("ZResourceLock::~ZResourceLock()");
  if (iResource->iRefCount)
    clearLock ();
}                               // ~IResourceLock

ZResourceLock & ZResourceLock::setLock (long aTimeOut)
{
  ZFUNCTRACE_DEVELOP ("ZResourceLock::setLock(long aTimeOut)");
  unsigned long rc;
  if (iResource->handle ())
      {
#ifdef ZC_OS2
        rc = DosRequestMutexSem ((HMTX) iResource->handle (), aTimeOut);
        if (rc == ERROR_TIMEOUT)
            {
              throwTimeout ();
            }
        else if (rc)
          throwErr (DosRequestMutexSemError);
#endif
#ifdef ZC_WIN
        rc = WaitForSingleObject (iResource->handle (), aTimeOut);
        if (rc == WAIT_TIMEOUT)
            {
              throwTimeout ();
            }
        else
          if (rc == WAIT_FAILED ||
              (rc != WAIT_OBJECT_0 && rc != WAIT_ABANDONED))
          throwErr (WaitForSingleObjectError);
#endif
      }                         // if
  iResource->iRefCount++;
  return *this;
}                               // setLock

ZResourceLock & ZResourceLock::clearLock ()
{
  ZFUNCTRACE_DEVELOP ("ZResourceLock::clearLock()");
  iResource->iRefCount--;
  if (iResource->handle ())
      {
#ifdef ZC_OS2
        unsigned long rc (0);
        rc = DosReleaseMutexSem ((HMTX) iResource->handle ());
        if (rc)
            {
              iResource->iRefCount++;
              throwErr (DosReleaseMutexSemError);
            }                   // if
#endif
#ifdef ZC_WIN
        if (!ReleaseMutex (iResource->handle ()))
            {
              iResource->iRefCount++;
              throwErr (ReleaseMutexError);
            }                   // if
#endif
      }                         // if
  return *this;
}                               // clearLock

///////////////////////// ZPrivateResource Implemention //////////////////////

ZExport0 ZPrivateResource::ZPrivateResource ():iResourceHandle (0)
{
  ZFUNCTRACE_DEVELOP ("ZPrivateResource::ZPrivateResource()");
#ifdef ZC_OS2
  unsigned long rc (0);
  rc = DosCreateMutexSem (0, (PHMTX) & iResourceHandle, 0, 0);
  if (rc)
    throwErr (DosCreateMutexSemError);
#endif
#ifdef ZC_WIN
  iResourceHandle = CreateMutex (0, 0, 0);
  if (iResourceHandle == 0)
    throwErr (CreateMutexError);
#endif
}                               // ZPrivateResource

ZPrivateResource::~ZPrivateResource ()
{
  ZFUNCTRACE_DEVELOP ("ZPrivateResource::~ZPrivateResource()");
  if (iReslockGate)
      {
        delete iReslockGate;
        iReslockGate = 0;
      }                         // if
  if (iResourceHandle)
      {
#ifdef ZC_OS2
        unsigned long rc (0);
        rc = DosCloseMutexSem ((HMTX) iResourceHandle);
        if (rc)
          throwErr (DosCloseMutexSemError);
#endif
#ifdef ZC_WIN
        if (!CloseHandle (iResourceHandle))
          throwErr (CloseHandleError);
#endif
      }                         // if
}                               // ~ZPrivateResource

///////////////////////// ZSharedResource Implemention ///////////////////////

ZSharedResource::
ZSharedResource (const ZString & aKeyName):iResourceHandle (0),
#ifdef ZC_OS2
iKeyName ("\\SEM32\\" + aKeyName)
#else
iKeyName (aKeyName)
#endif
{
  ZFUNCTRACE_DEVELOP
    ("ZSharedResource::ZSharedResource(const ZString& aKeyName)");
  unsigned long rc;
#ifdef ZC_OS2
  rc = DosCreateMutexSem ((char *) iKeyName, (PHMTX) & iResourceHandle, 0, 0);
  if (rc == ERROR_DUPLICATE_NAME)
      {
        rc = DosOpenMutexSem ((char *) iKeyName, (PHMTX) & iResourceHandle);
        if (rc)
          throwErr (DosOpenMutexSemError);
      }
  else if (rc)
    throwErr (DosCreateMutexSemError);
#endif
#ifdef ZC_WIN
  if (ZPlatform::isWinNTFamily ())
      {
        SECURITY_DESCRIPTOR sd;
        SECURITY_ATTRIBUTES sa;
        InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl (&sd, TRUE, (PACL) NULL, FALSE);
        sa.nLength = sizeof (sa);
        sa.lpSecurityDescriptor = &sd;
        sa.bInheritHandle = TRUE;
        iResourceHandle = CreateMutex (&sa, 0, (char *) iKeyName);
      }
  else
    iResourceHandle = CreateMutex (0, 0, (char *) iKeyName);

  if (iResourceHandle == 0)
      {
        rc = GetLastError ();
        if (rc == ERROR_ALREADY_EXISTS)
            {
              iResourceHandle =
                OpenMutex (MUTEX_ALL_ACCESS, 1, (char *) iKeyName);
              if (iResourceHandle == 0)
                throwErr (OpenMutexError);
            }
        else if (rc != 0)
          throwErr (CreateMutexError);
      }                         // if
#endif
}                               // ZSharedResource

ZSharedResource::~ZSharedResource ()
{
  ZFUNCTRACE_DEVELOP ("ZSharedResource::~ZSharedResource()");
  if (iReslockGate)
      {
        delete iReslockGate;
        iReslockGate = 0;
      }                         // if

  if (iResourceHandle)
      {
#ifdef ZC_OS2
        unsigned long rc (0);
        rc = DosCloseMutexSem ((HMTX) iResourceHandle);
        if (rc)
          throwErr (DosCloseMutexSemError);
#endif
#ifdef ZC_WIN
        if (!CloseHandle (iResourceHandle))
          throwErr (CloseHandleError);
#endif
      }                         // if
}                               // ~ZSharedResource

ZEXCLASSIMPLEMENT (ZResourceLockTimeout, ZException)
