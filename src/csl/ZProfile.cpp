/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZProfile.cpp
 * Application :  IBK Open Class Library
 * Purpose     :  Profile access
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.31  First implementation                        P.Koch, IBK
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

#include <ZTrace.hpp>
#include <ZProfile.hpp>
#include <ZExcept.hpp>

#ifdef ZC_OS2

#define INCL_WINERRORS
#define INCL_SHLERRORS
#define INCL_WINSHELLDATA
#include <os2.h>

// sys messages

static const char Colon[] = ": ";
static const char PrfCloseProfileName[] = "PrfCloseProfile";
static const char PrfOpenProfileName[] = "PrfOpenProfile";
static const char PrfQueryProfileName[] = "PrfQueryProfile";
static const char PrfQueryProfileDataName[] = "PrfQueryProfileData";
static const char PrfQueryProfileIntName[] = "PrfQueryProfileInt";
static const char PrfQueryProfileSizeName[] = "PrfQueryProfileSize";
static const char PrfQueryProfileStringName[] = "PrfQueryProfileString";
static const char PrfResetName[] = "PrfReset";
static const char PrfWriteProfileDataName[] = "PrfWriteProfileData";
static const char PrfWriteProfileStringName[] = "PrfWriteProfileString";
static const char WinInitializeName[] = "WinInitialize";

// other messages

static const char MissingPath[] = "missing path";
static const char MissingValueName[] = "missing value name";

#define throwSysErr(aFunction) ZTHROWEXC(ZString(aFunction)+Colon+getOs2Error())

ZString ZProfile::getOs2Error () const const
{
  ZFUNCTRACE_DEVELOP ("getOs2Error()");

  ZString msg;
  ERRORID err = WinGetLastError (iHab);
  switch (ERRORIDSEV (err))
      {
      case 0x0000:
        msg = "No Error - ";
        break;
      case 0x0004:
        msg = "Warning - ";
        break;
      case 0x000C:
        msg = "Severe - ";
        break;
      case 0x0010:
        msg = "Unrecoverable - ";
        break;
      case 0x0008:
      default:
        msg = "Error - ";
        break;
      }                         // switch
  switch (ERRORIDERROR (err))
      {
      case 0x1124:
        msg += "User or system initialization file cannot be closed.";
        break;
      case 0x1115:
        msg += "An invalid initialization-file handle was specified.";
        break;
      case 0x1301:
        msg += "Unable to open initialization file.";
        break;
      case 0x1309:
        msg += "An error occurred during memory management.";
        break;
      case 0x1303:
        msg += "A parameter to the function contained invalid data.";
        break;
      case 0x1304:
        msg +=
          "The application name, key name or program handle was not found.";
        break;
      case 0x110B:
        msg += "The supplied buffer was not large enough for the data.";
        break;
      case 0x130C:
        msg += "The profile string is not a valid zero-terminated string.";
        break;
      default:
        msg += ZString::hex (ERRORIDERROR (err));
      }                         // switch
  return msg;
}                               // getOs2Error

#endif // ZC_OS2

#ifdef ZC_WIN

#define NO_STRICT
#include <windows.h>

// #include "../Fluid/mmgr.h"

// string constants
static const char ValueNotFound[] = "value not found";
static const char UnsupportedDataType[] = "unsupported data type";
static const char Software[] = "Software";
static const char Colon[] = ": ";

static const char SRegCreateKeyEx[] = "RegCreateKeyEx";
static const char SRegOpenKeyEx[] = "RegOpenKeyEx";
static const char SRegSetValueEx[] = "RegSetValueEx";
static const char SRegQueryValueEx[] = "RegQueryValueEx";

typedef struct
{
  const char *iName;
  HKEY iHandle;
} SystemHandle;

