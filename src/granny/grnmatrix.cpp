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

#include <math.h>
//#include <xutility>
#include "../include.h"
#include "granny/grnmatrix.h"




static float identity[16] = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1
};

GrnMatrix::GrnMatrix(float *f)
{
	memcpy(matrix,f,sizeof(matrix));
}

GrnMatrix::GrnMatrix()
{
	setIdentity();
}

GrnMatrix::~GrnMatrix()
{
}

void GrnMatrix::setIdentity()
{
	memcpy(matrix,identity,sizeof(matrix));
}

void GrnMatrix::invert()
{
#define MAT(m,r,c) (m)[(c)*4+(r)]
#define m11 MAT(matrix,0,0)
#define m12 MAT(matrix,0,1)
#define m13 MAT(matrix,0,2)
#define m14 MAT(matrix,0,3)
#define m21 MAT(matrix,1,0)
#define m22 MAT(matrix,1,1)
#define m23 MAT(matrix,1,2)
#define m24 MAT(matrix,1,3)
#define m31 MAT(matrix,2,0)
#define m32 MAT(matrix,2,1)
#define m33 MAT(matrix,2,2)
#define m34 MAT(matrix,2,3)
#define m41 MAT(matrix,3,0)
#define m42 MAT(matrix,3,1)
#define m43 MAT(matrix,3,2)
#define m44 MAT(matrix,3,3)

	float det;
	float d12,d13,d23,d24,d34,d41;
	float tmp[16];

	d12=(m31*m42-m41*m32);
	d13=(m31*m43-m41*m33);
	d23=(m32*m43-m42*m33);
	d24=(m32*m44-m42*m34);
	d34=(m33*m44-m43*m34);
	d41=(m34*m41-m44*m31);
	tmp[0]= (m22*d34-m23*d24+m24*d23);
	tmp[1]=-(m21*d34+m23*d41+m24*d13);
	tmp[2]= (m21*d24+m22*d41+m24*d12);
	tmp[3]=-(m21*d23-m22*d13+m23*d12);

	det=m11*tmp[0]+m12*tmp[1]+m13*tmp[2]+m14*tmp[3];

	if (det==0.0)
	{
		for (int i=0;i<16;i++)
			matrix[i]=identity[i];
	}
	else
	{
		float invDet=1.0/det;
		tmp[0]*=invDet;
		tmp[1]*=invDet;
		tmp[2]*=invDet;
		tmp[3]*=invDet;

		tmp[4]=-(m12*d34-m13*d24+m14*d23)*invDet;
		tmp[5]= (m11*d34+m13*d41+m14*d13)*invDet;
		tmp[6]=-(m11*d24+m12*d41+m14*d12)*invDet;
		tmp[7]= (m11*d23-m12*d13+m13*d12)*invDet;

		d12=m11*m22-m21*m12;
		d13=m11*m23-m21*m13;
		d23=m12*m23-m22*m13;
		d24=m12*m24-m22*m14;
		d34=m13*m24-m23*m14;
		d41=m14*m21-m24*m11;

		tmp[8] = (m42*d34-m43*d24+m44*d23)*invDet;
		tmp[9] =-(m41*d34+m43*d41+m44*d13)*invDet;
		tmp[10]= (m41*d24+m42*d41+m44*d12)*invDet;
		tmp[11]=-(m41*d23-m42*d13+m43*d12)*invDet;
		tmp[12]=-(m32*d34-m33*d24+m34*d23)*invDet;
		tmp[13]= (m31*d34+m33*d41+m34*d13)*invDet;
		tmp[14]=-(m31*d24+m32*d41+m34*d12)*invDet;
		tmp[15]= (m31*d23-m32*d13+m33*d12)*invDet;

		for (int i=0;i<16;i++)
			matrix[i]=tmp[i];
	}
#undef m11
#undef m12
#undef m13
#undef m14
#undef m21
#undef m22
#undef m23
#undef m24
#undef m31
#undef m32
#undef m33
#undef m34
#undef m41
#undef m42
#undef m43
#undef m44
#undef MAT
}

// [x y z 1] * M

Point GrnMatrix::operator*(const Point &v)
{
	Point result;
	result.points[0]=(matrix[0]*v.points[0])+
		(matrix[4]*v.points[1])+
		(matrix[8]*v.points[2])+
		matrix[12];
	result.points[1]=(matrix[1]*v.points[0])+
		(matrix[5]*v.points[1])+
		(matrix[9]*v.points[2])+
		matrix[13];
	result.points[2]=(matrix[2]*v.points[0])+
		(matrix[6]*v.points[1])+
		(matrix[10]*v.points[2])+
		matrix[14];
	return result;
}

GrnMatrix& GrnMatrix::operator*=(const GrnMatrix &m)
{
	float tmp[16];
#define A(row,col) matrix[(col<<2)+row]
#define B(row,col) m.matrix[(col<<2)+row]
#define T(row,col) tmp[(col<<2)+row]
    int i;
	for (i=0;i<4;i++)
	{
		T(i,0)=A(i,0)*B(0,0)+A(i,1)*B(1,0)+A(i,2)*B(2,0)+A(i,3)*B(3,0);
		T(i,1)=A(i,0)*B(0,1)+A(i,1)*B(1,1)+A(i,2)*B(2,1)+A(i,3)*B(3,1);
		T(i,2)=A(i,0)*B(0,2)+A(i,1)*B(1,2)+A(i,2)*B(2,2)+A(i,3)*B(3,2);
		T(i,3)=A(i,0)*B(0,3)+A(i,1)*B(1,3)+A(i,2)*B(2,3)+A(i,3)*B(3,3);
	}
	memcpy(matrix,tmp,sizeof(matrix));
#undef A
#undef B
#undef T
	return *this;
}

