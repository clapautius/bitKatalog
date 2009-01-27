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
#include <string.h>
#include <sstream>

#include "xfcapp.h"
#include "xfc.h"
#include "xfcEntity.h"
#include "fs.h"
#include "misc.h"

#if defined(XFC_DEBUG)
#include <iostream>
using namespace std;
#endif


Xfc::Xfc()
{
    mState=0;
    mpDoc=NULL;
}


Xfc::~Xfc()
{
}


void Xfc::loadFile(std::string lPath) throw (std::string)
{
    // :fixme: - check state
    // :fixme: - check existance of file and throw a different error
    xmlDocPtr lpDoc;
    lpDoc=xmlParseFile(lPath.c_str());
    if(lpDoc==NULL)
        throw std::string("Xfc::loadFile(): - xmlParseFile error");
    mState=1;
    mpDoc=lpDoc;
}


void Xfc::saveToFile(std::string lPath, int lParams)
        throw (std::string)
{
    if(xmlSaveFormatFile(lPath.c_str(), getXmlDocPtr(), lParams)==-1)
    {
        throw std::string("Error saving file.");
    }
}


xmlDocPtr Xfc::getXmlDocPtr() throw (std::string)
{
    if(mState==1 || mState==2)
        return mpDoc;
    else
       throw std::string("Xfc::getXmlDocPtr(): - no xml doc"); 
} 


void Xfc::parseFileTree(ParserFuncType callBackFunc,
                        void *lpParam)
        throw (std::string)
{
    // :fixme: - check state
    xmlNodePtr lpChild;
    lpChild=xmlDocGetRootElement(mpDoc);
    if(lpChild==NULL) // empty document
        return;
    lpChild=mpDoc->xmlChildrenNode;
    while(lpChild!=NULL)
    {
        parseRec(0, "/", lpChild, callBackFunc, lpParam);        
        lpChild=lpChild->next;
    }
} 


void Xfc::parseDisk(ParserFuncType callBackFunc, 
                   std::string lDiskName, void *lpParam)
{
    xmlNodePtr lpChild;
    lpChild=getNodeForPath(std::string("/")+lDiskName);
    if(lpChild==NULL) // no such disk
        throw std::string("No such disk");
    parseRec(0, std::string("/")+lDiskName, lpChild, callBackFunc, lpParam);
}            


std::vector<std::string> Xfc::getDiskList()
{
    std::vector<std::string> lVect;
    std::string lS;
    xmlNodePtr lpChild;
    xmlNodePtr lpRoot;
    lpRoot=xmlDocGetRootElement(mpDoc);
    if(lpRoot==NULL) // empty document
        return lVect;
    lpChild=lpRoot->xmlChildrenNode;
    while(lpChild!=NULL)
    {
        lS=getNameOfElement(lpChild);
        lVect.push_back(lS);
#if defined(XFC_DEBUG)
        cout<<":debug: new disk: "<<lS.c_str()<<endl;
#endif 
        lpChild=lpChild->next;
    }
    return lVect;
}    


void Xfc::parseRec(unsigned int lDepth, std::string lPath, xmlNodePtr lpNode,
              ParserFuncType callBackFunc, void *lpParam)
        throw (std::string)
{
    int lRet;
    xmlNodePtr lpChild;
    std::string lS;
    lpChild=lpNode->xmlChildrenNode;
    
#if defined(XFC_DEBUG)
    cout<<":debug: parseRec(): addr="<<lpNode<<", depth="<<lDepth;
    cout<<", path="<<lPath.c_str()<<", lpNode->name="<<lpNode->name<<endl;
#endif

    while(lpChild!=NULL)
    {
#if defined(XFC_DEBUG)
        std::cout<<":debug: child addr="<<lpChild<<endl;
#endif 
        if(isFileOrDir(lpChild) || isDisk(lpChild))
        {
            lRet=callBackFunc(lDepth, lPath, *this, lpChild, lpParam);
            if(lRet==-1) // abandon
                return;
            lS=getNameOfElement(lpChild);
            if(lPath=="/")
                parseRec(lDepth+1, "/"+lS, lpChild, callBackFunc, lpParam);
            else
                parseRec(lDepth+1, lPath+"/"+lS, lpChild, callBackFunc, lpParam);
        }
        else
            parseRec(lDepth+1, lPath, lpChild, callBackFunc, lpParam);
        lpChild=lpChild->next;
    }
}


