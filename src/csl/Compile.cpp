/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Compile.cpp
 * Application :  CSL kernel
 * Purpose     :  Compiler
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.21  First implementation                        P.Koch, IBK
 * 2001.06.28  Change functions and/or to andOp/orOp       P.Koch, IBK
 *             because gcc v3.0 will complain otherwise.
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.26  Speed up function calls                     P.Koch, IBK
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

//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <ZCsl.hpp>
#include <ZFile.hpp>
#include <ZTrace.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"


using namespace std;

static const char iNulString[] = "";

void ZCsl::getSym0 ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::getSym0()");
  iSym = iInput->getSym (iIdent, iValue);
#ifdef CSL_LOG_SYMS
  cerr << iInput->iFileName << " ";
  switch (iSym)
      {
      case symBegin:
        cerr << "symBegin:   ";
        break;
      case symEnd:
        cerr << "symEnd:     ";
        break;
      case symLparen:
        cerr << "symLparen:  ";
        break;
      case symRparen:
        cerr << "symRparen:  ";
        break;
      case symLbrack:
        cerr << "symLbrack:  ";
        break;
      case symRbrack:
        cerr << "symRbrack:  ";
        break;
      case symRet:
        cerr << "symRet:     ";
        break;
      case symNot:
        cerr << "symNot:     ";
        break;
      case symVar:
        cerr << "symVar:     ";
        break;
      case symConst:
        cerr << "symConst:   ";
        break;
      case symRef:
        cerr << "symRef:     ";
        break;
      case symDo:
        cerr << "symDo:      ";
        break;
      case symIf:
        cerr << "symIf:      ";
        break;
      case symElse:
        cerr << "symElse:    ";
        break;
      case symWhile:
        cerr << "symWhile:   ";
        break;
      case symFor:
        cerr << "symFor:     ";
        break;
      case symTimes:
        cerr << "symTimes:   ";
        break;
      case symSlash:
        cerr << "symSlash:   ";
        break;
      case symPlus:
        cerr << "symPlus:    ";
        break;
      case symMinus:
        cerr << "symMinus:   ";
        break;
      case symMulBec:
        cerr << "symMulBec:  ";
        break;
      case symDivBec:
        cerr << "symDivBec:  ";
        break;
      case symAddBec:
        cerr << "symAddBec:  ";
        break;
      case symSubBec:
        cerr << "symSubBec:  ";
        break;
      case symIncr:
        cerr << "symIncr:    ";
        break;
      case symDecr:
        cerr << "symDecr:    ";
        break;
      case symComma:
        cerr << "symComma:   ";
        break;
      case symBecomes:
        cerr << "symBecomes: ";
        break;
      case symAnd:
        cerr << "symAnd:     ";
        break;
      case symOr:
        cerr << "symOr:      ";
        break;
      case symEql:
        cerr << "symEql:     ";
        break;
      case symNeq:
        cerr << "symNeq:     ";
        break;
      case symGtr:
        cerr << "symGtr:     ";
        break;
      case symGeq:
        cerr << "symGeq:     ";
        break;
      case symLss:
        cerr << "symLss:     ";
        break;
      case symLeq:
        cerr << "symLeq:     ";
        break;
      case symIdent:
        cerr << "symIdent:   " << '<' << iIdent << '>';
        break;
      case symLit:
        cerr << "symLit:     " << '<' << iIdent << '>';
        break;
      case symValue:
        cerr << "symValue:   " << '<' << iValue << '>';
        break;
      case symSemi:
        cerr << "symSemi:    ";
        break;
      case symThrow:
        cerr << "symThrow:   ";
        break;
      case symTry:
        cerr << "symTry:     ";
        break;
      case symCatch:
        cerr << "symCatch:   ";
        break;
      case symStatic:
        cerr << "symStatic:  ";
        break;
      case symExtern:
        cerr << "symExtern:  ";
        break;
      case symSizeof:
        cerr << "symSizeof:  ";
        break;
      case symResize:
        cerr << "symResize:  ";
        break;
      case symExist:
        cerr << "symExist:   ";
        break;
      case symBreak:
        cerr << "symBreak:   ";
        break;
      case symEof:
        cerr << "symEof:     ";
        break;
      case symContin:
        cerr << "symContin:  ";
        break;
      case symDiv:
        cerr << "symDiv:     ";
        break;
      case symMod:
        cerr << "symMod:     ";
        break;
      case symDivIBec:
        cerr << "symDivIBec: ";
        break;
      case symModBec:
        cerr << "symModBec:  ";
        break;
      case symCat:
        cerr << "symCat:     ";
        break;
      case symCatBec:
        cerr << "symCatBec:  ";
        break;
      case symDir:
        cerr << "symDir:     ";
        break;
      case symSwitch:
        cerr << "symSwitch:  ";
        break;
      case symCase:
        cerr << "symCase:    ";
        break;
      case symColon:
        cerr << "symColon:   ";
        break;
      case symDefault:
        cerr << "symDefault: ";
        break;
      case symTrace:
        cerr << "symTrace:   ";
        break;
      default:;
      }                         // switch
  cerr << endl;
#endif
  if (iSym == symEof)
      {
        if (iInput->iIfStack)
          throwExcept (msgEndifNotFound);
        InputStream *old = iInput;
        iInput = iInput->iPrev;
        old->iPrev = 0;
        delete old;
        if (iInput)
            {
              iSym = iInput->iSym;
              iIdent = iInput->iIdent;
              iValue = iInput->iValue;
            }                   // if
      }                         // if
}                               // getSym0

ZBoolean ZCsl::skipCode (ZBoolean elseOk)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::skipCode(ZBoolean elseOk)");
  for (ZBoolean endFound = zFalse; !endFound;)
      {
        while (iSym != symDir)
            {
              getSym0 ();
              if (iSym == symEof)
                throwExcept (msgEndifNotFound);
            }                   // while
        getSym0 ();
        switch (iSym)
            {
            case symIdent:
              if (iIdent == "endif")
                  {
                    getSym0 ();
                    endFound = zTrue;
                  }             // if
              break;
            case symIf:
              getSym0 ();
              if (skipCode (zTrue))
                skipCode (zFalse);
              break;
            case symElse:
              getSym0 ();
              if (!elseOk)
                throwExcept (msgCloseIfUnexp);
              return zTrue;
            default:
              throwExcept (msgDirExpct);
            }                   // switch
      }                         // for
  return zFalse;                // if block closed
}                               // skipCode

ZCsl::IfRecord::IfRecord (ZBoolean aElse, IfRecord * aPrev):
iElse (aElse), iPrev (aPrev)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::IfRecord::IfRecord(ZBoolean aElse, IfRecord* aPrev)");
}                               // IfRecord

ZCsl::IfRecord::~IfRecord ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::IfRecord::~IfRecord()");
  if (iPrev)
    delete iPrev;
}                               // ~IfRecord

void ZCsl::closeIfDir ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::closeIfDir()");
  IfRecord *rec = iInput->iIfStack;
  if (rec == 0)
    throwExcept (msgCloseIfUnexp);
  iInput->iIfStack = iInput->iIfStack->iPrev;
  rec->iPrev = 0;
  delete rec;
}                               // closeIfDir

