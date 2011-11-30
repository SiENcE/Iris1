/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZBlock.cpp
 * Application :  CSL kernel
 * Purpose     :  Block
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.21  First implementation                        P.Koch, IBK
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

#ifndef ZC_BUILDING_ZCSL
#define ZC_BUILDING_ZCSL
#endif

#include <ZCsl.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"

ZCsl::Block::Block (ZCsl * aParent, Block * aPrev,
                    const ZString & aName):iVars (0), iFuncs (0),
iPrev (aPrev), iParent (aParent), iCatch (-1), iName (aName)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Block::Block(ZCsl* aParent, Block* aPrev)");
}                               // Block

ZCsl::Block::~Block ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Block::~Block()");
  if (iVars)
    delete iVars;
  if (iFuncs)
    delete iFuncs;
  if (iPrev)
    delete iPrev;
}                               // ~Block

ZCsl::Variable * ZCsl::Block::findVar (const ZString & aVarName,
                                       ZBoolean aFail)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Block::findVar(const ZString& aVarName, ZBoolean aFail)");
  Variable *v = iVars;
  while (v)
      {
        if (v->match (aVarName))
          return v;
        v = v->iPrev;
      }                         // while
  if (aFail)
    iParent->throwExcept (msgVarNotFound, aVarName);
  return v;
}                               // findVar

ZCsl::Variable * ZCsl::Block::addVar (const ZString & aVarName,
                                      const ZString & aInitValue,
                                      ZBoolean aIsConst,
                                      ZBoolean aIsRef,
                                      ZBoolean aIsKnown, ZBoolean aIsExtern)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Block::addVar(...)");
  Variable *v = findVar (aVarName, zFalse);
  if (v)
      {
        if (!v->iIsExtern)
          iParent->throwExcept (msgVarAlrdyDef, aVarName);
        if (v->iIsConst != aIsConst ||
            (v->iRef && !aIsRef) ||
            (!v->iRef && aIsRef) ||
            aVarName.occurrencesOf ('[') != v->dims ())
          iParent->throwExcept (msgVarDiffers, aVarName);
        Variable var (iParent, aVarName, aInitValue,
                      aIsConst, aIsRef, aIsKnown, aIsExtern, 0);
        if (v->iVals)
          delete[]v->iVals;
        v->iVals = var.iVals;
        var.iVals = 0;
        v->iIsKnown = var.iIsKnown;
        v->iIsExtern = var.iIsExtern;
        for (int i = 0; i < MAXDIMS; i++)
          v->iDims[i] = var.iDims[i];
        return v;
      }                         // if
  iVars = new Variable (iParent, aVarName, aInitValue,
                        aIsConst, aIsRef, aIsKnown, aIsExtern, iVars);
  return iVars;
}                               // addVar
