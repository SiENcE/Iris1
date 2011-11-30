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

#if !defined( __HUELOADER_H__ )
#define __HUELOADER_H__

#include <map>
#include <string>

// #include "../Fluid/mmgr.h"

// This class handles the hue (color-table) loading

struct stHue
{
	unsigned int colors[32];
	unsigned int tableStart;
	unsigned int tableEnd;
};

class cHueLoader
{
private:
	std::map< unsigned short, stHue > hues;
public:
	cHueLoader ();
	virtual ~cHueLoader();

    void Init (std::string filename);
    void DeInit ();

	const stHue *getHue( unsigned short id );
        void GetRGBHue (unsigned short id, float & r, float & g, float & b);
};

// global hueloader object
extern cHueLoader pHueLoader;

#endif
