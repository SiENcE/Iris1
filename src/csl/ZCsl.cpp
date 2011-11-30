/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZCsl.cpp
 * Application :  CSL kernel
 * Purpose     :  Main module
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.21  First implementation                        P.Koch, IBK
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.28  Enhanced trace facilities                   P.Koch, IBK
 * 2001.08.16  Reloading same library was not prevented    P.Koch, IBK
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

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ZCsl.hpp>
#include <ZFile.hpp>

#if defined( ZC_GNU) && !defined( WIN32)
  #include <strstream>
#elif defined( WIN32)
  #include <strstream>
#else
  #include <strstrea.h>
#endif

#if ZC_UNIXFAM
  #include <sys/stat.h>
#else
  #include <sys\stat.h>
#endif

#include "Internal.h"

// #include "../Fluid/mmgr.h"

using namespace std;

ZExport0 ZCsl::ZCsl(int aFlags) :
   iDlls(0),
   iStats(0),
   iCalls(0),
   iTraceMode(traceNone),
   iTraceLevel(0),
   iFlags(aFlags),
   iState(isNormal),
   iList(zFalse),
   iInDirective(zFalse),
   iInput(0)
{
   ZFUNCTRACE_DEVELOP("ZCsl::ZCsl(int aFlags)");
   iBlock = new Block(this,0);
   iStack = iTos = new ZString[iStackSize = STACKCHUNK];
   iStackLimit = &iStack[iStackSize-1];
   *iTos = "<stack bottom>";
   openStat("ZCsl");

   iBlock->addVar("true", "1", zTrue, zFalse, zTrue);
   iBlock->addVar("false", "0", zTrue, zFalse, zTrue);

   iBlock->addVar("MAXLONG", ZString((long)ZBase::maxLong), zTrue, zFalse, zTrue);
   iBlock->addVar("PATHSEPARATOR", ZString(ZC_PATHSEPARATOR), zTrue, zFalse, zTrue);

   char buf[10];
   sprintf(buf, "%d.%02d", CSL_MAJOR_VERSION, CSL_MINOR_VERSION);
   iBlock->addVar("cslVersion", buf, zTrue, zFalse, zTrue);
   iBlock->addVar("cslCompiler", ZC_COMPILER, zTrue, zFalse, zTrue);
   iBlock->addVar("cslBuilt", ZString(__DATE__)+" "+__TIME__, zTrue, zFalse, zTrue);
#ifdef ZC_CSS_COMPATIBLE
   // these are for compatibility with CSS
   iBlock->addVar("cssVersion", buf, zTrue, zFalse, zTrue);
   iBlock->addVar("cssCompiler", ZC_COMPILER, zTrue, zFalse, zTrue);
   iBlock->addVar("cssBuilt", ZString(__DATE__)+" "+__TIME__, zTrue, zFalse, zTrue);
#endif
} // ZCsl

ZExport0 ZCsl::~ZCsl()
{
   ZFUNCTRACE_DEVELOP("ZCsl::~ZCsl()");
   if (iDlls) delete iDlls;
   delete iBlock;
   if (iStats) delete iStats;
   if (iCalls) delete iCalls;
   if (iInput) delete iInput;
   delete [] iStack;
} // ~ZCsl

ZExport(ZCsl&) ZCsl::set(const ZString& aVarName, const ZString& aValue)
{
   ZFUNCTRACE_DEVELOP("ZCsl::set(const ZString& aVarName, const ZString& aValue)");
   Variable *v;
   if (iCalls)
      v = iCalls->findVar(aVarName);
   else
      v = iBlock->findVar(aVarName);
   v->set(aValue);
   return *this;
} // set

ZExport(ZString) ZCsl::get(const ZString& aVarName)
{
   ZFUNCTRACE_DEVELOP("ZCsl::get(const ZString& aVarName)");
   Variable *v;
   if (iCalls)
      v = iCalls->findVar(aVarName);
   else
      v = iBlock->findVar(aVarName);
   return v->value();
} // get

