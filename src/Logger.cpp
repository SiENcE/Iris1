/*
 * Debug Logger, used to debug.
 *
 * This is a logger which will debug everything if it is turned on.
 *
 * Used to debug the application, when getting weird values or to make sure something happens.
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Nuno Ramiro (15-02-06)
 * Last change: 08-03-06 (Nuno Ramiro)
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

#include "Logger.h"



// Initialize static variables (NOTE: They're static MUST be initialized before and globaly).
std::fstream *Logger::m_fLogFile = NULL;
int Logger::m_iLogLevel = LEVEL_WARNING;

#ifndef _DEBUG
	bool Logger::m_bLogToFile = true;
#else
	bool Logger::m_bLogToFile = false;
#endif


#ifdef _DEBUG
Logger::Logger()
{
	// Force Error (this is a static class)
	assert( 1 != 1 );
}


Logger::~Logger()
{
}
#endif


/// Initializes the Logger using default debug file.
bool Logger::Init( const std::string sVersion )
{
	return Init( sVersion, "IrisDebug.log" );
}


/// Initializes the Logger given version.
bool Logger::Init( const std::string sVersion, const std::string sLogName )
{
	assert( sLogName.compare( "" ) );

	if ( m_bLogToFile )
	{
		if ( NULL == m_fLogFile )
		{
			m_fLogFile = new std::fstream( sLogName.c_str() );
			m_fLogFile->close();
		}

		m_fLogFile->open( sLogName.c_str(), std::ios::out | std::ios::trunc );

		if ( !m_fLogFile->is_open() )
		{
			return false;
		}
	}

	WriteLine( std::string( "Iris version " ) + sVersion.c_str() +
		std::string( " Copyright (©) 2003-2006 Iris Team" ) );
	WriteLine( "Iris comes with ABSOLUTELY NO WARRANTY" );
	WriteLine( "This is free software, and you are welcome to redistribute it" );
	WriteLine( "under certain conditions; for details, take a look into License.txt" );
#ifdef _DEBUG
	WriteLine( "\nAttention this is the debug version of Iris, if you are not a developer you \nshould not be using this version." );
#endif

	if ( m_bLogToFile )
	{
		m_fLogFile->flush();
	}

	return true;
}


/// Log, Writes cMessage to log file
void Logger::Write( const char *cMessage, const char *cFileName, const int iLine, const int iLevel )
{
	// Ignore messages below our loglevel
	if ( iLevel < m_iLogLevel )
	{
		return;
	}

	std::string sErrorMsg;

	switch ( iLevel )
	{
	case LEVEL_WARNING:
		sErrorMsg = "Warning: ";
		break;

	case LEVEL_ERROR:
		sErrorMsg = "Error: ";
		break;

	case LEVEL_CRITICAL:
		sErrorMsg = "CRITICAL Error: ";
		break;
	}

	char cLineStr[32];

	sprintf( cLineStr, "%d", iLine );

	sErrorMsg += "(" + std::string( cFileName ) + "," +
		std::string( cLineStr ) + "): " + std::string( cMessage );

	// Dump the message out to a file
	if ( m_bLogToFile )
	{
		(*m_fLogFile) << sErrorMsg.c_str();
		m_fLogFile->flush();
	}
	else
	{
		fprintf( stderr, sErrorMsg.c_str() );
	}
}


/// Log, Writes cMessage to log file
void Logger::Write( const char *cMessage )
{
	// Dump the message out to a file
	if ( m_bLogToFile )
	{
		(*m_fLogFile) << cMessage;
		m_fLogFile->flush();
	}
	else
	{
		fprintf( stderr, "%s", cMessage );
	}
}


/// Log, Writes sMessage to log file
void Logger::Write( const std::string sMessage )
{
	Write( (char *) sMessage.c_str() );
}


/// Write cMessage into cFileName with iLine and iLevel and adds a new line at the end.
void Logger::WriteLine( const char *cMessage, const char *cFileName, const int iLine, const int iLevel )
{
	Write( cMessage, cFileName, iLine, iLevel );
	WriteLine();
}


/// Write cMessage with a new line.
void Logger::WriteLine( const char *cMessage )
{
	Write( cMessage );
	WriteLine();
}


/// Write sMessage with a new line.
void Logger::WriteLine( const std::string sMessage )
{
	Write( ( sMessage + "\n" ) );
}


/// Write an empty line.
void Logger::WriteLine()
{
	Write( "\n" );
}


/// Write to screen when on debugging mode a message.
void Logger::WriteDebug( std::string sMessage )
{
	#ifdef _DEBUG
	Write( ( sMessage + "\n" ) );
	#endif
}


/// Get Logging level
int Logger::GetLoglevel( void )
{
	return m_iLogLevel;
}


/// Set Logging level
void Logger::SetLoglevel( const int iLogLevel )
{
	m_iLogLevel = iLogLevel;
}


/// Close log file and delete it.
void Logger::Close()
{
	if ( m_bLogToFile )
	{
		if ( m_fLogFile->is_open() )
		{
			WriteLine( "[End of File]" );
			m_fLogFile->flush();
			m_fLogFile->close();

			SAFE_DELETE( m_fLogFile );
		}
	}
}