static SystemHandle sysHandles[] = {
  {"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE}
  , {"HKEY_CURRENT_USER", HKEY_CURRENT_USER}
  , {"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT}
  , {"HKEY_USERS", HKEY_USERS}
  , {"HKEY_PERFORMANCE_DATA", HKEY_PERFORMANCE_DATA}
#if (WINVER >= 0x0400)
  , {"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG}
  , {"HKEY_DYN_DATA", HKEY_DYN_DATA}
#endif
};                              // sysHandles

#define throwSysErr(aFunction) ZTHROWEXC(ZString(aFunction)+Colon+getWinError())

static ZString getWinError ()
{
  ZFUNCTRACE_DEVELOP ("getWinError()");

  DWORD errcode = GetLastError ();
  LPTSTR lpMsgBuf;
  FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 errcode,
                 MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                 (LPTSTR) & lpMsgBuf, 0, NULL);
  ZString msg (lpMsgBuf);
  LocalFree (lpMsgBuf);
  return msg;
}                               // getWinError

static ZString splitPath (ZString & aPath)
{
  ZFUNCTRACE_DEVELOP ("splitPath(ZString& aPath)");
  long pos (aPath.indexOf (ZC_PATHSEPARATOR));
  if (!pos)
      {
        ZString ret (aPath);
        aPath = "";
        return ret;
      }                         // if
  ZString ret (aPath.subString (1, pos - 1));
  aPath = aPath.subString (pos + 1);
  return ret;
}                               // splitPath

static ZString stripPath (const ZString & aPath)
{
  ZFUNCTRACE_DEVELOP ("stripPath(const ZString& aPath)");
  ZString path (aPath);
  path.strip (ZC_PATHSEPARATOR);
  return path;
}                               // stripPath

static ZBoolean isSystemHandle (HKEY aHandle)
{
  ZFUNCTRACE_DEVELOP ("isSystemHandle(HKEY aHandle)");
  for (unsigned i = 0; i < sizeof (sysHandles) / sizeof (SystemHandle); i++)
    if (aHandle == sysHandles[i].iHandle)
      return zTrue;
  return zFalse;
}                               // isSystemHandle

static void closeHandle (HKEY aHandle)
{
  ZFUNCTRACE_DEVELOP ("closeHandle(HKEY aHandle)");
  if (!isSystemHandle (aHandle))
    RegCloseKey (aHandle);
}                               // closeHandle

static HKEY openHandle (HKEY aParentHandle, ZString aPath, ZBoolean aCreate)
{
  ZFUNCTRACE_DEVELOP
    ("openHandle(HKEY aParentHandle, ZString aPath, ZBoolean aCreate)");
  HKEY parentHandle (aParentHandle);
  HKEY handle;
  for (;;)
      {
        ZString token (splitPath (aPath));
        if (aCreate)
            {
              if (RegCreateKeyEx (parentHandle, token.constBuffer (), 0, "",
                                  REG_OPTION_NON_VOLATILE,
                                  (KEY_READ | KEY_WRITE), NULL, &handle,
                                  NULL) != ERROR_SUCCESS)
                  {
                    if (parentHandle != aParentHandle)
                      closeHandle (parentHandle);
                    throwSysErr (SRegCreateKeyEx);
                  }             // if
            }
        else
            {
              if (RegOpenKeyEx (parentHandle, token.constBuffer (), 0,
                                KEY_READ, &handle) != ERROR_SUCCESS)
                  {
                    if (parentHandle != aParentHandle)
                      closeHandle (parentHandle);
                    throwSysErr (SRegOpenKeyEx);
                  }             // if
            }                   // if
        if (parentHandle != aParentHandle)
          closeHandle (parentHandle);
        if (aPath.size () == 0)
          break;
        parentHandle = handle;
      }                         // for
  return handle;
}                               // openHandle

void ZProfile::openPath (ZBoolean aCreate)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::openPath(ZBoolean aCreate)");
  if (iPathHandle == 0 || (aCreate && iPathReadOnly))
      {
        closePath ();
        openRoot (aCreate);
        if (iPath.size ())
          iPathHandle = openHandle ((HKEY) iRootHandle, iPath, aCreate);
        else
          iPathHandle = iRootHandle;
        iPathReadOnly = !aCreate;
      }                         // if
}                               // openPath

void ZProfile::closePath ()
{
  ZFUNCTRACE_DEVELOP ("ZProfile::closePath()");
  if (iPathHandle)
      {
        if ((HKEY) iPathHandle != (HKEY) iRootHandle)
          closeHandle ((HKEY) iPathHandle);
        iPathHandle = 0;
      }                         // if
}                               // closePath

#endif // ZC_WIN

#ifdef ZC_WIN
void ZProfile::openRoot (ZBoolean aCreate)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::openRoot(ZBoolean aCreate)");
  if (iRootHandle == 0 || (aCreate && iRootReadOnly))
      {
        closeRoot ();
        HKEY parentKey (0);
        ZString path;
        while (!parentKey)
            {
              path = iRoot;
              ZString token (splitPath (path));
              for (unsigned i = 0;
                   i < sizeof (sysHandles) / sizeof (SystemHandle); i++)
                if (token == sysHandles[i].iName)
                    {
                      parentKey = sysHandles[i].iHandle;
                      break;
                    }           // if
              if (!parentKey)
                iRoot =
                  ZString (sysHandles[0].iName) + (char) ZC_PATHSEPARATOR +
                  iRoot;
            }                   // while
        if (path.length ())
          iRootHandle = openHandle (parentKey, path, aCreate);
        else
          iRootHandle = parentKey;
        iRootReadOnly = !aCreate;
      }                         // if
}                               // openRoot
#endif