ZExport(long) ZCsl::varSizeof(const ZString& aVarName)
{
   ZFUNCTRACE_DEVELOP("ZCsl::varSizeof(const ZString& aVarName)");
   Variable *v;
   if (iCalls)
      v = iCalls->findVar(aVarName);
   else
      v = iBlock->findVar(aVarName);
   return v->iSize;
} // varSizeOf

ZExport(ZCsl&) ZCsl::varResize(const ZString& aVarName)
{
   ZFUNCTRACE_DEVELOP("ZCsl::varResize(const ZString& aVarName)");
   Variable *v;
   if (iCalls)
      v = iCalls->findVar(pureVarName(aVarName));
   else
      v = iBlock->findVar(pureVarName(aVarName));
   v->resize(aVarName);
   return *this;
} // varResize

ZExport(ZBoolean) ZCsl::varExists(const ZString& aVarName)
{
   Variable *v;
   if (iCalls)
      v = iCalls->findVar(aVarName,zFalse);
   else
      v = iBlock->findVar(aVarName,zFalse);
   return v != 0;
} // varExists

static ZString findFile(const ZString& aFile, const ZString& aExt)
{
   ZFUNCTRACE_DEVELOP("findFile(const ZString& aFile, const ZString& aExt, char *path)");
   char *env;
#if ZC_WIN || ZC_OS2
   if (ZString::lowerCase(aExt)==".cmd" || ZString::lowerCase(aExt)==".bat")
#endif
#if ZC_UNIXFAM
   if (aExt.size()==0)
#endif
      env = "PATH";
   else
      env = "CSLPATH";
#ifdef ZC_CSS_COMPATIBLE
   ZString fn = ZFile::locateFile(aFile+aExt, env);
   if (!fn.length() && env == "CSLPATH") {
      env = "CSSPATH";
      fn = ZFile::locateFile(aFile+aExt, env);
   } // if
   return fn;
#else
   return ZFile::locateFile(aFile+aExt, env);
#endif
} // findFile

ZExport(ZCsl&) ZCsl::loadScript(const ZString& aFileName)
{
   ZFUNCTRACE_DEVELOP("ZCsl::loadScript(const ZString& aFileName)");

   // separate extention from filename
   ZString fname, ext;
   fname = ZFile::stripExtension(aFileName, ext);

   // search file
   ZString path;
   if (ext.size())
      path = findFile(fname, ext);
   else {
      path = findFile(fname, ".csl");
#ifdef ZC_CSS_COMPATIBLE
      if (path.size()==0)
         path = findFile(fname, ".css");
#endif
#if ZC_WIN || ZC_OS2
      if (path.size()==0) {
         path = findFile(fname, ".bat");
         if (path.size()==0)
            path = findFile(fname, ".cmd");
      } // if
#endif
#if ZC_UNIXFAM
      if (path.size()==0)
         path = findFile(fname, "");
#endif
   } // if
   if (path.size()==0)
      throwExcept(msgFileNotFound, aFileName);

   // check if allready loaded
#if ZC_WIN || ZC_OS2
   ZString sName(ZFile::stripPath(path).lowerCase());
#endif
#if ZC_UNIXFAM
   ZString sName(ZFile::stripPath(path));
#endif
   Block* b = iStats;
   while (b) {
      if (b->findVar("cslFileName")->value()==sName)
         return *this;
      b = b->iPrev;
   } // while

   // open ifstream
   ifstream iStr(path.constBuffer());
   if (!iStr.good()) throwExcept(msgFileOpenErr, path);

   return loadScript(sName, &iStr);
} // loadScript