bool Xfc::isFileOrDir(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    cout<<":debug: checking if it is a file/dir: "<<lpNode->name<<endl;
#endif
    ElementType type=getTypeOfElement(lpNode);
    if (eFile==type || eDir==type)
        return true;
    else
        return false;
}


bool Xfc::isDisk(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    cout<<":debug: checking if it is a disk: "<<lpNode->name<<endl;
#endif
    if (eDisk == getTypeOfElement(lpNode))
        return true;
    else
        return false;
}


std::string Xfc::getNameOfFile(xmlNodePtr lpNode)
        const throw (std::string)
{
    if(isFileOrDir(lpNode))
        return getNameOfElement(lpNode);
    else
        throw std::string("Xfc::getNameOfDisk(): node is not a disk");
}


std::string Xfc::getNameOfDisk(xmlNodePtr lpNode)
        const throw (std::string)
{
    if(isDisk(lpNode))
       return getNameOfElement(lpNode);
    else
        throw std::string("Xfc::getNameOfDisk(): node is not a disk");
}


std::string Xfc::getNameOfElement(xmlNodePtr lpNode)
        const throw (std::string)
{
    // :fixme: - check state
    
#if defined(XFC_DEBUG)
    cout<<":debug: in getNameOfElement()"<<endl;
#endif 
    std::string lS;
    if(lpNode==NULL)
        lS=""; 
    else
    {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
#if defined(XFC_DEBUG)
        cout<<"    :debug: searching name of element"<<endl;
#endif 
        while(lNameNode!=NULL)
        {
#if defined(XFC_DEBUG)
            cout<<"    :debug: found "<<(const char*)lNameNode->name<<endl;
#endif 
            if(!strcmp((const char*)lNameNode->name, "name"))
            {
                xmlChar *lpStr;
                lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)lpStr;
                xmlFree(lpStr);
                break;
            }
            lNameNode=lNameNode->next;
        }
    }
#if defined(XFC_DEBUG)
    cout<<":debug: name is: "<<lS.c_str()<<endl;
    cout<<":debug: out of getNameOfElement()"<<endl;
#endif 
    return lS;
}


void
Xfc::setNameOfElement(xmlNodePtr pNode, std::string newName) throw (std::string)
{
  xmlNodePtr pNameNode;  
  if( (pNameNode=getNameNode(pNode)) == NULL)
    throw std::string("No name"); // :fixme: - should add it
  xmlNodeSetContent(pNameNode, (xmlChar*)newName.c_str());
  // :fixme: use xmlEncodeSpecialChars().
  // :fixme: the old content ?
}
         

Xfc::ElementType
Xfc::getTypeOfElement(xmlNodePtr lpNode)
  const throw (std::string)
{
    // :fixme: - check state
    ElementType retVal=eUnknown;
#if defined(XFC_DEBUG)
    cout<<":debug: in getTypeOfElement()"<<endl;
#endif 
    if (lpNode==NULL)
        retVal=eUnknown;
    else {
        const char *pElemName=(const char*)lpNode->name;
        if (pElemName) {
            if (!strcmp(pElemName, "disk"))
                retVal=eDisk;
            else if (!strcmp(pElemName, "file"))
                retVal=eFile;
            else if (!strcmp(pElemName, "dir"))
                retVal=eDir;
            else
                retVal=eUnknown;
        }
    }
#if defined(XFC_DEBUG)
    cout<<":debug: type is: "<<retVal<<endl;
    cout<<":debug: out of getTypeOfElement()"<<endl;
#endif 
    return retVal;
}


