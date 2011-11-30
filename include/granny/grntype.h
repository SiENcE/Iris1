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

#ifndef __GRNTYPE_H__
#define __GRNTYPE_H__

#include <algorithm>
#include <memory.h>
#include <math.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;


class Point
{
public:
	Point()  { memset(points,0,sizeof(points)); }
	Point(const Point& p) { memcpy(points, p.points, sizeof(points)); }
	Point(float x, float y, float z, float w=0.0f) { points[0]=x;points[1]=y;points[2]=z;points[3]=w; }
	~Point() {}
	Point& operator+=(const Point &p) { points[0]+=p.points[0];points[1]+=p.points[1];points[2]+=p.points[2];points[3]+=p.points[3]; return *this; }
	Point& operator*=(float v) { points[0]*=v;points[1]*=v;points[2]*=v; return *this; }
	Point& operator/=(float v) { points[0]/=v;points[1]/=v;points[2]/=v; return *this; }
	Point  operator*(float v) { Point p(*this); return p*=v; }
	Point  operator/(float v) { Point p(*this); return p/=v; }
	bool operator==(const Point& p) { return memcpy(points,p.points,sizeof(points))==0; }
	float points[4];
	float length() { return sqrtf(points[0]*points[0] + points[1]*points[1] + points[2]*points[2]); }

	Point& normalize()
	{
		float len = length();
		if (len == 0.0f) return *this;
		return *this/=len;
	}
};

class gPolygon
{
public:
	gPolygon() {}
	~gPolygon() {}
	dword nodes[6];
};

template <class T>
void my_delete(T& v) { delete v; v = NULL; }

#endif
