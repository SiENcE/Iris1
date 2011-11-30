/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Interprt.cpp
 * Application :  CSL kernel
 * Purpose     :  Interpreter
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.22  First implementation                        P.Koch, IBK
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.26  Speed up function calls                     P.Koch, IBK
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

#ifndef ZC_BUILDING_ZCSL
#define ZC_BUILDING_ZCSL
#endif

#include <stdio.h>
#include <ZCsl.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"

static ZString decodeString (char *s)
{
  ZFUNCTRACE_DEVELOP ("ZString decodeString(char *s)");
  char *x = s;
  while (*x && *x != '\xdf')
    x++;
  if (!*x)
    return s;
  ZString d;
  while (*s)
    if (*s == '\xdf')
        {
          s++;
          if (*s == '\xdf')
            d += *s++;
          else
              {
                char x (0);
                d += x;
                s++;
              }                 // if
        }
    else
      d += *s++;
  return d;
}                               // decodeString

ZString ZCsl::dumpInstr (long aAddr, PackedInstruction * aInstr,
                         ZBoolean withStack)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::dumpInstr(long aAddr, PackedInstruction* aInstr, ZBoolean withStack)");
  char *opcode;
  short param (0);
  switch (aInstr->iCode)
      {
      case cdOpen:
        opcode = "open";
        param = 2;
        break;
      case cdClose:
        opcode = "close";
        break;
      case cdSize:
        opcode = "size";
        break;
      case cdRSize:
        opcode = "rsize";
        break;
      case cdExist:
        opcode = "exist";
        break;
      case cdNeg:
        opcode = "neg";
        break;
      case cdNot:
        opcode = "not";
        break;
      case cdLoad:
        opcode = "load";
        break;
      case cdStore:
        opcode = "store";
        break;
      case cdStorC:
        opcode = "storc";
        break;
      case cdPush:
        opcode = "push";
        param = 2;
        break;
      case cdPop:
        opcode = "pop";
        break;
      case cdDupl:
        opcode = "dupl";
        break;
      case cdDivI:
        opcode = "divi";
        break;
      case cdMod:
        opcode = "mod";
        break;
      case cdMul:
        opcode = "mul";
        break;
      case cdDiv:
        opcode = "div";
        break;
      case cdAdd:
        opcode = "add";
        break;
      case cdSub:
        opcode = "sub";
        break;
      case cdCat:
        opcode = "cat";
        break;
      case cdMulV:
        opcode = "mulv";
        break;
      case cdDivV:
        opcode = "divv";
        break;
      case cdDivIV:
        opcode = "diviv";
        break;
      case cdModV:
        opcode = "modv";
        break;
      case cdAddV:
        opcode = "addv";
        break;
      case cdSubV:
        opcode = "subv";
        break;
      case cdCatV:
        opcode = "catv";
        break;
      case cdIncV:
        opcode = "incv";
        break;
      case cdDecV:
        opcode = "decv";
        break;
      case cdLss:
        opcode = "lss";
        break;
      case cdLeq:
        opcode = "leq";
        break;
      case cdGtr:
        opcode = "gtr";
        break;
      case cdGeq:
        opcode = "geq";
        break;
      case cdEql:
        opcode = "eql";
        break;
      case cdNeq:
        opcode = "neq";
        break;
      case cdAnd:
        opcode = "and";
        break;
      case cdOr:
        opcode = "or";
        break;
      case cdJmp:
        opcode = "jmp";
        param = 1;
        break;
      case cdJF:
        opcode = "jf";
        param = 1;
        break;
      case cdJT:
        opcode = "jt";
        param = 1;
        break;
      case cdCall:
        opcode = "call";
        param = 3;
        break;
      case cdRet:
        opcode = "ret";
        break;
      case cdTrace:
        opcode = "trace";
        break;
      case cdTry:
        opcode = "try";
        param = 1;
        break;
      case cdThrow:
        opcode = "throw";
        break;
      case cdThroV:
        opcode = "throv";
        break;
      case cdAllTV:
        opcode = "alltv";
        param = 2;
        break;
      case cdAllTC:
        opcode = "alltc";
        param = 2;
        break;
      case cdAllV:
        opcode = "allv";
        break;
      case cdAllC:
        opcode = "allc";
        break;
      case cdAllVR:
        opcode = "allvr";
        break;
      case cdAllCR:
        opcode = "allcr";
        break;
      case cdNop:
        opcode = "nop";
        break;
      default:
        opcode = "????";
        break;
      }                         // switch
  ZString pp;
  switch (param)
      {
      case 1:
        pp = ZString (aInstr->iAddr);
        break;
      case 2:
        pp = ZString::exp (aInstr->iText);
        break;
      case 3:
        pp = ((Function *) aInstr->iFunc)->iName;
        break;
      default:;
      }                         // switch
  char line[80];
  if (withStack)
      {
        ZString tos (ZString::exp (*iTos));
        ZString tos1;
        if (iTos > iStack)
          tos1 = ZString::exp (*(iTos - 1));
        sprintf (line, "%8ld %-7.6s%-21.20s%-21.20s%-.20s",
                 aAddr, opcode, (char *) pp, (char *) tos, (char *) tos1);
      }
  else
    sprintf (line, "%8ld %-7.6s%-.62s", aAddr, opcode, (char *) pp);
  return line;
}                               // dumpInstr