std::string Xfc::getDescriptionOfNode(xmlNodePtr lpNode, xmlNodePtr* lpDescriptionNode) 
        const throw (std::string)
// ret "" if no description
{
// :fixme: - check state
    
    std::string lS="";
    if(lpNode!=NULL)
    {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
        while(lNameNode!=NULL)
        {
            if(!strcmp((const char*)lNameNode->name, "description"))
            {
                xmlChar *lpStr;
                lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)lpStr;
                xmlFree(lpStr);
                if(lpDescriptionNode!=NULL)
                {
                    *lpDescriptionNode=lNameNode;
                }
                break;
            }
            lNameNode=lNameNode->next;
        }
    }
    return lS;
}    


void
Xfc::addPathToXmlTree(std::string lPath,
                      int lMaxDepth, 
                      std::string lDiskId,
                      std::string lDiskCategory,
                      std::string lDiskDescription,
                      std::string lDiskLabel,
                      bool dontComputeSha)
  throw (std::string)
{
    bool lSkipFirstLevel=false;
    if(!isDirectory(lPath) && !isRegularFile(lPath))
    {
        throw std::string("Xfc::addDirToXmlTree(): Invalid path"); 
    }
    xmlNodePtr lpDisk;
    lpDisk=getNodeForPath(std::string("/")+lDiskId);
    if(isDirectory(lPath) && lPath[lPath.size()-1]=='/') // add only the subdirs
    {
        lSkipFirstLevel=true;
    }
    if(lpDisk==NULL)
        lpDisk=addNewDiskToXmlTree(lDiskId);
    if(!isSymlink(lPath)) // :fixme: - do something with the symlinks
    {
        if(isRegularFile(lPath))
        {
            addFileToXmlTree(lpDisk, lPath, dontComputeSha);
        }
        else if(isDirectory(lPath))
        {
            recAddPathToXmlTree(lpDisk, lPath, 0, lMaxDepth, lSkipFirstLevel, dontComputeSha);
        }
    }
} 


xmlNodePtr Xfc::addNewDiskToXmlTree(std::string lDiskName,
                              std::string lDiskCategory,
                              std::string lDiskDescription,
                              std::string lDiskLabel,
                              std::string lDiskCDate)
        throw (std::string)
        // :fixme: - check for duplicates
{
    xmlNodePtr lpRoot;
    xmlNodePtr lpNode;
    lpRoot=mpDoc->xmlChildrenNode;
    lpNode=xmlNewTextChild(lpRoot, NULL, (xmlChar*)"disk", NULL); // :bug: - conversion
    if(lpNode==NULL)
        throw std::string("Xfc::addNewDiskToXmlTree(): xmlNewTextChild() error");
    
    xmlNewTextChild(lpNode, NULL, (xmlChar*)"name", (xmlChar*)lDiskName.c_str());

    if(lDiskCategory!="")
        xmlNewTextChild(lpNode, NULL, (xmlChar*)"category", (xmlChar*)lDiskCategory.c_str());
    
    if(lDiskDescription!="")
        xmlNewTextChild(lpNode, NULL, (xmlChar*)"description", (xmlChar*)lDiskDescription.c_str());

    if(lDiskCDate!="")
        xmlNewTextChild(lpNode, NULL, (xmlChar*)"cdate", (xmlChar*)lDiskCDate.c_str());
    
    if(lDiskLabel!="")
        xmlNewTextChild(lpNode, NULL, (xmlChar*)"label", (xmlChar*)lDiskLabel.c_str());
        
    return lpNode;
}


