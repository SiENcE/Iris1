/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  Input.cpp
 * Application :  CSL kernel
 * Purpose     :  Input stream
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

#include <stdio.h>
#include <string.h>
#include <ZCsl.hpp>
#include <ZFile.hpp>
#include "Internal.h"

// #include "../Fluid/mmgr.h"

using namespace std;

ZCsl::InputStream::InputStream (ZCsl * aParent, const ZString & aFileName, istream * aStr, InputStream * aPrev):
iStr (aStr),
iStrAlloc (zFalse),
iFileName (aFileName),
iLineNo (0),
iPrev (aPrev), iParent (aParent), iIfStack (0), iSymLine (0), iSymPos (0)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::InputStream::InputStream(ZCsl*, const ZString&, istream*, InputStream*)");
  iLinePtr = iLine;
  getChar ();
  if (*iLinePtr == '@' || iLine.subString (1, 2) == "#!")
      {
        iLine = "";
        iLinePtr = iLine;
      }                         // if
}                               // constructor

ZCsl::InputStream::InputStream (ZCsl * aParent,
                                const ZString & aFileName,
                                InputStream * aPrev):iStrAlloc (zTrue),
iFileName (aFileName),
iLineNo (0),
iPrev (aPrev), iParent (aParent), iIfStack (0), iSymLine (0), iSymPos (0)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::InputStream::InputStream(ZCsl*, const ZString&, InputStream*)");
  ZString iName (ZFile::addExtension (aFileName, ".csl"));
  iStr = new ifstream (iName.constBuffer ());
#ifdef ZC_CSS_COMPATIBLE
  if (!iStr->good ())
      {
        delete iStr;
        iName = ZFile::addExtension (aFileName, ".css");
        iStr = new ifstream (iName.constBuffer ());
      }                         // if
#endif
  if (!iStr->good ())
      {
        delete iStr;
        iStr = 0;
        iParent->throwExcept (msgFileOpenErr, iName);
      }                         // if
  iLinePtr = iLine;
  getChar ();
  if (*iLinePtr == '@')
      {
        iLine = "";
        iLinePtr = iLine;
      }                         // if
}                               // constructor

ZCsl::InputStream::~InputStream ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::InputStream::~InputStream()");
  if (iPrev)
    delete iPrev;
  if (iIfStack)
    delete iIfStack;
  if (iStrAlloc && iStr)
    delete iStr;
}                               // destructor

void ZCsl::InputStream::getChar ()
{
  ZFUNCTRACE_DEVELOP ("ZCsl::InputStream::getChar()");
  if (!*iLinePtr)
      {
        if (iStr->eof ())
            {
              iChar = EOF;
              return;
            }                   // if
        iLine = ZString::lineFrom (*iStr);
        iLinePtr = iLine;
        iLineNo++;
        iChar = '\n';
        return;
      }                         // if
  iChar = *iLinePtr++;
  if (iChar < ' ')
    switch (iChar)
        {
        case 26:               // Ctrl-Z
          iChar = EOF;
          break;
        case '\r':
        case '\t':
        case '\n':
          iChar = ' ';
          break;
        default:
          iParent->throwExcept (msgInvChar,
                                "hex " +
                                ZString::hex ((unsigned char) iChar));
        }                       // switch
}                               // getChar