#ifdef ZC_OS2
void ZProfile::openRoot ()
{
  ZFUNCTRACE_DEVELOP ("ZProfile::openRoot()");
  if (iRootHandle == 0)
      {
        iRootHandle = PrfOpenProfile (iHab, iRoot);
        if (!iRootHandle)
          throwSysErr (PrfOpenProfileName);
      }                         // if
}                               // openRoot
#endif

void ZProfile::closeRoot ()
{
  ZFUNCTRACE_DEVELOP ("ZProfile::closeRoot()");
#ifdef ZC_WIN
  if (iRootHandle)
      {
        closePath ();
        closeHandle ((HKEY) iRootHandle);
        iRootHandle = 0;
      }                         // if
#endif
#ifdef ZC_OS2
  if (iRootHandle != 0 &&
      iRootHandle != HINI_SYSTEMPROFILE && iRootHandle != HINI_USERPROFILE)
      {
        if (!PrfCloseProfile (iRootHandle))
          throwSysErr (PrfCloseProfileName);
        iRootHandle = 0;
      }                         // if
#endif
}                               // closeRoot

ZExport0 ZProfile::ZProfile (const ZString & aRoot, const ZString & aPath)
{
  ZFUNCTRACE_DEVELOP
    ("ZProfile::ZProfile(const ZString& aRoot, const ZString& aPath)");
#ifdef ZC_WIN
  iRootHandle = 0;
  iPathHandle = 0;
  iRoot = stripPath (aRoot);
  setPath (aPath);
#endif
#ifdef ZC_OS2
  iHab = WinInitialize (0);
  if (iHab == 0)
    throwSysErr (WinInitializeName);
  iRoot = aRoot;
  iPath = aPath;
#endif
}                               // ZProfile

ZExport0 ZProfile::ZProfile (const ZProfile & aProfile)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::ZProfile(const ZProfile& aProfile)");
#ifdef ZC_WIN
  iRootHandle = 0;
  iPathHandle = 0;
#endif
#ifdef ZC_OS2
  iHab = WinInitialize (0);
  if (iHab == 0)
    throwSysErr (WinInitializeName);
#endif
  *this = aProfile;
}                               // ZProfile

ZExport0 ZProfile::~ZProfile ()
{
  ZFUNCTRACE_DEVELOP ("ZProfile::~ZProfile()");
  closeRoot ();
}                               // ~ZProfile

