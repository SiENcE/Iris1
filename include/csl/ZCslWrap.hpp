/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZCslWrap.hpp
 * Application :  IBK Open Class Library
 * Purpose     :  CSL wraper C++ API for kernel
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.06.01  First implementation                        P.Koch, IBK
 * 2001.06.29  Cast return value of ZCslWrapFunction::     P.Koch, IBK
 *             funcAddr to void* (gcc v3.0 complains)
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.28  Enhanced trace facilities                   P.Koch, IBK
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

#ifndef _ZCSLWRAP_
#define _ZCSLWRAP_

#include <csl/ZCslApi.h>
#include <csl/ZString.hpp>
#include <csl/ZDate.hpp>
#include <stdarg.h>

#ifndef ZC_BUILDING_ZCSLWRAP
  #ifdef ZC_IBM
    #pragma library("ZCslWrpI.lib")
  #endif
  #ifdef ZC_MICROSOFT
    #pragma comment(lib,"ZCslWrpM.lib")
  #endif
  #ifdef ZC_BORLAND
    #pragma comment(lib,"ZCslWrpB.lib")
  #endif
  #define ZCslWrpLink0 ZImport0
  #define ZCslWrpLink(type) ZImport(type)
#else
  #define ZCslWrpLink0 ZExport0
  #define ZCslWrpLink(type) ZExport(type)
#endif

#define ZCsl ZCslWrap

class ZCsl;

class ZCslWrapFunction : public ZBase
{
   public:
      ZCslWrapFunction(
         const ZString& aFileName,
         const ZString& aFuncName,
         ZString (*iFunc)(ZCsl*),
         ZCslWrapFunction* iNext
      );

      ~ZCslWrapFunction();

      ZString fileName() const { return iFileName; }
      ZString funcName() const { return iFuncName; }
      void* funcAddr() { return (void*)iFuncAddr; }
      ZCslWrapFunction* next() const { return iNext; }

   private:
      ZString iFileName;
      ZString iFuncName;
      ZString (*iFuncAddr)(ZCsl*);
      ZCslWrapFunction* iNext;
}; // ZCslWrapFunction

class ZCsl : public ZBase
{
   public:
      enum Flags {
         disInclude     = 0001,          // disable #include
         disLoadScript  = 0002,          // disable #loadScript
         disLoadLibrary = 0004,          // disable #loadLibrary
         disList        = 0010,          // disable #list
         disLogFile     = 0020,          // disable #logFile
         disIf          = 0040,          // disable #if ... #else ... #endif
         disError       = 0100,          // disable #error
         disMessage     = 0200,          // disable #message
         disAll         = 0377           // disable all
      };

      enum TraceMode {
         traceNone      = 0000,          // nothing
         traceCode      = 0001,          // pcode
         traceFuncs     = 0002,          // functions
         traceBlks      = 0004,          // blocks
         traceMsgs      = 0010,          // messages
         traceInfo      = 0016,          // functions, blocks and msgs (def)
         traceAll       = 0017           // everything
      };

      enum ShowMode {
         showFunctions,                  // show all functions
         showCallStack,                  // show function calls
         showFullStack,                  // show function calls & locals
         showGlobals,                    // show global identifiers
         showLibraries                   // show loaded functions
      };

      ZCslWrpLink0
         ZCsl(                           // constructor
            int aFlags = 0);                // flags

      ZCslWrpLink0
         ZCsl(                           // constructor
            ZCslHandle aHandle);            // existing handle

      ZCslWrpLink0
         ~ZCsl();                        // destructor

      ZCslWrpLink(ZCsl&)
         set(                            // set var or const value
            const ZString& aVarName,        // variable name
            const ZString& aValue = "");    // new value

      ZCslWrpLink(ZString)
         get(                            // get var or const value
            const ZString& aVarName);       // variable name

      ZCslWrpLink(long)
         varSizeof(                      // get size of variable
            const ZString& aVarName);       // name of variable

      ZCslWrpLink(ZCsl&)
         varResize(                      // resize variable
            const ZString& aVarName);       // name and new layout of variable