GrnMatrix& GrnMatrix::operator=(const GrnMatrix &m)
{
	memcpy(matrix,m.matrix, sizeof(matrix));
	return *this;
}

void GrnMatrix::setScale(const Point &scale)
{
	setIdentity();
	matrix[0]  = scale.points[0];
	matrix[5]  = scale.points[1];
	matrix[10] = scale.points[2];
}

void GrnMatrix::setRotationDegrees(Point rotation)
{
	rotation *= GRAD_PI2;
	setRotationRadians(rotation);
}

void GrnMatrix::setRotationRadians( const Point& rotation )
{
	double cr = cos( rotation.points[0] );
	double sr = sin( rotation.points[0] );
	double cp = cos( rotation.points[1] );
	double sp = sin( rotation.points[1] );
	double cy = cos( rotation.points[2] );
	double sy = sin( rotation.points[2] );

	matrix[0] = (float)( cp*cy );
	matrix[1] = (float)( cp*sy );
	matrix[2] = (float)( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	matrix[4] = (float)( srsp*cy-cr*sy );
	matrix[5] = (float)( srsp*sy+cr*cy );
	matrix[6] = (float)( sr*cp );

	matrix[8] = (float)( crsp*cy+sr*sy );
	matrix[9] = (float)( crsp*sy-sr*cy );
	matrix[10] = (float)( cr*cp );
}

Point GrnMatrix::getRotationDegrees() const
{
#define M(row,col) matrix[((col)*4)+row]

	double Y = -asin(M(2,0)); 
	double D = Y; 
	double C = cos(Y); 
	Y *= GRAD_PI; // <- this was "180.000f/PI" before 
	//   (not part of the bug) 

	double rotx, roty, X, Z; 

	if (fabs(C)>0.0005f) // <- C not Y 
	{ 
		rotx = M(2,2) / C; 
		roty = M(2,1)  / C; 
		X = atan2( roty, rotx ) * GRAD_PI; 
		rotx = M(0,0) / C; 
		roty = M(1,0) / C; 
		Z = atan2( roty, rotx ) * GRAD_PI; 
	} 
	else 
	{ 
		X  = 0.0f; 
		rotx = M(1,1); // <- no minus here 
		roty = -M(0,1); // <- but here, and not (1,0) 
		Z  = atan2( roty, rotx ) * (float)GRAD_PI; 
	} 

	// fix values that get below zero 
	// before it would set (!) values to 360 
	// that where above 360: 
	if (X < 0.00) X += 360.00; 
	if (Y < 0.00) Y += 360.00; 
	if (Z < 0.00) Z += 360.00; 

	return Point(X,Y,Z); 
#undef M
}

Point GrnMatrix::getRotationRadians() const 
{
	Point p = getRotationDegrees();
	p *= GRAD_PI2; 
	return p;
} 



Point GrnMatrix::getTranslation() const
{
	return Point(matrix[12], matrix[13], matrix[14]);
	//return Point(matrix[4], matrix[8], matrix[15]);
}

void GrnMatrix::setTranslation(const Point &v)
{
	setIdentity();
	matrix[12]=v.points[0];
	matrix[13]=v.points[1];
	matrix[14]=v.points[2];
}


void GrnMatrix::setTransform(const Point &quaternion, const Point &translation)
{
	float x,y,z,w,X,Y,Z;

	x=quaternion.points[0];
	y=quaternion.points[1];
	z=quaternion.points[2];
	w=quaternion.points[3];
	
	X=translation.points[0];
	Y=translation.points[1];
	Z=translation.points[2];

	float x2=x+x;  float y2=y+y;  float z2=z+z;
	float xx=x*x2; float xy=x2*y; float xz=x2*z; float xw=x2*w;
	float yy=y*y2; float yz=y2*z; float yw=y2*w;
	float zz=z*z2; float zw=z2*w;

	matrix[0]= 1-yy-zz;
	matrix[1]=   xy+zw;
	matrix[2]=   xz-yw;
	matrix[3]= 0;
	matrix[4]=   xy-zw;
	matrix[5]= 1-xx-zz;
	matrix[6]=   yz+xw;
	matrix[7]= 0;
	matrix[8]=   xz+yw;
	matrix[9]=   yz-xw;
	matrix[10]=1-xx-yy;
	matrix[11]=0;
	matrix[12]=X;
	matrix[13]=Y;
	matrix[14]=Z;
	matrix[15]=1;
}

Point QuaternionToEuler(Point point)
{
/*
	float *p = point.points;
	float X=p[0],Y=p[1],Z=p[2],W=p[3];

	double sqw = W*W;    
	double sqx = X*X;    
	double sqy = Y*Y;    
	double sqz = Z*Z; 
	double temp = 0.0;

	// heading = rotaton about z-axis 
	p[2] = (float) (atan2(2.0 * (X*Y +Z*W),(sqx - sqy - sqz + sqw))); 

	// bank = rotation about x-axis 
	p[0] = (float) (atan2(2.0 * (Y*Z +X*W),(-sqx - sqy + sqz + sqw))); 

	// attitude = rotation about y-axis 
	p[1] = (float) (asin(std::max(-1.0, std::min(1.0, -2.0 * (X*Z - Y*W))))); 

	p[3] = 0;
*/
	return point;
}

Point RadianToDegree(Point point)
{
	return point *= GRAD_PI2;
}


Point DegToRad(Point point)
{
	return point /= GRAD_PI2;	
}
