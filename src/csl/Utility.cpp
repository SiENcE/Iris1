/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Utility.cpp
 * Application :  CSL kernel
 * Purpose     :  Utilities
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.22  First implementation                        P.Koch, IBK
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.28  Enhanced trace facilities                   P.Koch, IBK
 * 2001.08.16  Fixed bug when adding predeclared function  P.Koch, IBK
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


using namespace std;

const char Zero[] = "0";
const char One[] = "1";

const char msgInvResizeRef[] = "Illegal resize on subindexed reference ";
const char msgFileNotFound[] = "file not found ";
const char msgInternalErr[] = "csl internal error ";
const char msgIndexNonConst[] = "index of identifier is non-const ";
const char msgInitNonConst[] = "initializer of identifier is non-const ";
const char msgInitExtern[] = "externals cannot be initialized ";
const char msgNewLineInLit[] = "newline in string constant ";
const char msgInvCatch[] = "catch pcode without a pending exception ";
const char msgInvParCnt[] = "invalid # of parameters ";
const char msgInvArgCnt[] = "invalid # of arguments ";
const char msgInvPar[] = "invalid parameter ";
const char msgInvIdent[] = "invalid identifier ";
const char msgInvChar[] = "invalid character ";
const char msgInvSymbol[] = "invalid symbol ";
const char msgInvBreak[] = "break not allowed in this context ";
const char msgInvContin[] = "continue not allowed outside loops ";
const char msgInvOpStrings[] = "invalid operator for strings ";
const char msgInvalidIndex[] = "invalid array index ";
const char msgVarNotFound[] = "variable or constant not found ";
const char msgVarAlrdyDef[] = "variable or constant allready declared ";
const char msgFuncAlrdyDef[] = "function allready declared ";
const char msgFuncNotFound[] = "function not found ";
const char msgBeginNotFound[] = "{ expected ";
const char msgEndNotFound[] = "} expected ";
const char msgSemiNotFound[] = "; expected ";
const char msgColonNotFound[] = ": expected ";
const char msgSyntax[] = "syntax error ";
const char msgIdentExpct[] = "identifier expected ";
const char msgDirExpct[] = "directive expected ";
const char msgUnknownDir[] = "unknown directive ";
const char msgIndexBound[] = "array index out of bounds ";
const char msgMissLvalue[] = "target of assignment must be a variable ";
const char msgMissLparen[] = "( expected ";
const char msgMissRparen[] = ") expected ";
const char msgMissLbrack[] = "[ expected ";
const char msgMissRbrack[] = "] expected ";
const char msgMissWhile[] = "'while' expected ";
const char msgMissFactor[] = "missing factor ";
const char msgFileOpenErr[] = "file cannot be opened ";
const char msgEarlyEof[] = "unexpected end of file ";
const char msgUnexpSymbol[] = "unexpected symbol ";
const char msgUnexpIdent[] = "unexpected identifier ";
const char msgTermAbnorm[] = "execution terminated abnormaly ";
const char msgSimpleExpct[] = "simple expression expected ";
const char msgUncaught[] = "uncaught exception ";
const char msgCatchNotFound[] = "catch expected ";
const char msgWriteConst[] = "attemp to change a const ";
const char msgVarSymExpct[] = "'var' or 'const' expected ";
const char msgInitExpct[] = "initializer for const expected ";
const char msgDimsMismatch[] = "dimensions mismatch ";
const char msgConstDenied[] = "invalid argument for non-const parameter ";
const char msgFuncNesting[] = "nesting of functions not possible ";
const char msgStackUdr[] = "expression stack underrun ";
const char msgDivZero[] = "divide by zero ";
const char msgIllgInstr[] = "illegal instruction in p-code ";
const char msgNoReturn[] = "no return from function ";
const char msgFuncDiffers[] =
  "function differs from previous forward declaration ";
const char msgFuncNotimp[] = "function not implemented ";
const char msgVarDiffers[] =
  "identifier differs from previous extern declaration ";
const char msgUnallocExtern[] = "accessing unallocated external ";
const char msgArrParamRef[] = "array parameters must be passed by reference ";
const char msgErrLoadingLib[] = "error loading library ";
const char msgInitialNotFnd[] = "no 'initialize' entry found in ";
const char msgCleanupNotFnd[] = "no 'cleanup' entry found in ";
const char msgIllGlobAddVar[] = "addVar not allowed on global scope ";
const char msgCloseIfUnexp[] = "no #if block open ";
const char msgEndifNotFound[] = "#if block not closed ";

