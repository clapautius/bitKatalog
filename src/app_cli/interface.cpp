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
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
  
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "interface.h"

using namespace std;

std::string gPrompt=">";

std::vector<std::string> getCommand()
{
    /*
    char lAux[1024];
    std::vector<std::string> lCmd;
    std::istringstream lStream;
    cout<<gPrompt.c_str();
    cin.getline(lAux,1022);
    lStream.str(lAux);
    while(!lStream.eof())
    {
        std::string lS;
        lStream>>lS;
        lCmd.push_back(lS);
    }
    */
    std::vector<std::string> lCmd;
    std::istringstream lStream;
    std::string lS;
    char *lpPtr=readline("$");
    add_history(lpPtr);
    lStream.str(lpPtr);
    while(!lStream.eof())
    {
        lStream>>lS;
        lCmd.push_back(lS);
    }

    free(lpPtr);
    return lCmd;
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