void ZCsl::directive ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::directive()");
  getSym0 ();
  Symbol dirsym (iSym);
  if (dirsym != symIdent && dirsym != symIf && dirsym != symElse)
    throwExcept (msgDirExpct);
  ZString direct;
  if (dirsym == symIdent)
    direct = iIdent;
  if (dirsym == symElse)
      {
        if (iFlags & disIf)
          throwExcept (msgUnknownDir, direct);
        if (iInput->iIfStack && iInput->iIfStack->iElse)
          throwExcept (msgCloseIfUnexp);
        closeIfDir ();
        getSym0 ();
        skipCode (zFalse);
      }
  else if (direct == "endif")
      {
        if (iFlags & disIf)
          throwExcept (msgUnknownDir, direct);
        closeIfDir ();
        getSym0 ();
      }
  else
      {
        Instruction head, body, tail;
        getSym0 ();
        orOp (0, &head, &body, &tail);
        iInDirective = zFalse;
        Instruction *blast = body.last ();
        if (blast->iCode != cdPush)
          throwExcept (msgInitNonConst);
        if (dirsym == symIf)
            {
              if (iFlags & disIf)
                goto reject;
              if (blast->iText.asDouble () == 0)
                  {
                    if (skipCode (zTrue))
                      iInput->iIfStack =
                        new IfRecord (zTrue, iInput->iIfStack);
                  }
              else
                iInput->iIfStack = new IfRecord (zFalse, iInput->iIfStack);
            }
        else if (direct == "include")
            {
              if (iFlags & disInclude)
                goto reject;
              ZString fname =
                ZFile::locateFile (ZFile::addExtension (blast->iText, ".csl"),
                                   "CSLPATH");
#ifdef ZC_CSS_COMPATIBLE
              if (fname.size () == 0)
                fname =
                  ZFile::locateFile (ZFile::
                                     addExtension (blast->iText, ".hss"),
                                     "CSSPATH");
#endif
              if (fname.size () == 0)
                throwExcept (msgFileNotFound, blast->iText);
              iInput = new InputStream (this, fname, iInput);
              getSym0 ();
            }
        else if (direct == "loadScript")
            {
              if (iFlags & disLoadScript)
                goto reject;
              iInDirective = zFalse;
              loadScript (blast->iText);
            }
        else if (direct == "loadLibrary")
            {
              if (iFlags & disLoadLibrary)
                goto reject;
              loadLibrary (blast->iText);
            }
        else if (direct == "logFile")
            {
              if (iFlags & disLogFile)
                goto reject;
              if (blast->iText.size ())
                call ("ZCsl", "sysLogFile", 1, (char *) blast->iText);
              else
                call ("ZCsl", "sysLogFile");
            }
        else if (direct == "list")
            {
              if (iFlags & disList)
                goto reject;
              iList = blast->iText.asDouble ()? zTrue : zFalse;
            }
        else if (direct == "error")
            {
              if (iFlags & disError)
                goto reject;
              throwExcept (blast->iText);
            }
        else if (direct == "block")
            {
              if (iCurrFunc)
                  {
                    Instruction *inst = iCurrFunc->iInst;
                    Instruction *opn = 0;
                    while (inst)
                        {
                          if (inst->iCode == cdOpen)
                            opn = inst;
                          else if (inst->iCode == cdClose)
                            opn = 0;
                          inst = inst->iNext;
                        }       // while
                    if (opn)
                      opn->iText = blast->iText;
                  }             // if
            }
        else if (direct == "message")
            {
              if (iFlags & disMessage)
                goto reject;
              std::cout << blast->iText << std::endl;
            }
        else
            {
            reject:
              throwExcept (msgUnknownDir, direct);
            }                   // if
      }                         // if
}                               // directive

void ZCsl::getSym ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::getSym()");
  getSym0 ();
  for (;;)
      {
        if (iInDirective || iSym != symDir)
          break;
        iInDirective = zTrue;
        try
        {
          directive ();
        }                       // try
        catch (ZException & exc)
        {
          iInDirective = zFalse;
          throw;
        }                       // catch
        iInDirective = zFalse;
      }                         // for
}                               // getSym

int ZCsl::index (Function * aFunc,
                 Instruction * hInst,
                 Instruction * bInst, Instruction * tInst, ZString & ident)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::index(...)");
  ZBoolean cnst (zTrue);
  int indexes (0);
  if (iSym == symLbrack)
      {
        ZString txt (ident + "[");
        while (iSym == symLbrack)
            {
              indexes++;
              ident += "[1]";
              getSym ();
              Instruction indInst;
              orOp (aFunc, hInst, &indInst, tInst);
              Instruction *ilast = indInst.last ();
              if (ilast->iCode == cdPush)
                txt += ilast->iText;
              else
                  {
                    bInst->add (cdPush, txt);
                    txt = iNulString;
                    if (cnst)
                      cnst = zFalse;
                    else
                      bInst->add (cdAdd);
                    bInst->add (&indInst)->add (cdAdd);
                  }             // if
              if (iSym != symRbrack)
                throwExcept (msgMissRbrack);
              getSym ();
              if (iSym == symLbrack)
                txt += "][";
            }                   // while
        txt += "]";
        bInst->add (cdPush, txt);
        if (!cnst)
          bInst->add (cdAdd);
      }
  else
    bInst->add (cdPush, ident);
  return indexes;
}                               // index

void ZCsl::callFunc (Function * aFunc,
                     Instruction * hInst,
                     Instruction * bInst,
                     Instruction * tInst, ZString & ident)
{
  ZFUNCTRACE_DEVELOP ("callFunc(...)");
  if (iSym == symLparen)
      {
        Function *func = findFunc (ident);
        int argc (0);
        Parameter *par = func->iParams;
        getSym ();
        if (iSym != symRparen)
            {
              Function *args = 0;
              for (;;)
                  {
                    if (!par)
                      throwExcept (msgInvParCnt, func->iName);
                    args = new Function (this, iNulString, args);
                    if (par->iIsRef)
                        {
                          // by reference
                          if (iSym != symIdent)
                            throwExcept (msgIdentExpct,
                                         func->iName + " #" + ZString (argc +
                                                                       1));
                          ZString varName (iIdent);
                          getSym ();
                          index (aFunc, hInst, args->iInst, tInst, varName);
                          Variable *var;
                          if (aFunc)
                            var = aFunc->findVar (varName);
                          else
                              {
                                var = iStat->findVar (varName, zFalse);
                                if (!var)
                                  var = iBlock->findVar (varName);
                              } // if
                          if (var->iIsConst && !par->iIsConst)
                            throwExcept (msgConstDenied,
                                         func->iName + "<--" + varName);
                        }
                    else
                      orOp (aFunc, hInst, args->iInst, tInst);    // by value
                    argc++;
                    par = par->iNext;
                    if (iSym != symComma)
                      break;
                    getSym ();
                  }             // for
              Function *arg = args;
              for (int a = 0; a < argc; a++)
                  {
                    bInst->add (arg->iInst);
                    arg = arg->iPrev;
                  }             // for
              delete args;
            }                   // if
        if (par && !par->iIsOptional)
          throwExcept (msgInvParCnt, func->iName);
        if (iSym != symRparen)
          throwExcept (msgMissRparen);
        getSym ();
        par = func->iParams;
        while (par)
            {
              if (par->iIsOptional)
                  {
                    bInst->add (cdPush, ZString (argc));
                    break;
                  }             // if
              par = par->iNext;
            }                   // while
        bInst->add (cdCall, func);
      }                         // if
}                               // callFunc