ZBoolean inStr (char ch, char *s)
{
  ZFUNCTRACE_DEVELOP ("inStr(char ch, char* s)");
  while (*s && ch != *s)
    s++;
  return *s && ch == *s;
}                               // inStr

ZBoolean validIdBeginChar (char ch)
{
  ZFUNCTRACE_DEVELOP ("validIdBeginChar(char ch)");
  if ('a' <= ch && ch <= 'z')
    return zTrue;
  if ('A' <= ch && ch <= 'Z')
    return zTrue;
  return inStr (ch, "$_");
}                               // validIdBeginChar

ZBoolean validIdChar (char ch)
{
  ZFUNCTRACE_DEVELOP ("validIdChar(char ch)");
  if ('0' <= ch && ch <= '9')
    return zTrue;
  return validIdBeginChar (ch);
}                               // validIdChar

ZBoolean isNumber (const char *s)
{
  ZFUNCTRACE_DEVELOP ("isNumber(const char* s)");
  if (*s == '-' || *s == '+')
    s++;
  ZBoolean any (zFalse);
  while ('0' <= *s && *s <= '9')
      {
        s++;
        any = zTrue;
      }
  if (!*s || !any)
    return any;
  if (*s == '.')
      {
        s++;
        while ('0' <= *s && *s <= '9')
          s++;
        if (!*s)
          return any;
      }                         // if
  any = zFalse;
  if (*s != 'e' && *s != 'E')
    return any;
  s++;
  if (*s == '-' || *s == '+')
    s++;
  while ('0' <= *s && *s <= '9')
      {
        s++;
        any = zTrue;
      }
  return any && *s == 0;
}                               // isNumber

ZString pureVarName (const ZString & aVarName)
{
  ZFUNCTRACE_DEVELOP ("pureVarName(const ZString& aVarName)");
  long pos = aVarName.indexOf ('[');
  if (!pos)
    return ZString (aVarName);
  return aVarName.subString (1, pos - 1);
}                               // pureVarName

void ZCsl::checkIdent (char *s)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::checkIdent(char* s)");
  if (!validIdBeginChar (*s))
    throwExcept (msgInvIdent);
  while (validIdChar (*s) || inStr (*s, "[]"))
    s++;
  if (*s)
    throwExcept (msgInvIdent);
}                               // checkIdent

void ZCsl::log (const char *aMessage)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::log(char* aMessage)");
  call ("ZCsl", "sysLog", 2, aMessage, "1");
}                               // log

void ZCsl::trace (char aFlag, const char *aMessage)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::trace(char aFlag, const ZString& aMessage)");
  if (aFlag == '-' && iTraceLevel > 0)
    iTraceLevel--;
  ZString space;
  if (iTraceLevel && aFlag != '#')
    space.leftJustify (iTraceLevel);
  cerr << space << aFlag << aMessage << endl;
  if (aFlag == '+')
    iTraceLevel++;
}                               // trace

ZCsl::Function * ZCsl::findFunc (const ZString & name, ZBoolean fail)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::findFunc(const ZString& name, ZBoolean fail)");
  Function *f = iBlock->iFuncs;
  while (f)
      {
        if (f->iName == name)
          return f;
        f = f->iPrev;
      }                         // while
  f = iStat->iFuncs;
  while (f)
      {
        if (f->iName == name)
          return f;
        f = f->iPrev;
      }                         // while
  if (fail)
    throwExcept (msgFuncNotFound, name);
  return 0;
}                               // findFunc