ZExport (ZProfile) ZProfile::systemProfile (const ZString & aPath)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::systemProfile(const ZString& aPath)");
#ifdef ZC_WIN
  return ZProfile (ZString (sysHandles[0].iName) + (char) ZC_PATHSEPARATOR +
                   Software, aPath);
#endif
#ifdef ZC_OS2
  ZProfile prf ("SystemProfile", aPath);
  prf.iRootHandle = HINI_SYSTEMPROFILE;
  return prf;
#endif
}                               // systemProfile

ZExport (ZProfile) ZProfile::userProfile (const ZString & aPath)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::userProfile(const ZString& aPath)");
#ifdef ZC_WIN
  return ZProfile (ZString (sysHandles[1].iName) + (char) ZC_PATHSEPARATOR +
                   Software, aPath);
#endif
#ifdef ZC_OS2
  ZProfile prf ("UserProfile", aPath);
  prf.iRootHandle = HINI_USERPROFILE;
  return prf;
#endif
}                               // userProfile

ZExport (ZProfile &) ZProfile::operator= (const ZProfile & aProfile)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::operator=(const ZProfile& aProfile)");
  closeRoot ();
  iRoot = aProfile.iRoot;
  iPath = aProfile.iPath;
#ifdef ZC_OS2
  if (aProfile.iRootHandle == HINI_SYSTEMPROFILE ||
      aProfile.iRootHandle == HINI_USERPROFILE)
    iRootHandle = aProfile.iRootHandle;
#endif
  return *this;
}                               // operator=

ZExport (ZProfile &) ZProfile::setPath (const ZString & aPath)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::setPath(const ZString& aPath)");
#ifdef ZC_WIN
  ZString newPath (stripPath (aPath));
  if (iPath != newPath)
      {
        closePath ();
        iPath = newPath;
      }                         // if
#endif
#ifdef ZC_OS2
  iPath = aPath;
#endif
  return *this;
}                               // setPath

ZExport (ZProfile &) ZProfile::getKeys (ZStringlist & aList)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::getKeys(ZStringlist& aList)");
  aList.drop ();
#ifdef ZC_WIN
  openPath (zFalse);
  char buffer[MAX_PATH];
  long bufsize;
  for (unsigned long key = 0;; key++)
      {
        bufsize = sizeof (buffer);
        if (RegEnumKeyEx ((HKEY) iPathHandle, key, buffer,
                          (LPDWORD) & bufsize, NULL, NULL, NULL,
                          NULL) != ERROR_SUCCESS)
          break;
        aList.addAsLast (buffer);
      }                         // for
#endif
#ifdef ZC_OS2
  if (iPath.size ())
    return *this;               // os/2 has no keys at this level

  openRoot ();

  // get size of list (not including the terminating 0)
  ULONG size;
  if (!PrfQueryProfileSize (iRootHandle, 0, 0, &size))
    throwSysErr (PrfQueryProfileSizeName);

  if (size)
      {
        // allocate buffer
        ZString buf (0, size++);

        // read list of keys
        if (!PrfQueryProfileData (iRootHandle, 0, 0, buf, &size))
          throwSysErr (PrfQueryProfileDataName);

        // parse keys into stringlist
        char *s = buf;
        while (size)
            {
              ZString key;
              while (*s && size)
                  {
                    key += *s++;
                    size--;
                  }
              if (key.size ())
                aList.addAsLast (key);
              if (size)
                  {
                    s++;
                    size--;
                  }
            }                   // while
      }                         // if
#endif
  return *this;
}                               // getKeys

ZExport (ZProfile &) ZProfile::getValues (ZStringlist & aList)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::getValues(ZStringlist& aList)");
  aList.drop ();
