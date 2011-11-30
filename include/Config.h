/*! \file Config.h
 * \brief Iris configurations loads from config.xml.
 * 
 * Config deals with all Iris configs, which are parsed from config.xml.
 *
 * They are stored in some variables which are accessed later.
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Alexander Oster.
 * Last change: 17-02-06 (Nuno Ramiro)
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <assert.h>
#include <string>
#include "xml.h"
#include "Exception.h"
#include "renderer/SDLScreen.h"
#include "FontManager.h"

// #include "../Fluid/mmgr.h"


/// \brief ParseData structure, parses all needed data from the file.
struct ParserData
{
	ParserData( const std::string &sName, int iType, void *pData )
		: sName( sName ), iType( iType ), pData( pData )
	{
	};

	std::string sName;
	int iType;
	void *pData;
};


/*
 * DataType is the type of data that we load, each node/section has different types.
 */
enum DataType
{
	IS_SECTION = 0,
	IS_INTEGER = 1,
	IS_STRING = 2,
	IS_BOOL = 3,
	IS_END = 4
};

///\brief Config class, this class will handle our Game configurations, loads Config.xml and handles every configuration.
class Config
{
public:
#ifdef _DEBUG
	Config();
	~Config();
#endif

	/// Initializes/Gets values from xml, MUST be initialized before using otherwise you might get weird values.
	static bool Init();
	/// Registers needed fonts
	//static bool RegisterFonts();
	/// Deletes everything Config related
	static void Close();

	// Getters
	// GFX

	/// Returns configuration version
	static std::string GetVersion();
	/// Returns configuration Width
	static int GetWidth();
	/// Returns configuration Height
	static int GetHeight();
	/// Returns configuration BPP.
	static int GetBPP();
	/// Returns configuration if it should start FullScreen or not.
	static int GetStartFullScreen();
	/// Returns configuration if we are using Cursor or not.
	static int GetCursor();
	/// Returns configuration Perspective.
	static int GetPerspective();
	/// Returns configuration Depth Buffer.
	static int GetDepthBuffer();
	/// Returns configuration Max Zoom allowed.
	static int GetMaxZoom();
	/// Returns configuration Max Angle allowed.
	static int GetMaxAngle();
	/// Returns configuration View Distance.
	static int GetViewDistance();
	/// Returns configuration Brightness.
	static int GetBrightness();
	/// Returns configuration Animation Smoothness.
	static int GetAnimationSmoothness();
	/// Returns configuration 2D Width.
	static int GetWidth2D();
	/// Returns configuration 2D Height
	static int GetHeight2D();
	/// Returns configuration if we are first person or not.
	static int GetFirstPerson();
	/// Returns configuration if we are hiding self or not.
	static int GetHideSelf();
	/// Returns configuration Roof Fade.
	static int GetRoofFade();

	// UO

	/// Returns configuration Start X Position.
	static int GetStartX();
	/// Returns configuration Start Y Position.
	static int GetStartY();
	/// Returns configuration Start Z Position.
	static int GetStartZ();
	/// Returns configuration if we are using AOS or not.
	static int GetAOS();
	/// Returns configuration Mul path.
	static std::string GetMulPath();
	/// Returns configuration Compressed Map.
	static std::string GetCompressedMap();
	/// Returns configuration if we are using popup or not.
	static int GetPopup();
	/// Returns configuration Client Version.
	static std::string GetClientVersion();
	/// Returns configuration Clilocs.
	static int GetClilocs();
	/// Returns configuration Clilocs Language.
	static std::string GetClilocLang();
	/// Returns configuration Speech Hue color.
	static int GetSpeechHue();
	/// Returns configuration if we are using AOS Tool Tips or not.
	static int GetAOSToolTips();
    //Artix:
    /// Returns configuration if we are using speech.mul support.
	static int GetUseSpeech();
	// Net

