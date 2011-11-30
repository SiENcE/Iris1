/*****
 *
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
 *
 *****/

#ifndef __GRNOBJECTS_H__
#define __GRNOBJECTS_H__

#include <vector>
#include "GrannyStream.h"

// #include "../Fluid/mmgr.h"

class Object
{
	public:
		Object();
		~Object();
		void addValue(dword key,dword value);
		dword getValue(dword key);
		bool findValue(dword key,dword value);
	private:
		dword keys[5],values[5];
		dword numKeys;
};

class Objects
{
public:
  Objects();
  ~Objects();
  void load( cGrannyStream *file, dword offset, dword baseOffset, dword children );
	dword getValue( dword object, dword key );
	dword findValue( dword key, dword value );
private:
	std::vector<Object *> objects;

	Object *object;
	dword key;

};

#endif