#ifdef ZC_WIN
  openPath (zFalse);
  char buffer[MAX_PATH];
  long bufsize;
  for (unsigned long key = 0;; key++)
      {
        bufsize = sizeof (buffer);
        if (RegEnumValue ((HKEY) iPathHandle, key, buffer,
                          (LPDWORD) & bufsize, NULL, NULL, NULL,
                          NULL) != ERROR_SUCCESS)
          break;
        aList.addAsLast (buffer);
      }                         // for
#endif
#ifdef ZC_OS2
  if (!iPath.size ())
    return *this;               // os/2 has no values at this level

  openRoot ();

  // get size of list (not including the terminating 0)
  unsigned long size;
  if (!PrfQueryProfileSize (iRootHandle, iPath, 0, &size))
    throwSysErr (PrfQueryProfileSizeName);

  if (size)
      {
        // create buffer
        ZString buf (0, size++);

        // read list of values
        if (!PrfQueryProfileData (iRootHandle, iPath, 0, buf, &size))
          throwSysErr (PrfQueryProfileDataName);

        // parse values into stringlist
        char *s = buf;
        while (size)
            {
              ZString key;
              while (*s && size)
                  {
                    key += *s++;
                    size--;
                  }
              if (key.size ())
                aList.addAsLast (key);
              if (size)
                  {
                    s++;
                    size--;
                  }
            }                   // while
      }                         // if
#endif
  return *this;
}                               // getValues

ZExport (ZBoolean) ZProfile::keyExists (const ZString & aKeyName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::keyExists(const ZString& aKeyName)");
  ZStringlist list;
  ZBoolean found;
  if (aKeyName.size ())
      {
        getKeys (list);
        found = list.find (aKeyName) >= 0;
      }
  else
      {
#ifdef ZC_WIN
        found = zTrue;
        try
        {
          openPath (zFalse);
        }
        catch (const ZException & exc)
        {
          found = zFalse;
        }                       // catch
#endif
#ifdef ZC_OS2
        ZString path (iPath);
        try
        {
          setPath ("");
          getKeys (list);
          found = list.find (path) >= 0;
          setPath (path);
        }                       // try
        catch (const ZException & exc)
        {
          setPath (path);
          throw;
        }                       // catch
#endif
      }                         // if
  return found;
}                               // keyExists

ZExport (ZBoolean) ZProfile::valueExists (const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::valueExists(const ZString& aValueName)");
#ifdef ZC_WIN
  try
  {
    openPath (zFalse);
  }
  catch (const ZException & exc)
  {
    return zFalse;
  }                             // catch
#endif
#ifdef ZC_OS2
  openRoot ();
#endif
  ZStringlist list;
  getValues (list);
  return list.find (aValueName) >= 0;
}                               // valueExists

ZExport (int) ZProfile::valueType (const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::valueType(const ZString& aValueName)");
#ifdef ZC_WIN
  openPath (zFalse);

  DWORD dataType;
  if (!RegQueryValueEx ((HKEY) iPathHandle, aValueName, 0,
                        &dataType, NULL, NULL) == ERROR_SUCCESS)
    ZTHROWEXC (ValueNotFound);

  switch (dataType)
      {
      case 1:                  // String
      case 2:                  // ExpandString
        return String;
      case 3:                  // Binary
      case 7:                  // MultiString
        return Binary;
      case 4:                  // DWord
      case 5:                  // DWordBigEndian
        return Integer;
      default:;
      }                         // switch
  return Other;
#endif
#ifdef ZC_OS2
  ZString val (value (aValueName));
  if (ZString (val.asLong ()) == val)
    return Integer;
  if (val.isPrintable ())
    return String;
  return Binary;
#endif
}                               // valueType

