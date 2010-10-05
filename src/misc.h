/***************************************************************************
 *   Copyright (C) 2004 by Tudor Marian Pristavu                           *
 *   tudor.mp@home.ro                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#if !defined(MISC_H)
#define MISC_H

#include <vector>

#include "xfcapp.h"

class Xfc;


std::string executeCommand(
    const char *lpPrg, volatile const bool*,
    const char *lpArg1=NULL, const char *lpArg2=NULL,
    const char *lpArg3=NULL, const char *lpArg4=NULL)
throw (std::string);

std::string execChecksum(std::string lPath, std::string lShaProgramPath,
                         volatile const bool*)
throw (std::string);  

std::string getTimeSinceMidnight();

int saveWithBackup(Xfc*, std::string path, std::string &error);

std::string sizeToHumanReadableSize(std::string sizeInBytes);

std::string vectorWStringsToString(std::vector<std::string>& vect,
                                   const char* pDelim=", ");

bool
contains(std::vector<std::string> vect, std::string elt);

#endif