      ZCslWrpLink(ZCsl&)
         loadScript(                     // load script from any istream
            const ZString& aFileName,       // file/module name for script
            istream* aStr);                 // input stream

      ZCslWrpLink(ZCsl&)
         loadScript(                     // load script from file
            const ZString& aFileName);      // file name to load

      ZCslWrpLink(ZCsl&)
         loadLibrary(                    // load a dll
            const ZString& aDllName);       // dll filename

      ZCslWrpLink(ZString)
         call(                           // call function
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            long aArgCount = 0,             // # of arguments
            char** aParam = 0,              // parameter list
            long* aSize = 0);               // parameter length's (-1 = asciz)

      ZCslWrpLink(ZString)
         call(                           // call function with string args only
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            int aArgCount,                  // # of arguments
            ...);                           // arguments as char*

      ZCslWrpLink(ZString)
         callEx(                         // call function with string or binary args
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            int aArgCount = 0,              // # of arguments
            ...);                           // arguments as pairs of char*/long

      ZCslWrpLink(ZCsl&)
         addFunc(                        // add a function
            const ZString& aFileName,       // file/module name for function
            const ZString& aFuncHeader,     // function header
            ZString (*aFunc)(ZCsl* aCsl));  // function address

      ZCslWrpLink(ZCsl&)
         addVar(                         // add a local var/const
            const ZString& aVarName,        // name and layout
            const ZString& aInitValue="",   // initial value
            ZBoolean aIsConst = zFalse);    // false=var, true=const

      ZCslWrpLink(ZDateTime)
          startDateTime() const;         // get start date & time

      ZCslWrpLink(TraceMode)
         traceMode() const;              // query current trace mode

      ZCslWrpLink(ZCsl&)
         setTraceMode(                   // set trace mode
            TraceMode aMode);               // new trace mode

      ZCslWrpLink(ZCsl&)
         trace(                          // provide runtime trace info
            const ZString& aMessage);       // message to trace

      ZCslWrpLink(ZCsl&)
         show(                           // show internal informations
            ShowMode aMode,                 // show mode
            long aDepth = 99999999);        // how much

      void
         enableErrorCheck(               // enable/disable err check
            ZBoolean aEnable = zTrue)
         { iErrorCheck = aEnable; }

      static ZCslWrpLink(ZCsl*)
         findWrapObject(                 // find wrap object
            ZCslHandle aHandle);

      static ZCslWrpLink(ZCsl*)
         createWrapObject(               // find or create wrap object
            ZCslHandle aHandle);

      ZCslWrpLink(ZCslWrapFunction*)
         findFunction(                   // function list
            const ZString& aFileName,
            const ZString& aFuncName);

      static ZCsl* wrapList() { return iWrapList; }
      static void setWrapList(ZCsl* aCsl) { iWrapList = aCsl; }

   private:
      void checkErrors(long aErrCount) const;  // throw ZException on errors

      static ZCsl* iWrapList;            // list of active wrapper objects

      ZCslHandle iCslHandle;             // C API handle
      ZBoolean iErrorCheck;              // error checking is enabled
      ZBoolean iCloseHandle;             // handle must mbe closed on delete
      ZCslWrapFunction* iFuncList;       // function list
      ZCsl* iNext;                       // next object in list
}; // ZCsl

#undef  ZCslInitLib
#define ZCslInitLib(var) \
\
void ZCslWrapInitLib(ZCsl*);\
\
extern "C" {\
  ZExportAPI(void) ZCslInitLibAPI(ZCslHandle aCsl)\
  {\
     ZCslWrapInitLib(ZCsl::createWrapObject(aCsl));\
  }\
}\
\
void ZCslWrapInitLib(ZCsl* var)

#undef  ZCslCleanupLib
#define ZCslCleanupLib(var) \
\
void ZCslWrapCleanupLib(ZCsl*);\
\
extern "C" {\
  ZExportAPI(void) ZCslCleanupLibAPI(ZCslHandle aCsl)\
  {\
     ZCslWrapCleanupLib(ZCsl::findWrapObject(aCsl));\
  }\
}\
\
void ZCslWrapCleanupLib(ZCsl* var)

#endif // _ZCSLWRAP_