ZCsl::Function * ZCsl::addFunc (Function * aFunc, ZBoolean aStatic)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::addFunc(Function* aFunc, ZBoolean aStatic)");
  ZBoolean iStatic (zFalse);
  Function *f = iBlock->iFuncs;

  while (f)
      {
        if (f->iName == aFunc->iName)
          break;
        f = f->iPrev;
      }                         // while
  if (!f)
      {
        iStatic = zTrue;
        f = iStat->iFuncs;
        while (f)
            {
              if (f->iName == aFunc->iName)
                break;
              f = f->iPrev;
            }                   // while
      }                         // if
  if (f)
      {
        if (f->iFunc || f->iPsize)
          throwExcept (msgFuncAlrdyDef, aFunc->iName);
        Parameter *iPar = f->iParams;
        Parameter *aPar = aFunc->iParams;
        while (iPar && aPar)
            {
              if (iPar->iName != aPar->iName ||
                  iPar->iIsConst != aPar->iIsConst ||
                  iPar->iIsRef != aPar->iIsRef ||
                  iPar->iIsOptional != aPar->iIsOptional)
                break;
              iPar = iPar->iNext;
              aPar = aPar->iNext;
            }                   // while
        if (iPar || aPar || aStatic != iStatic)
          throwExcept (msgFuncDiffers, aFunc->iName);

        if (f->iBlocks)
          delete f->iBlocks;
        f->iBlocks = aFunc->iBlocks;
        aFunc->iBlocks = 0;

        f->iStat = aFunc->iStat;
        aFunc->iStat = 0;

        if (f->iParams)
          delete f->iParams;
        f->iParams = aFunc->iParams;
        aFunc->iParams = 0;

        if (f->iInst)
          delete f->iInst;
        f->iInst = aFunc->iInst;
        aFunc->iInst = 0;

        if (f->iPcode)
          delete f->iPcode;
        f->iPcode = aFunc->iPcode;
        aFunc->iPcode = 0;

        f->iPsize = aFunc->iPsize;
        aFunc->iPsize = 0;

        f->iFunc = aFunc->iFunc;    // thanks to ghislain picard!

        delete aFunc;
        aFunc = f;
      }
  else
      {
        if (aStatic)
            {
              aFunc->iPrev = iStat->iFuncs;
              iStat->iFuncs = aFunc;
            }
        else
            {
              aFunc->iPrev = iBlock->iFuncs;
              iBlock->iFuncs = aFunc;
            }                   // if
      }                         // if
  return aFunc;
}                               // addFunc

void ZCsl::exec (Function * aFunc)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::exec(Function* aFunc)");
  iCalls = new Function (aFunc, iCalls);
  Block *saveStat = iStat;
  iStat = iCalls->iStat;
  iCalls->openBlock (funcHeader (aFunc));
  try
  {
    Parameter *par = iCalls->iParams;
    if (par)
        {
          ZBoolean anyOpts (zFalse);
          int argc (0);
          while (par)
              {
                argc++;
                if (par->iIsOptional)
                  anyOpts = zTrue;
                par = par->iNext;
              }                 // while
          if (anyOpts)
              {
                ZString argcStr = pop ();
                argc = argcStr.asInt ();
                iCalls->iBlocks->addVar ("argCount", argcStr, zTrue);
              }                 // if
          par = iCalls->iParams;
          for (int p = 0; p < argc; p++)
              {
                iCalls->iBlocks->addVar (par->iName, pop (), par->iIsConst,
                                         par->iIsRef);
                par = par->iNext;
              }                 // for
        }                       // if
    iCalls->iBlocks->addVar ("cslFunction", aFunc->iName, zTrue);
#ifdef ZC_CSS_COMPATIBLE
    iCalls->iBlocks->addVar ("cssFunction", aFunc->iName, zTrue);
#endif
    if (iCalls->iFunc)
      push ((*iCalls->iFunc) (this));
    else if (iCalls->iPsize)
      interpret (iCalls);
    else
      throwExcept (msgFuncNotimp);
  }                             // try
  catch (const ZException & exc)
  {
    iCalls->closeBlock ();
    Function *oldCalls = iCalls;
    iCalls = iCalls->iPrev;
    iStat = saveStat;
    oldCalls->iInst = 0;
    oldCalls->iPcode = 0;
    oldCalls->iText = 0;
    oldCalls->iPrev = 0;
    oldCalls->iParams = 0;
    delete oldCalls;
    throw;
  }                             // catch

  iCalls->closeBlock ();
  Function *oldCalls = iCalls;
  iCalls = iCalls->iPrev;
  iStat = saveStat;
  oldCalls->iInst = 0;
  oldCalls->iPcode = 0;
  oldCalls->iText = 0;
  oldCalls->iPrev = 0;
  oldCalls->iParams = 0;
  delete oldCalls;
}                               // exec