	/// Returns configuration Server IP.
	static std::string GetServer();
	/// Returns configuration Username.
	static std::string GetLogin();
	/// Returns configuration Password.
	static std::string GetPassword();
	/// Returns configuration Server Port.
	static int GetServerPort();
	/// Returns configuration Roof Fade Time.
	static int GetRoofFadeTime();
	/// Returns configuration Roof Fade Alpha.
	static int GetRoofFadeAlpha();
	/// Returns configuration if we are connecting to a Sphere Server.
	static int GetIsSphere();
	/// Returns configuration if we are connecting to a Sphere55R Server.
	static int GetIsSphere55R();
	/// Returns configuration if we are connecting to a Pol Server.
	static int GetIsPol();
	/// Returns configuration if we are connecting to a RunUO Server.
	static int GetIsRunUO();
	/// Returns configuration if we are connecting to a UOX3 Server.
	static int GetIsUox3();
	/// Returns configuration Client Key.
	static int GetClientKey();

	// Sound

	/// Returns configuration if we want to listen game's music or not.
	static int GetMusic();
	/// Returns configuration if we want to listen to game's sounds or not.
	static int GetSound();
	/// Returns configuration Sound Frequency.
	static int GetFrequency();
	/// Returns configuration if we want it to be Stereo or not.
	static int GetStereo();
	/// Returns configuration Chunk Size.
	static int GetChunkSize();
	/// Returns configuration Music Volume.
	static int GetMusicVolume();
	/// Returns configuration Sound Volume.
	static int GetSoundVolume();
	/// Returns configuration if we are using MP3 for musics or not.
	static int GetMP3();
	/// Returns configuration if we want to listen to Foot Steps.
	static int GetFootSteps();

	// Iris Client

	/// Returns configuration Iris Scripts Path
	static std::string GetScriptPath();
	/// Returns configuration Iris Mouse Move Time.
	static int GetMouseMotionTimer();


	// Setters

	/// Sets Cursor
	static void SetCursor( int iCursor );
	/// Sets Brightness.
	static void SetBrightness( int iBrightness );
	/// Sets using First Person.
	static void SetFirstPerson( int iFirstPerson );
	/// Sets HideSelf.
	static void SetHideSelf( int iHideSelf );
	/// Sets UserName.
	static void SetLogin( std::string sLogin );
	/// Sets listen to Music.
	static void SetMusic( int iMusic );
	/// Sets Password.
	static void SetPassword( std::string sPassword );
	/// Sets Perspective.
	static void SetPerspective( int iPerspective );
	/// Sets Roof Fade.
	static void SetRoofFade( int iRoofFade );
	/// Sets Roof Fade Time.
	static void SetRoofFadeTime( int iRoofFadeTime );
	/// Sets Server IP.
	static void SetServer( std::string sServer );
	/// Sets listen to Sound.
	static void SetSound( int iSound );
	/// Sets Speech Hue color.
	static void SetSpeechHue( int iSpeechHue );
	/// Sets Clilocs.
	static void SetClilocs( int iClilocs );


protected:
	// ParserData Vector (With all nodes)
	static std::vector<ParserData> m_vParserInfo;

	// GFX
	static std::string m_sVersion;
	static int m_iWidth, m_iHeight, m_iBPP, m_iStartFullScreen;
	static int m_iCursor, m_iPerspective, m_iDepthBuffer, m_iMaxZoom, m_iMaxAngle;
	static int m_iViewDistance, m_iBrightness;
	static int m_iWidth2D, m_iHeight2D;
	static int m_iFirstPerson, m_iHideSelf, m_iRoofFade;

	// UO
	static int m_iStartX, m_iStartY, m_iStartZ, m_iAOS;
	static std::string m_sMulPath, m_sCompressedMap;
	static int m_iPopup;
	static std::string m_sClientVersion;
	static int m_iClilocs;
	static std::string m_sClilocLang;
	static int m_iSpeechHue, m_iAOSToolTips, m_iSpeech; //Artix

	// Net
	static std::string m_sServer, m_sLogin, m_sPassword;
	static int m_iServerPort, m_iRoofFadeTime, m_iRoofFadeAlpha, m_iIsSphere, m_iIsSphere55R,
		m_iIsPol, m_iIsRunUO, m_iIsUox3, m_iClientKey;

	// Sound
	static int m_iMusic, m_iSound, m_iFrequency, m_iStereo, m_iChunkSize, m_iMusicVolume, m_iSoundVolume, m_iMP3, m_iFootSteps;

	// Iris Client
	static std::string m_sScriptPath;
	static int m_iMouseMotionTimer;
};

#endif
