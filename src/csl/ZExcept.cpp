/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZExcept.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  ZException is the general purpose exception class and is
 *                derived from ZStringlist.
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.20  First implementation                        P.Koch, IBK
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

#ifndef ZC_BUILDING_ZBASE
#define ZC_BUILDING_ZBASE
#endif

#include <ZExcept.hpp>

// #include "../Fluid/mmgr.h"

ZEXCLASSIMPLEMENT (ZException, ZStringlist)
