/* Copyright (c) 2001 IBK-Landquart-Switzerland. All rights reserved.
 *
 * Module      :  ZCsl.hpp
 * Application :  CSL kernel
 * Purpose     :  Native class interface
 *
 * Date        Description                                 Who
 * --------------------------------------------------------------------------
 * 2001.05.21  First implementation                        P.Koch, IBK
 * 2001.06.28  Change functions and/or to andOp/orOp       P.Koch, IBK
 *             because gcc v3.0 will complain otherwise.
 * 2001.07.07  Renaming from css to csl                    P.Koch, IBK
 * 2001.07.26  Speed up function calls                     P.Koch, IBK
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

#ifndef _ZCSL_
#define _ZCSL_

#define ZC_CSS_COMPATIBLE  // be compatible with CSS

#include <fstream>
#include <ZDate.hpp>
#include <ZDynLink.hpp>
#include <ZExcept.hpp>
#include <ZCsl.h>

class ZCsl : public ZBase
{
   public:
      enum Flags {
         disInclude     = 0001,          // disable #include
         disLoadScript  = 0002,          // disable #loadScript
         disLoadLibrary = 0004,          // disable #loadLibrary
         disList        = 0010,          // disable #list
         disLogFile     = 0020,          // disable #logFile
         disIf          = 0040,          // disable #if ... #else ... #endif
         disError       = 0100,          // disable #error
         disMessage     = 0200,          // disable #message
         disAll         = 0377           // disable all
      };

      enum TraceMode {
         traceNone      = 0000,          // nothing
         traceCode      = 0001,          // pcode
         traceFuncs     = 0002,          // functions
         traceBlks      = 0004,          // blocks
         traceMsgs      = 0010,          // messages
         traceInfo      = 0016,          // functions, blocks and msgs
         traceAll       = 0017           // everything
      };

      enum ShowMode {
         showFunctions,                  // show all functions
         showCallStack,                  // show function calls
         showFullStack,                  // show function calls & locals
         showGlobals,                    // show global identifiers
         showLibraries                   // show loaded functions
      };

      ZCslLink0
         ZCsl(                           // constructor
            int aFlags = 0);                // flags

      ZCslLink0
         ~ZCsl();                        // destructor

      ZCslLink(ZCsl&)
         set(                            // set var or const value
            const ZString& aVarName,        // variable name
            const ZString& aValue = "");    // new value

      ZCslLink(ZString)
         get(                            // get var or const value
            const ZString& aVarName);       // variable name

      ZCslLink(long)
         varSizeof(                      // get size of variable
            const ZString& aVarName);       // name of variable

      ZCslLink(ZCsl&)
         varResize(                      // resize variable
            const ZString& aVarName);       // name and new layout of variable

      ZCslLink(ZBoolean)
         varExists(                      // check var/const existence
            const ZString& aVarName);       // identifier

      ZCslLink(ZCsl&)
         loadScript(                     // load script from any istream
            const ZString& aFileName,       // file/module name for script
            std::istream* aStr);                 // input stream

      ZCslLink(ZCsl&)
         loadScript(                     // load script from file
            const ZString& aFileName);      // file name to load

      ZCslLink(ZCsl&)
         loadLibrary(                    // load a dll
            const ZString& aDllName);       // dll filename

      ZCslLink(ZString)
         call(                           // call function
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            long aArgCount = 0,             // # of arguments
            char** aParam = 0,              // parameter list
            long* aSize = 0);               // parameter length's (-1 = asciz)

      ZCslLink(ZString)
         call(                           // call function with string args only
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            int aArgCount,                  // # of arguments
            ...);                           // arguments as char*

      ZCslLink(ZString)
         callEx(                         // call function with string or binary args
            const ZString& aFileName,       // file/module caller belongs to
            const ZString& aFuncName,       // function name
            int aArgCount = 0,              // # of arguments
            ...);                           // arguments as pairs of char*/long

      ZCslLink(ZCsl&)
         addFunc(                        // add a function
            const ZString& aFileName,       // file/module name for function
            const ZString& aFuncHeader,     // function header
            ZString (*aFunc)(ZCsl* aCsl));  // function address

      ZCslLink(ZCsl&)
         addVar(                         // add a local var/const
            const ZString& aVarName,        // name and layout
            const ZString& aInitValue="",   // initial value
            ZBoolean aIsConst = zFalse);    // false=var, true=const

      ZCslLink(ZDateTime)
         startDateTime() const;          // get start date & time

      ZCslLink(TraceMode)
         traceMode() const;              // query current trace mode

      ZCslLink(ZCsl&)
         setTraceMode(                   // set trace mode
            TraceMode aMode);               // new trace mode

      ZCslLink(ZCsl&)
         trace(                          // provide runtime trace info
            const ZString& aMessage);       // message to trace

      ZCslLink(ZCsl&)
         show(                           // show internal informations
            ShowMode aMode,                 // show mode
            long aDepth = 99999999);        // how much

      // utility vars for C API
      ZException& errs() { return iErrs; }
      ZString& result() { return iResult; }

   private:
      class Library;
      friend class ZCsl::Library;

      class Instruction;
      friend class ZCsl::Instruction;

      class PackedInstruction;
      friend class ZCsl::PackedInstruction;

      class Parameter;
      friend class ZCsl::Parameter;

      class Function;
      friend class ZCsl::Function;

      class Variable;
      friend class ZCsl::Variable;

      class Block;
      friend class ZCsl::Block;

      class InputStream;
      friend class ZCsl::InputStream;

      enum Limits { MAXDIMS = 16 };      // max # of array-dimensions supported

      enum State { isNormal, isCompiling, isInterpreting };

      enum Opcode {
         cdOpen,     // open a block
         cdClose,    // close a block
         cdSize,     // get size of var
         cdExist,    // check identifier existence
         cdRSize,    // resize variable (initializer on tos)
         cdNeg,      // negate tos
         cdNot,      // boolean invert tos
         cdLoad,     // load variable to tos
         cdStore,    // store tos into variable
         cdStorC,    // store tos into variable ignoring const (initializer)
         cdPush,     // push iTxt onto tos
         cdPop,      // pop and discard tos
         cdDupl,     // push tos again
         cdMul,      // tos = tos-1 * tos
         cdDiv,      // tos = tos-1 / tos
         cdDivI,     // tos = tos-1 \ tos
         cdMod,      // tos = tos-1 % tos
         cdAdd,      // tos = tos-1 + tos
         cdSub,      // tos = tos-1 - tos
         cdCat,      // tos = tos+1 # tos
         cdMulV,     // var *= tos
         cdDivV,     // var /= tos
         cdDivIV,    // var \= tos
         cdModV,     // var %= tos
         cdAddV,     // var += tos
         cdSubV,     // var -= tos
         cdCatV,     // var #= tos
         cdIncV,     // var++
         cdDecV,     // var--
         cdLss,      // tos = tos-1 <  tos
         cdLeq,      // tos = tos-1 <= tos
         cdGtr,      // tos = tos-1 >  tos
         cdGeq,      // tos = tos-1 >= tos
         cdEql,      // tos = tos-1 == tos
         cdNeq,      // tos = tos-1 != tos
         cdAnd,      // tos = tos-1 && tos
         cdOr,       // tos = tos-1 || tos
         cdJmp,      // jump unconditionally
         cdJF,       // jump if tos false, consumes tos
         cdJT,       // jump if tos true, consumes tos
         cdCall,     // call function
         cdRet,      // return
         cdTry,      // open block with defined catch address
         cdThroV,    // throw exception, variable name is on tos
         cdThrow,    // throw exception, value is on tos
         cdAllTV,    // allocate thrown val as var
         cdAllTC,    // allocate thrown val as const
         cdAllV,     // alloc variable (initializer on tos)
         cdAllC,     // alloc constant (initializer on tos)
         cdAllVR,    // alloc variable reference (initializer on tos)
         cdAllCR,    // alloc constant reference (initializer on tos)
         cdTrace,    // trace info
         cdNop       // no operation
      };

      enum Symbol {
         symBegin,   symEnd,     symLparen,  symRparen,
         symLbrack,  symRbrack,  symRet,     symNot,
         symVar,     symConst,   symRef,     symDo,
         symIf,      symElse,    symWhile,   symFor,
         symTimes,   symSlash,   symPlus,    symMinus,
         symMulBec,  symDivBec,  symAddBec,  symSubBec,
         symIncr,    symDecr,    symComma,   symBecomes,
         symAnd,     symOr,      symEql,     symNeq,
         symGtr,     symGeq,     symLss,     symLeq,
         symIdent,   symLit,     symValue,   symSemi,
         symThrow,   symTry,     symCatch,   symEof,
         symStatic,  symExtern,  symSizeof,  symResize,
         symBreak,   symContin,  symDiv,     symMod,
         symDivIBec, symModBec,  symCat,     symCatBec,
         symDir,     symSwitch,  symCase,    symColon,
         symExist,   symDefault, symTrace
      };

      class Library
      {
         public:
            Library(                     // constructor
               ZCsl* aParent,               // parent
               const ZString& aDllName,     // library name
               Library* aPrev);             // previous library

            ~Library();                  // destructor

            ZDynamicLinkLibrary* iDll;   // dll object
            Library* iPrev;              // previous library in list
            ZBoolean iNative;            // library is native

         private:
            void initApi(                // init by api entry
               ZBoolean& aFound);           // procedure entry was found

            void initNative(             // init by native c++ entry
               ZBoolean& aFound);           // procedure entry was found

            void cleanupApi(             // cleanup by api entry
               ZBoolean& aFound);           // procedure entry was found

            void cleanupNative(          // cleanup by native c++ entry
               ZBoolean& aFound);           // procedure entry was found

            ZCsl*    iParent;            // parent
      };

      class Instruction
      {
         public:
            Instruction(                 // constructor
               Opcode aCode,                // opcode
               const ZString& aText,        // text parameter
               Instruction* aNext = 0);     // next instruction address

            Instruction(                 // constructor
               Opcode aCode = cdNop,        // opcode
               Instruction* aAddr = 0,      // junction address
               Instruction* aNext = 0);     // next instruction address

            ~Instruction();              // destructor

            ZBoolean cond() const;       // access iText as boolean

            void set(                    // set iText as boolean text
               ZBoolean aCond);             // condition

            Instruction* next();         // return next instruction

            Instruction* last();         // return last instruction

            Instruction* add(            // add a single instruction
               Opcode aCode,                // opcode
               const ZString& aText);       // text parameter

            Instruction* add(            // add a call instruction
               Opcode aCode,                // opcode
               Function* aFunc);            // function address

            Instruction* add(            // add a single instruction
               Opcode aCode,                // opcode
               Instruction* aAddr = 0);     // address

            Instruction* add(            // add an instruction sequence
               Instruction* aInstr);        // instruction sequence

            Instruction* copy(           // add (non destructive)
               Instruction* aInstr);        // instruction sequence

            Opcode iCode;                // instruction code
            ZString iText;               // text parameter
            Function* iFunc;             // function address
            Instruction* iAddr;          // address parameter
            Instruction* iNext;          // next instruction
      }; // Instruction

      #pragma pack(2)
      class PackedInstruction
      {
         public:
            Opcode iCode;                // opcode
            union {
               char* iText;              // text parameter
               void* iFunc;              // function address
               long iAddr;               // address parameter
            };
      }; // PackedInstruction
      #pragma pack()

      class Parameter
      {
         public:
            Parameter(                   // constructor
               const ZString& aName,        // parameter name
               ZBoolean aIsConst = zFalse,  // is a constant
               ZBoolean aIsRef = zFalse,    // is a reference
               ZBoolean aIsOptional = zFalse,// is optional
               Parameter* aNext = 0);       // next parameter

            ~Parameter();                // destructor

            ZString        iName;        // parameter Name
            ZBoolean       iIsConst;     // is a constant
            ZBoolean       iIsRef;       // is a reference
            ZBoolean       iIsOptional;  // is optional
            Parameter*     iNext;        // next parameter
      }; // Parameter

      class Variable
      {
         public:
            Variable(                    // constructor
               ZCsl* aParent,               // parent
               const ZString& aVarName,     // var name
               const ZString& aInitValue=ZString(), // initial value
               ZBoolean aIsConst = zFalse,  // is a const
               ZBoolean aIsRef = zFalse,    // is a reference
               ZBoolean aIsKnown = zFalse,  // is a known value
               ZBoolean aIsExtern = zFalse, // is external
               Variable* aPrev = 0);        // previous var in list

           ~Variable();                  // destructor

           ZBoolean match(               // match var name & index
              const ZString& aVarName);     // var name

           ZString value() const;        // get value

           long size() const;            // size of variable

           int dims() const;             // return # of dims

           Variable& set(                // set value
              const ZString& aValue = "",   // new value
              ZBoolean aInitConst = zFalse);// const initialization

           Variable& resize(             // resize a variable
              const ZString& aVarName);     // new variable declaration

           ZString      iName;           // variable name
           long         iIndex;          // index
           long         iSize;           // calculated rest size
           long         iDims[MAXDIMS];  // dimensions
           ZBoolean     iIsConst;        // is a constant
           ZBoolean     iIsKnown;        // is a known value
           ZBoolean     iIsExtern;       // is an external
           ZBoolean     iIsGlobal;       // is a global
           Variable*    iPrev;           // previous variable in list
           ZString*     iVals;           // values
           Variable*    iRef;            // referenced variable
           ZCsl*        iParent;         // parent
      }; // Variable

      class Block {
         public:
            Block(                       // constructor
               ZCsl* aParent,               // parent
               Block* aPrev,                // previos block
               const ZString& aName = ZString()); // block name for trace

            ~Block();                    // destructor

            Variable* findVar(           // find a variable
               const ZString& aVarName,     // variable name
               ZBoolean fail = zTrue);      // fail if not found

            Variable* addVar(            // add a variable to list
               const ZString& aVarName,     // variable name
               const ZString& aInitValue="",// initial value
               ZBoolean aIsConst = zFalse,  // is a constant
               ZBoolean aIsRef = zFalse,    // is a reference
               ZBoolean aIsKnown = zFalse,  // is a known value
               ZBoolean aIsExtern = zFalse);// is external

            Variable*     iVars;           // variables list
            Function*     iFuncs;          // functions list
            Block*        iPrev;           // previous block in stack
            ZCsl*         iParent;         // parent
            long          iCatch;          // catch address
            ZString*      iTos;            // stack ptr for catch address
            ZString       iName;           // block name for trace
      }; // Block

      class Function
      {
         public:
            Function(                    // constructor for interpreted function
               ZCsl* aParent,               // parent
               const ZString& aName,        // function name
               Function* aPrev);            // previous function

            Function(                    // constructor for native c++ function
               ZCsl* aParent,               // parent
               const ZString& aName,        // function name
               ZString (*aFunc)(ZCsl* aCsl),// function address
               Function* aPrev);            // previous function

            Function(                    // copy constructor
               Function* aFunction,         // function template
               Function* aPrev);            // previous

            ~Function();                 // destructor

            void openBlock(              // open a block
               const ZString& aName = ZString()); // block name for trace

            void closeBlock();           // close a block

            int paramCnt();    // get parameter count

            Parameter* param(            // get parameter
               int iIndex);                 // index

            Function& addPar(            // add parsed parameter
               const ZString& aName,        // parameter name
               ZBoolean aIsConst,           // has const attibute
               ZBoolean aIsRef,             // is a reference
               ZBoolean aIsOptional);       // is optional

            Variable* findVar(           // find a variable
               const ZString& aVarName,     // variable name
               ZBoolean aFail = zTrue);     // fail if not found

            ZString        iName;        // function name
            Function*      iPrev;        // previous function in list
            ZString        (*iFunc)(ZCsl* aCsl); // ptr to native c++ function
            Instruction*   iInst;        // function code
            PackedInstruction* iPcode;   // packed function code
            long           iPsize;       // packed code size
            char*          iText;        // text parameter buffer
            long           iTsize;       // size of param. buffer
            Block*         iBlocks;      // block list
            Block*         iStat;        // static variables
            Parameter*     iParams;      // parameter list begin
            ZCsl*          iParent;      // parent
      }; // Function

      class IfRecord {
         public:
            IfRecord(ZBoolean aElse, IfRecord* aPrev);
            ~IfRecord();

            ZBoolean    iElse;           // in else path
            IfRecord    *iPrev;          // previous if record
      }; // IfRecord

      class InputStream {
         public:
            InputStream(                 // open input stream
               ZCsl* aParent,               // parent
               const ZString& aFileName,    // file name
               std::istream* aStr,               // input stream
               InputStream* aPrev);         // previous file

            InputStream(                 // open input file
               ZCsl* aParent,               // parent
               const ZString& aFileName,    // file name
               InputStream* aPrev);         // previous file

            ~InputStream();              // destructor

            Symbol getSym(               // get Symbol from input
               ZString& aIdent,             // identifier address
               double& aValue);             // value address

            std::istream*       iStr;         // script input stream
            ZBoolean       iStrAlloc;    // stream was allocated
            ZString        iLine;        // last line read from iStr
            ZString        iFileName;    // file name
            long           iLineNo;      // line number in file
            char*          iLinePtr;     // read pointer in line
            InputStream*   iPrev;        // previous input stream
            ZCsl*          iParent;      // parent
            IfRecord*      iIfStack;     // #if stack
            int            iChar;        // last read character
            long           iSymLine;     // line # of last symbol
            long           iSymPos;      // position of last symbol in line
            Symbol         iSym;         // last read symbol
            ZString        iIdent;       // last read identifier
            double         iValue;       // last read value

         private:
            void getChar();              // get char from input
      }; // InputStream

      ZString*       iStack;             // work stack
      ZString*       iStackLimit;        // last available stackelem
      long           iStackSize;         // stack size
      ZString*       iTos;               // work stack pointer
      Library*       iDlls;              // list of loaded dll's
      Block*         iBlock;             // global public block
      Block*         iStats;             // all static blocks
      Function*      iCalls;             // call stack
      ZDateTime      iStartDateTime;     // start date & time
      TraceMode      iTraceMode;         // trace mode
      int            iTraceLevel;        // trace level
      int            iFlags;             // compiler directive flags
      State          iState;             // current state

      // compiler statics
      Block*         iStat;              // current static block
      ZBoolean       iList;              // list pcode
      ZBoolean       iInDirective;       // processing directive
      InputStream*   iInput;             // input stream list
      Symbol         iSym;               // last read symbol
      ZString        iIdent;             // last read identifier
      double         iValue;             // last read value
      Function*      iCurrFunc;          // current function

      // C API vars
      ZException     iErrs;              // error list
      ZString        iResult;            // result mem

      // functions in Utility.cpp
      void log(const char* aMessage = "");
      void trace(char aFlag, const char* aMessage = "");
      Variable* findVar(const ZString& name, ZBoolean fail = zTrue);
      Function* findFunc(const ZString& name, ZBoolean fail = zTrue);
      Function* addFunc(Function* aFunc, ZBoolean aStatic = zFalse);
      void checkIdent(char* s);
      void exec(Function* aFunc);
      void push(const ZString& aArg);
      ZString pop();
      void throwExcept(const ZString& aMsg, const ZString& aInfo = ZString());
      void showIdents(Block* b, char* lead, long& depth);
      ZString funcHeader(Function* f);
      void showFuncs(Function* f, ZBoolean v, long& depth);
      void showLibs(long& depth);

      // functions in Compile.cpp
      void getSym0();
      ZBoolean skipCode(ZBoolean elseOk);
      void closeIfDir();
      void directive();
      void getSym();
      void getParamList(
         Function* aFunc,
         ZBoolean aComp = zFalse);
      int index(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst,
         ZString& ident);
      void callFunc(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst,
         ZString& ident);
      void arrayInit(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst,
         const ZString& aName,
         long *dims);
      void factor(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void term(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void addops(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void lessgtr(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void equal(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void andOp(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void orOp(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst);
      void becomes(
         Function* aFunc,
         Instruction* hInst,
         Instruction* bInst,
         Instruction* tInst,
         ZBoolean aSave);
      void expr(
         Function* aFunc,
         Instruction* bInst,
         ZBoolean aSave=zTrue);
      void simple(
         Function* aFunc,
         Instruction* bInst);
      void stmt(
         Function* aFunc,
         Instruction* bInst,
         int flags,
         int brkLevel,
         int cntLevel);
      void openStat(const ZString& aFileName);
      void packCode(Function* aFunc);

      // functions in Interpret.cpp
      ZString dumpAdr(Instruction* aAdr);
      ZString dumpInstr(long aAddr, PackedInstruction* aInstr, ZBoolean withStack);
      void dump(Function* aFunc);
      void checkNum(const ZString& aVal);
      void interpret(Function* aFunc);
}; // ZCsl

#define ZCslInitLib(var) ZExport(void) ZCslInitLibNative(ZCsl* var)
#define ZCslCleanupLib(var) ZExport(void) ZCslCleanupLibNative(ZCsl* var)
#define ZC_CSLLIBTYPE "NATIVE"

#endif // _ZCSL_