ZExport(ZCsl&) ZCsl::loadLibrary(const ZString& aDllName)
{
   ZFUNCTRACE_DEVELOP("ZCsl::loadLibrary(const ZString& aDllName)");

   // check if allready loaded
   ZString path;
#if ZC_WIN || ZC_OS2
   ZString file =
      ZFile::addExtension(
         ZFile::stripPath(aDllName, path),
         ".dll"
      );
#endif
#if ZC_UNIXFAM
   ZString file =
      ZString("lib")+
      ZFile::addExtension(
         ZFile::stripPath(aDllName, path),
         ".so."+ZString(CSL_MAJOR_VERSION)
      );
#endif
   Library* lib = iDlls;
   while (lib) {
      if (ZFile::stripPath(lib->iDll->name()).lowerCase() == ZString::lowerCase(file))
         return *this;
      lib = lib->iPrev;
   } // while

   lib = new Library(this, path+file, 0);
   lib->iPrev = iDlls;
   iDlls = lib;
   return *this;
} // loadLibrary

ZExport(ZString) ZCsl::call(
   const ZString& aFileName,
   const ZString& aFuncName,
   long aArgCount,
   char** aArgs,
   long* aSize)
{
   ZFUNCTRACE_DEVELOP("ZCsl::call(const ZString&, const ZString&, long, char**, long*)");
   State        saveState(iState);
   Block*       saveStat = iStat;
   ZString      ret;

   try {
      iState = isNormal;

      Function* func = findFunc(aFuncName); // function available?

      // count mandatory and optional params
      int mandParams(0);
      int totParams(0);
      Parameter* par = func->iParams;
      while (par) {
         if (!par->iIsOptional) mandParams++;
         totParams++;
         par = par->iNext;
      } // while

      // check argument count against parameter list
      if (aArgCount < mandParams || aArgCount > totParams)
         throwExcept(msgInvArgCnt, func->iName);

      // everything ok, so push the arguments in reverse order
      openStat(aFileName);
      for (int i = aArgCount-1; i >= 0; i--)
         if (!aSize || aSize[i] < 0)
            push(aArgs[i]);
         else
            push(ZString(aArgs[i], aSize[i]));

      // if variable parameter count, push argCount
      if (totParams > mandParams) push(ZString(aArgCount));

      // off we go...
      iState = isInterpreting;
      exec(func);
      ret = pop();
   } // try
   catch (const ZException& exc) {
      iStat = saveStat;
      iState = saveState;
      throw;
   } // catch
   iStat = saveStat;
   iState = saveState;
   SAFE_DELETE_ARRAY( aArgs );
   return ret;
} // call

ZExport(ZString) ZCsl::call(
   const ZString& aFileName,
   const ZString& aFuncName,
   int aArgCount, ...)
{
   ZFUNCTRACE_DEVELOP("ZCsl::call(const ZString&, const ZString&, int, ...)");
   char** args = 0;

   if (aArgCount) {
      args = new char*[aArgCount];
      va_list argPtr;
      va_start(argPtr, aArgCount);
      for (int argc = 0; argc < aArgCount; argc++)
         args[argc] = va_arg(argPtr, char*);
      va_end(argPtr);
   } // if
   return call(aFileName, aFuncName, (long)aArgCount, args);
} // call

ZExport(ZString) ZCsl::callEx(
   const ZString& aFileName,
   const ZString& aFuncName,
   int aArgCount, ...)
{
   ZFUNCTRACE_DEVELOP("ZCsl::callEx(const ZString&, const ZString&, int, ...)");
   char** args = 0;
   long* size = 0;

   if (aArgCount) {
      args = new char*[aArgCount];
      size = new long[aArgCount];
      va_list argPtr;
      va_start(argPtr, aArgCount);
      for (int argc = 0; argc < aArgCount; argc++) {
         args[argc] = va_arg(argPtr, char*);
         size[argc] = va_arg(argPtr, long);
      } // for
      va_end(argPtr);
   } // if
   return call(aFileName, aFuncName, (long)aArgCount, args, size);
} // callEx

