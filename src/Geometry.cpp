//
// File: Geometry.cpp
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

#include "Geometry.h"

#include <math.h>
#include "include.h"

#include <cassert>
#include "irisgl.h"

#include <string.h>




#ifndef WIN32
// There is no any classes with std namespace included in this file
using namespace std;
#endif

unsigned int polycounter = 0;

float water_sin_table[32][9];

void CreateSinTable (void)
{
  for (int p = 0; p < 32; p++)
    for (int x = 0; x < 9; x++)
      //water_sin_table[p][x] = sin ((x / 4.0f + p / 16.0f) * 3.14159f);
      water_sin_table[p][x] = sin ((x / 2.0f + p / 16.0f) * 3.14159f);
}


void VectorCrossProduct (float *V1, float *V2, float *result)
{
  result[0] = V1[1] * V2[2] - V1[2] * V2[1];
  result[1] = V1[2] * V2[0] - V1[0] * V2[2];
  result[2] = V1[0] * V2[1] - V1[1] * V2[0];
}

void VectorSubtract (float *V1, float *V2, float *result)
{
  result[0] = V1[0] - V2[0];
  result[1] = V1[1] - V2[1];
  result[2] = V1[2] - V2[2];
}

void NormalizeVector (float *vector)
{
  float length =
    sqrt (vector[0] * vector[0] + vector[1] * vector[1] +
          vector[2] * vector[2]);
  vector[0] /= length;
  vector[1] /= length;
  vector[2] /= length;
}

void CalcNormal (float *Vertex1, float *Vertex2, float *Vertex3,
                 float *result)
{
  float V1[3];
  float V2[3];

  VectorSubtract (Vertex1, Vertex2, V1);
  VectorSubtract (Vertex2, Vertex3, V2);
  VectorCrossProduct (V1, V2, result);
  NormalizeVector (result);
}

void VectorAdd (float *Vector, float *result)
{
  result[0] += Vector[0];
  result[1] += Vector[1];
  result[2] += Vector[2];
}

float frustum[6][4];

void ExtractFrustum ()
{
  float proj[16];
  float modl[16];
  float clip[16];
  float t;

  /* Get the current PROJECTION matrix from OpenGL */
  glGetFloatv (GL_PROJECTION_MATRIX, proj);

  /* Get the current MODELVIEW matrix from OpenGL */
  glGetFloatv (GL_MODELVIEW_MATRIX, modl);

  /* Combine the two matrices (multiply projection by modelview) */
  clip[0] =
    modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] +
    modl[3] * proj[12];
  clip[1] =
    modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] +
    modl[3] * proj[13];
  clip[2] =
    modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] +
    modl[3] * proj[14];
  clip[3] =
    modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] +
    modl[3] * proj[15];

  clip[4] =
    modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] +
    modl[7] * proj[12];
  clip[5] =
    modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] +
    modl[7] * proj[13];
  clip[6] =
    modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] +
    modl[7] * proj[14];
  clip[7] =
    modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] +
    modl[7] * proj[15];

  clip[8] =
    modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] +
    modl[11] * proj[12];
  clip[9] =
    modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] +
    modl[11] * proj[13];
  clip[10] =
    modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] +
    modl[11] * proj[14];
  clip[11] =
    modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] +
    modl[11] * proj[15];

  clip[12] =
    modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] +
    modl[15] * proj[12];
  clip[13] =
    modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] +
    modl[15] * proj[13];
  clip[14] =
    modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] +
    modl[15] * proj[14];
  clip[15] =
    modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] +
    modl[15] * proj[15];

  /* Extract the numbers for the RIGHT plane */
  frustum[0][0] = clip[3] - clip[0];
  frustum[0][1] = clip[7] - clip[4];
  frustum[0][2] = clip[11] - clip[8];
  frustum[0][3] = clip[15] - clip[12];

  /* Normalize the result */
  t =
    sqrt (frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] +
          frustum[0][2] * frustum[0][2]);
  frustum[0][0] /= t;
  frustum[0][1] /= t;
  frustum[0][2] /= t;
  frustum[0][3] /= t;

  /* Extract the numbers for the LEFT plane */
  frustum[1][0] = clip[3] + clip[0];
  frustum[1][1] = clip[7] + clip[4];
  frustum[1][2] = clip[11] + clip[8];
  frustum[1][3] = clip[15] + clip[12];

  /* Normalize the result */
  t =
    sqrt (frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] +
          frustum[1][2] * frustum[1][2]);
  frustum[1][0] /= t;
  frustum[1][1] /= t;
  frustum[1][2] /= t;
  frustum[1][3] /= t;

  /* Extract the BOTTOM plane */
  frustum[2][0] = clip[3] + clip[1];
  frustum[2][1] = clip[7] + clip[5];
  frustum[2][2] = clip[11] + clip[9];
  frustum[2][3] = clip[15] + clip[13];

  /* Normalize the result */
  t =
    sqrt (frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] +
          frustum[2][2] * frustum[2][2]);
  frustum[2][0] /= t;
  frustum[2][1] /= t;
  frustum[2][2] /= t;
  frustum[2][3] /= t;

  /* Extract the TOP plane */
  frustum[3][0] = clip[3] - clip[1];
  frustum[3][1] = clip[7] - clip[5];
  frustum[3][2] = clip[11] - clip[9];
  frustum[3][3] = clip[15] - clip[13];

  /* Normalize the result */
  t =
    sqrt (frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] +
          frustum[3][2] * frustum[3][2]);
  frustum[3][0] /= t;
  frustum[3][1] /= t;
  frustum[3][2] /= t;
  frustum[3][3] /= t;

  /* Extract the FAR plane */
  frustum[4][0] = clip[3] - clip[2];
  frustum[4][1] = clip[7] - clip[6];
  frustum[4][2] = clip[11] - clip[10];
  frustum[4][3] = clip[15] - clip[14];

  /* Normalize the result */
  t =
    sqrt (frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] +
          frustum[4][2] * frustum[4][2]);
  frustum[4][0] /= t;
  frustum[4][1] /= t;
  frustum[4][2] /= t;
  frustum[4][3] /= t;

  /* Extract the NEAR plane */
  frustum[5][0] = clip[3] + clip[2];
  frustum[5][1] = clip[7] + clip[6];
  frustum[5][2] = clip[11] + clip[10];
  frustum[5][3] = clip[15] + clip[14];

  /* Normalize the result */
  t =
    sqrt (frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] +
          frustum[5][2] * frustum[5][2]);
  frustum[5][0] /= t;
  frustum[5][1] /= t;
  frustum[5][2] /= t;
  frustum[5][3] /= t;
}

