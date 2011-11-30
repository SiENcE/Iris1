/*
 * Created by Nuno Ramiro.
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

#include "SynchronizedTime.h"


int SynchronizedTime::m_iHours = 0, SynchronizedTime::m_iMinutes = 0,
		SynchronizedTime::m_iSeconds = 0;


#ifdef _DEBUG
SynchronizedTime::SynchronizedTime()
{
	// This should NEVER happen. (static class)
	assert( 1 != 1 );
}


SynchronizedTime::~SynchronizedTime()
{

}
#endif


void SynchronizedTime::Synchronize( int iHours, int iMinutes, int iSeconds )
{
	time_t now = time( NULL );
	tm *TimeInfo = localtime( &now );

	m_iHours = iHours - TimeInfo->tm_hour;
	m_iMinutes = iMinutes - TimeInfo->tm_min;
	m_iSeconds = iSeconds - TimeInfo->tm_sec;
}


int SynchronizedTime::GetHours()
{
	time_t now = time( NULL );
	tm *TimeInfo = localtime( &now );

	return m_iHours + TimeInfo->tm_hour;
}


int SynchronizedTime::GetMinutes()
{
	time_t now = time( NULL );
	tm *TimeInfo = localtime( &now );

	return m_iMinutes + TimeInfo->tm_min;
}


int SynchronizedTime::GetSeconds()
{
	time_t now = time( NULL );
	tm *TimeInfo = localtime( &now );

	return m_iSeconds + TimeInfo->tm_sec;
}