ZExport(ZCsl&) ZCsl::addFunc(
   const ZString& aFileName,
   const ZString& aFuncHeader,
   ZString (*aFunc)(ZCsl*))
{
   ZFUNCTRACE_DEVELOP("ZCsl::addFunc(const ZString&, const ZString&, ZString (*)(ZCsl*))");
   State        saveState(iState);
   Block*       saveStat = iStat;
   InputStream* saveInput = iInput;
   Symbol       saveSym(iSym);
   ZString      saveIdent(iIdent);
   double       saveValue(iValue);
   Function*    iFunc = 0;

   try {
      iState = isNormal;
      openStat(aFileName);
#if ZC_MICROSOFT
      // msvc patch: no istrstream constructor from const char*
      ZString funcHeader(aFuncHeader);
      istrstream iStr((char*)funcHeader);
#else
      istrstream iStr((const char*)aFuncHeader);
#endif
      iInput = new InputStream(this, aFileName, &iStr, 0);
      iState = isCompiling;
      getSym();
      ZBoolean iStatic(zFalse);
      if (iSym == symStatic) {
         iStatic = zTrue;
         getSym();
      } // if
      if (iSym != symIdent) throwExcept(msgIdentExpct);
      iFunc = new Function(this, iIdent, aFunc, 0);
      getSym();
      getParamList(iFunc);
      if (iSym != symEof) throwExcept(msgUnexpSymbol);
      addFunc(iFunc, iStatic);
   } // try
   catch (const ZException& exc) {
      if (iInput) delete iInput;
      if (iFunc) delete iFunc;
      iStat  = saveStat;
      iInput = saveInput;
      iSym   = saveSym;
      iIdent = saveIdent;
      iValue = saveValue;
      iState = saveState;
      throw;
   } // catch

   iStat  = saveStat;
   iInput = saveInput;
   iSym   = saveSym;
   iIdent = saveIdent;
   iValue = saveValue;
   iState = saveState;
   return *this;
} // addFunc

ZExport(ZCsl&) ZCsl::addVar(
   const ZString& aVarName,
   const ZString& aInitValue,
   ZBoolean aIsConst)
{
   ZFUNCTRACE_DEVELOP("ZCsl::addVar(const ZString&, const ZString&, ZBoolean)");
   if (!iCalls || !iCalls->iBlocks) throwExcept(msgIllGlobAddVar);
   iCalls->iBlocks->addVar(aVarName, aInitValue, aIsConst);
   return *this;
} // addVar

ZExport(ZDateTime) ZCsl::startDateTime() const
{
   ZFUNCTRACE_DEVELOP("ZCsl::startDateTime() const");
   return iStartDateTime;
} // startDateTime

ZExport(ZCsl::TraceMode) ZCsl::traceMode() const
{
   ZFUNCTRACE_DEVELOP("ZCsl::traceMode() const");
   return iTraceMode;
} // traceMode

ZExport(ZCsl&) ZCsl::setTraceMode(TraceMode aMode)
{
   ZFUNCTRACE_DEVELOP("ZCsl::setTraceMode(TraceMode aMode)");
   iTraceMode = aMode;
   return *this;
} // setTraceMode

ZExport(ZCsl&) ZCsl::trace(const ZString& aMessage)
{
   ZFUNCTRACE_DEVELOP("ZCsl::trace(const ZString& aMessage)");
   if (iTraceMode & traceMsgs)
      trace('>', aMessage);
   return *this;
} // trace

ZExport(ZCsl&) ZCsl::show(ShowMode aMode, long aDepth)
{
   ZFUNCTRACE_DEVELOP("ZCsl::show(ShowMode aMode, long aDepth)");
   switch (aMode) {
      case showFunctions: {
         showFuncs(iBlock->iFuncs, zFalse, aDepth);
         Block* ss = iStats;
         while (ss) {
            showFuncs(ss->iFuncs, zFalse, aDepth);
            ss = ss->iPrev;
         } // while
         break;
      }
      case showCallStack:
         showFuncs(iCalls, zFalse, aDepth);
         break;
      case showFullStack:
         showFuncs(iCalls, zTrue, aDepth);
         break;
      case showGlobals: {
         showIdents(iBlock, "", aDepth);
         Block* ss = iStats;
         while (ss) {
            showIdents(ss, ss->findVar("cslFileName")->value()+": static ", aDepth);
            ss = ss->iPrev;
         } // while
         break;
      }
      case showLibraries:
         showLibs(aDepth);
         break;
      default:;
   } // switch
   return *this;
} // show
