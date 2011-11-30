//
// File: MultisLoader.cpp
// Created by: x.diablo
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

#include "iris_endian.h"
#include "Exception.h"
#include "loaders/MultisLoader.h"
#include "Logger.h"
#include "uotype.h"
#include <string.h>
#include <iostream>




cMultisLoader *pMultisLoader=NULL;

using namespace std;
cMultisLoader::cMultisLoader (std::string filename, std::string indexname)
{
  MultiCount = 0;
	for(int i=0;i<MAX_MULTIS;i++) MultiList[i].ishouse=false;

	std::ifstream * multifile = NULL;
  std::ifstream * multiindex = NULL;
  try
  {
    multifile = new ifstream (filename.c_str (), ios::in | ios::binary);
  }
  catch (...)
  {
    THROWEXCEPTION ("Out of Memory");
  }

  try
  {
    multiindex = new ifstream (indexname.c_str (), ios::in | ios::binary);
  }
  catch (...)
  {
    delete multifile;
    multifile = NULL;
    THROWEXCEPTION ("Out of Memory");
  }
  string errstr = "Could not load multis file: ";
  if (!multifile->is_open ())
  {
        errstr += filename;
        delete multifile;
        delete multiindex;
        THROWEXCEPTION (errstr);
  }

  if (!multiindex->is_open ())
  {
        errstr += indexname;
        delete multifile;
        delete multiindex;
        THROWEXCEPTION (errstr);
  }
  multiindex->seekg (0, ios::end);

  int MultiLength = multiindex->tellg () / 12;
  /*
  int realnum = 0;
  for(int i=0;i<MultiCount;i++)
  {
	  unsigned int st;
		multiindex->read((char*)&st, sizeof(int));
		if (st!=0xffffffff)
		{
			realnum++;
		}
		multiindex->seekg (8, ios::cur);
	}
	*/
	if(MultiLength>=MAX_MULTIS)
	{
    THROWEXCEPTION ("multi file too big");
	}
	multiindex->seekg (0, ios::beg);

  for(int i=0;i<MultiLength;i++)
  {
		unsigned int offset, size, dummy;
		multiindex->read((char*)&offset, sizeof(int));
		multiindex->read((char*)&size, sizeof(int));
		multiindex->read((char*)&dummy, sizeof(int));
		if (offset!=0xffffffff)
		{
			int mc = size/12;
			multifile->seekg (offset, ios::beg);
			for(int c=0;c<mc;c++)
			{
				unsigned short tile;
				short x,y,z;
				unsigned int flag;
				stMultiPart multipart;
				
				multifile->read((char*)&tile, sizeof(short));
				multifile->read((char*)&x, sizeof(short));
				multifile->read((char*)&y, sizeof(short));
				multifile->read((char*)&z, sizeof(short));
				multifile->read((char*)&flag, sizeof(int));
				multipart.tile = tile;
				multipart.x = x;
				multipart.y = y;
				multipart.z = z;
				multipart.visible = (flag == 1);
				if(!multipart.visible) continue; // invisible blocks doesn't need for client
				MultiList[i].multiParts.push_back(multipart);
			}
			MultiCount++;
		}

	}
  // old hardcoded list
  for (int i=0x64; i<=0x7f; i++) MultiList[i].ishouse=true;
  MultiList[0x87].ishouse=true;
  MultiList[0x8d].ishouse=true;
  MultiList[0x96].ishouse=true;
  MultiList[0x98].ishouse=true;
  MultiList[0x9a].ishouse=true;
  MultiList[0x9c].ishouse=true;
  MultiList[0x9e].ishouse=true;
  MultiList[0xa0].ishouse=true;
  MultiList[0xa2].ishouse=true;
  MultiList[0xbb8].ishouse=true;
  MultiList[0x1388].ishouse=true;

  delete multifile;
  delete multiindex;

}
cMultisLoader::~cMultisLoader ()
{
	//assert(multiCount>0);
//	for (int a=0; a<8196; a++)
//		if (MultiList[a].multiParts) delete[] (MultiList[a].multiParts);	
}