bool PointInFrustum (float x, float y, float z)
{
  int p;

  for (p = 0; p < 6; p++)
    if (frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z +
        frustum[p][3] <= 0)
      return false;
  return true;
}

bool QuadInFrustum (float *quad, float tx, float ty)
{
  int p;

  float *v = quad;

  for (p = 0; p < 6; p++)
      {
        if (frustum[p][0] * (v[0] + tx) + frustum[p][1] * (v[1] + ty) +
            frustum[p][2] * v[2] + frustum[p][3] > 0)
          continue;
        if (frustum[p][0] * (v[3] + tx) + frustum[p][1] * (v[4] + ty) +
            frustum[p][2] * v[5] + frustum[p][3] > 0)
          continue;
        if (frustum[p][0] * (v[6] + tx) + frustum[p][1] * (v[7] + ty) +
            frustum[p][2] * v[8] + frustum[p][3] > 0)
          continue;
        if (frustum[p][0] * (v[9] + tx) + frustum[p][1] * (v[10] + ty) +
            frustum[p][2] * v[11] + frustum[p][3] > 0)
          continue;
        return false;
      }

  return true;
}

bool SphereInFrustum (float *sphere, float tx, float ty)
{
  int p;

  for (p = 0; p < 6; p++)
    if (frustum[p][0] * (sphere[0] + tx) + frustum[p][1] * (sphere[1] + ty) +
        frustum[p][2] * sphere[2] + frustum[p][3] <= -sphere[3])
      return false;
  return true;
}

float CheckSphereNearPlane (float *sphere, float tx, float ty)
{
  int p;
  float d;

  for (p = 0; p < 6; p++)
      {
        d =
          frustum[p][0] * (sphere[0] + tx) + frustum[p][1] * (sphere[1] +
                                                              ty) +
          frustum[p][2] * sphere[2] + frustum[p][3] + sphere[3];
        if ((d <= 0.0f) && (p < 5))
          return 0.0f;
      }
  return d;
}