void ZCsl::arrayInit (Function * aFunc,
                      Instruction * hInst,
                      Instruction * bInst,
                      Instruction * tInst, const ZString & aName, long *dims)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::arrayInit(...)");
  if (iSym == symBegin)
      {
        getSym ();
        long index (0);
        if (iSym != symEnd)
            {
              for (;; index++)
                  {
                    ZString name (aName + "[" + ZString (index) + "]");
                    if (iSym == symBegin)
                      if (dims)
                        arrayInit (aFunc, hInst, bInst, tInst, name,
                                   dims + 1);
                      else
                        arrayInit (aFunc, hInst, bInst, tInst, name, dims);
                    else
                        {
                          orOp (aFunc, hInst, bInst, tInst);
                          Instruction *blast = bInst->last ();
                          if (!dims)
                              {
                                if (blast->iCode != cdPush)
                                  throwExcept (msgInitNonConst);
                                Variable *var = iStat->findVar (name, zFalse);
                                if (!var)
                                  var = iBlock->findVar (name);
                                var->set (blast->iText, zTrue);
                              }
                          else
                              {
                                blast->add (cdPush, name)->add (cdStorC);
                              } // if
                        }       // if
                    if (iSym != symComma)
                      break;
                    getSym ();
                  }             // for
              if (dims && index > *dims)
                *dims = index + 1;
            }                   // if
        if (iSym != symEnd)
          throwExcept (msgEndNotFound);
        getSym ();
      }                         // if
}                               // arrayInit

void ZCsl::getParamList (Function * aFunc, ZBoolean aComp)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::getParamList(Function* aFunc, ZBoolean aComp)");
  if (iSym != symLparen)
    throwExcept (msgMissLparen);
  getSym ();
  ZBoolean isOptional (zFalse);
  while (iSym != symRparen)
      {
        if (iSym == symLbrack)
            {
              if (isOptional)
                throwExcept (msgUnexpSymbol, "[");
              isOptional = zTrue;
              getSym ();
            }                   // if
        ZBoolean isConst (zFalse), isRef (zFalse);
        if (iSym == symConst)
          isConst = zTrue;
        else if (iSym != symVar)
          throwExcept (msgVarSymExpct);
        getSym ();
        if (iSym == symRef)
            {
              isRef = zTrue;
              getSym ();
            }                   // if
        if (iSym != symIdent)
          throwExcept (msgIdentExpct);
        ZString parName (iIdent);
        ZString parName2 (iIdent);
        getSym ();
        int dim (0);
        while (iSym == symLbrack && dim < MAXDIMS)
            {
              if (!isRef)
                throwExcept (msgArrParamRef);
              getSym ();
              if (iSym != symRbrack)
                throwExcept (msgMissRbrack);
              parName += "[]";
              parName2 += "[1]";
              dim++;
              getSym ();
            }                   // if
        aFunc->addPar (parName, isConst, isRef, isOptional);
        if (aComp)
          aFunc->iBlocks->addVar (parName2, iNulString, isConst);
        if (iSym != symComma)
          break;
        getSym ();
      }                         // while
  if (isOptional)
      {
        if (iSym != symRbrack)
          throwExcept (msgMissRbrack);
        if (aComp)
          aFunc->iBlocks->addVar ("argCount", One, zTrue);
        getSym ();
      }                         // if
  if (aComp)
      {
        aFunc->iBlocks->addVar ("cslFunction", One, zTrue);
#ifdef ZC_CSS_COMPATIBLE
        aFunc->iBlocks->addVar ("cssFunction", One, zTrue);
#endif
      }                         // if
  if (iSym != symRparen)
    throwExcept (msgMissRparen);
  getSym ();
}                               // getParamList

void ZCsl::factor (Function * aFunc,
                   Instruction * hInst,
                   Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::factor(...)");
  if (iSym == symSizeof)
      {
        getSym ();
        ZBoolean paren (zFalse);
        if (iSym == symLparen)
            {
              paren = zTrue;
              getSym ();
            }                   // if
        if (iSym != symIdent)
          throwExcept (msgIdentExpct);
        ZString ident (iIdent);
        getSym ();
        Instruction varInst;
        index (aFunc, hInst, &varInst, tInst, ident);
        if (paren)
            {
              if (iSym != symRparen)
                throwExcept (msgMissRparen);
              getSym ();
            }                   // if
        Variable *var;
        if (aFunc)
          var = aFunc->findVar (ident);
        else
            {
              var = iStat->findVar (ident, zFalse);
              if (!var)
                var = iBlock->findVar (ident);
            }                   // if
        Instruction *vlast = varInst.last ();
        if (vlast->iCode == cdPush && var->iIsKnown)
            {
              var->match (vlast->iText);
              bInst->add (cdPush, ZString (var->iSize));
            }
        else
          bInst->add (&varInst)->add (cdSize);
      }
  else
   if (iSym == symExist)
      {
        getSym ();
        ZBoolean paren (zFalse);
        if (iSym == symLparen)
            {
              paren = zTrue;
              getSym ();
            }                   // if
        if (iSym != symIdent)
          throwExcept (msgIdentExpct);
        ZString ident (iIdent);
        getSym ();
        Instruction varInst;
        index (aFunc, hInst, &varInst, tInst, ident);
        if (paren)
            {
              if (iSym != symRparen)
                throwExcept (msgMissRparen);
              getSym ();
            }                   // if
        Variable *var;
        if (aFunc)
          var = aFunc->findVar (ident, zFalse);
        else
            {
              var = iStat->findVar (ident, zFalse);
              if (!var)
                var = iBlock->findVar (ident, zFalse);
            }                   // if
        if (iInDirective)
          bInst->add (cdPush, var ? One : Zero);
        else
          bInst->add (&varInst)->add (cdExist);
      }
  else
   if (iSym == symIncr || iSym == symDecr)
      {
        Symbol op (iSym);
        getSym ();
        if (iSym != symIdent)
          throwExcept (msgIdentExpct);
        ZString ident (iIdent);
        getSym ();
        Instruction varInst;
        index (aFunc, hInst, &varInst, tInst, ident);
        Instruction *vlast = varInst.last ();
        if (vlast->iCode == cdPush)
          ident = vlast->iText;
        Variable *var;
        if (aFunc)
          var = aFunc->findVar (ident);
        else
            {
              var = iStat->findVar (ident, zFalse);
              if (!var)
                var = iBlock->findVar (ident);
            }                   // if
        if (var->iIsConst)
          throwExcept (msgWriteConst);
        hInst->copy (&varInst)->add (op == symIncr ? cdIncV : cdDecV);
        bInst->add (&varInst)->add (cdLoad);
      }
  else
   if (iSym == symPlus || iSym == symMinus || iSym == symNot)
      {
        Symbol op (iSym);
        getSym ();
        factor (aFunc, hInst, bInst, tInst);
        Instruction *blast = bInst->last ();
        if (blast->iCode == cdPush)
            {
              switch (op)
                  {
                  case symMinus:
                    {
                      if (!isNumber (blast->iText.constBuffer ()))
                        throwExcept (msgInvOpStrings);
                      blast->iText = ZString (-blast->iText.asDouble ());
                      break;
                    }
                  case symNot:
                    blast->set (!blast->cond ());
                    break;
                  default:;
                  }             // switch
            }
        else
            {
              switch (op)
                  {
                  case symMinus:
                    blast->add (cdNeg);
                    break;
                  case symNot:
                    blast->add (cdNot);
                    break;
                  default:;    // symPlus
                  }             // switch
            }                   // if
      }
  else
   if (iSym == symIdent || iSym == symValue || iSym == symLit
         || iSym == symLparen)
      {
        switch (iSym)
            {
            case symIdent:
              {
                ZString ident (iIdent);
                getSym ();
                if (iSym == symLparen)
                  callFunc (aFunc, hInst, bInst, tInst, ident);
                else
                    {
                      Instruction varInst;
                      index (aFunc, hInst, &varInst, tInst, ident);
                      Instruction *vlast = varInst.last ();
                      if (vlast->iCode == cdPush)
                        ident = vlast->iText;
                      Variable *var;
                      if (aFunc)
                        var = aFunc->findVar (ident);
                      else
                          {
                            var = iStat->findVar (ident, zFalse);
                            if (!var)
                              var = iBlock->findVar (ident);
                          }     // if
                      if (iSym == symIncr || iSym == symDecr)
                          {
                            if (var->iIsConst)
                              throwExcept (msgWriteConst);
                            tInst->copy (&varInst)->add (iSym ==
                                                         symIncr ? cdIncV :
                                                         cdDecV);
                            getSym ();
                          }     // if
                      if (vlast->iCode == cdPush && var->iIsKnown)
                        bInst->add (cdPush, var->value ());
                      else
                        bInst->add (&varInst)->add (cdLoad);
                    }           // if
                break;
              }
            case symValue:
              bInst->add (cdPush, ZString (iValue));
              getSym ();
              break;
            case symLit:
              {
                ZString lit;
                do
                    {
                      lit += iIdent;
                      getSym ();
                    }
                while (iSym == symLit);
                bInst->add (cdPush, lit);
                break;
              }
            default:           // symLparen
              getSym ();
              orOp (aFunc, hInst, bInst, tInst);
              if (iSym != symRparen)
                throwExcept (msgMissRparen);
              getSym ();
              break;
            }                   // switch
      }
  else
    throwExcept (msgMissFactor);
}                               // factor

