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
// File system stuff

#if !defined(FS_H)
#define FS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vector>
#include <string>

typedef off64_t FileSizeType;

typedef struct stat64 FileStatType;

FileStatType getStatOfFile(std::string lPath) throw (std::string);

bool isDirectory(std::string lPath) throw (std::string);

bool isRegularFile(std::string lPath) throw (std::string);

bool isSymlink(std::string lPath) throw (std::string);

std::vector<std::string> getFileListInDir(std::string lPath) throw (std::string);

std::string getLastComponentOfPath(std::string lPath);

FileSizeType getFileSize(std::string lPath) throw (std::string);

bool fileExists(std::string lPath);

void rename(std::string lOldFile, std::string lNewFile)
        throw (std::string);

std::vector<std::string> tokenizePath(std::string lPath);
// input: a path
// output: a vector with tokens extracted from that path
  
#endif
