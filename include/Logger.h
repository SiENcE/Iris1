/*! \file Logger.h
 * \brief Logger Class, used to debug / log important information.
 * 
 * This is a logger which will debug everything if it is turned on.
 *
 * Used to debug the application, by saving everything into a log file.
 *
 * Should be used when getting weird values or to make sure something happens.
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Nuno Ramiro (15-02-06)
 * Last change: 23-02-06 (Nuno Ramiro)
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Common.h"
#include <string>
#include <fstream>
#include <assert.h>

// #include "../Fluid/mmgr.h"


/*
 * LogLevel enumeration
 */
enum LogLevel
{
	LEVEL_WARNING = 0,
	LEVEL_ERROR = 1,
	LEVEL_CRITICAL = 2,
	LEVEL_NONE = 3
};


///\brief The Logger class will enable you to log and debug what you need to.
/// This is a static class, and should be able to log you whatever you want.
class Logger
{
public:
	// Under release we do NOT need constructor/destructor since the class is static
#ifdef _DEBUG
	Logger();
	~Logger();
#endif

	/// Initializes the Logger given version, and log filename.
	static bool Init( const std::string sVersion );

	///\brief Initializes the Logger given version, and log filename.
	/// Note that you MUST include the extension (.log/.txt).
	static bool Init( const std::string sVersion, const std::string sLogName );

	/// Log, Write to file given Message, FileName, Line and Level.
	static void Write( const char *cMessage, const char *cFileName, const int iLine, const int iLevel );
	
	/// Log, Write to file given Message
	static void Write( const char *cMessage );

	/// Log, Write to file given Message
	static void Write( const std::string sMessage );

	/// Log, Write to file given Message, FileName, Line and Level and adds a line at the end.
	static void WriteLine( const char *cMessage, const char *cFileName, const int iLine, const int iLevel );

	/// Log, Write to a file given Message and adds a line at the end.
	static void WriteLine( const char *cMessage );

	/// Log, Write to a file given Message and adds a line at the end.
	static void WriteLine( const std::string sMessage );

	/// Log, Writes to a file an empty line.
	static void WriteLine();

	/// Writes a debug message to screen only when debugging.
	static void WriteDebug( std::string sMessage );

	/// Get Logging level
	static int GetLoglevel( void );

	/// Set Logging level
	static void SetLoglevel( const int iLogLevel );

	/// End Logging
	static void Close();

private:
	static int m_iLogLevel;				//<Log Level variable
	static bool m_bLogToFile;			//<Log to a file (yes/no) variable
	static std::fstream *m_fLogFile;	//<Log file
};

#endif //_LOGGER_H_
