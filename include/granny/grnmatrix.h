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

#ifndef __GRNMATRIX_H__
#define __GRNMATRIX_H__

#include "grntype.h"

// #include "../Fluid/mmgr.h"

class GrnMatrix
{
public:
	GrnMatrix();
	GrnMatrix(float *m);
	~GrnMatrix();
	GrnMatrix& operator*=(const GrnMatrix &m);
	Point operator*(const Point &v);
	GrnMatrix& operator=(const GrnMatrix &m);
	float &operator[](int i) { return matrix[i]; }
	bool operator==(const GrnMatrix &m) { return memcmp(matrix,m.matrix,sizeof(matrix)) == 0; }
	void invert();
	Point getRotationDegrees() const;
	Point getRotationRadians() const;
	Point getTranslation() const;
	void setTranslation(const Point &v);
	void setQuaternion(const Point &q) { setTransform(q, Point(0,0,0)); }
	void setTransform(const Point &quaternion, const Point &translation);
	void setRotationRadians(const Point& rotation);
	void setRotationDegrees(Point rotation);
	void setIdentity();
	void setScale(const Point &scale);
	float matrix[16];
};

Point QuaternionToEuler(Point p);
Point RadToDeg(Point p);
Point DegToRad(Point p);
#endif
