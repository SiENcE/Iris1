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

#include "iris_endian.h"
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "Config.h"
#include "loaders/SoundLoader.h"
#include <string>




// Compiler directives
#if  	defined(WIN32) && !defined(__MINGW32__)
// Visual C pragma
#pragma pack(1)
#define STRUCT_PACKED
#else
// GCC packed attribute
#define STRUCT_PACKED	__attribute__ ((packed))
#endif

struct sWaveHeader
{
  Uint8 RIFF_ID[4];
  Uint32 len;
  Uint8 WAVE_ID[4];

  Uint8 chunkID[4];
  Uint32 chunkSize;

  Sint16 wFormatTag;
  Uint16 wChannels;
  Uint32 dwSamplesPerSec;
  Uint32 dwAvgBytesPerSec;
  Uint16 wBlockAlign;
  Uint16 wBitsPerSample;

  Uint8 datachunkID[4];
  Uint32 datachunkSize;
  Uint32 data;
} __STRUCT_PACKED;

#ifdef WIN32
#pragma pack()
#endif

SoundLoader::SoundLoader ()
{
}

SoundLoader::~SoundLoader ()
{
  SoundFile.close ();
}

int SoundLoader::SoundInit (std::string mulPath)
{
  std::string indexpath = mulPath;
  indexpath += "soundidx.mul";

  IndexFile.open (indexpath.c_str (), std::ios::in | std::ios::binary);
  if (!IndexFile.is_open ())
      {
        Logger::WriteLine ("Couldn't open soundidx.mul in SoundLoader::SoundInit();",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return false;
        IndexFile.close ();
      }
  else
      {
        IndexFile.seekg (0, std::ios::end);
        indx = IndexFile.tellg () / 12;
        IndexFile.seekg (0, std::ios::beg);

        // Load the lookup table for sound effects
        for (int y = 0; y < indx; y++)
            {
              stSoundIndx Soundidx;
              IndexFile.read ((char *) &Soundidx.lookup, 4);
              IndexFile.read ((char *) &Soundidx.length, 4);
              IndexFile.read ((char *) &Soundidx.extra, 4);
              Soundidx.lookup = IRIS_SwapI32 (Soundidx.lookup);
              Soundidx.length = IRIS_SwapI32 (Soundidx.length);
              Soundidx.extra = IRIS_SwapI32 (Soundidx.extra);
              Soundindx.push_back (Soundidx);
            }
      }
  IndexFile.close ();

  std::string soundpath = mulPath;
  soundpath += "sound.mul";
  SoundFile.open (soundpath.c_str (), std::ios::in | std::ios::binary);

  if (!SoundFile.is_open ())
      {
        Logger::WriteLine ("Couldn't open sound.mul in SoundLoader::SoundInit();",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return false;
        SoundFile.close ();
      }
  else
    return true;
}

Mix_Chunk *SoundLoader::Load (int index)
{
  Mix_Chunk *wave_chunk;
  wave_chunk = NULL;

  if (index < 0 || index > indx)
      {
        //Logger::WriteLine( "Error client tried to load a nonexisting sound in SoundLoader::SoundLoad();", __FILE__, __LINE__, LEVEL_ERROR );
        wave_chunk = NULL;
        return wave_chunk;
      };

  //printf ("Playing sound %i\n", index);
  // ENDIANNESS ? (prolly not) - I have no idea ;) - maybe you have to take a look at it :)
  char fileName[16];
  char header[16];
  int datalen = Soundindx[index].length - 32;
  sWaveHeader *waveData =
    (sWaveHeader *) (new Uint8[datalen + sizeof (sWaveHeader)]);

  SoundFile.seekg (Soundindx[index].lookup, std::ios::beg);

  SoundFile.read ((char *) fileName, 16);
  SoundFile.read ((char *) header, 16);
  SoundFile.read ((char *) &waveData->data, (datalen));

  char RIFFstr[] = "RIFF";
  char WAVEstr[] = "WAVE";
  char fmtstr[] = "fmt ";
  char datastr[] = "data";
  memcpy (&waveData->RIFF_ID, RIFFstr, 4);
  memcpy (&waveData->WAVE_ID, WAVEstr, 4);
  memcpy (&waveData->chunkID, fmtstr, 4);
  memcpy (&waveData->datachunkID, datastr, 4);
  waveData->len = datalen + 36;
  waveData->chunkSize = 16;     // fmt size
  waveData->datachunkSize = datalen;
  waveData->wFormatTag = 1;
  waveData->wChannels = 1;
  waveData->dwSamplesPerSec = 22050;
  waveData->dwAvgBytesPerSec = 88200;
  waveData->wBlockAlign = 4;
  waveData->wBitsPerSample = 16;

//        printf("Len: %s %i\n", fileName, Soundindx[index].length);

/*        FILE * f = fopen ("test.wav", "wb");
        fwrite (waveData, 1, datalen  + sizeof (sWaveHeader), f);
        fclose (f);*/

  if (!
      (wave_chunk =
       Mix_LoadWAV_RW (SDL_RWFromMem
                       (waveData, datalen + sizeof (sWaveHeader)), 1)))
      {
        printf ("Mix_LoadWAV_RW: %s\n", Mix_GetError ());
        // handle error
      }

  /*if (!(wave_chunk=Mix_QuickLoad_RAW(waveData, Soundindx[index].length - 32)))
     {
     printf("Mix_QuickLoad_RAW: %s\n", Mix_GetError());
     // handle error
     } */

  //delete [] waveData;
  //waveData = NULL;
  return wave_chunk;
}