ZExport (ZString) ZProfile::value (const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::value(const ZString& aValueName)");
#ifdef ZC_WIN
  openPath (zFalse);

  DWORD dataType, size;
  if (!RegQueryValueEx ((HKEY) iPathHandle, aValueName, 0,
                        &dataType, NULL, &size) == ERROR_SUCCESS)
    ZTHROWEXC (ValueNotFound);

  if (size)
      {
        ZString str (0, size);
        RegQueryValueEx ((HKEY) iPathHandle, aValueName, 0,
                         NULL, (LPBYTE) (char *) str, (LPDWORD) & size);

        switch (dataType)
            {
            case 1:            // String
            case 2:            // ExpandString
            case 7:            // MultiString
              if (str.size () > 0 && str[str.size ()] == 0)
                return str.subString (1, str.size ());
            default:;
            }                   // switch
      }                         // if
#endif
#ifdef ZC_OS2
  if (!iPath.size ())
    ZTHROWEXC (MissingPath);
  if (!aValueName.size ())
    ZTHROWEXC (MissingValueName);
  openRoot ();

  // query size of data
  unsigned long size;
  if (!PrfQueryProfileSize (iRootHandle, iPath, aValueName, &size))
    throwSysErr (PrfQueryProfileSizeName);

  if (size > 0)
      {
        ZString buf (0, size++);

        // try to get as string first
        PrfQueryProfileString (iRootHandle, iPath, aValueName, "", buf, size);
        ERRORID err = WinGetLastError (iHab);
        if (err == 0)
            {
              if (buf[buf.size ()] == 0
                  && buf.subString (1, buf.size () - 1).isPrintable ())
                return buf.subString (1, buf.size () - 1);
              return buf;
            }                   // if

        if (ERRORIDERROR (err) != PMERR_INVALID_ASCIIZ)
          throwSysErr (PrfQueryProfileStringName);

        // try to get binary data
        if (!PrfQueryProfileData (iRootHandle, iPath, aValueName, buf, &size))
          throwSysErr (PrfQueryProfileDataName);
        return buf;
      }                         // if
#endif
  return ZString ();
}                               // value

ZExport (long) ZProfile::longValue (const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::longValue(const ZString& aValueName)");
#ifdef ZC_WIN
  openPath (zFalse);

  DWORD data, dataType, size (4);
  if (!RegQueryValueEx ((HKEY) iPathHandle, aValueName, 0,
                        &dataType, (LPBYTE) & data, &size) == ERROR_SUCCESS)
    ZTHROWEXC (ValueNotFound);

  if (dataType != 4)
    ZTHROWEXC (UnsupportedDataType);
  return data;
#endif
#ifdef ZC_OS2
  return value (aValueName).asLong ();
#endif
}                               // longValue

ZExport (ZProfile &) ZProfile::setValue (const ZString & aValue,
                                         const ZString & aValueName,
                                         int aType)
{
  ZFUNCTRACE_DEVELOP
    ("ZProfile::setValue(const ZString& aValue, const ZString& aValueName, int aType)");

  // find out type if auto
  if (aType == Auto)
      {
        ZString v (aValue);
        if (ZString (v.strip ().asLong ()) == v)
          aType = Integer;
        else
            {
              if (aValue.isPrintable ())
                aType = String;
              else
                aType = Binary;
            }                   // if
      }                         // if

#ifdef ZC_WIN
  long dataType;
  switch (aType)
      {
      case String:
        dataType = 1;           // String
        break;
      case Integer:
        return setValue (aValue.asLong (), aValueName);
      default:
        dataType = 3;           // Binary
      }                         // switch

  openPath (zTrue);

  if (valueExists (aValueName))
    deleteValue (aValueName);

  if (!RegSetValueEx ((HKEY) iPathHandle, aValueName, 0,
                      dataType, (LPBYTE) (const char *) aValue,
                      aValue.size ()) == ERROR_SUCCESS)
    throwSysErr (SRegSetValueEx);
#endif
#ifdef ZC_OS2
  if (!iPath.size ())
    ZTHROWEXC (MissingPath);
  if (!aValueName.size ())
    ZTHROWEXC (MissingValueName);
  openRoot ();

  switch (aType)
      {
      case String:
        if (!PrfWriteProfileString (iRootHandle, iPath, aValueName,
                                    ZString::exp (aValue)))
          throwSysErr (PrfWriteProfileStringName);
        break;
      case Integer:
        {
          ZString v (aValue);
          if (!PrfWriteProfileString (iRootHandle, iPath, aValueName,
                                      ZString (v.strip ().asLong ())))
            throwSysErr (PrfWriteProfileStringName);
          break;
        }                       // Integer
      default:
        {
          ZString val (aValue);
          if (!PrfWriteProfileData (iRootHandle, iPath, aValueName,
                                    (char *) val, val.size ()))
            throwSysErr (PrfWriteProfileDataName);
        }                       // default
      }                         // switch
#endif
  return *this;
}                               // setValue