xmlNodePtr
Xfc::addFileToXmlTree(xmlNodePtr lpNode, std::string lPath,
                      bool dontComputeSha)
  throw (std::string)
{
    std::string lName=getLastComponentOfPath(lPath);
    xmlNodePtr lpNewNode;
    xmlNodePtr lpNewNewNode;

#if defined(XFC_DEBUG)
    std::cout<<__FUNCTION__<<": adding file "<<lPath<<" to xml tree"<<std::endl;
#endif
    
    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"file", NULL); // :bug:
    if(lpNewNode==NULL)
        throw std::string("Xfc::addFileToXmlTree(): xmlNewTextChild() error");
    
    // :fixme: add all the other stuff
   
    // name
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"name", (xmlChar*)lName.c_str()); // :bug:
    
    // type
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"type", (xmlChar*)"file");
    
    // size
    FileSizeT lSize=getFileSize(lPath);
    std::ostringstream lStrOut;
    lStrOut<<lSize;
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"size", (xmlChar*)lStrOut.str().c_str()); // :bug:

    if(!dontComputeSha)
    {
      // sha1
      std::string lSum=sha1sum(lPath, std::string("sha1sum"));
      lpNewNewNode=xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"sum", 
                                   (xmlChar*)lSum.c_str());
      xmlNewProp(lpNewNewNode, (xmlChar*)"type", (xmlChar*)"sha1");
    }
    
    return lpNewNode;
}    


xmlNodePtr Xfc::addDirToXmlTree(xmlNodePtr lpNode, std::string lPath)
        throw (std::string)
{
    std::string lName=getLastComponentOfPath(lPath);
    xmlNodePtr lpNewNode;
    //xmlNodePtr lpNewNewNode;

#if defined(XFC_DEBUG)
    std::cout<<__FUNCTION__<<": adding dir "<<lPath<<" to xml tree"<<std::endl;
#endif

    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"file", NULL); // :bug:
    if(lpNewNode==NULL)
        throw std::string("Xfc::addDirToXmlTree(): xmlNewTextChild() error");
    
    // :fixme: add all the other stuff
   
    // name
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"name", (xmlChar*)lName.c_str()); // :bug:
    
    // type
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"type", (xmlChar*)"dir");
       
    return lpNewNode;
}    


//std::string Xfc::getXmlDocPath() const throw (std::string)
//{
//    if(mState==1 || mState==2)
//        return mFilePath;
//    else
//        throw std::string("Doc not initialized");
//}


void Xfc::recAddPathToXmlTree(xmlNodePtr lpCurrentNode, std::string lPath, 
                              int lLevel, int lMaxDepth,
                              bool lSkipFirstLevel,
                              bool dontComputeSha)
  throw (std::string)
{
    // lPath is a dir - check is done  before calling this functions
    bool isDir;
    bool isSym;
    xmlNodePtr lpNode;

#if defined(XFC_DEBUG)
    std::cout<<__FUNCTION__<<": path="<<lPath<<", level="<<lLevel<<std::endl;
#endif
    
    if(!(lLevel==0 && lSkipFirstLevel))
        lpNode=addDirToXmlTree(lpCurrentNode, lPath);
    else
        lpNode=lpCurrentNode;
    
    if(lLevel>=lMaxDepth && lMaxDepth!=-1)
        return;

    std::vector<std::string> list=getFileListInDir(lPath);
    std::string tempStr;
   
    for(unsigned int i=0;i<list.size();i++)
    {
        tempStr=lPath+"/"+list[i];
        try
        {
            isDir=isDirectory(tempStr);
            isSym=isSymlink(tempStr);
        }
        catch (std::string e)
        {
            // :fixme: - do something?
            throw;
        }
#if defined(XFC_DEBUG)
        if(isDir) {
            std::cout<<__FUNCTION__<<": "<<tempStr<<" is dir."<<std::endl;
        }
        else if(isSym) {
            std::cout<<__FUNCTION__<<": "<<tempStr<<" is symlink."<<std::endl;
        }
        else {
            std::cout<<__FUNCTION__<<": "<<tempStr<<" is regular file."<<std::endl;
        }
#endif

        if(!isSym) {
            if(isDir) {
                recAddPathToXmlTree(lpNode, tempStr, lLevel+1, lMaxDepth,
                                    lSkipFirstLevel, dontComputeSha);
            }    
            else if(isRegularFile(tempStr)) {
                addFileToXmlTree(lpNode, tempStr, dontComputeSha);
            }   
            // else just ignore it
        }
    }
} 