void ZCsl::term (Function * aFunc,
                 Instruction * hInst,
                 Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::term(...)");
  factor (aFunc, hInst, bInst, tInst);
  while (iSym == symTimes || iSym == symSlash || iSym == symDiv
         || iSym == symMod)
      {
        Symbol mul = iSym;
        getSym ();
        Instruction f2;
        factor (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
            {
              ZString t1 (blast->iText);
              ZString t2 (flast->iText);
              if (!isNumber (t1.constBuffer ())
                  || !isNumber (t2.constBuffer ()))
                throwExcept (msgInvOpStrings);
              switch (mul)
                  {
                  case symTimes:
                    blast->iText = ZString (t1.asDouble () * t2.asDouble ());
                    break;
                  case symSlash:
                    {
                      double x2 = t2.asDouble ();
                      if (!x2)
                        throwExcept (msgDivZero);
                      blast->iText = ZString (t1.asDouble () / x2);
                      break;
                    }
                  case symDiv:
                    {
                      long x2 = t2.asInt ();
                      if (!x2)
                        throwExcept (msgDivZero);
                      blast->iText = ZString (t1.asInt () / x2);
                      break;
                    }
                  default:
                    {
                      long x2 = t2.asInt ();
                      if (!x2)
                        throwExcept (msgDivZero);
                      blast->iText = ZString (t1.asInt () % x2);
                    }
                  }             // switch
            }
        else
          blast->add (&f2)->add (mul == symTimes ? cdMul :
                                 mul == symSlash ? cdDiv :
                                 mul == symDiv ? cdDivI : cdMod);
      }                         // while
}                               // term

void ZCsl::addops (Function * aFunc,
                   Instruction * hInst,
                   Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::addops(...)");
  term (aFunc, hInst, bInst, tInst);
  while (iSym == symPlus || iSym == symMinus || iSym == symCat)
      {
        Symbol add = iSym;
        getSym ();
        Instruction f2;
        term (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
            {
              ZString t1 (blast->iText);
              ZString t2 (flast->iText);
              if (!isNumber (t1.constBuffer ())
                  || !isNumber (t2.constBuffer ()) || add == symCat)
                  {
                    if (add == symMinus)
                      throwExcept (msgInvOpStrings);
                    blast->iText += t2;
                  }
              else
                  {
                    double x1 = t1.asDouble ();
                    double x2 = t2.asDouble ();
                    if (add == symPlus)
                      blast->iText = ZString (x1 + x2);
                    else
                      blast->iText = ZString (x1 - x2);
                  }             // if
            }
        else
          blast->add (&f2)->add ((add == symCat)
                                 ? cdCat : (add == symPlus) ? cdAdd : cdSub);
      }                         // while
}                               // addops

void ZCsl::lessgtr (Function * aFunc,
                    Instruction * hInst,
                    Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::lessgtr(...)");
  addops (aFunc, hInst, bInst, tInst);
  while (iSym == symLss || iSym == symGtr || iSym == symLeq || iSym == symGeq)
      {
        Symbol cmp = iSym;
        getSym ();
        Instruction f2;
        addops (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
            {
              ZString t1 (blast->iText);
              ZString t2 (flast->iText);
              if (!isNumber (t1.constBuffer ())
                  || !isNumber (t2.constBuffer ()))
                  {
                    switch (cmp)
                        {
                        case symLss:
                          blast->set (t1 < t2);
                          break;
                        case symLeq:
                          blast->set (t1 <= t2);
                          break;
                        case symGtr:
                          blast->set (t1 > t2);
                          break;
                        default:   // symGeq
                          blast->set (t1 >= t2);
                        }       // switch
                  }
              else
                  {
                    double x1 (t1.asDouble ());
                    double x2 (t2.asDouble ());
                    switch (cmp)
                        {
                        case symLss:
                          blast->set (x1 < x2);
                          break;
                        case symLeq:
                          blast->set (x1 <= x2);
                          break;
                        case symGtr:
                          blast->set (x1 > x2);
                          break;
                        default:   // symGeq
                          blast->set (x1 >= x2);
                        }       // switch
                  }             // if
            }
        else
            {
              blast->add (&f2);
              switch (cmp)
                  {
                  case symLss:
                    blast->add (cdLss);
                    break;
                  case symLeq:
                    blast->add (cdLeq);
                    break;
                  case symGtr:
                    blast->add (cdGtr);
                    break;
                  default:     // symGeq
                    blast->add (cdGeq);
                  }             // switch
            }                   // if
      }                         // while
}                               // lessgtr

void ZCsl::equal (Function * aFunc,
                  Instruction * hInst,
                  Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::equal(...)");
  lessgtr (aFunc, hInst, bInst, tInst);
  while (iSym == symEql || iSym == symNeq)
      {
        Symbol cmp = iSym;
        getSym ();
        Instruction f2;
        lessgtr (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
            {
              ZString t1 (blast->iText);
              ZString t2 (flast->iText);
              if (!isNumber (t1.constBuffer ())
                  || !isNumber (t2.constBuffer ()))
                  {
                    if (cmp == symEql)
                      blast->set (t1 == t2);
                    else
                      blast->set (t1 != t2);
                  }
              else
                  {
                    double x1 = t1.asDouble ();
                    double x2 = t2.asDouble ();
                    if (cmp == symEql)
                      blast->set (x1 == x2);
                    else
                      blast->set (x1 != x2);
                  }             // if
            }
        else
          blast->add (&f2)->add (cmp == symEql ? cdEql : cdNeq);
      }                         // while
}                               // equal

void ZCsl::andOp (Function * aFunc,
                  Instruction * hInst,
                  Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::andOp(...)");
  equal (aFunc, hInst, bInst, tInst);
  while (iSym == symAnd)
      {
        getSym ();
        Instruction f2;
        equal (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
          blast->set (blast->cond () && flast->cond ());
        else
          blast->add (&f2)->add (cdAnd);
      }                         // while
}                               // and

void ZCsl::orOp (Function * aFunc,
                 Instruction * hInst,
                 Instruction * bInst, Instruction * tInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::orOp(...)");
  andOp (aFunc, hInst, bInst, tInst);
  while (iSym == symOr)
      {
        getSym ();
        Instruction f2;
        andOp (aFunc, hInst, &f2, tInst);
        Instruction *blast = bInst->last ();
        Instruction *flast = f2.last ();
        if (blast->iCode == cdPush && flast->iCode == cdPush)
          blast->set (blast->cond () || flast->cond ());
        else
          blast->add (&f2)->add (cdOr);
      }                         // while
}                               // or

void ZCsl::becomes (Function * aFunc,
                    Instruction * hInst,
                    Instruction * bInst, Instruction * tInst, ZBoolean aSave)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::becomes(...)");
  Instruction body;
  orOp (aFunc, hInst, &body, tInst);
  if (iSym == symBecomes || iSym == symCatBec ||
      iSym == symAddBec || iSym == symSubBec ||
      iSym == symMulBec || iSym == symDivBec ||
      iSym == symDivIBec || iSym == symModBec)
      {
        Symbol symBec (iSym);
        getSym ();
        Instruction *l = body.last ();
        if (l->iCode != cdLoad)
          throwExcept (msgMissLvalue);
        delete l->iNext;
        l->iCode = cdNop;
        l->iNext = 0;
        becomes (aFunc, hInst, bInst, tInst, zTrue);
        l = bInst->last ();
        ZBoolean cnst (l->iCode == cdPush);
        if (aSave && !cnst)
          bInst->add (cdDupl);
        bInst->add (&body);
        switch (symBec)
            {
            case symAddBec:
              bInst->add (cdAddV);
              break;
            case symSubBec:
              bInst->add (cdSubV);
              break;
            case symCatBec:
              bInst->add (cdCatV);
              break;
            case symMulBec:
              bInst->add (cdMulV);
              break;
            case symDivBec:
              bInst->add (cdDivV);
              break;
            case symDivIBec:
              bInst->add (cdDivIV);
              break;
            case symModBec:
              bInst->add (cdModV);
              break;
            default:
              bInst->add (cdStore);
            }                   // switch
        if (aSave && cnst)
          bInst->add (cdPush, l->iText);
      }
  else
      {
        if (aSave)
          bInst->add (&body);
        else
            {
              // check for functions called
              Instruction *i = &body;
              while (i)
                  {
                    if (i->iCode == cdCall)
                        {
                          bInst->add (&body)->add (cdPop);
                          break;
                        }       // if
                    i = i->iNext;
                  }             // while
            }                   // if
      }                         // if
}                               // becomes

void ZCsl::expr (Function * aFunc, Instruction * bInst, ZBoolean aSave)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::expr(Function* aFunc, Instruction* bInst, ZBoolean aSave)");
  Instruction body, tail;
  becomes (aFunc, bInst, &body, &tail, aSave);
  if (aSave)
      {
        Instruction *blast = body.last ();
        ZBoolean cnst (blast->iCode == cdPush);
        ZString cval (blast->iText);
        if (cnst)
            {
              delete blast->iNext;
              blast->iNext = 0;
              blast->iCode = cdNop;
              blast->iText = iNulString;
            }                   // if
        bInst->add (&body)->add (&tail);
        if (cnst)
          bInst->add (cdPush, cval);
      }
  else
    bInst->add (&body)->add (&tail);
}                               // expr

void ZCsl::simple (Function * aFunc, Instruction * bInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::simple(Function* aFunc, Instruction* bInst)");
  if (iSym == symVar || iSym == symConst)
      {
        Symbol varSym (iSym);
        do
            {
              getSym ();
              if (iSym != symIdent)
                throwExcept (msgIdentExpct);
              ZString ident (iIdent);
              ZString rawName (ident);
              getSym ();
              Instruction varadr, tail;
              index (aFunc, bInst, &varadr, &tail, ident);
              Instruction *vlast = varadr.last ();
              if (iSym == symBecomes)
                  {
                    getSym ();
                    if (iSym == symBegin)
                        {
                          long dims[MAXDIMS];
                          int d;
                          for (d = 0; d < MAXDIMS; d++)
                            dims[d] = 0;
                          Instruction body;
                          arrayInit (aFunc, bInst, &body, &tail, rawName,
                                     dims);
                          for (d = 0; d < MAXDIMS; d++)
                              {
                                if (!dims[d])
                                  break;
                                rawName += "[" + ZString (dims[d]) + "]";
                              } // for
                          bInst->add (cdPush)->add (cdPush,
                                                    rawName)->add (varSym ==
                                                                   symConst ?
                                                                   cdAllC :
                                                                   cdAllV)->
                            add (&body);
                          aFunc->iBlocks->addVar (rawName, iNulString,
                                                  varSym == symConst);
                        }
                    else
                        {
                          expr (aFunc, bInst);
                          Instruction *blast = bInst->last ();
                          bInst->add (&varadr)->add (varSym ==
                                                     symConst ? cdAllC :
                                                     cdAllV);
                          ZString val;
                          if (blast->iCode == cdPush)
                            val = blast->iText;
                          aFunc->iBlocks->addVar (vlast->iCode ==
                                                  cdPush ? vlast->
                                                  iText : ident, val,
                                                  varSym == symConst, zFalse,
                                                  vlast->iCode == cdPush
                                                  && blast->iCode == cdPush);
                        }       // if
                    bInst->add (&tail);
                  }
              else
                  {
                    if (varSym == symConst)
                      throwExcept (msgInitExpct);
                    aFunc->iBlocks->addVar (vlast->iCode ==
                                            cdPush ? vlast->iText : ident);
                    bInst->add (cdPush)->add (&varadr)->add (cdAllV)->
                      add (&tail);
                  }             // if
            }
        while (iSym == symComma);
      }
  else
    while (zTrue)
        {
          expr (aFunc, bInst, zFalse);
          if (iSym != symComma)
            break;
          getSym ();
        }                       // while
}                               // simple

void ZCsl::stmt (Function * aFunc,
                 Instruction * bInst, int flags, int brkLevel, int cntLevel)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::stmt(...)");
  static Instruction *breakAddr = (Instruction *) 1;
  static Instruction *continueAddr = (Instruction *) 2;

  switch (iSym)
      {
      case symBreak:
        {
          if (!(flags & 1))
            throwExcept (msgInvBreak);
          getSym ();
          while (brkLevel--)
            bInst->add (cdClose);
          bInst->add (cdJmp, breakAddr);
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          break;
        }
      case symContin:
        {
          if (!(flags & 2))
            throwExcept (msgInvContin);
          getSym ();
          while (cntLevel--)
            bInst->add (cdClose);
          bInst->add (cdJmp, continueAddr);
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          break;
        }
      case symThrow:
        {
          getSym ();
          expr (aFunc, bInst);
          if (bInst->last ()->iCode == cdLoad)
            bInst->last ()->iCode = cdThroV;
          else
            bInst->add (cdThrow);
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          break;
        }
      case symTry:
        {
          getSym ();
          if (iSym != symBegin)
            throwExcept (msgBeginNotFound);
          aFunc->openBlock ();
          bInst->add (cdOpen);
          Instruction *tryInst = bInst->add (cdTry);
          getSym ();
          while (iSym != symEnd)
            stmt (aFunc, bInst, flags, brkLevel + 1, cntLevel + 1);
          getSym ();
          bInst->add (cdClose);
          aFunc->closeBlock ();
          Instruction *jmp = bInst->add (cdJmp);
          if (iSym != symCatch)
            throwExcept (msgCatchNotFound);
          getSym ();
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();
          if (iSym != symVar && iSym != symConst)
            throwExcept (msgVarSymExpct);
          Symbol varSym (iSym);
          getSym ();
          if (iSym != symIdent)
            throwExcept (msgIdentExpct);
          ZString ident (iIdent);
          getSym ();
          if (iSym != symLbrack)
            throwExcept (msgMissLbrack);
          getSym ();
          if (iSym != symRbrack)
            throwExcept (msgMissRbrack);
          getSym ();
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          if (iSym != symBegin)
            throwExcept (msgBeginNotFound);
          aFunc->openBlock ();
          tryInst->iAddr = bInst->add (cdOpen);
          aFunc->iBlocks->addVar (ident + "[1]", iNulString,
                                  varSym == symConst);
          bInst->add (varSym == symConst ? cdAllTC : cdAllTV, ident);
          getSym ();
          while (iSym != symEnd)
            stmt (aFunc, bInst, flags, brkLevel + 1, cntLevel + 1);
          getSym ();
          bInst->add (cdClose);
          aFunc->closeBlock ();
          jmp->iAddr = bInst->next ();
          break;
        }
      case symBegin:
        {
          aFunc->openBlock ();
          bInst->add (cdOpen);
          getSym ();
          while (iSym != symEnd)
            stmt (aFunc, bInst, flags, brkLevel + 1, cntLevel + 1);
          getSym ();
          bInst->add (cdClose);
          aFunc->closeBlock ();
          break;
        }
      case symRet:
        {
          getSym ();
          if (iSym != symSemi)
            expr (aFunc, bInst);
          else
            bInst->add (cdPush);
          bInst->add (cdRet);
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          break;
        }
      case symResize:
        {
          getSym ();
          if (iSym != symIdent)
            throwExcept (msgIdentExpct);
          Variable *var = aFunc->findVar (iIdent);
          if (var->iIsConst)
            throwExcept (msgWriteConst);
          ZString ident (iIdent);
          getSym ();
          Instruction body, tail;
          if (index (aFunc, bInst, &body, &tail, ident) != var->dims ())
            throwExcept (msgInvalidIndex);
          bInst->add (&body)->add (cdRSize)->add (&tail);
          break;
        }
      case symIf:
        {
          getSym ();
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();
          Instruction cond;
          expr (aFunc, &cond);
          Instruction *clast = cond.last ();
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          if (clast->iCode == cdPush)
              {
                ZString t1 = clast->iText;
                ZBoolean yes;
                if (isNumber (t1.constBuffer ()))
                  yes = t1.asDouble () != 0.0;
                else
                  yes = t1.size () > 0;
                Instruction trash;
                if (yes)
                    {
                      stmt (aFunc, bInst, flags, brkLevel, cntLevel);
                      if (iSym == symElse)
                          {
                            getSym ();
                            stmt (aFunc, &trash, flags, brkLevel, cntLevel);
                          }     // if
                    }
                else
                    {
                      stmt (aFunc, &trash, flags, brkLevel, cntLevel);
                      if (iSym == symElse)
                          {
                            getSym ();
                            stmt (aFunc, bInst, flags, brkLevel, cntLevel);
                          }     // if
                    }           // if
              }
          else
              {
                bInst->add (&cond);
                Instruction *ifInst = bInst->add (cdJF);
                stmt (aFunc, bInst, flags, brkLevel, cntLevel);
                if (iSym == symElse)
                    {
                      Instruction *elseInst = bInst->add (cdJmp);
                      ifInst->iAddr = bInst->next ();
                      ifInst = elseInst;
                      getSym ();
                      stmt (aFunc, bInst, flags, brkLevel, cntLevel);
                    }           // if
                ifInst->iAddr = bInst->next ();
              }                 // if
          break;
        }
      case symSwitch:
        {
          getSym ();
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();
          expr (aFunc, bInst);
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          if (iSym != symBegin)
            throwExcept (msgBeginNotFound);
          aFunc->openBlock ();
          bInst->add (cdOpen)->add (cdPush, Zero)->add (cdPush,
                                                        "%")->add (cdAllV);
          Instruction *fix = bInst->next ();
          Instruction *nxt = 0;
          getSym ();
          while (iSym == symCase)
              {
                getSym ();
                // duplicate switch value
                bInst->add (cdDupl);
                // get expression
                expr (aFunc, bInst);
                if (iSym != symColon)
                  throwExcept (msgColonNotFound);
                getSym ();
                // compare
                bInst->add (cdEql);
                Instruction *ifInst = bInst->add (cdJF);
                bInst->add (cdPush, "%")->add (cdIncV);
                if (nxt)
                  nxt->iAddr = bInst->next ();
                while (iSym != symCase &&
                       iSym != symDefault && iSym != symEnd)
                  stmt (aFunc, bInst, flags | 1, 0, cntLevel + 1);
                Instruction *l = bInst->last ();
                if (l->iCode == cdJmp && l->iAddr == breakAddr)
                  nxt = 0;
                else
                  nxt = bInst->add (cdJmp);
                ifInst->iAddr = bInst->next ();
              }                 // while
          if (iSym == symDefault)
              {
                getSym ();
                if (iSym != symColon)
                  throwExcept (msgColonNotFound);
                getSym ();
                bInst->add (cdPush, "%")->add (cdLoad);
                Instruction *ifInst = bInst->add (cdJT);
                if (nxt)
                  nxt->iAddr = bInst->next ();
                while (iSym != symEnd)
                  stmt (aFunc, bInst, flags | 1, 0, cntLevel + 1);
                Instruction *l = bInst->last ();
                if (l->iCode == cdJmp && l->iAddr == breakAddr)
                    {
                      delete l->iNext;
                      l->iCode = cdNop;
                      l->iNext = 0;
                    }           // if
                ifInst->iAddr = bInst->next ();
              }
          else if (nxt)
            nxt->iAddr = bInst->next ();
          if (iSym != symEnd)
            throwExcept (msgEndNotFound);
          getSym ();
          while (fix)
              {
                if (fix->iCode == cdJmp && fix->iAddr == breakAddr)
                  fix->iAddr = bInst->next ();
                fix = fix->iNext;
              }                 // while
          bInst->add (cdClose);
          aFunc->closeBlock ();
          break;
        }
      case symDo:
        {
          getSym ();
          Instruction *start = bInst->next ();
          stmt (aFunc, bInst, 3, 0, 0);
          if (iSym != symWhile)
            throwExcept (msgMissWhile);
          getSym ();
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();
          Instruction cond;
          expr (aFunc, &cond);
          Instruction *clast = cond.last ();
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          Instruction *check = bInst->next ();
          if (clast->iCode == cdPush)
              {
                ZString t1 = cond.iText;
                ZBoolean yes;
                if (isNumber (t1.constBuffer ()))
                  yes = t1.asDouble () != 0.0;
                else
                  yes = t1.size () > 0;
                if (yes)
                  bInst->add (cdJmp, start);
              }
          else
              {
                check = bInst->next ();
                bInst->add (&cond)->add (cdJT, start);
              }                 // if
          while (start)
              {
                if (start->iCode == cdJmp)
                    {
                      if (start->iAddr == breakAddr)
                        start->iAddr = bInst->next ();
                      else if (start->iAddr == continueAddr)
                        start->iAddr = check;
                    }           // if
                start = start->iNext;
              }                 // while
          break;
        }
      case symWhile:
        {
          getSym ();
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();
          Instruction cond;
          expr (aFunc, &cond);
          Instruction *clast = cond.last ();
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          Instruction *condInst = bInst->next ();
          Instruction *fix = condInst;
          if (clast->iCode == cdPush)
              {
                ZString t1 = cond.iText;
                ZBoolean yes;
                if (isNumber (t1.constBuffer ()))
                  yes = t1.asDouble () != 0.0;
                else
                  yes = t1.size () > 0;
                if (yes)
                    {
                      stmt (aFunc, bInst, 3, 0, 0);
                      bInst->add (cdJmp, condInst);
                    }
                else
                    {
                      Instruction trash;
                      stmt (aFunc, &trash, 3, 0, 0);
                    }           // if
              }
          else
              {
                bInst->add (&cond);
                Instruction *branchInst = bInst->add (cdJF);
                stmt (aFunc, bInst, 3, 0, 0);
                bInst->add (cdJmp, condInst);
                branchInst->iAddr = bInst->next ();
              }                 // if
          while (fix)
              {
                if (fix->iCode == cdJmp)
                    {
                      if (fix->iAddr == breakAddr)
                        fix->iAddr = bInst->next ();
                      else if (fix->iAddr == continueAddr)
                        fix->iAddr = condInst;
                    }           // if
                fix = fix->iNext;
              }                 // while
          break;
        }
      case symFor:
        {
          getSym ();            // read (
          if (iSym != symLparen)
            throwExcept (msgMissLparen);
          getSym ();            // first of initializer
          if (iSym != symSemi)
            simple (aFunc, bInst);  // initializer
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          Instruction cond;
          if (iSym == symSemi)
            cond.add (cdPush, One);
          else
            expr (aFunc, &cond);
          Instruction *clast = cond.last ();
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          Instruction trail;
          if (iSym != symRparen)
            simple (aFunc, &trail);
          if (iSym != symRparen)
            throwExcept (msgMissRparen);
          getSym ();
          Instruction *condInst = bInst->next ();
          Instruction *trailAddr = condInst;
          if (clast->iCode == cdPush)
              {
                ZString t1 = cond.iText;
                ZBoolean yes;
                if (isNumber (t1.constBuffer ()))
                  yes = t1.asDouble () != 0.0;
                else
                  yes = t1.size () > 0;
                if (yes)
                    {
                      if (iSym == symBegin)
                        stmt (aFunc, bInst, 3, 0, 0);
                      else
                          {
                            bInst->add (cdOpen);
                            stmt (aFunc, bInst, 3, 0, 0);
                            bInst->add (cdClose);
                          }     // if
                      trailAddr = bInst->next ();
                      bInst->add (&trail);
                      bInst->add (cdJmp, condInst);
                    }
                else
                    {
                      Instruction trash;
                      stmt (aFunc, &trash, 3, 0, 0);
                    }           // if
              }
          else
              {
                bInst->add (&cond);
                Instruction *jmpFalse = bInst->add (cdJF);
                if (iSym == symBegin)
                  stmt (aFunc, bInst, 3, 0, 0);
                else
                    {
                      bInst->add (cdOpen);
                      stmt (aFunc, bInst, 3, 0, 0);
                      bInst->add (cdClose);
                    }           // if
                trailAddr = bInst->next ();
                bInst->add (&trail);
                if (trailAddr == bInst->next ())
                  trailAddr = condInst;
                bInst->add (cdJmp, condInst);
                jmpFalse->iAddr = bInst->next ();
              }                 // if
          while (condInst)
              {
                if (condInst->iCode == cdJmp)
                    {
                      if (condInst->iAddr == breakAddr)
                        condInst->iAddr = bInst->next ();
                      else if (condInst->iAddr == continueAddr)
                        condInst->iAddr = trailAddr;
                    }           // if
                condInst = condInst->iNext;
              }                 // while
          break;
        }
      case symTrace:
        {
          getSym ();
          expr (aFunc, bInst);
          bInst->add (cdTrace);
          if (iSym != symSemi)
            throwExcept (msgSemiNotFound);
          getSym ();
          break;
        }
      case symSemi:
        getSym ();
        break;
      case symEof:
        throwExcept (msgEarlyEof);
      default:
        simple (aFunc, bInst);
        if (iSym != symSemi)
          throwExcept (msgSemiNotFound);
        getSym ();
      }                         // switch
}                               // stmt

void ZCsl::openStat (const ZString & aFileName)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::openStat(const ZString& aFileName)");
  iStat = iStats;
  while (iStat)
      {
        Variable *var = iStat->findVar ("cslFileName");
        if (var->value () == aFileName)
          return;
        iStat = iStat->iPrev;
      }                         // while
  iStat = iStats = new Block (this, iStats);
  iStat->addVar ("cslFileName", aFileName, zTrue);
#ifdef ZC_CSS_COMPATIBLE
  iStat->addVar ("cssFileName", aFileName, zTrue);
#endif
}                               // openStat

static void encodeString (ZString & s)
{
  ZFUNCTRACE_DEVELOP ("encodeString(ZString& s)");
  int l (s.length ());
  long i;
  for (i = 1; i <= l; i++)
    if (s[i] == '\xdf' || s[i] == '\0')
      break;
  if (i >= l)
    return;
  ZString d;
  for (i = 1; i <= l; i++)
    if (s[i] == '\xdf')
      d += "\xdf\xdf";
    else if (s[i] == '\0')
      d += "\xdf\x30";
    else
      d += s[i];
  s = d;
}                               // encodeString

void ZCsl::packCode (Function * aFunc)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::packCode(Function* aFunc)");
  static char empty[1] = { 0 };
  Instruction *i = aFunc->iInst;
  aFunc->iPsize = aFunc->iTsize = 0;
  while (i->iNext)
      {
        aFunc->iPsize++;
        switch (i->iCode)
            {
            case cdPush:
            case cdAllTV:
            case cdAllTC:
            case cdOpen:
              if (i->iText.length ())
                  {
                    encodeString (i->iText);
                    Instruction *ii;
                    for (ii = aFunc->iInst; i->iText != ii->iText;
                         ii = ii->iNext);
                    if (ii == i)
                      aFunc->iTsize += i->iText.length () + 1;
                  }             // if
            default:;
            }                   // switch
        i = i->iNext;
      }                         // while
  if (aFunc->iPsize)
      {
        PackedInstruction *p = aFunc->iPcode =
          new PackedInstruction[aFunc->iPsize];
        if (aFunc->iTsize)
          aFunc->iText = new char[aFunc->iTsize];
        char *t = aFunc->iText;
        for (i = aFunc->iInst; i->iNext; i = i->iNext, p++)
            {
              p->iCode = i->iCode;
              switch (i->iCode)
                  {
                  case cdPush:
                  case cdAllTV:
                  case cdAllTC:
                  case cdOpen:
                    if (i->iText.length ())
                        {
                          char *tt;
                          for (tt = aFunc->iText; tt < t;)
                              {
                                char *s = i->iText;
                                p->iText = tt;
                                while (*tt && *tt == *s)
                                    {
                                      tt++;
                                      s++;
                                    }
                                if (!*tt && !*s)
                                  break;
                                while (*tt++);
                              } // for
                          if (tt == t)
                              {
                                p->iText = t;
                                char *s = i->iText;
                                while (*s)
                                  *t++ = *s++;
                                *t++ = 0;
                              } // if
                        }
                    else
                      p->iText = empty;
                    break;
                  case cdJmp:
                  case cdJF:
                  case cdJT:
                  case cdTry:
                    {
                      long addr (0);
                      Instruction *ii;
                      for (ii = aFunc->iInst; ii; ii = ii->iNext, addr++)
                        if (ii == i->iAddr)
                            {
                              p->iAddr = addr;
                              break;
                            }   // if
                      break;
                    }
                  case cdCall:
                    p->iFunc = i->iFunc;
                    break;
                  default:
                    p->iAddr = 0;
                  }             // switch
            }                   // for
      }                         // if
  delete aFunc->iInst;
  aFunc->iInst = 0;
}                               // packCode