void ZCsl::dump (Function * aFunc)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::dump(Function* aFunc)");
  log ();
  log (funcHeader (aFunc).constBuffer ());
  log ();
  log ("address  opcode parameter");
  log
    ("-------- ------ --------------------------------------------------------------");
  PackedInstruction *p = aFunc->iPcode;
  for (long i (0); i < aFunc->iPsize; i++)
    log (dumpInstr (i, p++, zFalse).constBuffer ());
  ZString msg =
    "code: " + ZString (aFunc->iPsize * sizeof (PackedInstruction)) + "  " +
    "text: " + ZString (aFunc->iTsize) + "  " +
    "total: " + ZString (aFunc->iPsize * sizeof (PackedInstruction) +
                         aFunc->iTsize);
  log ();
  log (msg.constBuffer ());
}                               // dump

void ZCsl::checkNum (const ZString & aVal)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::checkNum(const ZString& aVal)");
  if (!isNumber (aVal))
    throwExcept (msgInvOpStrings, aVal);
}                               // checkNum

void ZCsl::interpret (Function * aFunc)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::interpret(Function* aFunc)");
  ZException *excpt = 0;
  PackedInstruction *loAddr = aFunc->iPcode;
  PackedInstruction *hiAddr = loAddr + aFunc->iPsize - 1;
  PackedInstruction *i = loAddr;
  ZString v2;
  Variable *v;
  double d2;
  long i2;
  ZBoolean head (zTrue);
  while (zTrue)
      {
        try
        {
          if (iTraceMode & traceCode)
              {
                if (head)
                    {
                      trace ('#');
                      trace ('#', funcHeader (aFunc).constBuffer ());
                      trace ('#');
                      trace ('#',
                             "address  opcode parameter            tos                  tos-1");
                      trace ('#',
                             "-------- ------ -------------------- -------------------- --------------------");
                      head = zFalse;
                    }           // if
                trace ('#',
                       dumpInstr (i - aFunc->iPcode, i,
                                  zTrue).constBuffer ());
              }
          switch (i->iCode)
              {
              case cdOpen:
                aFunc->openBlock (i->iText);
                i++;
                break;
              case cdClose:
                aFunc->closeBlock ();
                i++;
                break;
              case cdSize:
                push (aFunc->findVar (pop ())->iSize);
                i++;
                break;
              case cdRSize:
                {
                  ZString varName (pop ());
                  aFunc->findVar (pureVarName (varName))->resize (varName);
                  i++;
                  break;
                }
              case cdTrace:
                if (iTraceMode & traceMsgs)
                  trace ('>', pop ().constBuffer ());
                else
                  pop ();
                i++;
                break;
              case cdExist:
                {
                  ZString ret (One);
                  try
                  {
                    Variable *var = aFunc->findVar (pop (), zFalse);
                    if (!var
                        || var->iIsExtern
                        || var->iIndex < 0 || var->iIndex >= var->size ())
                      ret = Zero;
                  }             // try
                  catch (const ZException & exc)
                  {
                    ret = Zero;
                  }             // catch
                  push (ret);
                  i++;
                  break;
                }
              case cdNeg:
                checkNum (*iTos);
                *iTos = ZString (-iTos->asDouble ());
                i++;
                break;
              case cdNot:
                if (isNumber (iTos->constBuffer ()))
                  *iTos = iTos->asDouble ()? Zero : One;
                else
                  *iTos = *iTos == "" ? One : Zero;
                i++;
                break;
              case cdLoad:
                *iTos = aFunc->findVar (*iTos)->value ();
                i++;
                break;
              case cdStore:
                {
                  Variable *var = aFunc->findVar (pop ());
                  var->set (pop ());
                  i++;
                  break;
                }
              case cdStorC:
                {
                  Variable *var = aFunc->findVar (pop ());
                  var->set (pop (), zTrue);
                  i++;
                  break;
                }
              case cdPush:
                push (decodeString (i->iText));
                i++;
                break;
              case cdPop:
                pop ();
                i++;
                break;
              case cdDupl:
                push (*iTos);
                i++;
                break;
              case cdDivI:
                i2 = pop ().asInt ();
                if (!i2)
                  throwExcept (msgDivZero);
                checkNum (*iTos);
                *iTos = ZString (iTos->asInt () / i2);
                i++;
                break;
              case cdMod:
                i2 = pop ().asInt ();
                if (!i2)
                  throwExcept (msgDivZero);
                checkNum (*iTos);
                *iTos = ZString (iTos->asInt () % i2);
                i++;
                break;
              case cdMul:
                v2 = pop ();
                checkNum (v2);
                checkNum (*iTos);
                *iTos = ZString (iTos->asDouble () * v2.asDouble ());
                i++;
                break;
              case cdDiv:
                d2 = pop ().asDouble ();
                if (!d2)
                  throwExcept (msgDivZero);
                checkNum (*iTos);
                *iTos = ZString (iTos->asDouble () / d2);
                i++;
                break;
              case cdAdd:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos += v2;
                else
                  *iTos = ZString (iTos->asDouble () + v2.asDouble ());
                i++;
                break;
              case cdSub:
                v2 = pop ();
                checkNum (v2);
                checkNum (*iTos);
                *iTos = ZString (iTos->asDouble () - v2.asDouble ());
                i++;
                break;
              case cdCat:
                v2 = pop ();
                *iTos += v2;
                i++;
                break;
              case cdMulV:
                v = aFunc->findVar (pop ());
                v2 = pop ();
                checkNum (v2);
                checkNum (v->value ());
                v->set (v->value ().asDouble () * v2.asDouble ());
                i++;
                break;
              case cdDivV:
                v = aFunc->findVar (pop ());
                d2 = pop ().asDouble ();
                if (!d2)
                  throwExcept (msgDivZero);
                checkNum (v->value ());
                v->set (v->value ().asDouble () / d2);
                i++;
                break;
              case cdDivIV:
                v = aFunc->findVar (pop ());
                i2 = pop ().asInt ();
                if (!i2)
                  throwExcept (msgDivZero);
                checkNum (v->value ());
                v->set (v->value ().asInt () / i2);
                i++;
                break;
              case cdModV:
                v = aFunc->findVar (pop ());
                i2 = pop ().asInt ();
                if (!i2)
                  throwExcept (msgDivZero);
                checkNum (v->value ());
                v->set (v->value ().asInt () % i2);
                i++;
                break;
              case cdAddV:
                v = aFunc->findVar (pop ());
                v2 = pop ();
                if (!isNumber (v->value ().constBuffer ())
                    || !isNumber (v2.constBuffer ()))
                  v->set (v->value () + v2);
                else
                  v->set (v->value ().asDouble () + v2.asDouble ());
                i++;
                break;
              case cdSubV:
                v = aFunc->findVar (pop ());
                v2 = pop ();
                checkNum (v2);
                checkNum (v->value ());
                v->set (v->value ().asDouble () - v2.asDouble ());
                i++;
                break;
              case cdCatV:
                v = aFunc->findVar (pop ());
                v2 = pop ();
                v->set (v->value () + v2);
                i++;
                break;
              case cdIncV:
                v = aFunc->findVar (pop ());
                checkNum (v->value ());
                v->set (v->value ().asDouble () + 1.0);
                i++;
                break;
              case cdDecV:
                v = aFunc->findVar (pop ());
                checkNum (v->value ());
                v->set (v->value ().asDouble () - 1.0);
                i++;
                break;
              case cdLss:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos < v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () < v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdLeq:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos <= v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () <= v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdGtr:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos > v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () > v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdGeq:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos >= v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () >= v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdEql:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos == v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () == v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdNeq:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = *iTos != v2 ? One : Zero;
                else
                  *iTos = iTos->asDouble () != v2.asDouble ()? One : Zero;
                i++;
                break;
              case cdAnd:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = (iTos->size () > 0 && v2.size () > 0) ? One : Zero;
                else
                  *iTos = (iTos->asDouble () != 0.0
                           && v2.asDouble () != 0.0) ? One : Zero;
                i++;
                break;
              case cdOr:
                v2 = pop ();
                if (!isNumber (v2.constBuffer ())
                    || !isNumber (iTos->constBuffer ()))
                  *iTos = (iTos->size () > 0 || v2.size () > 0) ? One : Zero;
                else
                  *iTos = (iTos->asDouble () != 0.0
                           || v2.asDouble () != 0.0) ? One : Zero;
                i++;
                break;
              case cdJmp:
                i = loAddr + i->iAddr;
                break;
              case cdJF:
                {
                  v2 = pop ();
                  ZBoolean ok;
                  if (isNumber (v2.constBuffer ()))
                    ok = v2.asDouble () == 0.0;
                  else
                    ok = v2 == "";
                  i = ok ? loAddr + i->iAddr : i + 1;
                  break;
                }
              case cdJT:
                {
                  v2 = pop ();
                  ZBoolean ok;
                  if (isNumber (v2.constBuffer ()))
                    ok = v2.asDouble () != 0.0;
                  else
                    ok = v2 != "";
                  i = ok ? loAddr + i->iAddr : i + 1;
                  break;
                }
              case cdCall:
                exec ((Function *) i->iFunc);
                i++;
                head = zTrue;
                break;
              case cdRet:
                return;
              case cdTry:
                aFunc->iBlocks->iCatch = i->iAddr;
                aFunc->iBlocks->iTos = iTos;
                i++;
                break;
              case cdThrow:
                throw ZException (pop ());
              case cdThroV:
                {
                  v = aFunc->findVar (pop ());
                  int cnt (v->iSize);
                  ZException exc (v->value ());
                  for (int i = 1; i < cnt; i++)
                    exc.addAsLast (v->iVals[v->iIndex + i]);
                  throw exc;
                }
              case cdAllTV:
              case cdAllTC:
                {
                  if (!excpt)
                    throw ZException (msgInvCatch);
                  int cnt (excpt->count ());
                  Variable *var =
                    aFunc->iBlocks->addVar (decodeString (i->iText) + "[" +
                                            ZString (cnt ? cnt : 1) + "]", "",
                                            i->iCode == cdAllTC);
                  for (int c = 0; c < cnt; c++)
                    var->iVals[c] = (*excpt)[c];
                  delete excpt;
                  excpt = 0;
                  i++;
                  break;
                }
              case cdAllV:
                v2 = pop ();
                aFunc->iBlocks->addVar (v2, pop ());
                i++;
                break;
              case cdAllC:
                v2 = pop ();
                aFunc->iBlocks->addVar (v2, pop (), zTrue);
                i++;
                break;
              case cdAllVR:
                v2 = pop ();
                aFunc->iBlocks->addVar (v2, pop (), zFalse, zTrue);
                i++;
                break;
              case cdAllCR:
                v2 = pop ();
                aFunc->iBlocks->addVar (v2, pop (), zTrue, zTrue);
                i++;
                break;
              case cdNop:
                i++;
                break;
              default:
                throwExcept (msgIllgInstr);
              }                 // switch
          if (i < loAddr || i > hiAddr)
            throwExcept (msgNoReturn);
        }                       // try
        catch (const ZException & exc)
        {
          while (aFunc->iBlocks->iPrev && aFunc->iBlocks->iCatch < 0)
            aFunc->closeBlock ();
          if (aFunc->iBlocks->iPrev)
              {
                i = loAddr + aFunc->iBlocks->iCatch;
                iTos = aFunc->iBlocks->iTos;
                aFunc->closeBlock ();
                if (excpt)
                  delete excpt;
                excpt = new ZException (exc);
              }
          else
            throw;
        }                       // catch
      }                         // while
}                               // interpret
