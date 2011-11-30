/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Variable.cpp
 * Application :  CSL kernel
 * Purpose     :  Variables
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

static int _dims (const char *nm)
{
  ZFUNCTRACE_DEVELOP ("_dims(const char* nm)");
  int i (0);
  while (*nm)
    if (*nm++ == '[')
      i++;
  return i;
}                               // dims

ZCsl::Variable::Variable (ZCsl * aParent, const ZString & aVarName, const ZString & aInitValue, ZBoolean aIsConst, ZBoolean aIsRef, ZBoolean aIsKnown, ZBoolean aIsExtern, Variable * aPrev):
iIsConst (aIsConst),
iIsKnown (aIsKnown && aIsConst),
iIsExtern (aIsExtern),
iIsGlobal (zFalse), iPrev (aPrev), iRef (0), iParent (aParent)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::Variable(...)");
  iName = pureVarName (aVarName);
  const char *a = (const char *) aVarName + iName.length ();
  if (aIsRef)
      {
        iRef = iParent->iCalls->iPrev->findVar (aInitValue);
        // set up dimensions
        int refDims = iRef->dims () - _dims (aInitValue);
        int myDims = _dims (a);
        if (refDims != myDims)
          iParent->throwExcept (msgDimsMismatch, aVarName);
        int dim;
        for (dim = 0; dim < MAXDIMS; dim++)
          iDims[dim] = 0;
        for (dim = 0; dim < myDims; dim++)
          iDims[dim] = iRef->iDims[iRef->dims () - myDims + dim];
        // get array offset
        iRef->match (aInitValue);
        iVals = &iRef->iVals[iRef->iIndex];
        // set const & name
        iIsConst = iRef->iIsConst || aIsConst;
      }
  else
      {
        int dim;
        for (dim = 0; dim < MAXDIMS; dim++)
          iDims[dim] = 0;
        dim = 0;
        while (*a == '[' && dim < MAXDIMS)
            {
              a++;
              while ('0' <= *a && *a <= '9')
                  {
                    iDims[dim] = 10 * iDims[dim] + *a - '0';
                    a++;
                  }
              if (*a++ != ']' || !iDims[dim++])
                iParent->throwExcept (msgInvalidIndex, aVarName);
            }                   // while
        if (*a)
          iParent->throwExcept (msgInvalidIndex, aVarName);
        long sz = size ();
        iVals = new ZString[sz];
        for (int i = 0; i < sz; i++)
          iVals[i] = aInitValue;
      }                         // if
}                               // Variable

ZCsl::Variable::~Variable ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::~Variable()");
  if (!iRef && iVals)
    delete[]iVals;
  if (iPrev)
    delete iPrev;
}                               // destructor

ZBoolean ZCsl::Variable::match (const ZString & aName)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::match(const ZString& aName)");
  if (pureVarName (aName) != iName)
    return zFalse;
  const char *a = (const char *) aName + iName.length ();
  iIndex = 0;
  int dim (0);
  int dimcnt (dims ());
  while (*a == '[' && dim < MAXDIMS)
      {
        a++;
        long index (0);
        while ('0' <= *a && *a <= '9')
            {
              index = 10 * index + *a - '0';
              a++;
            }
        if (*a++ != ']' || !iDims[dim])
          iParent->throwExcept (msgInvalidIndex, aName);
        if (index >= iDims[dim] && iParent->iState != isCompiling)
          iParent->throwExcept (msgInvalidIndex, aName);
        for (int k = dim + 1; k < dimcnt; k++)
          index *= iDims[k];
        iIndex += index;
        dim++;
      }                         // while
  if (*a)
    iParent->throwExcept (msgInvalidIndex, aName);
  iSize = 1;
  while (dim < dimcnt)
    iSize *= iDims[dim++];
  return zTrue;
}                               // match

ZString ZCsl::Variable::value () const 
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::value() const");
  if (iIsExtern)
    iParent->throwExcept (msgUnallocExtern, iName);
  if (iIndex < 0 || iIndex >= size ())
    iParent->throwExcept (msgInvalidIndex, iName);
  return iVals[iIndex];
}                               // value

long ZCsl::Variable::size () const 
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::size() const");
  long sz (1);
  for (int i = 0; i < MAXDIMS; i++)
      {
        if (!iDims[i])
          break;
        sz *= iDims[i];
      }                         // for
  return sz;
}                               // size

int ZCsl::Variable::dims () const 
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::dims() const");
  int i;
  for (i = 0; i < MAXDIMS; i++)
    if (!iDims[i])
      break;
  return i;
}                               // dims

ZCsl::Variable & ZCsl::Variable::set (const ZString & aValue,
                                      ZBoolean aInitConst)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::Variable::set(const ZString& aValue, ZBoolean aInitConst)");
  if (iIsExtern)
    iParent->throwExcept (msgUnallocExtern, iName);
  if (iIsConst && !aInitConst)
    iParent->throwExcept (msgWriteConst, iName);
  if (iIndex < 0 || iIndex >= size ())
    iParent->throwExcept (msgInvalidIndex, iName);
  iVals[iIndex] = aValue;
  return *this;
}                               // set

ZCsl::Variable & ZCsl::Variable::resize (const ZString & aVarName)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Variable::resize(const ZString& aVarName)");
  if (iIsExtern)
    iParent->throwExcept (msgUnallocExtern, iName);
  Variable *r = iRef;
  while (r)
      {
        for (int i = 0; i < MAXDIMS; i++)
          if (iDims[i] != r->iDims[i])
            iParent->throwExcept (msgInvResizeRef, iName);
        r = r->iRef;
      }                         // while
  const char *a = aVarName;
  // skip to indexes
  while (*a && *a != '[')
    a++;
  long newDims[MAXDIMS];
  int dim;
  for (dim = 0; dim < MAXDIMS; dim++)
    newDims[dim] = 0;
  dim = 0;
  while (*a == '[' && dim < MAXDIMS)
      {
        a++;
        while ('0' <= *a && *a <= '9')
            {
              newDims[dim] = 10 * newDims[dim] + *a - '0';
              a++;
            }
        if (*a++ != ']' || !newDims[dim] || !iDims[dim])
          iParent->throwExcept (msgInvalidIndex, aVarName);
        dim++;
      }                         // while
  if (*a || (dim < MAXDIMS && iDims[dim]))
    iParent->throwExcept (msgInvalidIndex, aVarName);
  long sz (1);
  for (dim = 0; dim < MAXDIMS; dim++)
      {
        if (!newDims[dim])
          break;
        sz *= newDims[dim];
      }                         // for
  long szOld (size ());
  if (sz > szOld)
      {
        ZString *newVals = new ZString[sz];
        ZString *n = newVals;
        ZString *o = iVals;
        while (szOld--)
          *n++ = *o++;
        delete[]iVals;
        iVals = newVals;
        r = iRef;
        while (r)
            {
              r->iVals = newVals;
              r = r->iRef;
            }                   // while
      }                         // if
  for (dim = 0; dim < MAXDIMS; dim++)
      {
        iDims[dim] = newDims[dim];
        r = iRef;
        while (r)
            {
              r->iDims[dim] = newDims[dim];
              r = r->iRef;
            }                   // while
      }                         // for
  return *this;
}                               // resize
