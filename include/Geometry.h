//
// File: Geometry.h
// Created by: Alexander Oster - tensor@ultima-iris.de
//
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

 /*
   Contains some basic Geometric functions
 */
 
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

// #include "../Fluid/mmgr.h"


class cVector 
{
    private:
        float m_fields [3];
    public:
        cVector ();
        cVector (float x, float y, float z);
        void Set (float x, float y, float z);
        void CrossProduct (cVector vector1, cVector vector2);
        float DotProduct (cVector vector);
        void Normalize ();
        float Len ();
        float field (int index);
        void setField (int index, float value);
        float x ();
        float y ();
        float z ();
        void Scale (float factor);
};

class cMatrix
{
    private:
        float m_fields [4][4];
    public:
        cMatrix ();
        void Identity ();
        void RotationMatrix (cVector axis, float angle);
        void TranslationMatrix (cVector vector);
        void Multiply (cMatrix & matrix);
        cVector Apply (cVector vector);
        cVector ApplyRotation (cVector vector);
        float field (int i, int j);
};

// One day, everything should be object based, but i have no time for that atm

/** Extracts and saves the current OpenGL frustum */
extern void ExtractFrustum();

/** checks, if a point is in the current frustum */
extern bool PointInFrustum( float x, float y, float z );
/** checks, if a quad is in the current frustum */
extern bool QuadInFrustum( float * quad, float tx, float ty );
/** checks, if a sphere is in the current frustum */
extern bool SphereInFrustum( float * sphere , float tx, float ty );

extern void NormalizeVector(float * vec);
extern void VectorAdd(float * Vector, float * result);
extern void CalcNormal(float * Vertex1, float * Vertex2, float * Vertex3, float * result);
extern float CheckSphereNearPlane( float * sphere , float tx, float ty );
extern void InvertMatrix(float matrix[], float out[]);
extern void Vec3TransformCoord(float v[], float m[], float result[]);
extern void Vec3TransformRotation(float v[], float m[], float result[]);

extern void Vec4TransformCoord(float v[], float m[], float result[]);
extern float GetSquaredDistRayPoint(float vecOrigin[3], float vecDir[3], float vecPoint[3], float & lamda);
extern unsigned int polycounter;
extern float water_sin_table[32][9];
extern void CreateSinTable(void);
extern int flag_cullbackfaces;
bool IntersectTriangleWithRay(float vecOrigin[], float vecDir[], float vecP1[], float vecP2[], float vecP3[], float & lambda);
bool IntersectTriangleWithRay2(float vecOrigin[], float vecDir[], float vecP1[], float vecP2[], float vecP3[], float & lambda);

#endif //_GEOMETRY_H_