ZExport (ZCsl &) ZCsl::loadScript (const ZString & aFileName, istream * aStr)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::loadScript(const ZString& aFileName, istream* aStr)");
  State saveState (iState);
  Block *saveStat = iStat;
  InputStream *saveInput = iInput;
  Symbol saveSym (iSym);
  ZString saveIdent (iIdent);
  double saveValue (iValue);

  try
  {
    iState = isNormal;
    iCurrFunc = 0;
    openStat (aFileName);
    iInput = new InputStream (this, aFileName, aStr, 0);
    iState = isCompiling;
    getSym ();
    while (iSym == symVar || iSym == symConst ||
           iSym == symStatic || iSym == symExtern || iSym == symIdent)
        {
          ZBoolean iStatic (zFalse), iExtern (zFalse);
          if (iSym == symStatic)
              {
                iStatic = zTrue;
                getSym ();
              }
          else if (iSym == symExtern)
              {
                iExtern = zTrue;
                getSym ();
              }                 // if
          if (iSym != symVar && iSym != symConst && iSym != symIdent)
            throwExcept (msgVarSymExpct);
          Symbol varSym (iSym);
          if (iSym == symVar || iSym == symConst)
            getSym ();
          if (iSym != symIdent)
            throwExcept (msgIdentExpct);
          ZString ident (iIdent);
          getSym ();
          if (iSym == symLparen)
              {
                if (iExtern)
                  throwExcept (msgSemiNotFound);
                if (varSym == symConst)
                  throwExcept (msgInitExpct);
                Function *aFunc = new Function (this, ident, 0);
                aFunc->openBlock ();
                getParamList (aFunc, zTrue);
                aFunc = addFunc (aFunc, iStatic);
                if (iSym == symSemi)
                    {
                      aFunc->closeBlock ();
                      delete aFunc->iInst;
                      aFunc->iInst = 0;
                      getSym ();
                    }
                else
                    {
                      iCurrFunc = aFunc;
                      if (iSym != symBegin)
                        throwExcept (msgBeginNotFound);
                      Instruction *bInst = aFunc->iInst;
                      getSym ();
                      while (iSym != symEnd)
                        stmt (aFunc, bInst, 0, 0, 0);
                      getSym ();
                      ZBoolean addRet (zFalse);
                      if (!bInst->iNext)
                        addRet = zTrue;
                      else if (bInst->last ()->iCode != cdRet)
                        addRet = zTrue;
                      if (addRet)
                        bInst->add (cdPush)->add (cdRet);
                      aFunc->closeBlock ();
                      packCode (aFunc);
                      if (iList)
                        dump (aFunc);
                      iCurrFunc = 0;
                    }           // if
              }
          else
              {
                // variable/constant declaration
                if (varSym == symIdent)
                  throwExcept (msgVarSymExpct);
                for (;;)
                    {
                      Instruction varInst;
                      Instruction valInst;
                      if (iExtern)
                          {
                            while (iSym == symLbrack)
                                {
                                  getSym ();
                                  if (iSym != symRbrack)
                                    throwExcept (msgMissRbrack);
                                  getSym ();
                                  ident += "[1]";
                                }   // while
                            iBlock->addVar (ident, iNulString,
                                            varSym == symConst, zFalse,
                                            zFalse, zTrue);
                          }
                      else
                          {
                            ZString rawName (ident);
                            Instruction head, tail;
                            index (0, &head, &varInst, &tail, ident);
                            Instruction *vlast = varInst.last ();
                            ZBoolean arrInit (zFalse);
                            if (vlast->iCode != cdPush)
                              throwExcept (msgIndexNonConst);
                            if (iSym != symBecomes)
                                {
                                  if (varSym == symConst)
                                    throwExcept (msgInitExpct);
                                  valInst.add (cdPush);
                                }
                            else
                                {
                                  getSym ();
                                  if (iSym == symBegin)
                                      {
                                        valInst.add (cdPush);
                                        arrInit = zTrue;
                                      }
                                  else
                                      {
                                        expr (0, &valInst);
                                        vlast = valInst.last ();
                                      }   // if
                                }   // if
                            if (vlast->iCode != cdPush)
                              throwExcept (msgInitNonConst);
                            Variable *var = iStat->findVar (ident, zFalse);
                            if (var)
                                {
                                  if (!var->iIsExtern)
                                    throwExcept (msgVarAlrdyDef);
                                  if (!iStatic)
                                    throwExcept (msgVarDiffers, ident);
                                }
                            else
                                {
                                  var = iBlock->findVar (ident, zFalse);
                                  if (var)
                                      {
                                        if (!var->iIsExtern)
                                          throwExcept (msgVarAlrdyDef);
                                        if (iStatic)
                                          throwExcept (msgVarDiffers, ident);
                                      }   // if
                                }   // if
                            if (iStatic)
                              var = iStat->addVar (varInst.last ()->iText,
                                                   valInst.last ()->iText,
                                                   varSym == symConst, zFalse,
                                                   zTrue);
                            else
                              var = iBlock->addVar (varInst.last ()->iText,
                                                    valInst.last ()->iText,
                                                    varSym == symConst,
                                                    zFalse, zTrue);
                            var->iIsGlobal = zTrue;
                            if (arrInit)
                                {
                                  Instruction head, tail;
                                  arrayInit (0, &head, &valInst, &tail,
                                             rawName, 0);
                                }   // if
                          }     // if
                      if (iSym != symComma)
                        break;
                      getSym ();
                      if (iSym != symIdent)
                        throwExcept (msgIdentExpct);
                      ident = iIdent;
                      getSym ();
                    }           // for
                if (iSym != symSemi)
                  throwExcept (msgSemiNotFound);
                getSym ();
              }                 // if
        }                       // while
    if (iSym != symEof)
      throwExcept (msgUnexpSymbol);
  }                             // try
  catch (const ZException & exc)
  {
    if (iInput)
      delete iInput;
    iStat = saveStat;
    iInput = saveInput;
    iSym = saveSym;
    iIdent = saveIdent;
    iValue = saveValue;
    iState = saveState;
    throw;
  }                             // catch

  iStat = saveStat;
  iInput = saveInput;
  iSym = saveSym;
  iIdent = saveIdent;
  iValue = saveValue;
  iState = saveState;
  return *this;
}                               // loadScript