void InvertMatrix (float matrix[], float out[])
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
  float d12, d13, d23, d24, d34, d41;
  float tmp[16];

  d12 = (m31 * m42 - m41 * m32);
  d13 = (m31 * m43 - m41 * m33);
  d23 = (m32 * m43 - m42 * m33);
  d24 = (m32 * m44 - m42 * m34);
  d34 = (m33 * m44 - m43 * m34);
  d41 = (m34 * m41 - m44 * m31);
  tmp[0] = (m22 * d34 - m23 * d24 + m24 * d23);
  tmp[1] = -(m21 * d34 + m23 * d41 + m24 * d13);
  tmp[2] = (m21 * d24 + m22 * d41 + m24 * d12);
  tmp[3] = -(m21 * d23 - m22 * d13 + m23 * d12);

  det = m11 * tmp[0] + m12 * tmp[1] + m13 * tmp[2] + m14 * tmp[3];

  if (det == 0.0)
      {
        memset (out, 0, 16 * sizeof (float));
        out[0] = 1.0f;
        out[5] = 1.0f;
        out[10] = 1.0f;
        out[15] = 1.0f;
      }
  else
      {
        float invDet = 1.0 / det;

        tmp[0] *= invDet;
        tmp[1] *= invDet;
        tmp[2] *= invDet;
        tmp[3] *= invDet;

        tmp[4] = -(m12 * d34 - m13 * d24 + m14 * d23) * invDet;
        tmp[5] = (m11 * d34 + m13 * d41 + m14 * d13) * invDet;
        tmp[6] = -(m11 * d24 + m12 * d41 + m14 * d12) * invDet;
        tmp[7] = (m11 * d23 - m12 * d13 + m13 * d12) * invDet;

        d12 = m11 * m22 - m21 * m12;
        d13 = m11 * m23 - m21 * m13;
        d23 = m12 * m23 - m22 * m13;
        d24 = m12 * m24 - m22 * m14;
        d34 = m13 * m24 - m23 * m14;
        d41 = m14 * m21 - m24 * m11;

        tmp[8] = (m42 * d34 - m43 * d24 + m44 * d23) * invDet;
        tmp[9] = -(m41 * d34 + m43 * d41 + m44 * d13) * invDet;
        tmp[10] = (m41 * d24 + m42 * d41 + m44 * d12) * invDet;
        tmp[11] = -(m41 * d23 - m42 * d13 + m43 * d12) * invDet;
        tmp[12] = -(m32 * d34 - m33 * d24 + m34 * d23) * invDet;
        tmp[13] = (m31 * d34 + m33 * d41 + m34 * d13) * invDet;
        tmp[14] = -(m31 * d24 + m32 * d41 + m34 * d12) * invDet;
        tmp[15] = (m31 * d23 - m32 * d13 + m33 * d12) * invDet;

        for (int i = 0; i < 16; i++)
          out[i] = tmp[i];
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


void Vec3TransformCoord (float v[], float m[], float result[])
{
  float temp[3];
  for (int i = 0; i < 3; i++)
    temp[i] = v[i];
  result[0] = temp[0] * m[0] + temp[1] * m[4] + temp[2] * m[8] + m[12];
  result[1] = temp[0] * m[1] + temp[1] * m[5] + temp[2] * m[9] + m[13];
  result[2] = temp[0] * m[2] + temp[1] * m[6] + temp[2] * m[10] + m[14];
}

void Vec3TransformRotation (float v[], float m[], float result[])
{
  float temp[3];
  for (int i = 0; i < 3; i++)
    temp[i] = v[i];
  result[0] = temp[0] * m[0] + temp[1] * m[4] + temp[2] * m[8];
  result[1] = temp[0] * m[1] + temp[1] * m[5] + temp[2] * m[9];
  result[2] = temp[0] * m[2] + temp[1] * m[6] + temp[2] * m[10];
}

void Vec4TransformCoord (float v[], float m[], float result[])
{
  float temp[4];
  for (int i = 0; i < 4; i++)
    temp[i] = v[i];
  result[0] =
    temp[0] * m[0] + temp[1] * m[4] + temp[2] * m[8] + temp[3] * m[12];
  result[1] =
    temp[0] * m[1] + temp[1] * m[5] + temp[2] * m[9] + temp[3] * m[13];
  result[2] =
    temp[0] * m[2] + temp[1] * m[6] + temp[2] * m[10] + temp[3] * m[14];
  result[3] =
    temp[0] * m[3] + temp[1] * m[7] + temp[2] * m[11] + temp[3] * m[15];
}

float GetSquaredDistRayPoint (float vecOrigin[3], float vecDir[3],
                              float vecPoint[3], float &lambda)
{

  float vecX[3], vecPX[3];
  int i;

  // Calculate nearest point on Ray relative to vecPoint
  lambda = -((vecOrigin[0] * vecDir[0] + vecOrigin[1] * vecDir[1] +
              vecOrigin[2] * vecDir[2]) - (vecPoint[0] * vecDir[0] +
                                           vecPoint[1] * vecDir[1] +
                                           vecPoint[2] * vecDir[2])) /
    (vecDir[0] * vecDir[0] + vecDir[1] * vecDir[1] + vecDir[2] * vecDir[2]);

  for (i = 0; i < 3; i++)
    vecX[i] = vecOrigin[i] + lambda * vecDir[i];

  // Calculate squared distance of vecX and vecPoint

  for (i = 0; i < 3; i++)
    vecPX[i] = vecX[i] - vecPoint[i];

  return (vecPX[0] * vecPX[0] + vecPX[1] * vecPX[1] + vecPX[2] * vecPX[2]);
}


bool IntersectPlaneWithRay (float vecOrigin[], float vecDir[], float vecP[],
                            float vecNormal[], float result[], float &lambda)
{
  int i;


  float divisor = ((vecNormal[0] * vecDir[0]) +
                   (vecNormal[1] * vecDir[1]) + (vecNormal[2] * vecDir[2]));

  if (!divisor)
    return false;

  lambda = -(vecNormal[0] * (vecOrigin[0] - vecP[0]) +
             vecNormal[1] * (vecOrigin[1] - vecP[1]) +
             vecNormal[2] * (vecOrigin[2] - vecP[2])) / divisor;

  for (i = 0; i < 3; i++)
    result[i] = vecOrigin[i] + lambda * vecDir[i];

  //float res = (result[0] - vecP[0]) * vecNormal[0] + (result[1] - vecP[1]) * vecNormal[1] + (result[2] - vecP[2]) * vecNormal[2];

  return true;
}

bool IntersectTriangleWithRay2 (float vecOrigin[], float vecDir[],
                                float vecP1[], float vecP2[], float vecP3[],
                                float &lambda)
{
  int i;

  float vecU[3], vecV[3], vecNormal[3];
  for (i = 0; i < 3; i++)
      {
        vecU[i] = vecP2[i] - vecP1[i];
        vecV[i] = vecP3[i] - vecP1[i];
      }
  VectorCrossProduct (vecU, vecV, vecNormal);

  float vecX[3];

  if (!IntersectPlaneWithRay
      (vecOrigin, vecDir, vecP1, vecNormal, vecX, lambda))
    return false;

  float alpha, beta;

/*	printf("Origin: %.2f %.2f %.2f\n", vecOrigin[0], vecOrigin[1], vecOrigin[2]);
	printf("Dir: %.2f %.2f %.2f\n", vecDir[0], vecDir[1], vecDir[2]);
	printf("X: %.2f %.2f %.2f\n", vecX[0], vecX[1], vecX[2]);
	printf("P1: %.2f %.2f %.2f\n", vecP1[0], vecP1[1], vecP1[2]);
	printf("P2: %.2f %.2f %.2f\n", vecP2[0], vecP2[1], vecP2[2]);
	printf("P3: %.2f %.2f %.2f\n", vecP3[0], vecP3[1], vecP3[2]);  
	*/
/*	for (i = 0; i < 3; i++) {
		if (!vecU[i]) vecU[i] = 0.0000001f;
		if (!vecV[i]) vecV[i] = 0.0000001f;
	}*/

  try
  {
    int idx1 = 0;
    int idx2 = 0;

    if (vecV[0] && vecV[1])
        {
          idx1 = 0;
          idx2 = 1;
        }
    else if (vecV[0] && vecV[2])
        {
          idx1 = 0;
          idx2 = 2;
        }
    else if (vecV[1] && vecV[2])
        {
          idx1 = 1;
          idx2 = 2;
        }
    else
        {

          if (vecU[0])
            idx1 = 0;
          else if (vecU[1])
            idx1 = 1;
          else
            idx1 = 2;

          if (vecV[0])
            idx2 = 0;
          else if (vecV[1])
            idx2 = 1;
          else
            idx2 = 2;
          alpha = (vecX[idx1] - vecP1[idx1]) / vecU[idx1];
          beta = (vecX[idx2] - vecP1[idx2] - alpha * vecU[idx2]) / vecV[idx2];
          return ((alpha >= 0.0f) && (alpha <= 1.0f) &&
                  (beta >= 0.0f) && (beta <= 1.0f) && (alpha <= 1.0f - beta));

        }

    if (vecU[idx1] * vecV[idx2] == vecU[idx2] * vecV[idx1])
        {
          vecU[idx1] += 0.000001f;
        }

    alpha = ((vecV[idx2] * vecX[idx1]) - (vecV[idx1] * vecX[idx2]) +
             (vecV[idx1] * vecP1[idx2]) - (vecV[idx2] * vecP1[idx1])) /
      (vecV[idx2] * vecU[idx1] - vecV[idx1] * vecU[idx2]);

    beta = (vecX[idx1] - vecP1[idx1] - alpha * vecU[idx1]) / vecV[idx1];

    return ((alpha >= 0.0f) && (alpha <= 1.0f) &&
            (beta >= 0.0f) && (beta <= 1.0f) && (alpha <= 1.0f - beta));

  }
  catch (...)
  {
    printf ("EXCEPTION\n");
  }

}

float CalcDeterminant (float detD[9])
{
  return +(detD[0] * detD[4] * detD[8])
    + (detD[1] * detD[5] * detD[6])
    + (detD[2] * detD[3] * detD[7])
    - (detD[6] * detD[4] * detD[2])
    - (detD[7] * detD[5] * detD[0]) - (detD[8] * detD[3] * detD[1]);
}

bool IntersectTriangleWithRay (float vecOrigin[], float vecDir[],
                               float vecP1[], float vecP2[], float vecP3[],
                               float &lambda)
{
  int i;

  float vecU[3], vecV[3], vecNormal[3];
  for (i = 0; i < 3; i++)
      {
        vecU[i] = vecP2[i] - vecP1[i];
        vecV[i] = vecP3[i] - vecP1[i];
      }

  float detD[9];
  float DMain, Dlambda, Dalpha, Dbeta;

  VectorCrossProduct (vecU, vecV, vecNormal);

  detD[0] = vecU[0];
  detD[1] = vecV[0];
  detD[2] = -vecDir[0];
  detD[3] = vecU[1];
  detD[4] = vecV[1];
  detD[5] = -vecDir[1];
  detD[6] = vecU[2];
  detD[7] = vecV[2];
  detD[8] = -vecDir[2];
  DMain = CalcDeterminant (detD);

  if (!DMain)
    return false;

  detD[2] = vecOrigin[0] - vecP1[0];
  detD[5] = vecOrigin[1] - vecP1[1];
  detD[8] = vecOrigin[2] - vecP1[2];

  Dlambda = CalcDeterminant (detD);
  lambda = Dlambda / DMain;

  float vecX[3];
  for (i = 0; i < 3; i++)
    vecX[i] = vecOrigin[i] + vecDir[i] * lambda;

  detD[2] = vecNormal[0];
  detD[5] = vecNormal[1];
  detD[8] = vecNormal[2];
  DMain = CalcDeterminant (detD);
  if (!DMain)
      {
        printf ("This should never ever happen!!!\n");
        return false;
      }

  detD[0] = vecX[0] - vecP1[0];
  detD[1] = vecV[0];
  detD[2] = vecNormal[0];
  detD[3] = vecX[1] - vecP1[1];
  detD[4] = vecV[1];
  detD[5] = vecNormal[1];
  detD[6] = vecX[2] - vecP1[2];
  detD[7] = vecV[2];
  detD[8] = vecNormal[2];
  Dalpha = CalcDeterminant (detD);

  detD[0] = vecU[0];
  detD[1] = vecX[0] - vecP1[0];
  detD[2] = vecNormal[0];
  detD[3] = vecU[1];
  detD[4] = vecX[1] - vecP1[1];
  detD[5] = vecNormal[1];
  detD[6] = vecU[2];
  detD[7] = vecX[2] - vecP1[2];
  detD[8] = vecNormal[2];
  Dbeta = CalcDeterminant (detD);


  float alpha = Dalpha / DMain;
  float beta = Dbeta / DMain;

  return ((alpha >= 0.0f) && (alpha <= 1.0f) &&
          (beta >= 0.0f) && (beta <= 1.0f) && (alpha <= 1.0f - beta));

}



cVector::cVector ()
{
  Set (0.0f, 0.0f, 0.0f);
}

cVector::cVector (float x, float y, float z)
{
  Set (x, y, z);
}

void cVector::Set (float x, float y, float z)
{
  m_fields[0] = x;
  m_fields[1] = y;
  m_fields[2] = z;
}

float cVector::x ()
{
  return m_fields[0];
}

float cVector::y ()
{
  return m_fields[1];
}

float cVector::z ()
{
  return m_fields[2];
}

void cVector::CrossProduct (cVector vector1, cVector vector2)
{
  Set (vector1.y () * vector2.z () - vector1.z () * vector2.y (), // = x
       vector1.z () * vector2.x () - vector1.x () * vector2.z (), // = y
       vector1.x () * vector2.y () - vector1.y () * vector2.x ()  // = z
    );
}

float cVector::DotProduct (cVector vector)
{
  return x () * vector.x () + y () * vector.y () + z () * vector.z ();
}

float cVector::Len ()
{
  return sqrt (m_fields[0] * m_fields[0] + m_fields[1] * m_fields[1] +
               m_fields[2] * m_fields[2]);
}

void cVector::Normalize ()
{
  float len = Len ();
  if (len > 0.0f)
    Scale (1.0f / len);
}

float cVector::field (int index)
{
  assert ((index >= 0) && (index < 3));
  return m_fields[index];
}

void cVector::setField (int index, float value)
{
  assert ((index >= 0) && (index < 3));
  m_fields[index] = value;
}

void cVector::Scale (float factor)
{
  for (int i = 0; i < 3; i++)
    m_fields[i] *= factor;
}


cMatrix::cMatrix ()
{
  Identity ();
}

void cMatrix::Identity ()
{
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      m_fields[i][j] = (i == j) ? 1.0f : 0.0f;  // Kronecker Symbol ;)
}

void cMatrix::RotationMatrix (cVector axis, float angle)    // Creates a rotation matrix
{
  float V[3];
  int i;

  axis.Normalize ();

  for (i = 0; i < 3; i++)

  V[i] = axis.field (i);

  float c = cos (angle);
  float c1 = 1.0f - c;
  float s = sin (angle);

  m_fields[0][0] = V[0] * V[0] * c1 + c;
  m_fields[1][0] = V[0] * V[1] * c1 - V[2] * s;
  m_fields[2][0] = V[0] * V[2] * c1 + V[1] * s;

  m_fields[0][1] = V[1] * V[0] * c1 + V[2] * s;
  m_fields[1][1] = V[1] * V[1] * c1 + c;
  m_fields[2][1] = V[1] * V[2] * c1 - V[0] * s;

  m_fields[0][2] = V[2] * V[0] * c1 - V[1] * s;
  m_fields[1][2] = V[2] * V[1] * c1 + V[0] * s;
  m_fields[2][2] = V[2] * V[2] * c1 + c;

  m_fields[3][3] = 1.0f;
  for (i = 0; i < 3; i++)
      {                         // fill rest with 0.0
        m_fields[i][3] = 0.0f;
        m_fields[3][i] = 0.0f;
      }
}

void cMatrix::TranslationMatrix (cVector vector)
{
  Identity ();
  m_fields[0][3] = vector.x ();
  m_fields[1][3] = vector.y ();
  m_fields[2][3] = vector.z ();
}

void cMatrix::Multiply (cMatrix & matrix)
{
  int i, j;
  float new_fields[4][4];

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      new_fields[i][j] = m_fields[i][0] * matrix.field (0, j) +
        m_fields[i][1] * matrix.field (1, j) +
        m_fields[i][2] * matrix.field (2, j) +
        m_fields[i][3] * matrix.field (3, j);
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      m_fields[i][j] = new_fields[i][j];
}

cVector cMatrix::Apply (cVector vector)
{
  cVector vectemp;
  for (int i = 0; i < 3; i++)
    vectemp.setField (i, m_fields[i][0] * vector.x () +
                      m_fields[i][1] * vector.y () +
                      m_fields[i][2] * vector.z () + m_fields[i][3]);
  return vectemp;
}

cVector cMatrix::ApplyRotation (cVector vector)
{
  cVector vectemp;
  for (int i = 0; i < 3; i++)
    vectemp.setField (i, m_fields[i][0] * vector.x () +
                      m_fields[i][1] * vector.y () +
                      m_fields[i][2] * vector.z ());
  return vectemp;
}

float cMatrix::field (int i, int j)
{
  assert ((i >= 0) && (i < 4) && (j >= 0) && (j < 4));
  return m_fields[i][j];
}
