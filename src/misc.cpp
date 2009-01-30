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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>

#include <string>

#include "misc.h"

#if defined(XFC_DEBUG)
#include <iostream>
using namespace std;
#endif

std::string executeCommand(const char *lpPrg, const char *lpArg1, const char *lpArg2,
                                       const char *lpArg3, const char *lpArg4) 
    throw (std::string)
{
    int lRet;
    int lFd[2];
    char lAux[256];
    std::string lOutput;
    
#if defined(XFC_DEBUG)
    cout<<"executing command: "<<lpPrg<<endl;
    cout<<"params: ";
    if(lpArg1!=NULL)
        cout<<lpArg1<<endl;
    if(lpArg2!=NULL)
        cout<<lpArg2<<endl;
    if(lpArg3!=NULL)
        cout<<lpArg3<<endl;
    if(lpArg4!=NULL)
        cout<<lpArg4<<endl;
    cout<<"end params:"<<endl;
#endif
    
    if(pipe(lFd)==-1)
        throw std::string("executeCommand(): pipe error");
    lRet=fork();
    if(lRet==-1)
    {
        close(lFd[0]);
        close(lFd[1]);
        throw std::string("executeCommand(): fork error");
    }
    if(lRet==0) // child
    {
        close(lFd[0]);
        dup2(lFd[1], 1);
        execlp(lpPrg, lpPrg, lpArg1, lpArg2, lpArg3, lpArg3, lpArg4, NULL);
        close(lFd[1]);
        throw std::string("executeCommand(): exec error");
    }
    else // parent
    {
        int lLen;
        close(lFd[1]);
        while((lLen=read(lFd[0], lAux, 255))>0)
        {
            lAux[lLen]=0;
            lOutput+=lAux;
        }
        if(lLen<0) // read error
        {
            int lInt;
#if defined(XFC_DEBUG)
            cout<<"executeCommand(): read error "<<endl;
#endif 
            if(waitpid(lRet, &lInt, 0)!=lRet)
            {
                close(lFd[0]);
                throw std::string("waitpid() error");
            }
            close(lFd[0]);
            throw std::string("executeCommand(): read error");
        }
#if defined(XFC_DEBUG)
        cout<<"executeCommand(): command output: "<<lOutput.c_str();
        cout<<endl<<"end commnad output"<<endl; 
#endif 
        int lInt;
        if(waitpid(lRet, &lInt, 0)!=lRet)
        {
            close(lFd[0]);
            throw std::string("waitpid() error");
        }
        if(WIFEXITED(lInt))
        {
            if(WEXITSTATUS(lInt)!=0)
            {
                close(lFd[0]);
                throw std::string("child returned status != 0");
            }
        }
        else
        {
            close(lFd[0]);
            throw std::string("child exited abnormally");
        }
        close(lFd[0]);
        return lOutput;
    }
}


std::string sha1sum(std::string lPath, std::string lShaProgramPath)
        throw (std::string)
{
    std::string lSum;
    lSum=executeCommand(lShaProgramPath.c_str(), lPath.c_str());
    return lSum.substr(0, lSum.find_first_of(' '));
}


std::string
getTimeSinceMidnight()
{
    struct timeval currentTime;
    unsigned long int days;
    unsigned long int todaySecs;
    char buffer[15];
    gettimeofday(&currentTime, NULL);
    days=currentTime.tv_sec/(3600*24);
    todaySecs=currentTime.tv_sec-days*3600*24;
    snprintf(buffer, 14, "%02lu:%02lu:%02lu.%03lu", todaySecs/3600,
             (todaySecs%3600)/60, todaySecs%60, currentTime.tv_usec/1000);
    return std::string(buffer);
}
