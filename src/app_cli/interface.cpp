/***************************************************************************
 *   Copyright (C) 2004 by Tudor Marian Pristavu                           *
 *   clapautiusAtGmailDotCom                                               *
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
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
  
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "xfcapp.h"
#include "interface.h"
#include "xfclib.h"

using namespace std;

std::string gPrompt=">";

extern XfcLogger gcLog;

typedef enum {
    eStInitial,
    eStParam,
    eStSpace,
    eStString,
    eStEscape
} States;


vector<string> getCommand()
{
    vector<string> cmd;
    std::string str;
    char *pPtr=readline("$");
    States state=eStInitial, prevState=eStInitial;
    for (unsigned int i=0; pPtr[i]; i++) {
        switch(state) {
        case eStInitial:
        case eStParam:
            if ('"'==pPtr[i]) {
                state=eStString;
            }
            else if (isspace(pPtr[i])) {
                if (!str.empty()) {
                    cmd.push_back(str);
                    str.clear();
                    state=eStSpace;
                }
            }
            else if ('\\'==pPtr[i]) {
                state=eStEscape;
                prevState=eStParam;
            }
            else {
                state=eStParam;
                str+=pPtr[i];
            }
            break;
        case eStSpace:
            if ('"'==pPtr[i]) {
                state=eStString;
            }
            else if(isspace(pPtr[i]))
                ;
            else if ('\\'==pPtr[i]) {
                state=eStEscape;
                prevState=eStParam;
            }
            else {
                state=eStParam;
                str+=pPtr[i];
            }
            break;
        case eStEscape:
            str+=pPtr[i];
            state=prevState;
            break;
        case eStString:
            if ('"'==pPtr[i]) {
                state=eStParam;
            }
            else if ('\\'==pPtr[i]) {
                state=eStEscape;
                prevState=eStString;
            }
            else {
                str+=pPtr[i];
            }
            break;
        } // end switch
    }
    // finished
    if (!str.empty()) {
        cmd.push_back(str);
    }
    add_history(pPtr);
    free(pPtr);

    gcLog<<xfcInfo<<"new user command: ";
    for (unsigned int i=0; i<cmd.size(); i++) {
        if (0!=i)
            gcLog<<", ";
        gcLog<<cmd[i];
    }
    gcLog<<eol;
    return cmd;
}
    
    
void displayError(std::string lS)
{
    cout<<lS.c_str()<<endl;
} 


void displayError(std::string lS, std::string lS2)
{
    cout<<lS.c_str()<<lS2.c_str()<<endl;
} 


void displayMessage(std::string lMsg1, std::string lMsg2, 
                    std::string lMsg3, std::string lMsg4)
{
    cout<<lMsg1.c_str();
    if(lMsg2!="")
        cout<<lMsg2.c_str();
    if(lMsg3!="")
        cout<<lMsg3.c_str();
    if(lMsg4!="")
        cout<<lMsg4.c_str();
    cout<<endl;
}

