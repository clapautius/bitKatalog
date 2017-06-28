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
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#include "misc.h"
#include "xfclib.h"
#include "xfc.h"
#include "fs.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;
using std::ostringstream;

/**
 * helper function
 *
 * @return true if the process has ended and everything in ok, false on waitpid
 * error
 *
 * @param[in] pAbortFlag - pointer to a bool var. that should signal the
 * abortion of the operation. May be NULL.
 *
 **/
bool
waitChildProcess(pid_t process, volatile const bool *pAbortFlag, int &status, int inputFd, string &cmdOutput)
{
    int rc, len;
    char buf[100];
    bool killed=false;
    fd_set set;
    struct timeval tv;
    tv.tv_sec=1;
    tv.tv_usec=0;

    while (true) {
        // read command output
        // :fixme: if less than 99 bytes are available, the operation would block?
        FD_ZERO(&set);
        FD_SET(inputFd, &set);
        rc=select(inputFd+1, &set, NULL, NULL, &tv);
        if (rc>0) {
            if ((len=read(inputFd, buf, 99))>0) {
                buf[len]=0;
                cmdOutput+=buf;
            }
        }

        // check if child is still alive
        rc=waitpid(process, &status, WNOHANG);
        if (process == rc) { // finished
            gLog<<xfcDebug<<__FUNCTION__<<": waitpid returned "<<rc<<". status="<<status<<eol;
            // read the rest of the output
            while ((len=read(inputFd, buf, 99))>0) {
                buf[len]=0;
                cmdOutput+=buf;
            }
            return true;
        }
        else if (0 == rc) { // not finished
            if (pAbortFlag && *pAbortFlag) {
                gLog<<xfcDebug<<__FUNCTION__<<": abort flag is on"<<eol;
                cout.flush();
                if (!killed) {
                    gLog<<xfcInfo<<__FUNCTION__<<": Abort flag is on, killing process, pid="<<process<<eol;
                    cout.flush();
                    kill(process, 15);
                    killed=true;
                }
            }
        }
        else { // error
            gLog<<xfcDebug<<__FUNCTION__<<": waitpid returned "<<rc<<eol;
            return false;
        }
    }
}


std::string
executeCommand(
    const char *lpPrg, volatile const bool *pAbortFlag, const char *lpArg1,
    const char *lpArg2, const char *lpArg3, const char *lpArg4)
    throw (std::string)
{
    pid_t prPid;
    int fds[2];
    std::string output;

    gLog<<xfcInfo<<"executing external command: "<<lpPrg<<eol<<"params: ";
    if(lpArg1) gLog<<lpArg1;
    if(lpArg2) gLog<<", "<<lpArg2;
    if(lpArg3) gLog<<", "<<lpArg3;
    if(lpArg4) gLog<<", "<<lpArg4;
    gLog<<eol;
    
    if (pipe(fds)==-1)
        throw std::string("executeCommand(): pipe error");
    prPid=fork();
    if (prPid==-1) {
        close(fds[0]);
        close(fds[1]);
        throw std::string("executeCommand(): fork error");
    }
    if (prPid==0) { // child
        close(fds[0]);
        dup2(fds[1], 1);
        execlp(lpPrg, lpPrg, lpArg1, lpArg2, lpArg3, lpArg3, lpArg4, NULL);
        close(fds[1]);
        cerr<<endl<<endl<<"Error executing command: "<<lpPrg<<endl<<endl;
        exit(1);
    }
    else { // parent
        int status;
        close(fds[1]);
        if (!waitChildProcess(prPid, pAbortFlag, status, fds[0], output)) {
            close(fds[0]);
            throw std::string("waitpid() error");
        }
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status)!=0) {
                close(fds[0]);
                throw std::string("Error executing external command: ")+lpPrg;
            }
        }
        else {
            close(fds[0]);
            throw std::string("Error executing external command (2): ")+lpPrg;
        }
        close(fds[0]);
        gLog<<xfcInfo<<__FUNCTION__<<": command output: "<<output<<eol;
        return output;
    }
}


std::string
execChecksum(std::string lPath, std::string lShaProgramPath, volatile const bool *pAbortFlag)
    throw (std::string)
{
    std::string lSum;
    lSum=executeCommand(lShaProgramPath.c_str(), pAbortFlag, lPath.c_str());
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


/**
 * @retval 0 - ok
 * @retval -1 - backup error
 * @retval -2 - xml save error
 * @retval -3 - other error
 **/
int
saveWithBackup(Xfc *pXfcCat, std::string path, std::string &error)
{
    int rc=0;
    string name;
    char pathBuf[PATH_MAX+1];
    string realPath;
    if (fileExists(path)) {
        realPath=realpath(path.c_str(), pathBuf);
        if (realPath.empty()) {
            error=strerror(errno);
            return -3;
        }
        if (fileExists(realPath)) {
            name=realPath+"~";
            if (copyFile(realPath, name)!=0) {
                rc=-1;
                error="copy error";
            }
        }
    }
    else {
        realPath=path;
    }
    if (0==rc) {
        try {
            pXfcCat->saveToFile(realPath, 1);
        }
        catch(std::string e) {
            error=e;
            rc=-2;
        }
    }
    return rc;
}


string sizeToHumanReadableSize(string sizeInBytes)
{
    unsigned long int sizeNum;
    char *pRet=NULL;
    ostringstream strm;
    sizeNum=strtol(sizeInBytes.c_str(), &pRet, 10);
    if (pRet==sizeInBytes.c_str()) {
        strm<<"Invalid size";
    }
    else if (sizeNum<1024) {
        strm<<sizeNum<<" B";
    }
    else {
        if (sizeNum>=1024 && sizeNum<1024*1024) {
            strm<<std::fixed<<std::setprecision(1)<<(float)(sizeNum/1024);
            strm<<" KB";
        }
        else {
            strm<<std::fixed<<std::setprecision(1)<<(float)(sizeNum/1024/1024);
            strm<<" MB";
        }
        strm<<" ("<<sizeNum<<" B)";
    }
    return strm.str();
}


string
vectorWStringsToString(vector<string>& vect, const char *pDelim)
{
    string str;
    for (uint i=0; i<vect.size(); i++) {
        if (i>0) {
            str+=pDelim;
        }
        str+=vect[i];
    }
    return str;
}


bool
contains(vector<string> vect, string elt)
{
    for (uint i=0; i<vect.size(); i++)
        if (vect[i]==elt)
            return true;
    return false;
}


long int
stringToInt(std::string str) throw (std::string)
{
    long int sizeNum;
    char *pRet=NULL;
    sizeNum=strtol(str.c_str(), &pRet, 10);
    if (pRet==str.c_str()) {
        throw std::string("Cannot convert string to unsigned long int. String=")+str;
    }
    return sizeNum;
}
