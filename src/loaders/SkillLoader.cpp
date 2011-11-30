//
// File: SkillLoader.cpp
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


#include "iris_endian.h"
#include "loaders/SkillLoader.h"
#include "Logger.h"
#include "Exception.h"
#include "uotype.h"
#include <string.h>
#include <iostream>




using namespace std;

cSkillLoader pSkillLoader;

std::string cSkillEntry::name ()
{
    return m_name;
}

unsigned char cSkillEntry::use_button ()
{
    return m_use_button;
}

cSkillEntry::cSkillEntry (std::string name, unsigned char use_button)
{
    m_name =name;
    m_use_button = use_button;
}

cSkillLoader::cSkillLoader ()
{
}

cSkillLoader::~cSkillLoader ()
{
    DeInit ();
}


void cSkillLoader::Init (std::string filename, std::string indexname)
{
  string errstr;
  ifstream *skillfile;
  ifstream *skillindex;

  DeInit ();

  skillfile = new ifstream (filename.c_str (), ios::in | ios::binary);
  skillindex = new ifstream (indexname.c_str (), ios::in | ios::binary);

  errstr = "Could not load skill file: ";
  if (!skillfile->is_open ())
      {
        errstr += filename;
        delete skillfile;
        delete skillindex;
        skillfile = NULL;
        skillindex = NULL;
        THROWEXCEPTION (errstr);
      }

  if (!skillindex->is_open ())
      {
        errstr += indexname;
        delete skillfile;
        delete skillindex;
        skillfile = NULL;
        skillindex = NULL;
        THROWEXCEPTION (errstr);
      }

  skillindex->seekg (0, ios::end);
  unsigned long idxE = skillindex->tellg ();
  int skill_count = idxE / 12;
  struct stIndexRecord indexRecord;

  skillindex->seekg (0, ios::beg);

  for (int i = 0; i < skill_count; i++)
      {
        skillindex->read ((char *) &indexRecord, 12);
        indexRecord.offset = IRIS_SwapU32 (indexRecord.offset);
        indexRecord.length = IRIS_SwapU32 (indexRecord.length);
        indexRecord.extra = IRIS_SwapU32 (indexRecord.extra);
        if ((indexRecord.offset != 0xffffffff)
            && (indexRecord.length != 0xffffffff))
            {
              char *name = (char *) malloc (indexRecord.length);
              Uint8 use_button;
              memset (name, 0, indexRecord.length);
              skillfile->seekg (indexRecord.offset, ios::beg);
              skillfile->read ((char *) &use_button, 1);
              skillfile->read (name, indexRecord.length - 1);

              cSkillEntry * entry = new cSkillEntry (name, use_button);

              skilllist.insert (make_pair ((Uint16) (i + 1), entry));

              free (name);
            }
      }

  delete skillindex;
  delete skillfile;
}

void cSkillLoader::DeInit ()
{
  std::map < Uint16, cSkillEntry * >::iterator iter;
  for (iter = skilllist.begin (); iter != skilllist.end (); iter++)
    delete iter->second;
  skilllist.clear ();
}

cSkillEntry *cSkillLoader::GetSkill (Uint16 id)
{
  std::map < Uint16, cSkillEntry * >::iterator iter;
  iter = skilllist.find (id);
  if (iter != skilllist.end ())
    return iter->second;
  return NULL;
}
