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

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <vector>

typedef off64_t FileSizeType;

typedef struct stat64 FileStatType;

FileStatType getStatOfFile(std::string path) throw(std::string);

bool isDirectory(std::string path) throw(std::string);

bool isRegularFile(std::string path) throw(std::string);

bool isSymlink(std::string path) throw(std::string);

std::vector<std::string> getFileListInDir(std::string path) throw(std::string);

std::string getLastComponentOfPath(std::string path);

FileSizeType getFileSize(std::string path) throw(std::string);

bool fileExists(std::string path);

void rename(std::string lOldFile, std::string lNewFile) throw(std::string);

/**
 * @retval 0 - ok
 * @retval -1 - I/O error at reading
 * @retval -2 - I/O error at writing
 * @retval -3 - dest. file exists
 * @retval -4 - unknown error
 **/
int copyFile(std::string src, std::string dest);

std::vector<std::string> tokenizePath(std::string path);
// input: a path
// output: a vector with tokens extracted from that path

#endif
