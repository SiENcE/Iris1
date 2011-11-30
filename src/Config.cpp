/*
 * Created by Alexander Oster.
 * Changed: 10-01-04 (SiENcE)
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

#include "Config.h"


// Initializing variables
std::vector<ParserData> Config::m_vParserInfo;

std::string Config::m_sVersion = "0.8.6";

int Config::m_iWidth = 640, Config::m_iHeight = 480, Config::m_iBPP = 16, Config::m_iStartFullScreen = 0,
Config::m_iCursor = 1, Config::m_iPerspective = 1, Config::m_iDepthBuffer = 16, Config::m_iMaxZoom = 10,
Config::m_iMaxAngle = 75, Config::m_iViewDistance = 8, Config::m_iBrightness = 5,
Config::m_iWidth2D = 640, Config::m_iHeight2D = 480, Config::m_iFirstPerson = 0, Config::m_iHideSelf = 0,
Config::m_iRoofFade = 1, Config::m_iStartX = 1500, Config::m_iStartY = 1000, Config::m_iStartZ = 0, Config::m_iAOS = 1,
//Artix: added m_iSpeech var for speech.mul 
Config::m_iPopup = 1, Config::m_iClilocs = 1, Config::m_iSpeechHue = 100, Config::m_iAOSToolTips = 0, Config::m_iSpeech=0;

std::string Config::m_sMulPath = "", Config::m_sCompressedMap = "", Config::m_sClientVersion = "4.0.10a", Config::m_sScriptPath = "root",
Config::m_sClilocLang = "enu", Config::m_sServer = "localhost", Config::m_sLogin = "Account", Config::m_sPassword = "";

int Config::m_iServerPort = 2593, Config::m_iRoofFadeTime = 2000, Config::m_iRoofFadeAlpha = 1,
Config::m_iIsSphere = 0, Config::m_iIsSphere55R = 0, Config::m_iIsPol = 0, Config::m_iIsRunUO = 1, Config::m_iIsUox3 = 0,
Config::m_iClientKey = 1, Config::m_iMusic = 1, Config::m_iSound = 1, Config::m_iFrequency = 22050, Config::m_iStereo = 1,
Config::m_iChunkSize = 1024, Config::m_iMusicVolume = 60, Config::m_iSoundVolume = 80, Config::m_iMP3 = 1,
Config::m_iFootSteps = 0, Config::m_iMouseMotionTimer = 500;


#ifdef _DEBUG
Config::Config()
{
	// This IS a static class should NEVER be instanced.
	assert( 1 != 1 );
}


Config::~Config()
{
}
#endif


bool Config::Init()
{
	// Default values, if they are not set under config.xml those will be used.
	// Section GFX
	m_vParserInfo.push_back( ParserData( "GFX", IS_SECTION, NULL ) );
	m_vParserInfo.push_back( ParserData( "WIDTH", IS_INTEGER, &m_iWidth ) );
	m_vParserInfo.push_back( ParserData( "HEIGHT", IS_INTEGER, &m_iHeight ) );
	m_vParserInfo.push_back( ParserData( "BPP", IS_INTEGER, &m_iBPP ) );
	m_vParserInfo.push_back( ParserData( "CURSOR", IS_INTEGER, &m_iCursor ) );
	m_vParserInfo.push_back( ParserData( "PERSPECTIVE", IS_INTEGER, &m_iPerspective ) );
	m_vParserInfo.push_back( ParserData( "FULLSCREEN", IS_INTEGER, &m_iStartFullScreen ) );
	m_vParserInfo.push_back( ParserData( "ROOF_FADE_TIME", IS_INTEGER, &m_iRoofFadeTime ) );
	m_vParserInfo.push_back( ParserData( "ROOF_FADE_ALPHA", IS_INTEGER, &m_iRoofFadeAlpha ) );
	m_vParserInfo.push_back( ParserData( "ZBUFFER_SIZE", IS_INTEGER, &m_iDepthBuffer ) );
	m_vParserInfo.push_back( ParserData( "MAXZOOM", IS_INTEGER, &m_iMaxZoom ) );
	m_vParserInfo.push_back( ParserData( "MAXANGLE", IS_INTEGER, &m_iMaxAngle ) );
	m_vParserInfo.push_back( ParserData( "VIEWDISTANCE", IS_INTEGER, &m_iViewDistance ) );
	m_vParserInfo.push_back( ParserData( "BRIGHTNESS", IS_INTEGER, &m_iBrightness ) );

	// Section UO
	m_vParserInfo.push_back( ParserData( "UO", IS_SECTION, NULL ) );
	m_vParserInfo.push_back( ParserData( "STARTX", IS_INTEGER, &m_iStartX ) );
	m_vParserInfo.push_back( ParserData( "STARTY", IS_INTEGER, &m_iStartY ) );
	m_vParserInfo.push_back( ParserData( "STARTZ", IS_INTEGER, &m_iStartZ ) );
	m_vParserInfo.push_back( ParserData( "MULPATH", IS_STRING, &m_sMulPath ) );
	// m_vParserInfo.push_back( ParserData( "COMPRESSED_MAP", IS_STRING, &m_sCompressedMap ) );
	m_vParserInfo.push_back( ParserData( "POPUP_MENU", IS_INTEGER, &m_iPopup ) );
	m_vParserInfo.push_back( ParserData( "AOS", IS_INTEGER, &m_iAOSToolTips ) );
	m_vParserInfo.push_back( ParserData( "CLIENT_IDENTIFICATION", IS_STRING, &m_sClientVersion ) );
	m_vParserInfo.push_back( ParserData( "USE_CLILOCS", IS_INTEGER, &m_iClilocs ) );
	m_vParserInfo.push_back( ParserData( "CLILOC_LANGUAGE", IS_STRING, &m_sClilocLang ) );
	m_vParserInfo.push_back( ParserData( "SPEECH_HUE", IS_INTEGER, &m_iSpeechHue ) );
	m_vParserInfo.push_back( ParserData( "AOSTOOLTIPS", IS_INTEGER, &m_iAOSToolTips ) );
    //Artix added speech.mul config option
    m_vParserInfo.push_back( ParserData( "USE_SPEECH", IS_INTEGER, &m_iSpeech ) );
	// Section NET
	m_vParserInfo.push_back( ParserData( "NET", IS_SECTION, NULL ) );
	m_vParserInfo.push_back( ParserData( "PORT", IS_INTEGER, &m_iServerPort ) );
	m_vParserInfo.push_back( ParserData( "SERVER", IS_STRING, &m_sServer ) );
	m_vParserInfo.push_back( ParserData( "LOGIN", IS_STRING, &m_sLogin ) );
	m_vParserInfo.push_back( ParserData( "PASSWORD", IS_STRING, &m_sPassword ) );
	m_vParserInfo.push_back( ParserData( "IS_SPHERE", IS_INTEGER, &m_iIsSphere ) );
	m_vParserInfo.push_back( ParserData( "IS_SPHERE55R", IS_INTEGER, &m_iIsSphere55R ) );
	m_vParserInfo.push_back( ParserData( "IS_POL", IS_INTEGER, &m_iIsPol ) );
	m_vParserInfo.push_back( ParserData( "IS_RUNUO", IS_INTEGER, &m_iIsRunUO ) );
	m_vParserInfo.push_back( ParserData( "IS_UOX3", IS_INTEGER, &m_iIsUox3 ) );
	m_vParserInfo.push_back( ParserData( "CLIENT_KEY", IS_INTEGER, &m_iClientKey ) );

	// Section Sound
	m_vParserInfo.push_back( ParserData( "SOUND", IS_SECTION, NULL ) );
	m_vParserInfo.push_back( ParserData( "MUSIC", IS_INTEGER, &m_iMusic ) );
	m_vParserInfo.push_back( ParserData( "SOUND", IS_INTEGER, &m_iSound ) );
	m_vParserInfo.push_back( ParserData( "FREQUENCY", IS_INTEGER, &m_iFrequency ) );
	m_vParserInfo.push_back( ParserData( "STEREO", IS_INTEGER, &m_iStereo ) );
	m_vParserInfo.push_back( ParserData( "CHUNKSIZE", IS_INTEGER, &m_iChunkSize ) );
	m_vParserInfo.push_back( ParserData( "MUSICVOLUME", IS_INTEGER, &m_iMusicVolume ) );
	m_vParserInfo.push_back( ParserData( "SOUNDVOLUME", IS_INTEGER, &m_iSoundVolume ) );
	m_vParserInfo.push_back( ParserData( "MP3", IS_INTEGER, &m_iMP3 ) );
	m_vParserInfo.push_back( ParserData( "FOOTSTEPS", IS_INTEGER, &m_iFootSteps ) );

	// Section Iris Client
	m_vParserInfo.push_back( ParserData( "IRIS", IS_SECTION, NULL ) );
	m_vParserInfo.push_back( ParserData( "SCRIPTS_PATH", IS_STRING, &m_sScriptPath ) );
	m_vParserInfo.push_back( ParserData( "MOUSEOVER_TIMER", IS_INTEGER, &m_iMouseMotionTimer ) );
	m_vParserInfo.push_back( ParserData( "", IS_END, NULL ) );


	// After adding to the vector our struct start filling it with actual values
	XML::Parser kParser;

	kParser.loadData( "./xml/config.xml" );
	XML::Node *kDocument = kParser.parseDocument();
	XML::Node *kConfig = kDocument->findNode( "CONFIG" );

	if ( !kConfig )
	{
		THROWEXCEPTION( "Could not find configuration node." );

		return false;
	}

	XML::Node *kValue = NULL, *kSection = NULL;
	// Read all values from config.xml
	for ( int i = 0; m_vParserInfo[i].iType != IS_END; i++ )
	{
		const ParserData &Data = m_vParserInfo[i];

		if ( Data.iType == IS_SECTION )
		{
			kSection = kConfig->findNode( Data.sName );
		}
		else
		{
			// If no section is loaded get from <config>
			kValue = kSection != NULL ? kSection->findNode( Data.sName ) : kConfig->findNode( Data.sName );

			if ( kValue != NULL )
			{
				if ( Data.iType == IS_BOOL )
				{
					*reinterpret_cast<bool *>( Data.pData ) = kValue->asBool();
				}
				else if ( Data.iType == IS_INTEGER )
				{
					*reinterpret_cast<int *>( Data.pData ) = kValue->asInteger();
				}
				else if ( Data.iType == IS_STRING )
				{
					*reinterpret_cast<std::string *>( Data.pData ) = kValue->asString();
				}
			}
		}
	}


	// Read Fonts bit of the file
	XML::Node *kFontSet = kConfig->findNode( "FONTSET" );

	if ( kFontSet )
	{
		int idx = 0;
		XML::Node *kFontNode = NULL;
		FontInfo kFontInfo;

		while ( ( kFontNode = kFontSet->findNode( "FONT", idx++ ) ) )
		{
			if ( !kFontNode->lookupAttribute( "ID", kFontInfo.iId ) )
			{
				continue;
			}

			if ( !kFontNode->lookupAttribute( "FILE", kFontInfo.sFileName ) )
			{
				continue;
			}

			if ( !kFontNode->lookupAttribute( "NAME", kFontInfo.sFontName ) )
			{
				continue;
			}

			if ( !kFontNode->lookupAttribute( "SIZE", kFontInfo.iSize ) )
			{
				continue;
			}

			if ( !kFontNode->lookupAttribute( "HUE",  kFontInfo.iHue ) )
			{
				continue;
			}

			FontManager::GetInstance()->AddTTFFont( kFontInfo.iId, kFontInfo.sFileName, kFontInfo.sFontName, kFontInfo.iSize, kFontInfo.iHue );
		}
	}


	// Depth Buffer
	if ( m_iDepthBuffer <= 0 )
	{
		m_iDepthBuffer = 16;
	}

	// View Distance
	if ( m_iViewDistance < 3 )
	{
		m_iViewDistance = 3;
	}

	// Max Zoom
	if ( m_iMaxZoom <= 0 )
	{
		m_iMaxZoom = 0;
	}

	// Max Angle
	if ( m_iMaxAngle < 10 )
	{
		m_iMaxAngle = 10;
	}

	// Max Angle
	if ( m_iMaxAngle > 90 )
	{
		m_iMaxAngle = 90;
	}

	// Fix Mul path if no / at the end
	if ( m_sMulPath.size() )
	{
		char cLastChar = m_sMulPath[m_sMulPath.length() - 1];

		if ( ( cLastChar != '/' ) && ( cLastChar != '\\' ) )
		{
			m_sMulPath += "/";
		}
	}

	return true;
}


void Config::Close()
{
	m_vParserInfo.clear();
	m_vParserInfo.empty();
}


// GFX
std::string Config::GetVersion()
{
	return m_sVersion;
}


int Config::GetWidth()
{
	return m_iWidth;
}


int Config::GetHeight()
{
	return m_iHeight;
}


int Config::GetBPP()
{
	return m_iBPP;
}


int Config::GetStartFullScreen()
{
	return m_iStartFullScreen;
}


int Config::GetCursor()
{
	return m_iCursor;
}


int Config::GetPerspective()
{
	return m_iPerspective;
}


int Config::GetDepthBuffer()
{
	return m_iDepthBuffer;
}


int Config::GetMaxZoom()
{
	return m_iMaxZoom;
}


int Config::GetMaxAngle()
{
	return m_iMaxAngle;
}


int Config::GetViewDistance()
{
	return m_iViewDistance;
}


int Config::GetBrightness()
{
	return m_iBrightness;
}


int Config::GetWidth2D()
{
	return m_iWidth2D;
}


int Config::GetHeight2D()
{
	return m_iHeight2D;
}


int Config::GetFirstPerson()
{
	return m_iFirstPerson;
}


int Config::GetHideSelf()
{
	return m_iHideSelf;
}


int Config::GetRoofFade()
{
	return m_iRoofFade;
}


// UO
int Config::GetStartX()
{
	return m_iStartX;
}


int Config::GetStartY()
{
	return m_iStartY;
}


int Config::GetStartZ()
{
	return m_iStartZ;
}


int Config::GetAOS()
{
	return m_iAOS;
}


std::string Config::GetMulPath()
{
	return m_sMulPath;
}


std::string Config::GetCompressedMap()
{
	return m_sCompressedMap;
}


int Config::GetPopup()
{
	return m_iPopup;
}


std::string Config::GetClientVersion()
{
	return m_sClientVersion;
}


int Config::GetClilocs()
{
	return m_iClilocs;
}


std::string Config::GetClilocLang()
{
	return m_sClilocLang;
}


int Config::GetSpeechHue()
{
	return m_iSpeechHue;
}


int Config::GetAOSToolTips()
{
	return m_iAOSToolTips;
}

//Artix: speech.mul config option
int Config::GetUseSpeech()
{
	return m_iSpeech;
}

// Net
std::string Config::GetServer()
{
	return m_sServer;
}


std::string Config::GetLogin()
{
	return m_sLogin;
}


std::string Config::GetPassword()
{
	return m_sPassword;
}


int Config::GetServerPort()
{
	return m_iServerPort;
}


int Config::GetRoofFadeTime()
{
	return m_iRoofFadeTime;
}


int Config::GetRoofFadeAlpha()
{
	return m_iRoofFadeAlpha;
}


int Config::GetIsSphere()
{
	return m_iIsSphere;
}


int Config::GetIsSphere55R()
{
	return m_iIsSphere55R;
}


int Config::GetIsPol()
{
	return m_iIsPol;
}


int Config::GetIsRunUO()
{
	return m_iIsRunUO;
}


int Config::GetIsUox3()
{
	return m_iIsUox3;
}


int Config::GetClientKey()
{
	return m_iClientKey;
}


// Sound
int Config::GetMusic()
{
	return m_iMusic;
}


int Config::GetSound()
{
	return m_iSound;
}


int Config::GetFrequency()
{
	return m_iFrequency;
}


int Config::GetStereo()
{
	return m_iStereo;
}


int Config::GetChunkSize()
{
	return m_iChunkSize;
}


int Config::GetMusicVolume()
{
	return m_iMusicVolume;
}


int Config::GetSoundVolume()
{
	return m_iSoundVolume;
}


int Config::GetMP3()
{
	return m_iMP3;
}


int Config::GetFootSteps()
{
	return m_iFootSteps;
}


// Iris Client
std::string Config::GetScriptPath()
{
	return m_sScriptPath;
}


int Config::GetMouseMotionTimer()
{
	return m_iMouseMotionTimer;
}


void Config::SetCursor( int iCursor )
{
	m_iCursor = iCursor;
}


void Config::SetBrightness( int iBrightness )
{
	m_iBrightness = iBrightness;
}


void Config::SetFirstPerson( int iFirstPerson )
{
	m_iFirstPerson = iFirstPerson;
}


void Config::SetHideSelf( int iHideSelf )
{
	m_iHideSelf = iHideSelf;
}


void Config::SetLogin( std::string sLogin )
{
	m_sLogin = sLogin;
}


void Config::SetMusic( int iMusic )
{
	m_iMusic = iMusic;
}


void Config::SetPassword( std::string sPassword )
{
	m_sPassword = sPassword;
}


void Config::SetPerspective( int iPerspective )
{
	m_iPerspective = iPerspective;
}


void Config::SetRoofFade( int iRoofFade )
{
	m_iRoofFade = iRoofFade;
}


void Config::SetRoofFadeTime( int iRoofFadeTime )
{
	m_iRoofFadeTime = iRoofFadeTime;
}


void Config::SetServer( std::string sServer )
{
	m_sServer = sServer;
}


void Config::SetSound( int iSound )
{
	m_iSound = iSound;
}


void Config::SetSpeechHue( int iSpeechHue )
{
	m_iSpeechHue = iSpeechHue;
}


void Config::SetClilocs( int iClilocs )
{
	m_iClilocs = iClilocs;
}

