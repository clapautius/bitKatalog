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
#include "plugins.h"

#include <fstream>
#include <sstream>

#if defined(XFC_DEBUG)
#include <iostream>
using namespace std;
#endif

#include "fs.h"
#include "xfc.h"


void importFdbbat(std::string lPathToFile, Xfc &lrXfc)
    throw (std::string)
{
    std::ifstream lFin;
    char lAux[1024];
    std::string lLine;
    std::string lCmd;
    std::string lNumber;
    std::string lPath;
    std::ostringstream lOutStream;
    std::istringstream lInStream;
    unsigned int lInt, lInt2;
    std::string lDisk;
    std::vector<std::string> lTokens;
    std::string lS;
    xmlNodePtr lpNode, lpParentNode;

    std::string lStr1, lStr2, lStr3, lStr4, lStr5, lStr6;
    std::string lFileType;
    std::string lFileSize;
    std::string lFileSum;

    if(lrXfc.getState()!=1 && lrXfc.getState()!=2)
        throw std::string("No catalog loaded");
    
    lFin.open(lPathToFile.c_str());
    if(!lFin.good())
    {
        throw std::string("Error opening fdbbat file");
    }
    
    int lLinePos=0;
    while(!lFin.eof())
    {
        lFin.getline(lAux, 1022);
#if defined(XFC_DEBUG)
        cout<<":debug: line read: "<<lAux<<endl;
#endif 
/*
        if(lAux[strlen(lAux)-1]!='\n') // doesnt end with '\n' ? - strange - line too long
        {
            throw std::string("Line too long in fdbbat file");
        }
        else
        {
            lAux[strlen(lAux)-1]=0; // remove '\n'
        }
       */ 
        lLine=lAux;
        if(lLine=="") // empty line - ignore
           continue; 
        if(lLinePos%2==0) // should be a 'addNode' line
        {
            unsigned int lSpc1, lSpc2;
            lSpc1=lLine.find_first_of(' ');
            lSpc2=lLine.substr(lSpc1+1).find_first_of(' ')+lSpc1+1;
            if(lSpc1==std::string::npos || lSpc2==std::string::npos)
            {
                lOutStream.str("");
                lOutStream<<"Invalid line in fdbbat, line: "<<lLinePos;
                throw lOutStream.str();
            }
            lCmd=lLine.substr(0, lSpc1);
            if(lCmd!="addNode")
            {
                lOutStream.str("");
                lOutStream<<"Invalid line in fdbbat, line: "<<lLinePos<<", 'addNode' expected";
                throw lOutStream.str();
            }
            lNumber=lLine.substr(lSpc1+1, lSpc2-lSpc1-1);
            if(lNumber!="1")
            {
                lOutStream.str("");
                lOutStream<<"Invalid line in fdbbat, line: "<<lLinePos<<", '1' expected";
                throw lOutStream.str();
            }                
            lPath=lLine.substr(lSpc2+1);
            lPath=lPath.substr(lPath.find_first_of('/')+1); // skip first '/'
            int lFound=0;
            for(unsigned short i=0;i<lPath.size();i++)
            {
                if(lPath[i]=='/')
                {
                    if(lFound==0)
                    {
                        lInt=i;
                        lFound++;
                    }
                    else if(lFound==1)
                    {
                        lInt2=i;
                        break;
                    }
                }
            }
            lTokens=tokenizePath(lPath);
            lDisk=lTokens[1];
            lPath=lPath.substr(lInt2); // keep initial '/'
            
            if((lpParentNode=lrXfc.getNodeForPath(lDisk))==NULL) // no such disk
            {
#if defined(XFC_DEBUG)
                cout<<":debug: adding new disk: "<<lDisk.c_str()<<endl;
#endif                            
                lrXfc.addNewDiskToXmlTree(lDisk.substr(lDisk.find_first_of('/')+1));
                lpParentNode=lrXfc.getNodeForPath(lDisk);
            }
            
            // is this necesarry ?
            /*
            lS=lDisk;
            for(int i=2;i<lTokens.size()-1;i++)
            {
                lS+="/";
                lS+=lTokens[i];
                lpNode=lrXfc.getNodeForPath(lS);
                if(lpNode==NULL) // we have to add it
                {
                        lpNode=xmlNewTextChild(lpParentNode , NULL, (xmlChar*)"file", NULL); // :bug:
                        if(lpNode==NULL)
                            throw std::string("fdbbat plugin: xmlNewTextChild() error");    
   
                        // name
                        xmlNewTextChild(lpNode, NULL, (xmlChar*)"name", (xmlChar*)lTokens[i].c_str()); // :bug:
    
                        // type
                        xmlNewTextChild(lpNode, NULL, (xmlChar*)"type", (xmlChar*)"dir");
                }
                lpParentNode=lpNode;
            }
            */
            // end is this necesarry ?
                
#if defined(XFC_DEBUG)
            cout<<":debug: new file to add: "<<lPath.c_str()<<" , line: "<<lLinePos<<endl;
#endif                            
        }
        else // shoud be a entity line
        {
            lInStream.str(lLine);
            lInStream>>lStr1>>lStr2>>lStr3>>lStr4>>lStr5>>lStr6;
            lInStream.clear();
            if(lStr1!="<")
            {
                lOutStream.str("");
                lOutStream<<"'<' expected, line: "<<lLinePos;
                throw lOutStream.str();
            }
            if(lStr2!="tdbEntity")
            {
                lOutStream.str("");
                lOutStream<<"'tdbEntity' expected, line: "<<lLinePos;
                throw lOutStream.str();
            }
            lInt=lStr3.find_first_of('"');
            if(lInt==std::string::npos)
                throw std::string("npos at the wrong time");
            lInt2=lStr3.find_last_of('"');
            if(lInt2==std::string::npos)
                throw std::string("npos at the wrong time");
            if(lStr3.substr(0, lInt+1)!="type=\"")
            {
                lOutStream.str("");
                lOutStream<<"'type=' expected, line: "<<lLinePos;
                throw lOutStream.str();
            }
            lFileType=lStr3.substr(lInt+1, lInt2-1-lInt);
#if defined(XFC_DEBUG)
            cout<<":debug: type="<<lFileType.c_str()<<endl;
#endif
            std::string lBasePath=lPath.substr(0, lPath.find_last_of('/'));
            if(lFileType=="dir") // is a directory
            {                                              
#if defined(XFC_DEBUG)
                cout<<":debug: adding dir"<<endl;            
#endif 
                lpParentNode=lrXfc.getNodeForPath(std::string(lDisk+lBasePath));
                if(lpParentNode==NULL) // we have to add it
                {
                    throw std::string("An element that should be there is missing");
                }
                lpNode=xmlNewTextChild(lpParentNode , NULL, (xmlChar*)"file", NULL); // :bug:
                if(lpNode==NULL)
                    throw std::string("fdbbat plugin: xmlNewTextChild() error");    
   
                // name
                xmlNewTextChild(lpNode, NULL, (xmlChar*)"name", 
                    (xmlChar*)lPath.substr(lPath.find_last_of('/')+1).c_str()); // :bug:

                // type
                xmlNewTextChild(lpNode, NULL, (xmlChar*)"type", (xmlChar*)"dir");
            }
            else if(lFileType=="file") // is a file
            {
                lInt=lStr4.find_first_of('"');
                if(lInt==std::string::npos)
                    throw std::string("npos at the wrong time");
                lInt2=lStr4.find_last_of('"');
                if(lInt2==std::string::npos)
                    throw std::string("npos at the wrong time");

                if(lStr4.substr(0, lInt+1)!="size=\"")
                {
                    lOutStream.str("");
                    lOutStream<<"'size=' expected, line: "<<lLinePos;
                    throw lOutStream.str();
                }
                lFileSize=lStr4.substr(lInt+1, lInt2-1-lInt);

                lInt=lStr6.find_first_of('"');
                if(lInt==std::string::npos)
                    throw std::string("npos at the wrong time");
                lInt2=lStr6.find_last_of('"');
                if(lInt2==std::string::npos)
                    throw std::string("npos at the wrong time");

                if(lStr6.substr(0, lInt+1)!="sha1=\"")
                {
                    lOutStream.str("");
                    lOutStream<<"'sha1=' expected, line: "<<lLinePos;
                    throw lOutStream.str();
                }
                lFileSum=lStr6.substr(lInt+1, lInt2-1-lInt);
                
#if defined(XFC_DEBUG)
                cout<<":debug: adding file"<<endl;            
#endif 
                lpParentNode=lrXfc.getNodeForPath(std::string(lDisk+lBasePath));
                if(lpParentNode==NULL) // we have to add it
                {
                    throw std::string("An element that should be there is missing");
                }
                lpNode=xmlNewTextChild(lpParentNode , NULL, (xmlChar*)"file", NULL); // :bug:
                if(lpNode==NULL)
                    throw std::string("fdbbat plugin: xmlNewTextChild() error");    
   
                // name
                xmlNewTextChild(lpNode, NULL, (xmlChar*)"name",
                               (xmlChar*)lPath.substr(lPath.find_last_of('/')+1).c_str()); // :bug:

                // type
                xmlNewTextChild(lpNode, NULL, (xmlChar*)"type", (xmlChar*)"file");
                
                // size
                xmlNewTextChild(lpNode, NULL, (xmlChar*)"size", (xmlChar*)lFileSize.c_str()); // :bug:

                // sum
                lpNode=xmlNewTextChild(lpNode, NULL, (xmlChar*)"sum", (xmlChar*)lFileSum.c_str()); // :bug:
                xmlNewProp(lpNode, (xmlChar*)"type", (xmlChar*)"sha1");
            }           
        }
        lLinePos++;
    } // end while
    lFin.close();
} 