xmlNodePtr Xfc::getNodeForPath(std::string lPath) const
        throw ()
{
    xmlNodePtr lpRoot;
    lpRoot=xmlDocGetRootElement(mpDoc);
    if(lpRoot==NULL)
        return NULL;
    if(lPath=="/")
        return lpRoot;
    return getNodeForPathRec(lPath, lpRoot);
}


xmlNodePtr Xfc::getNodeForPathRec(std::string lPath, xmlNodePtr lpNode) const
            throw ()
{
    xmlNodePtr lNameNode;
    std::string lFileName;
    bool lLastComponent=false;
    
    if(lPath=="")
        return NULL;
    unsigned int lSeparatorPos=lPath.find_first_of('/');
    if(lSeparatorPos==std::string::npos) // last component
    {
        lFileName=lPath;
        lLastComponent=true;
    }
    else if(lSeparatorPos==0)
    {
        return getNodeForPathRec(lPath.substr(1), lpNode);
    }
    else
    {
        lFileName=lPath.substr(0, lSeparatorPos);
    }

    lNameNode=lpNode->xmlChildrenNode;     
    while(lNameNode!=NULL)
    {
        std::string lS;
        if(!strcmp((const char*)lNameNode->name, "file") || 
            !strcmp((const char*)lNameNode->name, "disk"))
        {
            if(!strcmp((const char*)lNameNode->name, "disk"))
                lS=getNameOfDisk(lNameNode);
            else
                lS=getNameOfFile(lNameNode);
            if(lS==lFileName)
            {
                if(lLastComponent)
                    return lNameNode;
                else
                    return getNodeForPathRec(lPath.substr(lSeparatorPos+1), lNameNode);
            }
        }
        lNameNode=lNameNode->next;
    }
    return NULL;
}


std::vector<std::string> Xfc::getDetailsForNode(xmlNodePtr lpNode) throw (std::string)
{
    if(!isFileOrDir(lpNode) && !isDisk(lpNode))
    {
        throw std::string("Xfc::getDetailsForNode(): Node is not a file node");
    }
    std::vector<std::string> lVect;
    
    xmlNodePtr lpDescription;
    // description
    lpDescription=getDescriptionNode(lpNode);
    if(lpDescription==NULL)
    {
        lVect.push_back(std::string(""));
    }
    else
    {
        lVect.push_back(getValueOfNode(lpDescription));
    }

    // cdate
    if(isDisk(lpNode))
    {
        std::string lS=getCDate(lpNode, NULL);
        lVect.push_back(lS);
    }
    else
        lVect.push_back("");
    
    // checksum
    std::string lS;
    lS=getCheckSumOf(lpNode);
    lVect.push_back(lS);
    
    for(int i=3;i<=9;i++)
        lVect.push_back(std::string(""));
        
    // labels
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(isLabel(lpChildNode))
        {
            lVect.push_back(getValueOfNode(lpChildNode));
        }
        lpChildNode=lpChildNode->next;
    }    
    return lVect;
}


void Xfc::createNew(std::string lCatalogName) throw ()
{
    if(mState==1 || mState==2) // there is a catalog - must be deleted
    {
        xmlFreeDoc(mpDoc);
    }  
    mpDoc=xmlNewDoc((const xmlChar*)"1.0"); // :fixme:
   
    xmlNodePtr lpNewNode;
    lpNewNode=xmlNewNode(NULL, (const xmlChar*)"catalog"); // :fixme:
    xmlDocSetRootElement(mpDoc, lpNewNode);
    if(lCatalogName!="")
    {
        lpNewNode=xmlDocGetRootElement(mpDoc);
        xmlNewProp(lpNewNode, (xmlChar*)"name", (xmlChar*)lCatalogName.c_str());
    }
    mState=2; // loaded and modified
}


