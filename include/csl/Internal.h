/* Copyright (c) 1998-2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Internal.h
 * Application :  CSL kernel
 * Purpose     :  Internal idents and functions
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * Feb 2000    Added switch/case/default                   P.Koch, IBK
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

#define STACKCHUNK  100        // stack chunk allocation size

// constants in Utility.cpp

extern const char Zero[];
extern const char One[];

extern const char msgInvResizeRef[];
extern const char msgFileNotFound[];
extern const char msgInternalErr[];
extern const char msgIndexNonConst[];
extern const char msgInitNonConst[];
extern const char msgInitExtern[];
extern const char msgNewLineInLit[];
extern const char msgInvCatch[];
extern const char msgInvParCnt[];
extern const char msgInvArgCnt[];
extern const char msgInvPar[];
extern const char msgInvIdent[];
extern const char msgInvChar[];
extern const char msgInvSymbol[];
extern const char msgInvBreak[];
extern const char msgInvContin[];
extern const char msgInvOpStrings[];
extern const char msgInvalidIndex[];
extern const char msgVarNotFound[];
extern const char msgVarAlrdyDef[];
extern const char msgFuncAlrdyDef[];
extern const char msgFuncNotFound[];
extern const char msgBeginNotFound[];
extern const char msgEndNotFound[];
extern const char msgSemiNotFound[];
extern const char msgColonNotFound[];
extern const char msgSyntax[];
extern const char msgIdentExpct[];
extern const char msgDirExpct[];
extern const char msgUnknownDir[];
extern const char msgIndexBound[];
extern const char msgMissLvalue[];
extern const char msgMissLparen[];
extern const char msgMissRparen[];
extern const char msgMissLbrack[];
extern const char msgMissRbrack[];
extern const char msgMissWhile[];
extern const char msgMissFactor[];
extern const char msgFileOpenErr[];
extern const char msgEarlyEof[];
extern const char msgUnexpSymbol[];
extern const char msgUnexpIdent[];
extern const char msgTermAbnorm[];
extern const char msgSimpleExpct[];
extern const char msgUncaught[];
extern const char msgCatchNotFound[];
extern const char msgWriteConst[];
extern const char msgVarSymExpct[];
extern const char msgInitExpct[];
extern const char msgDimsMismatch[];
extern const char msgConstDenied[];
extern const char msgFuncNesting[];
extern const char msgStackUdr[];
extern const char msgDivZero[];
extern const char msgIllgInstr[];
extern const char msgNoReturn[];
extern const char msgFuncDiffers[];
extern const char msgFuncNotimp[];
extern const char msgVarDiffers[];
extern const char msgUnallocExtern[];
extern const char msgArrParamRef[];
extern const char msgErrLoadingLib[];
extern const char msgInitialNotFnd[];
extern const char msgCleanupNotFnd[];
extern const char msgIllGlobAddVar[];
extern const char msgCloseIfUnexp[];
extern const char msgEndifNotFound[];

// functions in Utility.cpp
ZBoolean inStr(char ch, char* s);
ZBoolean validIdBeginChar(char ch);
ZBoolean validIdChar(char ch);
ZBoolean isNumber(const char* s);
ZString pureVarName(const ZString& aVarName);
