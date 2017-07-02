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
#include "fs.h"

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>

using namespace std;

FileStatType getStatOfFile(std::string path) throw(std::string)
{
    FileStatType statVal;
    int ret;
    ret = lstat64(path.c_str(), &statVal);
    if (ret == 0)
        return statVal;
    throw std::string("getStatOfFile(): stat() error for file: ") + path +
        ", error=" + strerror(errno);
}

bool isDirectory(std::string path) throw(std::string)
{
    FileStatType statVal;
    statVal = getStatOfFile(path);
    if (S_ISDIR(statVal.st_mode))
        return true;
    else
        return false;
}

bool isRegularFile(std::string path) throw(std::string)
{
    FileStatType statVal;
    statVal = getStatOfFile(path);
    if (S_ISREG(statVal.st_mode))
        return true;
    else
        return false;
}

bool isSymlink(std::string path) throw(std::string)
{
    FileStatType statVal;
    statVal = getStatOfFile(path);
    if (S_ISLNK(statVal.st_mode))
        return true;
    else
        return false;
}

std::vector<std::string> getFileListInDir(std::string path) throw(std::string)
{
    DIR *pDir = NULL;
    std::vector<std::string> fileList;
    struct dirent *pDirEnt = NULL;
    pDir = opendir(path.c_str());
    if (pDir == NULL) {
        throw std::string("getFileListInDir(): error in opendir");
    }
    pDirEnt = readdir(pDir);
    while (pDirEnt != NULL) {
        if (strcmp(pDirEnt->d_name, ".") && strcmp(pDirEnt->d_name, "..")) {
            fileList.push_back(pDirEnt->d_name);
        }
        pDirEnt = readdir(pDir);
    }
    closedir(pDir);
    return fileList;
}

std::string getLastComponentOfPath(std::string path)
{
    size_t pos;
    pos = path.find_last_of('/');
    if (pos == std::string::npos)
        return path;
    else {
        if (pos < path.size() - 1)
            return path.substr(pos + 1);
        else
            return std::string("");
    }
}

FileSizeType getFileSize(std::string path) throw(std::string)
{
    FileStatType statVal;
    statVal = getStatOfFile(path);
    return statVal.st_size;
}

bool fileExists(std::string path)
{
    if (access(path.c_str(), 0) == 0)
        return true;
    else
        return false;
}

void rename(std::string oldFile, std::string newFile) throw(std::string)
{
    if (rename(oldFile.c_str(), newFile.c_str()) == 0)
        return;
    else
        throw std::string("rename(): rename error");
}

std::vector<std::string> tokenizePath(std::string path)
{
    std::vector<std::string> tokens;
    std::string token;
    for (unsigned int i = 0; i < path.size(); i++) {
        if (path[i] == '/') {
            if (token != "") {
                tokens.push_back(token);
                token = "";
            }
        } else
            token += path[i];
    }
    if (token != "")
        tokens.push_back(token);
    return tokens;
}

/**
 * Copy file src to dest.
 * Overwrites destination if exists.
 **/
int copyFile(string src, string dest)
{
    int rc = 0;
    unsigned long int len = 0, remaining = 0;
    ifstream fin;
    fstream fout;
    char *buf = NULL;
    fin.open(src.c_str(), ios::in | ios::binary);
    if (!fin.good()) {
        rc = -1;
    }
    if (0 == rc) {
        fout.open(dest.c_str(), ios::out);
        if (!fout.good()) {
            rc = -2;
        }
    }
    if (0 == rc) {
        fin.seekg(0, ios::end);
        len = fin.tellg();
        remaining = len;
        fin.seekg(0, ios::beg);
        buf = new char[100000];
        while (fin.good()) {
            fin.read(buf, 100000);
            fout.write(buf, fin.gcount());
            if (!fout.good()) {
                rc = -2;
                break;
            } else {
                remaining -= fin.gcount();
            }
        }
    }
    if (0 == rc && remaining > 0) {
        rc = -4;
    }
    delete[] buf;
    fin.close();
    fout.close();
    return rc;
}
