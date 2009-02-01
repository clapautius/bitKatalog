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

#include <iostream>
#include <string>

#include "misc.h"

using namespace std;


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
#if defined(XFC_DEBUG)
            cout<<__FUNCTION__<<": waitpid returned "<<rc<<". status="<<status<<endl;
#endif
            return true;
        }
        else if (0 == rc) { // not finished
            if (*pAbortFlag) {
#if defined(XFC_DEBUG)
                cout<<":debug: abort flag is on"<<endl;
                cout.flush();
#endif
                
                if (!killed) {
#if defined(XFC_DEBUG)
                    cout<<__FUNCTION__<<": Abort flag is on, killing process, pid="<<process<<ends;
                    cout.flush();
#endif
                    kill(process, 15);
                    killed=true;
                }
            }
        }
        else { // error
#if defined(XFC_DEBUG)
            cout<<__FUNCTION__<<": waitpid returned "<<rc<<endl;
#endif
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
#if defined(XFC_DEBUG)
    cout<<"executing command: "<<lpPrg<<endl<<"params: ";
    if(lpArg1) cout<<lpArg1;
    if(lpArg2) cout<<", "<<lpArg2;
    if(lpArg3) cout<<", "<<lpArg3;
    if(lpArg4) cout<<", "<<lpArg4;
    cout<<endl;
#endif
    
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
#if defined(XFC_DEBUG)
        cout<<__FUNCTION__<<": command output: "<<output.c_str();
#endif 
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