int Xfc::getState() const throw()
{
    return mState;
} 



xmlNodePtr Xfc::getFirstFileNode(xmlNodePtr lpNode)
// ret null if no such node
{
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(isFileOrDir(lpChildNode))
            return lpChildNode;
        lpChildNode=lpChildNode->next;
    }
    return NULL;
}        


xmlNodePtr Xfc::getFirstLabelNode(xmlNodePtr lpNode)
// ret null if no such node
{
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(isLabel(lpChildNode))
            return lpChildNode;
        lpChildNode=lpChildNode->next;
    }
    return NULL;
}        


xmlNodePtr Xfc::getDescriptionNode(xmlNodePtr lpNode)
// ret null if no such node
{
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(isDescription(lpChildNode))
            return lpChildNode;
        lpChildNode=lpChildNode->next;
    }
    return NULL;
}        


xmlNodePtr Xfc::getNameNode(xmlNodePtr lpNode)
// ret null if no such node
{
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(!strcmp((const char*)lpChildNode->name, "name"))
            return lpChildNode;
        lpChildNode=lpChildNode->next;
    }
    return NULL;
}        


void Xfc::addLabelTo(std::string lPath,
                        std::string lLabel)
            throw (std::string)
{
    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node");
    xmlNodePtr lpNewNode;
    xmlNodePtr lpInsertBeforeNode;
    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"label", (xmlChar*)lLabel.c_str()); // :bug:
    // :fixme: - check ret value

    lpInsertBeforeNode=getFirstFileNode(lpNode);
    if(lpInsertBeforeNode!=NULL)
    {
        if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
            throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
    }            
}


void Xfc::removeLabelFrom(std::string lPath,
                     std::string lLabel)
        throw (std::string)
{
    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node");
    bool lFound=false;
    
    // labels
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while(lpChildNode!=NULL)
    {
        if(isLabel(lpChildNode))
        {
            if(getValueOfNode(lpChildNode)==lLabel)
            {
                lFound=true;
                xmlUnlinkNode(lpChildNode);
                xmlFreeNode(lpChildNode);
                break;
            }                
        }
        lpChildNode=lpChildNode->next;
    }    
}


void Xfc::setDescriptionOf(std::string lPath,
                     std::string lDescription)
        throw (std::string)
{
    xmlNodePtr lpNode;
    xmlNodePtr lpDescriptionNode;
    std::string lS;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node");
    lS=getDescriptionOfNode(lpNode, &lpDescriptionNode);
    if(lS!="") // there is already a description
    {
        xmlNodeSetContent(lpDescriptionNode, (xmlChar*)lDescription.c_str()); // :bug: - encoding
    }
    else
    {
        xmlNodePtr lpNewNode;
        xmlNodePtr lpInsertBeforeNode;
        lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"description", 
                                (xmlChar*)lDescription.c_str()); // :bug:
        // :fixme: - check ret value

        lpInsertBeforeNode=getFirstLabelNode(lpNode);
        if(lpInsertBeforeNode!=NULL)
        {
            if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
                throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
        }        
        else
        {
            lpInsertBeforeNode=getFirstFileNode(lpNode);
            if(lpInsertBeforeNode!=NULL)
            {
                if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
                    throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
            }        
        }   
    }
}


bool Xfc::isLabel(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    cout<<":debug: checking if it is a label: "<<lpNode->name<<endl;
#endif 
    if(!strcmp((const char*)lpNode->name, "label"))
        return true;
    else
        return false;
}


bool Xfc::isDescription(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    cout<<":debug: checking if it is a description: "<<lpNode->name<<endl;
#endif 
    if(!strcmp((const char*)lpNode->name, "description"))
        return true;
    else
        return false;
}


std::string Xfc::getValueOfNode(xmlNodePtr lpNode) throw (std::string)
{
    std::string lS;
    xmlChar *lpStr;
    lpStr=xmlNodeListGetString(mpDoc, lpNode->xmlChildrenNode, 1);
    lS=(char*)lpStr;
    xmlFree(lpStr);
    return lS;
}        


