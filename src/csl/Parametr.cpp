/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Parametr.cpp
 * Application :  CSL kernel
 * Purpose     :  Parameter
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

// #include "../Fluid/mmgr.h"

ZCsl::Parameter::Parameter (const ZString & aName,
                            ZBoolean aIsConst,
                            ZBoolean aIsRef,
                            ZBoolean aIsOptional,
                            Parameter * aNext):iName (aName),
iIsConst (aIsConst), iIsRef (aIsRef), iIsOptional (aIsOptional), iNext (aNext)
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Parameter::Parameter(...)");
}                               // Parameter

ZCsl::Parameter::~Parameter ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::Parameter::~Parameter()");
  if (iNext)
    delete iNext;
}                               // Parameter