ZExport (ZProfile &) ZProfile::setValue (long aValue,
                                         const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP
    ("ZProfile::setValue(const ZString& aValue, const ZString& aValueName, int aType)");
#ifdef ZC_WIN
  openPath (zTrue);

  if (valueExists (aValueName))
    deleteValue (aValueName);

  if (!RegSetValueEx ((HKEY) iPathHandle, aValueName, 0,
                      4, (LPBYTE) & aValue, 4) == ERROR_SUCCESS)
    throwSysErr (SRegSetValueEx);
#endif
#ifdef ZC_OS2
  setValue (ZString (aValue));
#endif
  return *this;
}                               // setValue

ZExport (ZProfile &) ZProfile::deleteValue (const ZString & aValueName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::deleteValue(const ZString& aValueName)");
#ifdef ZC_WIN
  openPath (zFalse);
  RegDeleteValue ((HKEY) iPathHandle, aValueName);
#endif
#ifdef ZC_OS2
  if (!iPath.size ())
    ZTHROWEXC (MissingPath);
  if (!aValueName.size ())
    ZTHROWEXC (MissingValueName);
  openRoot ();
  if (!PrfWriteProfileString (iRootHandle, iPath, aValueName, 0))
    throwSysErr (PrfWriteProfileStringName);
#endif
  return *this;
}                               // deleteValue

ZExport (ZProfile &) ZProfile::deleteKey (const ZString & aKeyName)
{
  ZFUNCTRACE_DEVELOP ("ZProfile::deleteKey(const ZString& aKeyName)");
#ifdef ZC_WIN
  // delete all values and subkeys first
  ZString oldPath (iPath);
  try
  {
    setPath (iPath + (char) ZC_PATHSEPARATOR + aKeyName);
    openPath (zFalse);

    ZStringlist list;
    int i;

    // delete all values
    setValue (ZString ());      // just clear out default value
    getValues (list);
    for (i = 0; i < list.count (); i++)
      if (list[i].size ())
          {
            deleteValue (list[i]);
          }                     // if

    // delete all subkeys
    getKeys (list);
    for (i = 0; i < list.count (); i++)
      deleteKey (list[i]);
  }                             // try
  catch (const ZException & exc)
  {
    setPath (oldPath);
    throw;
  }                             // catch

  setPath (oldPath);
  openPath (zFalse);
  RegDeleteKey ((HKEY) iPathHandle, aKeyName);
#endif
#ifdef ZC_OS2
  if (iPath.size () && aKeyName.size ())
    return *this;               // no keys at this level

  openRoot ();

  if (!iPath.size () && !aKeyName.size ())
      {
        // delete all applications:

        // will only do this if not the system- or user profile!
        if (iRootHandle == HINI_SYSTEMPROFILE ||
            iRootHandle == HINI_USERPROFILE)
          return *this;

        ZStringlist apps;
        getKeys (apps);
        for (long i = 0; i < apps.count (); i++)
          if (!PrfWriteProfileString (iRootHandle, apps[i], 0, 0))
            throwSysErr (PrfWriteProfileStringName);
      }
  else
      {
        if (!PrfWriteProfileString (iRootHandle,
                                    iPath.size ()? iPath : aKeyName, 0, 0))
          throwSysErr (PrfWriteProfileStringName);
      }                         // if
#endif
  return *this;
}                               // deleteKey
