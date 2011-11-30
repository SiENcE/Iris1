/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZCslApi.h
 * Application :  IBK Open Class Library
 * Purpose     :  CSL C API
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.25  First implementation                        P.Koch, IBK
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

#ifndef _ZCSLAPI_
#define _ZCSLAPI_

#include <csl/ZBase.h>
#include <csl/ZCsl.h>

#ifndef ZC_BUILDING_ZCSLAPI
  #ifdef ZC_IBM
    #pragma library("ZCslApiI.lib")
  #endif
  #ifdef ZC_MICROSOFT
    #pragma comment(lib,"ZCslApiM.lib")
  #endif
  #ifdef ZC_BORLAND
    #pragma comment(lib,"ZCslApiB.lib")
  #endif
  #define ZCslApiLink(type) ZImportAPI(type)
#else
  #define ZCslApiLink(type) ZExportAPI(type)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* ZCslHandle;                /* CSL Handle */

/* ZCslOpen initialization flags */

#define ZCslDisInclude     0001           /* disable #include */
#define ZCslDisLoadScript  0002           /* disable #loadScript */
#define ZCslDisLoadLibrary 0004           /* disable #loadLibrary */
#define ZCslDisList        0010           /* disable #list */
#define ZCslDisLogFile     0020           /* disable #logFile */
#define ZCslDisIf          0040           /* disable #if / #else / #endif */
#define ZCslDisError       0100          /* disable #error */
#define ZCslDisMessage     0200          /* disable #message */
#define ZCslDisAll         0377          /* disable all */

ZCslApiLink(long) ZCslOpen(              /* Create CSL context & alloc mem */
   ZCslHandle *aHandle,                     /* address of handle var */
   long aFlags                              /* initialization flags */
);

ZCslApiLink(long) ZCslClose(             /* Destroy CSL Context & release mem */
   ZCslHandle aHandle                       /* CSL handle */
);

ZCslApiLink(long) ZCslSet(               /* set var or const value */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aVarName,                    /* variable name */
   const char *aValue,                      /* new value (NULL = "") */
   long aSize                               /* value size, -1 = ASCIZ */
);

ZCslApiLink(long) ZCslGet(               /* get var or const value */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aVarName,                    /* variable name */
   char *aBuffer,                           /* buffer for value (NULL = query size) */
   long *aSize                              /* buffer size */
);

ZCslApiLink(long) ZCslVarSizeof(         /* get size of variable */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aVarName,                    /* name of variable */
   long *aSize                              /* buffer for size */
);

ZCslApiLink(long) ZCslVarResize(         /* resize variable */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aVarName                     /* name and new layout of variable */
);

ZCslApiLink(long) ZCslLoadScriptMem(     /* load script from memory */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aFileName,                   /* file/module name for script */
   const char *aStr                         /* css source code */
);

ZCslApiLink(long) ZCslLoadScriptFile(    /* load script from file */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aFileName                    /* file name to load script */
);

ZCslApiLink(long) ZCslLoadLibrary(       /* load a CSL dll */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aDllName                     /* dll filename */
);

ZCslApiLink(long) ZCslCall(              /* call function (string args only) */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aFileName,                   /* file/module caller belongs to */
   const char *aFuncName,                   /* function name */
   long aArgCount,                          /* # of arguments following */
   char *aArgs[]                            /* argument list */
);

ZCslApiLink(long) ZCslCallEx(            /* call function (string or binary args) */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aFileName,                   /* file/module caller belongs to */
   const char *aFuncName,                   /* function name */
   long aArgCount,                          /* # of arguments following */
   char *aArgs[],                           /* argument list */
   long aSize[]                             /* argument length's (-1 = asciz) */
);

ZCslApiLink(long) ZCslGetResult(         /* get function result */
   ZCslHandle aHandle,                      /* CSL handle */
   char *aBuffer,                           /* buffer for value (NULL = query size) */
   long *aSize                              /* buffer size */
);

ZCslApiLink(long) ZCslAddFunc(           /* add a function */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aFileName,                   /* file/module name for function */
   const char *aFuncHeader,                 /* function header */
   void (ZFuncptrAPI aFunc)(ZCslHandle aHandle) /* function address */
);

ZCslApiLink(long) ZCslAddVar(            /* add a local var/const */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aVarName,                    /* name and layout of var/const */
   const char *aInitVal,                    /* initial value */
   long aIsConst                            /* 0=var, 1=const */
);

ZCslApiLink(long) ZCslSetResult(         /* set return result */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aBuffer,                     /* buffer for result */
   long aSize                               /* buffer size, -1 = ASCIZ */
);

ZCslApiLink(long) ZCslSetError(          /* set/add error text */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aBuffer,                     /* buffer for result */
   long aSize                               /* buffer size, -1 = ASCIZ */
);

ZCslApiLink(long) ZCslStartDateTime(     /* get start date/time */
   ZCslHandle aHandle,                      /* CSL handle */
   long *aYear,                             /* year (NULL = not required) */
   long *aMonth,                            /* month 1...12 (NULL = not required) */
   long *aDay,                              /* day 1...31 (NULL = not required) */
   long *aHour,                             /* hour 0...23 (NULL = not required) */
   long *aMinute,                           /* minute 0...59 (NULL = not required) */
   long *aSecond                            /* second 0...59 (NULL = not required) */
);

/* Trace mode constants */

#define ZCslTraceNone     0000           /* trace nothing */
#define ZCslTraceCode     0001           /* pcode */
#define ZCslTraceFuncs    0002           /* functions */
#define ZCslTraceBlks     0004           /* blocks */
#define ZCslTraceMsgs     0010           /* messages */
#define ZCslTraceInfo     0016           /* functions, blocks and msgs */
#define ZCslTraceAll      0017           /* everything */

ZCslApiLink(long) ZCslTraceMode(         /* query trace mode */
   ZCslHandle aHandle,                      /* CSL handle */
   long *aMode                              /* returned trace mode */
);

ZCslApiLink(long) ZCslSetTraceMode(      /* enable/disable tracing */
   ZCslHandle aHandle,                      /* CSL handle */
   long aMode                               /* new trace mode */
);

ZCslApiLink(long) ZCslTrace(             /* write trace message */
   ZCslHandle aHandle,                      /* CSL handle */
   const char *aMessage                     /* message to write */
);

/* ZCslShow mode constants */

#define ZCslShowFunctions    0           /* show all functions */
#define ZCslShowCallStack    1           /* show function calls */
#define ZCslShowFullStack    2           /* show function calls & locals */
#define ZCslShowGlobals      3           /* show global identifiers */
#define ZCslShowLibraries    4           /* show loaded functions */

ZCslApiLink(long) ZCslShow(              /* show internal informations */
   ZCslHandle aHandle,                      /* CSL handle */
   long aMode,                              /* show mode */
   long aDepth                              /* how much, -1 = all */
);

/* ZCsl functions returns the # of error texts (0 = no error) */

ZCslApiLink(long) ZCslGetError(          /* get error text */
   ZCslHandle aHandle,                      /* CSL handle */
   long aIndex,                             /* index of text (0 based) */
   char *aBuffer,                           /* buffer for error text (NULL = query size) */
   long *aSize                              /* buffer size information */
);

/* Library Macros */

#define ZCslInitLib(var) ZExportAPI(void) ZCslInitLibAPI(ZCslHandle var)
#define ZCslCleanupLib(var) ZExportAPI(void) ZCslCleanupLibAPI(ZCslHandle var)
#define ZC_CSLLIBTYPE "API"

#ifdef __cplusplus
}
#endif

#endif /* _ZCSLAPI_ */
