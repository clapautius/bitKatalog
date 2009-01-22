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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>


struct stat getStatOfFile(std::string lPath) throw (std::string)
{
    struct stat lStat;
    int lRet;
    lRet=lstat(lPath.c_str(), &lStat);
    if(lRet==0)
        return lStat;
    throw std::string("getStatOfFile(): stat() error for file: ")+lPath+
        ", error="+strerror(errno);
}
    

bool isDirectory(std::string lPath) throw (std::string)
{
    struct stat lStat;
    lStat=getStatOfFile(lPath);
    if(S_ISDIR(lStat.st_mode))
        return true;
    else
        return false;
}


bool isRegularFile(std::string lPath) throw (std::string)
{
    struct stat lStat;
    lStat=getStatOfFile(lPath);
    if(S_ISREG(lStat.st_mode))
        return true;
    else
        return false;
}


bool
isSymlink(std::string lPath) throw (std::string)
{
    struct stat lStat;
    lStat=getStatOfFile(lPath);
    if(S_ISLNK(lStat.st_mode))
        return true;
    else
        return false;
}


std::vector<std::string> getFileListInDir(std::string lPath) 
        throw (std::string)
{
    DIR *lpDir;
    std::vector<std::string> lFileList;
    struct dirent *lpDirEnt;
    lpDir=opendir(lPath.c_str());
    if(lpDir==NULL)
    {
        throw std::string("getFileListInDir(): error in opendir");
    }   
    lpDirEnt=readdir(lpDir); 
    while(lpDirEnt!=NULL)
    {
        if(strcmp(lpDirEnt->d_name, ".") && strcmp(lpDirEnt->d_name, ".."))
        {
            lFileList.push_back(lpDirEnt->d_name);
        }
        lpDirEnt=readdir(lpDir);
    }
    closedir(lpDir);
    return lFileList;
}


std::string getLastComponentOfPath(std::string lPath)
{
    unsigned int lPos;
    lPos=lPath.find_last_of('/');
    if(lPos==std::string::npos)
        return lPath;
    else
    {
        if(lPos<lPath.size()-1)
            return lPath.substr(lPos+1);
        else
            return std::string("");
    }
}


FileSizeT getFileSize(std::string lPath)
        throw (std::string)
{
    struct stat lStat;
    lStat=getStatOfFile(lPath);
    return lStat.st_size;
} 


bool fileExists(std::string lPath)
{
    if(access(lPath.c_str(), 0)==0)
        return true;
    else
        return false;
} 

void rename(std::string lOldFile, std::string lNewFile)
        throw (std::string)
{
    if(rename(lOldFile.c_str(), lNewFile.c_str())==0)
        return;
    else 
        throw std::string("rename(): rename error");
} 


std::vector<std::string> tokenizePath(std::string lPath)
{
    std::vector<std::string> lTokens;
    std::string lToken;
    for(unsigned int i=0;i<lPath.size();i++) 
    {
        if(lPath[i]=='/')
        {
            if(lToken!="")
            {
                lTokens.push_back(lToken);
                lToken="";
            }     
        } 
       else
           lToken+=lPath[i];
    }  
    if(lToken!="")
        lTokens.push_back(lToken);
    return lTokens;
}  