ZCsl::Symbol ZCsl::InputStream::getSym (ZString & aIdent, double &aValue)
{
  ZFUNCTRACE_DEVELOP
    ("ZCsl::InputStream::getSym(ZString& aIdent, double& aValue)");
  char *hexDigits = "0123456789abcdefABCDEF";

  typedef struct
  {
    char *iIdent;
    int iSym;
  } SymIdent;

  static const SymIdent symIdents[] = {
    {"break", (int) symBreak},
    {"case", (int) symCase},
    {"catch", (int) symCatch},
    {"const", (int) symConst},
    {"continue", (int) symContin},
    {"default", (int) symDefault},
    {"do", (int) symDo},
    {"else", (int) symElse},
    {"exists", (int) symExist},
    {"extern", (int) symExtern},
    {"for", (int) symFor},
    {"if", (int) symIf},
    {"resize", (int) symResize},
    {"return", (int) symRet},
    {"sizeof", (int) symSizeof},
    {"static", (int) symStatic},
    {"switch", (int) symSwitch},
    {"throw", (int) symThrow},
    {"trace", (int) symTrace},
    {"try", (int) symTry},
    {"var", (int) symVar},
    {"while", (int) symWhile}
  };

  while (iChar == ' ' || iChar == '\n')
    getChar ();
  if (iChar != EOF)
      {
        iSymLine = iLineNo;
        iSymPos = iLinePtr - (char *) iLine;
      }                         // if
  switch (iChar)
      {
      case EOF:
        iSym = symEof;
        break;
      case '*':
        getChar ();
        if (iChar == '=')
            {
              getChar ();
              iSym = symMulBec;
            }
        else
          iSym = symTimes;
        break;
      case '{':
        getChar ();
        iSym = symBegin;
        break;
      case '}':
        getChar ();
        iSym = symEnd;
        break;
      case ';':
        getChar ();
        iSym = symSemi;
        break;
      case ':':
        getChar ();
        iSym = symColon;
        break;
      case ',':
        getChar ();
        iSym = symComma;
        break;
      case '(':
        getChar ();
        iSym = symLparen;
        break;
      case ')':
        getChar ();
        iSym = symRparen;
        break;
      case '[':
        getChar ();
        iSym = symLbrack;
        break;
      case ']':
        getChar ();
        iSym = symRbrack;
        break;
      case '%':
        getChar ();
        iSym = symMod;
        break;
      case '\\':
        getChar ();
        iSym = symDiv;
        break;
      case '#':
        getChar ();
        iSym = symDir;
        break;
      case '+':
        getChar ();
        if (iChar == '+')
            {
              getChar ();
              iSym = symIncr;
            }
        else if (iChar == '=')
            {
              getChar ();
              iSym = symAddBec;
            }
        else
          iSym = symPlus;
        break;
      case '-':
        getChar ();
        if (iChar == '-')
            {
              getChar ();
              iSym = symDecr;
            }
        else if (iChar == '=')
            {
              getChar ();
              iSym = symSubBec;
            }
        else
          iSym = symMinus;
        break;
      case '!':
        getChar ();
        if (iChar == '=')
            {
              getChar ();
              iSym = symNeq;
            }
        else
          iSym = symNot;
        break;
      case '<':
        getChar ();
        if (iChar == '=')
            {
              getChar ();
              iSym = symLeq;
            }
        else
          iSym = symLss;
        break;
      case '>':
        getChar ();
        if (iChar == '=')
            {
              getChar ();
              iSym = symGeq;
            }
        else
          iSym = symGtr;
        break;
      case '=':
        getChar ();
        if (iChar == '=')
            {
              getChar ();
              iSym = symEql;
            }
        else
          iSym = symBecomes;
        break;
      case '&':
        getChar ();
        if (iChar == '&')
            {
              getChar ();
              iSym = symAnd;
            }
        else
          iSym = symRef;
        break;
      case '|':
        getChar ();
        if (iChar == '|')
            {
              getChar ();
              iSym = symOr;
            }
        else if (iChar == '=')
            {
              getChar ();
              iSym = symCatBec;
            }
        else
          iSym = symCat;
        break;
      case '/':
        getChar ();
        if (iChar == '*')
            {
              // comment
              getChar ();
              do
                  {
                    while (iChar != EOF && iChar != '*')
                      getChar ();
                    if (iChar == EOF)
                      iParent->throwExcept (msgEarlyEof);
                    getChar ();
                  }
              while (iChar != '/');
              getChar ();
              return getSym (aIdent, aValue);
            }
        else if (iChar == '/')
            {
              // linecomment
              getChar ();
              while (iChar != '\n' && iChar != EOF)
                getChar ();
              return getSym (aIdent, aValue);
            }
        else if (iChar == '=')
            {
              getChar ();
              iSym = symDivBec;
            }
        else
          iSym = symSlash;
        break;
      case '\'':
      case '"':
        {                       // literal
          char hyphen (iChar);
          getChar ();
          iIdent = "";
          while (iChar != hyphen)
              {
                switch (iChar)
                    {
                    case EOF:
                      iParent->throwExcept (msgEarlyEof);
                    case '\n':
                      iParent->throwExcept (msgNewLineInLit);
                    case '\\':
                      {
                        getChar ();
                        switch (iChar)
                            {
                            case EOF:
                              iParent->throwExcept (msgEarlyEof);
                            case '\n':
                              iParent->throwExcept (msgNewLineInLit);
                            case 'e':
                              iIdent += '\33';
                              getChar ();
                              break;
                            case 't':
                              iIdent += '\t';
                              getChar ();
                              break;
                            case 'r':
                              iIdent += '\r';
                              getChar ();
                              break;
                            case 'n':
                              iIdent += '\n';
                              getChar ();
                              break;
                            case 'f':
                              iIdent += '\f';
                              getChar ();
                              break;
                            case 'x':
                              {
                                getChar ();
                                int x = 0, n = 0;
                                while (inStr (iChar, hexDigits) && n < 2)
                                    {
                                      int digit;
                                      if ('0' <= iChar && iChar <= '9')
                                        digit = iChar - '0';
                                      else if ('a' <= iChar && iChar <= 'f')
                                        digit = iChar - 'a' + 10;
                                      else
                                        digit = iChar - 'A' + 10;
                                      x = x * 16 + digit;
                                      getChar ();
                                      n++;
                                    }     // while
                                iIdent += char (x);
                                break;
                              }
                            default:
                              if (iChar >= '0' && iChar <= '7')
                                  {
                                    int x = 0, n = 0;
                                    while (iChar >= '0' && iChar <= '7'
                                           && n < 3)
                                        {
                                          x = x * 8 + iChar - '0';
                                          getChar ();
                                          n++;
                                        } // while
                                    iIdent += char (x);
                                  }
                              else
                                  {
                                    iIdent += char (iChar);
                                    getChar ();
                                  } // if
                            }   // switch
                        break;
                      }         // case '\\'
                    default:
                      iIdent += char (iChar);
                      getChar ();
                    }           // switch
              }                 // while
          getChar ();
          iSym = symLit;
          aIdent = iIdent;
          break;
        }                       // case literal
      default:
        if ('0' <= iChar && iChar <= '9')
            {
              iValue = 0.0;
              if (iChar == '0')
                  {
                    // either octal or hex
                    getChar ();
                    if (iChar == '.')
                      goto thoughItsDecimal;
                    if (iChar == 'x' || iChar == 'X')
                        {
                          // hex
                          getChar ();
                          while (inStr (iChar, hexDigits))
                              {
                                int digit;
                                if ('0' <= iChar && iChar <= '9')
                                  digit = iChar - '0';
                                else if ('a' <= iChar && iChar <= 'f')
                                  digit = iChar - 'a' + 10;
                                else
                                  digit = iChar - 'A' + 10;
                                iValue = iValue * 16.0 + digit;
                                getChar ();
                              } // while
                        }
                    else
                        {
                          // octal
                          while ('0' <= iChar && iChar <= '7')
                              {
                                iValue = iValue * 8.0 + iChar - '0';
                                getChar ();
                              } // while
                        }       // if
                  }
              else
                  {
                    // decimal
                  thoughItsDecimal:
                    while ('0' <= iChar && iChar <= '9')
                        {
                          iValue = iValue * 10.0 + iChar - '0';
                          getChar ();
                        }       // while
                    if (iChar == '.')
                        {
                          getChar ();
                          double fact (0.1);
                          while ('0' <= iChar && iChar <= '9')
                              {
                                iValue += fact * (iChar - '0');
                                fact /= 10.0;
                                getChar ();
                              } // while
                          if (iChar == 'e' || iChar == 'E')
                              {
                                getChar ();
                                ZString xx ("1.0e");
                                if (iChar == '-' || iChar == '+')
                                    {
                                      xx += char (iChar);
                                      getChar ();
                                    }     // if
                                while ('0' <= iChar && iChar <= '9')
                                    {
                                      xx += char (iChar);
                                      getChar ();
                                    }     // while
                                iValue *= xx.asDouble ();
                              } // if
                        }       // if
                  }             // if
              iSym = symValue;
              aValue = iValue;
            }
        else if (validIdBeginChar (iChar))
            {
              // read ident
              iIdent = "";
              while (validIdChar (iChar))
                  {
                    iIdent += char (iChar);
                    getChar ();
                  }

              // binary search for sym ident
              const char *i = iIdent.constBuffer ();
              int l = 0;
              int r = sizeof (symIdents) / sizeof (SymIdent) - 1;
              iSym = symIdent;
              for (;;)
                  {
                    int m = (l + r) / 2;
                    int c = strcmp (i, symIdents[m].iIdent);
                    if (c == 0)
                        {
                          iSym = (Symbol) symIdents[m].iSym;
                          break;
                        }       // if
                    if (c < 0)
                      r = m - 1;
                    else
                      l = m + 1;
                    if (l > r)
                        {
                          aIdent = iIdent;
                          break;
                        }       // if
                  }             // for
            }
        else
          iParent->throwExcept (msgInvChar,
                                "hex " +
                                ZString::hex ((unsigned char) iChar));
      }                         // switch
  return iSym;
}                               // getSym
