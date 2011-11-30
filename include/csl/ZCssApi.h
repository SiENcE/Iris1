/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZCslApi.h
 * Application :  IBK Open Class Library
 * Purpose     :  CSL C API backward compatibility
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.07.07  First implementation                        P.Koch, IBK
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

#ifndef _ZCSSAPI_
#define _ZCSSAPI_

#include <csl/ZCslApi.h>

#define ZCssHandle          ZCslHandle
#define ZCssDisInclude      ZCslDisInclude
#define ZCssDisLoadScript   ZCslDisLoadScript
#define ZCssDisLoadLibrary  ZCslDisLoadLibrary
#define ZCssDisList         ZCslDisList
#define ZCssDisLogFile      ZCslDisLogFile
#define ZCssDisIf           ZCslDisIf
#define ZCssDisAll          ZCslDisAll
#define ZCssApiLink         ZCslApiLink
#define ZCssOpen            ZCslOpen
#define ZCssClose           ZCslClose
#define ZCssSet             ZCslSet
#define ZCssGet             ZCslGet
#define ZCssVarSizeof       ZCslVarSizeof
#define ZCssVarResize       ZCslVarResize
#define ZCssLoadScriptMem   ZCslLoadScriptMem
#define ZCssLoadScriptFile  ZCslLoadScriptFile
#define ZCssLoadLibrary     ZCslLoadLibrary
#define ZCssCall            ZCslCall
#define ZCssCallEx          ZCslCallEx
#define ZCssGetResult       ZCslGetResult
#define ZCssAddFunc         ZCslAddFunc
#define ZCssAddVar          ZCslAddVar
#define ZCssSetResult       ZCslSetResult
#define ZCssSetError        ZCslSetError
#define ZCssStartDateTime   ZCslStartDateTime
#define ZCssShowFunctions   ZCslShowFunctions
#define ZCssShowCallStack   ZCslShowCallStack
#define ZCssShowFullStack   ZCslShowFullStack
#define ZCssShowGlobals     ZCslShowGlobals
#define ZCssShowLibraries   ZCslShowLibraries
#define ZCssShow            ZCslShow
#define ZCssGetError        ZCslGetError
#define ZCssInitLib         ZCslInitLib
#define ZCssCleanupLib      ZCslCleanupLib
#define ZC_CSSLIBTYPE       ZC_CSLLIBTYPE

#endif /* _ZCSSAPI_ */