void ZCsl::push (const ZString & aArg)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::push(const ZString& aArg)");
  if (iTos == iStackLimit)
      {
        // reallocate bigger stack
        ZString *newStack = new ZString[iStackSize + STACKCHUNK];
        for (int i = 0; i < iStackSize; i++)
          newStack[i] = iStack[i];
        delete[]iStack;
        iStack = newStack;
        iTos = &iStack[iStackSize - 1];
        iStackSize += STACKCHUNK;
        iStackLimit = &iStack[iStackSize - 1];
      }                         // if
  *(++iTos) = aArg;
}                               // push

ZString ZCsl::pop ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::pop()");
  if (iTos == iStack)
    throwExcept (msgStackUdr);
  return *iTos--;
}                               // pop

void ZCsl::throwExcept (const ZString & aMsg, const ZString & aInfo)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::throwExcept(...)");
  ZString msg (aMsg);
  if (aInfo.size ())
    msg += " (" + aInfo + ")";
  switch (iState)
      {
      case isCompiling:
        if (iInput)
            {
              msg = iInput->iFileName + "("
                + ZString (iInput->iSymLine)
                + ":" + ZString (iInput->iSymPos) + ") : " + aMsg;
            }                   // if
        break;
      case isInterpreting:
        msg = iStat->findVar ("cslFileName")->value ()
          + "(" + iCalls->iName + ") : " + aMsg;
        break;
      default:;                // isNormal
      }                         // switch
  ZTHROWEXC (msg);
}                               // throwExcept

void ZCsl::showIdents (Block * b, char *lead, long &depth)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::showIdents(Block* b, char* lead, long& depth)");
  Variable *v = b->iVars;
  while (v && depth > 0)
      {
        ZString msg (lead);
        msg += v->iIsConst ? "const " : "var ";
        if (v->iRef)
          msg += "&";
        msg += v->iName;
        if (v->iIsExtern)
            {
              if (v->dims ())
                for (int d = 0; d < v->dims (); d++)
                  msg += "[]";
              msg += ", unallocated external";
            }
        else
            {
              if (v->dims ())
                  {
                    for (int d = 0; d < v->dims (); d++)
                      msg += "[" + ZString (v->iDims[d]) + "]";
                  }
              else
                  {
                    v->match (v->iName);
                    msg += " = " + v->value ();
                  }             // if
            }                   // if
        log (msg.constBuffer ());
        v = v->iPrev;
        depth--;
      }                         // while
}                               // showIdents

ZString ZCsl::funcHeader (Function * f)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::funcHeader(Function* f)");
  ZString txt = f->iStat->findVar ("cslFileName")->value () + ": ";
  Function *ff = f->iStat->iFuncs;
  ZString iStat;
  while (ff)
      {
        if (ff->iName == f->iName)
            {
              iStat = "static ";
              break;
            }                   // if
        ff = ff->iPrev;
      }                         // while
  txt += iStat + "var " + f->iName + "(";
  Parameter *p = f->iParams;
  ZBoolean opt (zFalse);
  while (p)
      {
        if (p->iIsOptional && !opt)
            {
              opt = zTrue;
              txt += "[";
            }                   // if
        txt += p->iIsConst ? "const " : "var ";
        if (p->iIsRef)
          txt += "&";
        txt += p->iName;
        p = p->iNext;
        if (p)
          txt += ", ";
      }                         // for
  if (opt)
    txt += "]";
  txt += ")";
  return txt;
}                               // funcHeader

void ZCsl::showFuncs (Function * f, ZBoolean v, long &depth)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::showFuncs(Function* f, ZBoolean v, long& depth)");
  while (f && depth > 0)
      {
        ZString txt = funcHeader (f);
        if (f->iFunc)
          txt += " API";
        else if (f->iPcode)
          txt += " CSL";
        else
          txt += " unimplemented";
        log (txt.constBuffer ());
        if (v)
            {
              Block *b = f->iBlocks;
              while (b)
                  {
                    long d (99999999);
                    showIdents (b, "  ", d);
                    b = b->iPrev;
                  }             // while
              log ();
            }                   // if
        f = f->iPrev;
        depth--;
      }                         // while
}                               // showFuncs

void ZCsl::showLibs (long &depth)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::showLibs(long& depth)");
  Library *lib = iDlls;
  while (lib && depth > 0)
      {
        const char *tp = lib->iNative ? "Native" : "API";
        log ((lib->iDll->fullName () + " (" + tp + ")").constBuffer ());
        lib = lib->iPrev;
        depth--;
      }                         // while
}                               // showIdents