XfcEntity Xfc::getEntityFromNode(xmlNodePtr lpNode)
        throw (std::string)
{
  XfcEntity lEnt(lpNode, this);
  return lEnt;
}


void Xfc::setCDate(std::string lDiskName, std::string lCDate)
    throw (std::string)
{
    xmlNodePtr lpNode;
    xmlNodePtr lpCDateNode;
    std::string lS;
    lpNode=getNodeForPath(lDiskName);
    if(lpNode==NULL)
        throw std::string("No such node");
    std::istringstream lSin;
    lSin.str(lCDate);
    int lYear, lMonth, lDay;
    char c1,c2;
    lSin>>lYear>>c1>>lMonth>>c2>>lDay;
    if(!(lYear>1900 && lYear<2100 && c1=='-' && c2=='-' && lMonth>=1 && lMonth<=12 && lDay>=1 && lDay<=31))
    // :fixme: no. of days related to month
        throw std::string("Date must be in iso format: YYYY-MM-DD");
        
    lS=getCDate(std::string("/")+lDiskName, &lpCDateNode);
    if(lS!="") // there is already a description
    {
        xmlNodeSetContent(lpCDateNode, (xmlChar*)lCDate.c_str()); // :bug: - encoding
    }
    else
    {
        xmlNodePtr lpNewNode;
        xmlNodePtr lpInsertAfterNode;
        lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"cdate", 
                                (xmlChar*)lCDate.c_str()); // :bug:
        // :fixme: - check ret value

        lpInsertAfterNode=getNameNode(lpNode);
        if(lpInsertAfterNode!=NULL)
        {
            if(xmlAddNextSibling(lpInsertAfterNode, lpNewNode)==NULL)
                throw std::string("Xfc::setCDate() error - error in xmlAddNextSibling()");
        }        
        else
        {
            throw std::string("Xfc::setCDate() error - no 'name' node");
        }        
    }
}


std::string Xfc::getCDate(std::string lDiskName, xmlNodePtr *lpCDateNode) 
        const throw (std::string)
// ret "" if no cdate
{
// :fixme: - check state
    xmlNodePtr lpNode;
    lpNode=getNodeForPath(std::string("/")+lDiskName);
    if(lpNode==NULL)
        throw std::string("No such node: ")+lDiskName;
    return getCDate(lpNode, lpCDateNode);
}    


std::string Xfc::getCDate(xmlNodePtr lpNode, xmlNodePtr *lpCDateNode)
const
throw (std::string)
{
    std::string lS="";
    // :fixme: - check if is a disk
    if(lpNode!=NULL)
    {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
        while(lNameNode!=NULL)
        {
            if(!strcmp((const char*)lNameNode->name, "cdate"))
            {
                xmlChar *lpStr;
                lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)lpStr;
                xmlFree(lpStr);
                if(lpCDateNode!=NULL)
                {
                    *lpCDateNode=lNameNode;
                }
                break;
            }
            lNameNode=lNameNode->next;
        }
    }
    return lS;
}


std::string Xfc::getCheckSumOf(std::string lPath)
const
throw (std::string)
{
// :fixme: - check state

    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node: ")+lPath;

    return getCheckSumOf(lpNode);
}

std::string Xfc::getCheckSumOf(xmlNodePtr lpNode)
const
throw (std::string)
{
// :fixme: - check state
        
    std::string lS="";
    xmlNodePtr lNameNode=lpNode;
    lNameNode=lpNode->xmlChildrenNode; 
    while(lNameNode!=NULL)
    {
        if(!strcmp((const char*)lNameNode->name, "sum"))
        {
            xmlChar *lpStr;
            lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
            lS=(char*)lpStr;
            xmlFree(lpStr);
            break;
        }
        lNameNode=lNameNode->next;
    }
    return lS;
}


std::string Xfc::getVersionString()
{
  return std::string(XFCAPP_VERSION);
}
