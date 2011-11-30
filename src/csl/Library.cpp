/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Library.cpp
 * Application :  CSL kernel
 * Purpose     :  Library
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.22  First implementation                        P.Koch, IBK
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
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

#ifndef ZC_BUILDING_ZCSL
#define ZC_BUILDING_ZCSL
#endif

#include <ZCsl.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"

ZCsl::Library::Library (ZCsl * aParent, const ZString & aDllName,
                        Library * aPrev):iPrev (aPrev), iParent (aParent)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Library::Library(ZCsl* aParent, const ZString& aDllName, Library* aPrev)");
  try
  {
    iDll = new ZDynamicLinkLibrary (aDllName);
    ZString (iDll->fullName ());    // force dll open
  }                             // try
  catch (const ZException & exc)
  {
    iParent->throwExcept (msgErrLoadingLib + aDllName);
  }                             // catch
  ZBoolean found (zFalse);
  try
  {
#ifdef ZC_WIN
    initApi (found);
#else
    initNative (found);
#endif
  }                             // try
  catch (const ZException & exc)
  {
    if (found)
      throw;
    try
    {
#ifdef ZC_WIN
      initNative (found);
#else
      initApi (found);
#endif
    }                           // try
    catch (const ZException & exc)
    {
      if (found)
          {
            ZString msg (exc[0]);
            for (int i = 1; i < exc.count (); i++)
              msg += "\r\n" + exc[i];
            iParent->throwExcept (msg);
          }                     // if
      iParent->throwExcept (msgInitialNotFnd + aDllName);
    }                           // catch
  }                             // catch
}                               // Library

ZCsl::Library::~Library ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Library::~Library()");
  ZBoolean found (zFalse);
  try
  {
    if (iNative)
      cleanupNative (found);
    else
      cleanupApi (found);
  }                             // try
  catch (const ZException & exc)
  {
    if (found)
        {
          ZString msg (exc[0]);
          for (int i = 1; i < exc.count (); i++)
            msg += "\r\n" + exc[i];
          iParent->throwExcept (msg);
        }                       // if
    iParent->throwExcept (msgCleanupNotFnd + iDll->name ());
  }                             // catch
  delete iDll;
  if (iPrev)
    delete iPrev;
}                               // ~Library

void ZCsl::Library::initApi (ZBoolean & aFound)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Library::initApi(ZBoolean& aFound)");

  // find procedure address
  void (ZFuncptrAPI initialize) (ZCsl *);
  initialize =
    (void (ZFuncptrAPI) (ZCsl *)) iDll->procAddress ("ZCslInitLibAPI");
  aFound = zTrue;

  // prepare C API vars
  ZException & errs = iParent->errs ();
  errs.drop ();
  iParent->result () = "";

  // call function
  ZTRACE_DEVELOP ("iParent = " + ZString::hex ((unsigned long) iParent));
  (*initialize) (iParent);

  // throw exception if error occured
  if (errs.count ())
    throw errs;
  iNative = zFalse;
}                               // initApi

void ZCsl::Library::initNative (ZBoolean & aFound)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Library::initNative(ZBoolean& aFound)");

  // find procedure address
  void (*initialize) (ZCsl *);
  initialize = (void (*)(ZCsl *)) iDll->procAddress (
#ifdef ZC_IBM
                                                      "ZCslInitLibNative__FP4ZCsl"
#endif
#ifdef ZC_BORLAND
                                                      "@ZCslInitLibNative$qp4ZCsl"
#endif
#ifdef ZC_MICROSOFT
                                                      "?ZCslInitLibNative@@YAXPAVZCsl@@@Z"
#endif
#ifdef ZC_GNU
                                                      "ZCslInitLibNative__FP4ZCsl"
#endif
    );
  aFound = zTrue;
  (*initialize) (iParent);
  iNative = zTrue;
}                               // initNative

void ZCsl::Library::cleanupApi (ZBoolean & aFound)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Library::cleanupApi(ZBoolean& aFound)");

  // find procedure address
  void (ZFuncptrAPI cleanup) (ZCsl *);
  cleanup =
    (void (ZFuncptrAPI) (ZCsl *)) iDll->procAddress ("ZCslCleanupLibAPI");
  aFound = zTrue;

  // prepare C API vars
  ZException & errs = iParent->errs ();
  errs.drop ();
  iParent->result () = "";

  // call function
  (*cleanup) (iParent);

  // throw exception if error occured
  if (errs.count ())
    throw errs;
}                               // cleanupApi

void ZCsl::Library::cleanupNative (ZBoolean & aFound)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Library::cleanupNative(ZBoolean& aFound)");

  // find procedure address
  void (*cleanup) (ZCsl *);
  cleanup = (void (*)(ZCsl *)) iDll->procAddress (
#ifdef ZC_IBM
                                                   "ZCslCleanupLibNative__FP4ZCsl"
#endif
#ifdef ZC_BORLAND
                                                   "@ZCslCleanupLibNative$qp4ZCsl"
#endif
#ifdef ZC_MICROSOFT
                                                   "?ZCslCleanupLibNative@@YAXPAVZCsl@@@Z"
#endif
#ifdef ZC_GNU
                                                   "ZCslCleanupLibNative__FP4ZCsl"
#endif
    );
  aFound = zTrue;
  (*cleanup) (iParent);
}                               // cleanupNative
