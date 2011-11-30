/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Instruct.cpp
 * Application :  CSL kernel
 * Purpose     :  Instruction
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.22  First implementation                        P.Koch, IBK
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

#include <ZCsl.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"

ZCsl::Instruction::Instruction (Opcode aCode, const ZString & aText, Instruction * aNext):
iCode (aCode), iText (aText), iAddr (0), iNext (aNext)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Instruction::Instruction(Opcode, const ZString&, Instruction*)");
}                               // Instruction

ZCsl::Instruction::Instruction (Opcode aCode, Instruction * aAddr, Instruction * aNext):
iCode (aCode), iAddr (aAddr), iNext (aNext)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Instruction::Instruction(Opcode, Instruction*, Instruction*)");
}                               // Instruction

ZCsl::Instruction::~Instruction ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::~Instruction()");
  if (iNext)
    delete iNext;
}                               // ~Instruction

ZBoolean ZCsl::Instruction::cond () const
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::cond() const");
  if (iText == "0" || iText == "")
    return zFalse;
  if (iText == "1")
    return zTrue;
  if (isNumber (iText))
    return iText.asDouble () != 0.0;
  else
    return zTrue;
}                               // cond

void ZCsl::Instruction::set (ZBoolean aCond)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::set(ZBoolean aCond)");
  iText = aCond ? "1" : "0";
}                               // set

ZCsl::Instruction * ZCsl::Instruction::next ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::next()");
  Instruction *i = this;
  while (i->iNext)
    i = i->iNext;
  return i;
}                               // next

ZCsl::Instruction * ZCsl::Instruction::last ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::last()");
  Instruction *p = 0;
  Instruction *i = this;
  while (i->iNext)
      {
        p = i;
        i = i->iNext;
      }                         // while
  if (!p)
    ZTHROWEXC (ZString (msgInternalErr) + "(Instruction::last)");
  return p;
}                               // last

ZCsl::Instruction * ZCsl::Instruction::add (Opcode aCode,
                                            const ZString & aText)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Instruction::add(Opcode aCode, const ZString& aText)");
  Instruction *n = next ();
  n->iCode = aCode;
  n->iText = aText;
  n->iNext = new Instruction;
  return n;
}                               // add

ZCsl::Instruction * ZCsl::Instruction::add (Opcode aCode, Function * aFunc)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Instruction::add(Opcode aCode, Function* aFunc)");
  Instruction *n = next ();
  n->iCode = aCode;
  n->iFunc = aFunc;
  n->iNext = new Instruction;
  return n;
}                               // add

ZCsl::Instruction * ZCsl::Instruction::add (Opcode aCode, Instruction * aAddr)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Instruction::add(Opcode aCode, Instruction* aAddr)");
  Instruction *n = next ();
  n->iCode = aCode;
  n->iAddr = aAddr;
  n->iNext = new Instruction;
  return n;
}                               // add

ZCsl::Instruction * ZCsl::Instruction::add (Instruction * aInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::add(Instruction* aInst)");
  Instruction *n = next ();
  if (aInst->iNext)
      {
        n->iCode = aInst->iCode;
        n->iAddr = aInst->iAddr;
        n->iText = aInst->iText;
        n->iFunc = aInst->iFunc;
        n->iNext = aInst->iNext;
        Instruction *i = n;
        while (n->iNext)
            {
              if (n->iAddr == aInst)
                n->iAddr = i;
              n = n->iNext;
            }                   // while
        aInst->iCode = cdNop;
        aInst->iText = "";
        aInst->iFunc = 0;
        aInst->iAddr = 0;
        aInst->iNext = 0;
      }                         // if
  return n;
}                               // add

ZCsl::Instruction * ZCsl::Instruction::copy (Instruction * aInst)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Instruction::copy(Instruction* aInst)");
  Instruction *i = aInst;
  Instruction *n = next ();
  Instruction *n2 = n;
  ZBoolean reloc (zFalse);
  while (i->iNext)
      {
        n->iCode = i->iCode;
        n->iAddr = i->iAddr;
        if (i->iAddr)
          reloc = zTrue;
        n->iText = i->iText;
        n->iFunc = i->iFunc;
        n->iNext = new Instruction;
        i = i->iNext;
        n = n->iNext;
      }                         // if
  if (reloc)
      {
        n = n2;
        while (n->iNext)
            {
              if (n->iAddr)
                  {
                    Instruction *ii = aInst;
                    Instruction *nn = n2;
                    while (ii)
                        {
                          if (ii == n->iAddr)
                              {
                                n->iAddr = nn;
                                break;
                              } // if
                          ii = ii->iNext;
                          nn = nn->iNext;
                        }       // while
                  }             // if
              n = n->iNext;
            }                   // while
      }                         // if
  return n;
}                               // copy
