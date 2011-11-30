//
// File: Camera.cpp
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


#include <iostream>
#include <math.h>
#include "renderer/Camera.h"
#include "include.h"
#include "Config.h"
#include "Geometry.h"
#include "irisgl.h"




using namespace std;

Camera pCamera;
int defmaxangle;
int def_fadetime;
int force_rotation;

Camera::Camera ()
{
  Reset ();
  defmaxangle = Config::GetMaxAngle();
  def_fadetime = Config::GetRoofFadeTime();
  force_rotation = false;
}

Camera::~Camera ()
{
}

void Camera::Reset (void)
{
  x = 0;
  y = 0;
  z = 0;
  anglex = 0;
  angley = 0.0f;
  anglez = 225.0f;
  blockx = 0;
  blocky = 0;
  zoom = 8.0f;
  camhaschanged = true;
}

void Camera::PlaceGLRotationMatrix (void)
{
  glLoadIdentity ();
  glEnable (GL_NORMALIZE);
//  glTranslatef(0.0f, 0.0f, -5.0f - zoom);
  glRotatef (anglex - 45.0, 1.0, 0.0, 0.0);
  glRotatef (angley, 0.0, 1.0, 0.0);
  glRotatef (-anglez + 0.0, 0.0, 0.0, 1.0);
  glScalef (-1.0, 1.0, 1.0f);
//  glTranslatef(x - 0.5f, y - 0.5f, (z - 15.0f) * 0.1f);
/*  glLoadIdentity();
  glRotatef(anglex - 45.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(angley, 0.0f, 1.0f, 0.0f);
  glRotatef(-anglez, 0.0f, 0.0f, 1.0f);*/
  //glScalef(-1.0f, 1.0f, 0.1f);
}

void Camera::PlaceGLMatrix (void)
{
  glLoadIdentity ();
  glEnable (GL_NORMALIZE);
  glTranslatef (0.0f, 0.0f, -5.0f - zoom);
  glRotatef (anglex - 45.0, 1.0, 0.0, 0.0);
  glRotatef (angley, 0.0, 1.0, 0.0);
  glRotatef (-anglez + 0.0, 0.0, 0.0, 1.0);
  glScalef (-1.0, 1.0, 1.0f);
  glTranslatef (x - 0.5f, y - 0.5f, (z - 15.0f) * 0.1f);
}

void Camera::Rotate (float anglex, float angley, float anglez)
{
  this->anglex += anglex;
  this->angley += angley;
  this->anglez += anglez;
  camhaschanged = true;
  int amount = 0;
  
  if ( Config::GetHideSelf() )
   amount = 120 - Config::GetMaxAngle();

  
  if (this->anglex < 45 - 80 && this->zoom <0.5f)
   Config::SetRoofFade( 0 );
  else{
       Config::SetFirstPerson( 0 );
       if( !Config::GetHideSelf() )
        Config::SetRoofFade( 1 );
      }
   
  if (this->anglex < 45 - ( Config::GetMaxAngle() + amount) ){
    this->anglex = 45.0f - (float) ( Config::GetMaxAngle() + amount );
    Config::SetFirstPerson( 1 );
  }
 
   
  
  if (this->anglex > 45.0f)
    this->anglex = 45.0f;
}

int Camera::Move (float value)
{
  int result = false;

  this->x -= sin (anglez / 180 * PI) * value;
  this->y -= cos (anglez / 180 * PI) * value;
  if (x > 8.0)
      {
        x -= 8.0;
        blockx--;
        result = true;
      }

  if (x < -8.0)
      {
        x += 8.0;
        blockx++;
        result = true;
      }

  if (y > 8.0)
      {
        y -= 8.0;
        blocky--;
        result = true;
      }

  if (y < -8.0)
      {
        y += 8.0;
        blocky++;
        result = true;
      }

  camhaschanged = true;
  return result;
}

float Camera::GetX (void)
{
  return x;
}

float Camera::GetY (void)
{
  return y;
}

float Camera::GetZ (void)
{
  return z;
}

float Camera::GetAngleX (void)
{
  return anglex;
}

float Camera::GetAngleY (void)
{
  return angley;
}

float Camera::GetAngleZ (void)
{
  return anglez;
}

int Camera::GetBlockX (void)
{
  return blockx;
}

int Camera::GetBlockY (void)
{
  return blocky;
}


void Camera::SetX (float px)
{
  x = px;
  camhaschanged = true;
}

void Camera::SetY (float py)
{
  y = py;
  camhaschanged = true;
}

void Camera::SetZ (float pz)
{
  z = pz;
  camhaschanged = true;
}

void Camera::SetAngleX (float anglex)
{
  this->anglex = anglex;
  camhaschanged = true;
}

void Camera::SetAngleY (float angley)
{
  this->angley = angley;
  camhaschanged = true;
}

void Camera::SetAngleZ (float anglez)
{
  this->anglez = anglez;
  camhaschanged = true;
}

void Camera::SetBlockX (int px)
{
  blockx = px;
  camhaschanged = true;
}

void Camera::SetBlockY (int py)
{
  blocky = py;
  camhaschanged = true;
}

