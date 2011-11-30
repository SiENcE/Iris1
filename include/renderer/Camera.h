//
// File: Camera.h
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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"

// #include "../Fluid/mmgr.h"

struct CameraState {
	float x, y, z;
 	float anglex, angley, anglez;
 	int blockx, blocky;
};

class Camera
{
private:
   float m_projection_matrix[16];
   float m_modelview_matrix[16];

public:
    Camera ();
   virtual ~Camera ();

   virtual void Reset(void);
   virtual void PlaceGLMatrix(void); /* Sets the View of the Camera (Translation & Rotation) */
   virtual void PlaceGLRotationMatrix(void); /* Sets the View of the Camera (only Rotation) */
   virtual void Rotate(float anglex, float angley, float anglez);
   virtual int Move(float value); /* returns, if a new mapblock must be loaded */

   float GetX(void);
   float GetY(void);
   float GetZ(void);

   float GetAngleX(void);
   float GetAngleY(void);
   float GetAngleZ(void);

   int GetBlockX(void);
   int GetBlockY(void);

   void GetCameraState(struct CameraState &state);
   void SetCameraState(struct CameraState state);

   void SetX(float px);
   void SetY(float py);
   void SetZ(float pz);

   void SetAngleX(float anglex);
   void SetAngleY(float angley);
   void SetAngleZ(float anglez);

   void SetBlockX(int px);
   void SetBlockY(int py);
   
   void CamUpdate() { camhaschanged = true; }
   int CamHasChanged (void);
   int CamHasChanged_DontReset (void) { return camhaschanged; }

   void GetGamePosition(unsigned int &x, unsigned int &y, int &z);
   void GetGamePosition(float &x, float &y, float &z);
   
   void ChangeZoom(float value);

   void CreatePickRay(int mousex, int mousey, float vecOrigin[3], float vecDir[3]);
   void GetRenderCoords(float vecP[3], int & x, int & y);
   
   void FetchMatrix ();
   float GetZoom () { return this->zoom; }
   
   void SetForceRotation(bool force);
   bool forceRotation();
   
protected:

 float x, y, z;
 float anglex, angley, anglez;
 int blockx, blocky;
 float zoom;

 int camhaschanged;
};

extern Camera pCamera;

#endif //_CAMERA_H_