int Camera::CamHasChanged (void)
{
  int result = camhaschanged;

  camhaschanged = false;
  return result;
}

void Camera::GetGamePosition (unsigned int &x, unsigned int &y, int &z)
{
  x = GetBlockX () * 8 - (int) GetX ();
  y = GetBlockY () * 8 - (int) GetY ();
  z = -(int) GetZ ();
}

void Camera::GetGamePosition( float &x, float &y, float &z )
{
  x = GetBlockX() * 8.0 - GetX();
  y = GetBlockY() * 8.0 - GetY();
  z = -GetZ();
}

void Camera::GetCameraState (struct CameraState &state)
{
  state.x = x;
  state.y = y;
  state.z = z;
  state.anglex = anglex;
  state.angley = angley;
  state.anglez = anglez;
  state.blockx = blockx;
  state.blocky = blocky;
}

void Camera::SetCameraState (struct CameraState state)
{
  if ((x != state.x) || (y != state.y) || (z != state.z)
      || (anglex != state.anglex) || (angley != state.angley)
      || (anglez != state.anglez) || (x != state.x) || (y != state.y))
    camhaschanged = true;

  x = state.x;
  y = state.y;
  z = state.z;
  anglex = state.anglex;
  angley = state.angley;
  anglez = state.anglez;
  blockx = state.blockx;
  blocky = state.blocky;
}

void Camera::ChangeZoom (float value)
{
  zoom += value;
  
  if ( zoom > Config::GetMaxZoom() )
    zoom = (float)Config::GetMaxZoom();
    
  float maxvalue = -5.0f;
  
  if ( Config::GetFirstPerson() ){
   //maxvalue = -5.0f;
   if(zoom < 0.5)
    Config::SetRoofFade( 0 );
   else
    Config::SetRoofFade( 1 );
  }

  if (zoom < maxvalue)
    zoom = maxvalue;
  camhaschanged = true;
 
  if(zoom < -3.2f){
   Config::SetHideSelf( 1 );
   Config::SetPerspective( 1 );
   }
  else
   Config::SetHideSelf( 0 );
      
}

void Camera::CreatePickRay (int mousex, int mousey, float vecPickRayOrigin[3],
                            float vecPickRayDir[3])
{
  float vecTemp[3];
  float projmatrix[16];
  float matrix[16];
  int i;

  glGetFloatv (GL_PROJECTION_MATRIX, projmatrix);

  vecTemp[0] = (((2.0f * mousex) / Config::GetWidth() ) - 1) / projmatrix[0];
  vecTemp[1] = -(((2.0f * mousey) / Config::GetHeight() ) - 1) / projmatrix[5];
  vecTemp[2] = -1.0f;

// Matrix stuff

  glMatrixMode (GL_MODELVIEW_MATRIX);
  glPushMatrix ();
  PlaceGLMatrix ();

  glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
  InvertMatrix (matrix, matrix);
  glPopMatrix ();

// Transform Screenspace-PickRay into Worldspace-PickRay

  Vec3TransformCoord (vecTemp, matrix, vecPickRayDir);
  vecPickRayOrigin[0] = matrix[12];
  vecPickRayOrigin[1] = matrix[13];
  vecPickRayOrigin[2] = matrix[14];
  for (i = 0; i < 3; i++)
    vecPickRayDir[i] -= vecPickRayOrigin[i];
  NormalizeVector (vecPickRayDir);

// PickRay Intersection with NearPlane is the Origin of the PickRay
  for (i = 0; i < 3; i++)
    vecPickRayOrigin[i] += vecPickRayDir[i] * 0.2f;   // 0.2f = NEAR_Z
}


void Camera::GetRenderCoords (float vecP[3], int &x, int &y)
{
//    glMatrixMode(GL_MODELVIEW_MATRIX); 
//        glPushMatrix();
//    PlaceGLMatrix();
  //glLoadIdentity();

  float vecTemp[4], vecTemp2[4], vecTemp3[4];
  for (int i = 0; i < 3; i++)
    vecTemp[i] = vecP[i];
  vecTemp[3] = 1.0f;


  Vec4TransformCoord (vecTemp, m_modelview_matrix, vecTemp2);
  Vec4TransformCoord (vecTemp2, m_projection_matrix, vecTemp3);

  float fx = vecTemp3[0] / vecTemp3[3];
  float fy = vecTemp3[1] / vecTemp3[3];

  x = (int) ((fx + 1.0f) * Config::GetWidth() * 0.5f);
  y = (int) ((-fy + 1.0f) * Config::GetHeight() * 0.5f);
  //Vec3TransformCoord(vecTemp, projmatrix, vecTemp2);
//        glPopMatrix();
}


void Camera::FetchMatrix ()
{
  glGetFloatv (GL_PROJECTION_MATRIX, m_projection_matrix);
  glGetFloatv (GL_MODELVIEW_MATRIX, m_modelview_matrix);
}


void Camera::SetForceRotation(bool force)
{
  force_rotation=force;   
}

bool Camera::forceRotation()
{
  return force_rotation;   
}

