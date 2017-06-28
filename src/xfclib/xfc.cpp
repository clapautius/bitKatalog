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
#include <fstream>

#include "xfcapp.h"
#include "xfclib.h"
#include "xfc.h"
#include "xfcEntity.h"
#include "fs.h"
#include "misc.h"
#include "plugins.h"

#if defined(XFC_DEBUG)
    #include <iostream>
    using std::cout;
    using std::endl;
#endif

using std::vector;
using std::string;
using std::map;


/**
 * helper function
 * @retval 0 : OK;
 * @retval -1 : user abort / stop;
 * @retval -2 : error;
 **/
int
addLabelCallback(unsigned int depth __attribute__((unused)), std::string path, Xfc& rXfc,
                 xmlNodePtr pNode, void *pParam)
{
    std::string curName;
    curName=rXfc.getNameOfElement(pNode); // :fixme: utf8 -> string ?
    rXfc.addLabelTo(path+"/"+curName, string((const char*)pParam));
    return 0;
}


/**
 * helper function
 * @retval 0 : OK;
 * @retval -1 : user abort / stop;
 * @retval -2 : error;
 **/
int
addLabelsCallback(unsigned int depth __attribute__((unused)), string path,
                  Xfc& rXfc, xmlNodePtr pNode, void *pParam)
{
    rXfc.addLabelsTo(pNode, static_cast<const vector<string>*>(pParam), true);
    return 0;
}


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
    if (xmlSaveFormatFileEnc(lPath.c_str(), getXmlDocPtr(), "utf-8", lParams)
        == -1)
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


/**
 * Parses XML tree calling callBackFunc for every xml node of type dir, disk,
 * file or root.
 * When callBackFunc returns -1, it abandons parsing.
 **/
void Xfc::parseFileTree(ParserFuncType callBackFunc, void *lpParam)
    throw (std::string)
{
#ifdef XFC_TIMING
    auto entry = MONO_TIME_NOW;
#endif
    // :fixme: - check state
    xmlNodePtr lpChild;
    lpChild=xmlDocGetRootElement(mpDoc);
    if (lpChild==NULL) // empty document
        return;
    if (callBackFunc(0, "/", *this, lpChild, lpParam) == -1)
        return; // abandon
    lpChild=mpDoc->xmlChildrenNode;
    while (lpChild!=NULL) {
        parseRec(0, "/", lpChild, callBackFunc, lpParam);
        lpChild=lpChild->next;
    }
#ifdef XFC_TIMING
    auto exit = MONO_TIME_NOW;
    std::cout << __FUNCTION__ << ": time (ms): " << MONO_TIME_DIFF_MS(entry, exit)
              << std::endl;
#endif
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

    while (lpChild!=NULL) {
#if defined(XFC_DEBUG)
        std::cout<<":debug: child addr="<<lpChild<<endl;
#endif 
        if (isFileOrDir(lpChild) || isDisk(lpChild)) {
#if defined(XFC_DEBUG)
            cout<<":debug: calling callbackFunc for element at addr: "<<lpChild<<endl;
#endif
            lRet=callBackFunc(lDepth, lPath, *this, lpChild, lpParam);
            if (lRet==-1) // abandon
                return;
            lS=getNameOfElement(lpChild);
            if (lPath=="/")
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
    //cout<<":debug: checking if it is a file/dir: "<<lpNode->name<<endl;
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
    //cout<<":debug: checking if it is a disk: "<<lpNode->name<<endl;
#endif
    if (eDisk == getTypeOfElement(lpNode))
        return true;
    else
        return false;
}


bool Xfc::isRoot(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    //cout<<":debug: checking if it is a root item: "<<lpNode->name<<endl;
#endif
    if (eRoot == getTypeOfElement(lpNode))
        return true;
    else
        return false;
}


/**
 * Check if node is dir, disk, file or root.
 * Is more efficient than calling isDir || isFile || ...
 **/
bool
Xfc::isDirDiskFileOrRoot(xmlNodePtr pNode) const throw()
{
    ElementType type=getTypeOfElement(pNode);
    if (eDir == type || eDisk == type || eFile == type || eRoot == type)
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
//    cout<<":debug: entering function "<<__FUNCTION__<<endl;
#endif 
    std::string lS;
    if (lpNode==NULL)
        lS="";
    else if (eRoot == getTypeOfElement(lpNode)) {
        xmlChar *pStr;
        pStr=xmlGetProp(lpNode, (const xmlChar*)"name");
        if (pStr) {
            lS=(char*)pStr;
            xmlFree(pStr);
        }
    }
    else {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
#if defined(XFC_DEBUG)
        //cout<<"    :debug: searching name of element"<<endl;
#endif 
        while (lNameNode!=NULL) {
#if defined(XFC_DEBUG)
            //cout<<"    :debug: found "<<(const char*)lNameNode->name<<endl;
#endif 
            if (!strcmp((const char*)lNameNode->name, "name")) {
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
//    cout<<":debug: name is: "<<lS.c_str()<<". Exiting function "<<__FUNCTION__<<endl;
#endif 
    return lS;
}


void
Xfc::setNameOfElement(xmlNodePtr pNode, std::string newName) throw (std::string)
{
    if (eRoot == getTypeOfElement(pNode)) {
        xmlNewProp(pNode, (xmlChar*)"name", (xmlChar*)newName.c_str());
    }
    else {
        xmlNodePtr pNameNode;  
        if ( (pNameNode=getNameNode(pNode)) == NULL)
            throw std::string("No name"); // :fixme: - should add it
        xmlNodeSetContent(pNameNode, (xmlChar*)newName.c_str());
        // :fixme: use xmlEncodeSpecialChars().
        // :fixme: the old content ?
    }
}
         

Xfc::ElementType
Xfc::getTypeOfElement(xmlNodePtr lpNode)
  const throw (std::string)
{
    // :fixme: - check state
    ElementType retVal=eUnknown;
#if defined(XFC_DEBUG)
    //cout<<":debug: in getTypeOfElement()"<<endl;
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
            else if (!strcmp(pElemName, "catalog"))
                retVal=eRoot;
            else
                retVal=eUnknown;
        }
    }
#if defined(XFC_DEBUG)
    //cout<<":debug: type is: "<<retVal<<endl;
    //cout<<":debug: out of getTypeOfElement()"<<endl;
#endif 
    return retVal;
}


std::string Xfc::getDescriptionOfNode(xmlNodePtr lpNode, xmlNodePtr* lpDescriptionNode) 
        const throw (std::string)
// ret "" if no description
{
    // :fixme: - check state
    std::string lS="";
    if (lpNode!=NULL) {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
        while (lNameNode!=NULL) {
            if (!strcmp((const char*)lNameNode->name, "description")) {
                xmlChar *lpStr;
                lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)lpStr;
                xmlFree(lpStr);
                if (lpDescriptionNode!=NULL) {
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
Xfc::addPathToXmlTree(
    string lPath, int lMaxDepth, volatile const bool *pAbortFlag,
    vector<XmlParamForFileCallback> cbList1, vector<XmlParamForFileChunkCallback> cbList2,
    string lDiskId, string lDiskCategory, string lDiskDescription, string lDiskLabel)
  throw (std::string)
{
    bool lSkipFirstLevel=false;
    gLog<<xfcInfo<<__FUNCTION__<<": path="<<lPath<<eol;
    if (!isDirectory(lPath) && !isRegularFile(lPath)) {
        throw std::string("Xfc::addDirToXmlTree(): Invalid path"); 
    }
    xmlNodePtr lpDisk;
    lpDisk=getNodeForPath(std::string("/")+lDiskId);
    if (isDirectory(lPath) && lPath[lPath.size()-1]=='/') { // add only the subdirs
        lSkipFirstLevel=true;
    }
    if(lpDisk==NULL)
        lpDisk=addNewDiskToXmlTree(lDiskId);
    if(!isSymlink(lPath)) { // :fixme: - do something with the symlinks
        if (isRegularFile(lPath)) {
            addFileToXmlTree(lpDisk, lPath, cbList1, cbList2, pAbortFlag);
        }
        else if (isDirectory(lPath)) {
            recAddPathToXmlTree(lpDisk, lPath, 0, lMaxDepth, pAbortFlag, cbList1, cbList2,
                                lSkipFirstLevel);
        }
    }
} 


xmlNodePtr Xfc::addNewDiskToXmlTree(std::string lDiskName, std::string lDiskCategory,
                              std::string lDiskDescription, std::string lDiskLabel,
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
    
    if (xmlNewTextChild(lpNode, NULL, (xmlChar*)"name",
                        (xmlChar*)lDiskName.c_str()) == NULL) {
        throw string(__FUNCTION__) + "xmlNewTextChild() error";
    }

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


void
Xfc::deleteDiskFromXmlTree(std::string diskName) throw (std::string)
{
    xmlNodePtr pDiskNode=getNodeForPath(diskName);
    if (!pDiskNode) {
        throw string("No such disk");
    }
    if (!isDisk(pDiskNode)) {
        throw string("Path is not a disk");
    }
    xmlUnlinkNode(pDiskNode);
    xmlFreeNode(pDiskNode);
}


/**
 * helper function
 **/
map<string, string>
Xfc::bufferCallbacks(string path, vector<XmlParamForFileChunkCallback> &cbList2,
                     volatile const bool *pAbortFlag)
{
    ifstream fin;
    bool firstTime=true;
    bool lastTime=false;
    map<string, string> params;
    string s1, s2;
    char readBuf[READ_BUF_LEN];
    gLog<<xfcDebug<<"trying to open "<<path<<" ... ";
    fin.open(path.c_str(), ios::binary);
    if (!fin.good()) {
        gLog<<eol<<xfcWarn<<"error opening "<<path<<eol;
        throw std::string("error opening file")+path;
    }
    gLog<<"ok"<<eol;
    while (fin.good()) {
        fin.read(readBuf, READ_BUF_LEN);
        if (fin.eof())
            lastTime=true;
        // :debug:
        //gLog<<xfcDebug<<": new iteration: file pos="<<fin.tellg()<<", gcount=";
        //gLog<<fin.gcount()<<", firstTime="<<firstTime<<", lastTime="<<lastTime<<eol;
        for (unsigned int i=0; i<cbList2.size(); i++) {
            if (cbList2[i](readBuf, fin.gcount(), firstTime, lastTime, &s1, &s2,
                           pAbortFlag)>=0) {
                if (lastTime) {  // final iteration, add xml elements
                    gLog<<xfcDebug<<__FUNCTION__<<": adding param. "<<s1;
                    gLog<<" with value "<<s2<<eol;
                    params.insert(pair<string, string>(s1, s2));
                }
            } // error at some iteration
            else
                throw string("error at some callback function");
        } // end for
        firstTime=false;

        // user abort
        if (pAbortFlag && *pAbortFlag)
            break;
    } // end while(fin.good())
    fin.close();
    return params;
}


/**
 **/
xmlNodePtr
Xfc::addFileToXmlTree(
    xmlNodePtr lpNode, string path,
    vector<XmlParamForFileCallback> cbList1, vector<XmlParamForFileChunkCallback> cbList2,
    volatile const bool *pAbortFlag)
  throw (std::string)
{
    std::string lName=getLastComponentOfPath(path);
    xmlNodePtr lpNewNode;
    xmlNodePtr lpNewNewNode;
    gLog<<xfcDebug<<__FUNCTION__<<": adding file "<<path<<" to xml tree"<<eol;
    
    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"file", NULL); // :bug:
    if(lpNewNode==NULL)
        throw std::string("Xfc::addFileToXmlTree(): xmlNewTextChild() error");
    
    // :fixme: add all the other stuff
   
    // name
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"name", (xmlChar*)lName.c_str()); // :bug:
    
    // size
    FileSizeType lSize=getFileSize(path);
    std::ostringstream lStrOut;
    lStrOut<<lSize;
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"size", (xmlChar*)lStrOut.str().c_str()); // :bug:

    for (unsigned int i=0; i<cbList1.size(); i++) {
        string param, value;
        std::string xmlParam, xmlValue;
        std::vector<std::string> xmlAttrs;
        if (cbList1[i](path, &param, &value, pAbortFlag)>=0) {
            if (SHA256LABEL==param) {
                lpNewNewNode=xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"sum", 
                                             (xmlChar*)value.c_str());
                xmlNewProp(lpNewNewNode, (xmlChar*)"type", (xmlChar*)SHA256LABEL);
            }
            else if (SHA1LABEL==param) {
                lpNewNewNode=xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"sum", 
                                             (xmlChar*)value.c_str());
                xmlNewProp(lpNewNewNode, (xmlChar*)"type", (xmlChar*)SHA1LABEL);
            }
        }
        else
            throw std::string("Error at callback :fixme:");
    }
    if (cbList2.size()>0) {
        map<string, string> params;
        params=bufferCallbacks(path, cbList2, pAbortFlag);
        // check if we have sha1 sum
        if (!params[SHA1LABEL].empty()) {
            lpNewNewNode=xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"sum",
                                         (xmlChar*)params[SHA1LABEL].c_str());
            xmlNewProp(lpNewNewNode, (xmlChar*)"type", (xmlChar*)SHA1LABEL);
        }
        // check if we have sha256 sum
        if (!params[SHA256LABEL].empty()) {
            lpNewNewNode=xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"sum",
                                         (xmlChar*)params[SHA256LABEL].c_str());
            xmlNewProp(lpNewNewNode, (xmlChar*)"type", (xmlChar*)SHA256LABEL);
        }
    } // end cbList2

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

    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"dir", NULL); // :bug:
    if(lpNewNode==NULL)
        throw std::string("Xfc::addDirToXmlTree(): xmlNewTextChild() error");
    
    // :fixme: add all the other stuff
   
    // name
    xmlNewTextChild(lpNewNode, NULL, (xmlChar*)"name", (xmlChar*)lName.c_str()); // :bug:
    
    return lpNewNode;
}    


void
Xfc::recAddPathToXmlTree(
    xmlNodePtr lpCurrentNode, std::string lPath, int lLevel, int lMaxDepth,
    volatile const bool *pAbortFlag,
    std::vector<XmlParamForFileCallback> cbList1,
    std::vector<XmlParamForFileChunkCallback> cbList2, bool lSkipFirstLevel)
    throw (std::string)
{
    // lPath is a dir - check is done  before calling this functions
    bool isDir;
    bool isSym;
    xmlNodePtr lpNode;
    gLog<<xfcDebug<<__FUNCTION__<<": path="<<lPath<<" level="<<lLevel<<eol;
    
    if (!(lLevel==0 && lSkipFirstLevel))
        lpNode=addDirToXmlTree(lpCurrentNode, lPath);
    else
        lpNode=lpCurrentNode;
    
    if(lLevel>=lMaxDepth && lMaxDepth!=-1)
        return;

    std::vector<std::string> list=getFileListInDir(lPath);
    std::string tempStr;
    for (unsigned int i=0;i<list.size();i++) {
        tempStr=lPath+"/"+list[i];
        try {
            isDir=isDirectory(tempStr);
            isSym=isSymlink(tempStr);
        }
        catch (std::string e) {
            // :fixme: - do something?
            throw;
        }
        gLog<<xfcDebug<<__FUNCTION__<<": "<<tempStr;
        if(isDir)
            gLog<<" is dir."<<eol;
        else if(isSym)
            gLog<<" is symlink."<<eol;
        else
            gLog<<" is regular file."<<eol;

        if(!isSym) {
            if(isDir) {
                recAddPathToXmlTree(lpNode, tempStr, lLevel+1, lMaxDepth, pAbortFlag,
                                    cbList1, cbList2, lSkipFirstLevel);
            }    
            else if(isRegularFile(tempStr)) {
                addFileToXmlTree(lpNode, tempStr, cbList1, cbList2, pAbortFlag);
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
    if (lpRoot==NULL)
        return NULL;
    if (lPath=="/")
        return lpRoot;
    return getNodeForPathRec(lPath, lpRoot);
}


xmlNodePtr Xfc::getNodeForPathRec(std::string lPath, xmlNodePtr lpNode) const
            throw ()
{
    xmlNodePtr lNameNode;
    std::string lFileName;
    bool lLastComponent=false;
    if (lPath=="")
        return NULL;
    size_t lSeparatorPos = lPath.find_first_of('/');
    if (lSeparatorPos==std::string::npos) { // last component
        lFileName=lPath;
        lLastComponent=true;
    }
    else if (lSeparatorPos==0) {
        return getNodeForPathRec(lPath.substr(1), lpNode);
    }
    else {
        lFileName=lPath.substr(0, lSeparatorPos);
    }
    lNameNode=lpNode->xmlChildrenNode;     
    while (lNameNode!=NULL) {
        std::string lS;
        ElementType type=getTypeOfElement(lNameNode);
        if (eDisk==type || eDir==type || eFile==type) {
            if (eDisk==type)
                lS=getNameOfDisk(lNameNode);
            else
                lS=getNameOfFile(lNameNode);
            if (lS==lFileName) {
                if (lLastComponent)
                    return lNameNode;
                else
                    return getNodeForPathRec(lPath.substr(lSeparatorPos+1), lNameNode);
            }
        }
        lNameNode=lNameNode->next;
    }
    return NULL;
}


/**
 * Get all the details of a node.
 * The node must be a file, dir, disk or root.
 *
 * Map keys: description, size, cdate, sha1sum, sha256sum.
 *
 * @sa getLabelsForNode
 **/
std::map<std::string, std::string> Xfc::getDetailsForNode(xmlNodePtr lpNode) throw (std::string)
{
    std::map<std::string, std::string> details;
    string str;
    
    if (! (isDirDiskFileOrRoot(lpNode))) {
        throw std::string(__FUNCTION__)+": Node is not a root/dir/file node";
    }
    
    // description
    xmlNodePtr pDescription;
    if ((pDescription=getDescriptionNode(lpNode))) {
        details.insert(pair<string, string>("description", getValueOfNode(pDescription)));
    }

    // cdate
    if (isDisk(lpNode)) {
        str=getCDate(lpNode, NULL);
        if (!str.empty()) {
            details.insert(pair<string, string>("cdate", str));
            str.clear();
        }
    }

    // size
    if (isFileOrDir(lpNode)) {
        str=getParamValueForNode(lpNode, "size");
        if (!str.empty()) {
            details.insert(pair<string, string>("size", str));
            str.clear();
        }
    }

    // checksum
    std::string lS;
    lS=getChecksumOf(lpNode, SHA256LABEL);
    if (!lS.empty())
        details.insert(pair<string, string>(SHA256LABEL, lS));
    lS=getChecksumOf(lpNode, SHA1LABEL);
    if (!lS.empty())
        details.insert(pair<string, string>(SHA1LABEL, lS));
    
    return details;
}


vector<string>
Xfc::getLabelsForNode(xmlNodePtr lpNode) throw (string)
{
    vector<string> labels;
    if (! (isDirDiskFileOrRoot(lpNode))) {
        throw std::string(__FUNCTION__)+": Node is not a root/dir/file node";
    }
    
    // get XML labels
    xmlNodePtr pChildNode;
    pChildNode=lpNode->xmlChildrenNode; 
    while (pChildNode!=NULL) {
        if (isLabel(pChildNode)) {
            labels.push_back(getValueOfNode(pChildNode));
        }
        pChildNode=pChildNode->next;
    }
    return labels;
}


void Xfc::createNew(std::string lCatalogName) throw ()
{
    if (mState==1 || mState==2) { // there is a catalog - must be deleted
        xmlFreeDoc(mpDoc);
    }  
    mpDoc=xmlNewDoc((const xmlChar*)"1.0"); // :fixme:
   
    xmlNodePtr lpNewNode;
    lpNewNode=xmlNewNode(NULL, (const xmlChar*)"catalog"); // :fixme:
    xmlDocSetRootElement(mpDoc, lpNewNode);
    if (lCatalogName!="") {
        lpNewNode=xmlDocGetRootElement(mpDoc);
        xmlNewProp(lpNewNode, (xmlChar*)"name", (xmlChar*)lCatalogName.c_str());
    }
    mState=2; // loaded and modified
}


int Xfc::getState() const throw()
{
    return mState;
} 


string
Xfc::getParamValueForNode(xmlNodePtr pNode, string param)
{
    string ret;
    xmlNodePtr pChildNode;
    pChildNode=pNode->xmlChildrenNode; 
    while (pChildNode!=NULL) {
        if (!strcmp((const char*)pChildNode->name, param.c_str())) {
            ret=getValueOfNode(pChildNode);
            break;
        }
        pChildNode=pChildNode->next;
    }
    return ret;
}


xmlNodePtr Xfc::getFirstFileNode(xmlNodePtr lpNode)
// ret null if no such node
{
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while (lpChildNode!=NULL) {
        if (isFileOrDir(lpChildNode))
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
    while (lpChildNode!=NULL) {
        if (isLabel(lpChildNode))
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
    while (lpChildNode!=NULL) {
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
    while (lpChildNode!=NULL) {
        if (!strcmp((const char*)lpChildNode->name, "name"))
            return lpChildNode;
        lpChildNode=lpChildNode->next;
    }
    return NULL;
}        


/**
 * Add a new label to the specified node. Doesnt check if the label already
 * exists.
 * Throws std::string if the path is invalid (the node doesn't exist).
 *
 * @param[in] path : copmplete path
 * @param[in] label : the label
 **/
void
Xfc::addLabelTo(std::string lPath, std::string lLabel)
    throw (std::string)
{
    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if (lpNode==NULL)
        throw std::string("No such node");

    xmlNodePtr lpNewNode;
    xmlNodePtr lpInsertBeforeNode;
    lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"label", (xmlChar*)lLabel.c_str()); // :bug:
    // :fixme: - check ret value

    lpInsertBeforeNode=getFirstFileNode(lpNode);
    if (lpInsertBeforeNode!=NULL) {
        if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
            throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
    }            
}


/**
 * Add a new label to the specified node.
 *
 * @param[in] path : copmplete path
 * @param[in] label : the label
 * @param[in] checkDuplicate : if true, first check if label already exists.
 **/
void
Xfc::addLabelsTo(xmlNodePtr pNode, const vector<string> *pLabels,
                 bool checkDuplicate)
    throw (std::string)
{
    vector<string> currentLabels;
    if (pNode==NULL)
        throw std::string("NULL node");

    gLog<<xfcDebug<<__FUNCTION__<<" adding vector of labels to node"<<eol;
    if (checkDuplicate) {
        currentLabels=getLabelsForNode(pNode);
    }

    for (uint i=0; i<pLabels->size(); i++) {
        if (checkDuplicate && contains(currentLabels, pLabels->at(i))) {
            gLog<<xfcDebug<<__FUNCTION__<<" node already contains label "<<
                pLabels->at(i)<<eol;
            continue;
        }
        gLog<<xfcDebug<<__FUNCTION__<<" adding label "<<pLabels->at(i)<<
            " to node"<<eol;
        xmlNodePtr pNewNode;
        xmlNodePtr pInsertBeforeNode;
        pNewNode=xmlNewTextChild(pNode , NULL, (xmlChar*)"label",
                                 (xmlChar*)pLabels->at(i).c_str()); // :bug:
        // :fixme: - check ret value
        
        pInsertBeforeNode=getFirstFileNode(pNode);
        if (pInsertBeforeNode!=NULL) {
            if(xmlAddPrevSibling(pInsertBeforeNode, pNewNode)==NULL)
                throw std::string(__FUNCTION__)+
                    " - error in xmlAddPrevSibling()";
        }
    }
}


void Xfc::removeLabelFrom(std::string lPath, std::string lLabel)
        throw (std::string)
{
    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node");
    
    // labels
    xmlNodePtr lpChildNode;
    lpChildNode=lpNode->xmlChildrenNode; 
    while (lpChildNode!=NULL) {
        if (isLabel(lpChildNode)) {
            if (getValueOfNode(lpChildNode)==lLabel) {
                xmlUnlinkNode(lpChildNode);
                xmlFreeNode(lpChildNode);
                break;
            }                
        }
        lpChildNode=lpChildNode->next;
    }    
}



/**
 * Checks if already exists.
 **/
void
Xfc::addLabelRecTo(string path, string label) throw (string)
{
    xmlNodePtr pNode;
    pNode=getNodeForPath(path);
    if (pNode==NULL)
        throw std::string("No such node");
    addLabelTo(path, label);
    parseRec(0, path, pNode, addLabelCallback, (void*)label.c_str());
}


void
Xfc::addLabelsRecTo(string path, vector<string> labels)
    throw (std::string)
{
    xmlNodePtr pNode;
    pNode=getNodeForPath(path);
    if (pNode==NULL)
        throw std::string("No such node");
    addLabelsTo(pNode, &labels, true);
    parseRec(0, path, pNode, addLabelsCallback, (void*)&labels);
}


void
Xfc::removeLabelRecFrom(string path, string label) throw (string)
{
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
    if (lS!="") { // there is already a description
        xmlNodeSetContent(lpDescriptionNode, (xmlChar*)lDescription.c_str()); // :bug: - encoding
    }
    else {
        xmlNodePtr lpNewNode;
        xmlNodePtr lpInsertBeforeNode;
        lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"description", 
                                (xmlChar*)lDescription.c_str()); // :bug:
        // :fixme: - check ret value

        lpInsertBeforeNode=getFirstLabelNode(lpNode);
        if (lpInsertBeforeNode!=NULL) {
            if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
                throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
        }        
        else {
            lpInsertBeforeNode=getFirstFileNode(lpNode);
            if (lpInsertBeforeNode!=NULL) {
                if(xmlAddPrevSibling(lpInsertBeforeNode, lpNewNode)==NULL)
                    throw std::string("Xfc::addLabelToNode() error - error in xmlAddPrevSibling()");
            }        
        }   
    }
}


bool Xfc::isLabel(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    //cout<<":debug: checking if it is a label: "<<lpNode->name<<endl;
#endif 
    if (!strcmp((const char*)lpNode->name, "label"))
        return true;
    else
        return false;
}


bool Xfc::isDescription(xmlNodePtr lpNode) const throw()
{
#if defined(XFC_DEBUG)
    //cout<<":debug: checking if it is a description: "<<lpNode->name<<endl;
#endif 
    if (!strcmp((const char*)lpNode->name, "description"))
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


XfcEntity
Xfc::getEntityFromPath(std::string path) throw (std::string)
{
    xmlNodePtr pNode=getNodeForPath(path);
    if (pNode) {
        XfcEntity ent = getEntityFromNode(pNode);
        return ent;
    }
    throw string(__FUNCTION__)+": No such node";
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
    if (lS!="") { // there is already a description
        xmlNodeSetContent(lpCDateNode, (xmlChar*)lCDate.c_str()); // :bug: - encoding
    }
    else {
        xmlNodePtr lpNewNode;
        xmlNodePtr lpInsertAfterNode;
        lpNewNode=xmlNewTextChild(lpNode , NULL, (xmlChar*)"cdate", 
                                (xmlChar*)lCDate.c_str()); // :bug:
        // :fixme: - check ret value

        lpInsertAfterNode=getNameNode(lpNode);
        if (lpInsertAfterNode!=NULL) {
            if(xmlAddNextSibling(lpInsertAfterNode, lpNewNode)==NULL)
                throw std::string("Xfc::setCDate() error - error in xmlAddNextSibling()");
        }        
        else {
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
    if (lpNode!=NULL) {
        xmlNodePtr lNameNode;
        lNameNode=lpNode->xmlChildrenNode; 
        while (lNameNode!=NULL) {
            if (!strcmp((const char*)lNameNode->name, "cdate")) {
                xmlChar *lpStr;
                lpStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)lpStr;
                xmlFree(lpStr);
                if (lpCDateNode!=NULL) {
                    *lpCDateNode=lNameNode;
                }
                break;
            }
            lNameNode=lNameNode->next;
        }
    }
    return lS;
}


std::string
Xfc::getChecksumOf(std::string lPath, std::string type)
    const throw (std::string)
{
    // :fixme: - check state

    xmlNodePtr lpNode;
    lpNode=getNodeForPath(lPath);
    if(lpNode==NULL)
        throw std::string("No such node: ")+lPath;

    return getChecksumOf(lpNode, type);
}


std::string
Xfc::getChecksumOf(xmlNodePtr lpNode, std::string type)
    const throw (std::string)
{
    // :fixme: - check state

    if (type.empty())
        type="sha256";
    
    std::string lS="";
    xmlNodePtr lNameNode=lpNode;
    lNameNode=lpNode->xmlChildrenNode; 
    while (lNameNode!=NULL) {
        if (!strcmp((const char*)lNameNode->name, "sum")) {
            xmlChar *pStr;
            pStr=xmlGetProp(lNameNode, (const xmlChar*)"type");
            if (pStr && type==(const char*)pStr) {
                xmlFree(pStr);
                pStr=xmlNodeListGetString(mpDoc, lNameNode->xmlChildrenNode, 1);
                lS=(char*)pStr;
                xmlFree(pStr);
                break;
            }
        }
        lNameNode=lNameNode->next;
    }
    return lS;
}


std::string Xfc::getVersionString()
{
  return std::string(XFCAPP_VERSION);
}


/**
 * helper function
 **/
EntityDiff
Xfc::compareItems(string catalogName, XfcEntity &rEnt, string diskPath,
                  unsigned int pathPrefixLen, bool &rShaWasMissing,
                  volatile const bool *pAbortFlag)
{
    EntityDiff diff;
    string str;
    map<string, string> details;
    map<string, string> diskDetails;
    bool hasSha256=false, hasSha1=false;
    bool openFile=false;
    vector<XmlParamForFileChunkCallback> cbList;
    details=rEnt.getDetails();
    ostringstream ostr;
    gLog<<xfcDebug<<"comparing items with name "<<catalogName<<eol;

    if (!isDirectory(diskPath)) {
        // first check size
        ostr<<getFileSize(diskPath);
        if (ostr.str() != details["size"]) {
            diff.type=eDiffSize;
            diff.name=diskPath.substr(pathPrefixLen+1); // including trailing '/'
            diff.catalogValue=details["size"];
            diff.diskValue=ostr.str();
            gLog<<xfcDebug<<"different sizes for "<<catalogName<<eol;
            gLog<<"catalog val="<<details["size"]<<", disk val="<<ostr.str()<<eol;
            return diff;
        }

        if (details[SHA256LABEL].empty() && details[SHA1LABEL].empty())
            rShaWasMissing=true;

        if (!details[SHA256LABEL].empty()) {
            hasSha256=true;
            cbList.push_back(sha256UsingBufCallback);
        }
        if (!details[SHA1LABEL].empty()) {
            hasSha1=true;
            cbList.push_back(sha1UsingBufCallback);
        }
        openFile=hasSha1 || hasSha256;

        if (openFile) {
            try {
                diskDetails=bufferCallbacks(diskPath, cbList, pAbortFlag);
            }
            catch (string e) {
                diff.type=eDiffErrorOnDisk;
                diff.name=diskPath.substr(pathPrefixLen+1); // including trailing '/'
                diff.diskValue=e;
                return diff;
            }
            // sha256
            if (hasSha256) {
                if (diskDetails[SHA256LABEL]!=details[SHA256LABEL]) {
                    diff.type=eDiffSha256Sum;
                    diff.name=diskPath.substr(pathPrefixLen+1); // including trailing '/'
                    diff.catalogValue=details[SHA256LABEL];
                    diff.diskValue=diskDetails[SHA256LABEL];
                    gLog<<xfcDebug<<"different sha256 for "<<catalogName<<eol;
                    gLog<<"catalog val="<<details[SHA256LABEL];
                    gLog<<", disk val="<<diskDetails[SHA256LABEL]<<eol;
                    return diff;
                }
            }
            // sha1
            if (hasSha1) {
                if (diskDetails[SHA1LABEL]!=details[SHA1LABEL]) {
                    diff.type=eDiffSha1Sum;
                    diff.name=diskPath.substr(pathPrefixLen+1); // including trailing '/'
                    diff.catalogValue=details[SHA1LABEL];
                    diff.diskValue=diskDetails[SHA1LABEL];
                    gLog<<xfcDebug<<"different sha1 for "<<catalogName<<eol;
                    gLog<<"catalog value="<<details[SHA1LABEL];
                    gLog<<", disk value="<<diskDetails[SHA1LABEL]<<eol;
                    return diff;
                }
            }
        } // end if(openFile)
    } // end file compare

    return diff;
}


/**
 * @retval < 0 - failure
 * @retval 0 - stopped by user
 * @retval 1 - ok, checksums were present
 * @retval 2 - ok, some checksums were missing
 **/
int
Xfc::verifyDirectory(string catalogPath, string diskPath, unsigned int pathPrefixLen,
                     vector<EntityDiff> *pDifferences, volatile const bool *pAbortFlag)
{
    vector<string> namesInCatalog;
    vector<XfcEntity> entitiesInCatalog;
    map<string, string> details;
    vector<string> namesOnDisk;
    EntityDiff diff;
    bool shaWasMissing=false;

    gLog<<xfcDebug<<"Verifying dir: "<<diskPath<<eol;
    gLog<<xfcDebug<<"Catalog path is: "<<catalogPath<<eol;

    if (pAbortFlag && *pAbortFlag) {
        gLog<<xfcInfo<<__FUNCTION__<<": stopped by user"<<eol;
        return 0;
    }
    
    EntityIterator entIterator(*this, catalogPath);
    while (entIterator.hasMoreChildren()) {
        XfcEntity ent = entIterator.getNextChild();
        entitiesInCatalog.push_back(ent);
        namesInCatalog.push_back(ent.getName());
    }
    
    try {
        namesOnDisk=getFileListInDir(diskPath);
    }
    catch(std::string e) {
        // do something (add this to error list?) :fixme:
        gLog<<xfcWarn<<__FUNCTION__<<": error in getFileListInDir()"<<eol;
        return -1;
    }
    
    for (unsigned int i=0;i<namesOnDisk.size();i++)
        if (isDirectory(diskPath+"/"+namesOnDisk[i])) {
            bool found=true;
            // check if it exists in catalog
            for (unsigned int j=0;j<namesInCatalog.size();j++)
                if (namesInCatalog[j]==namesOnDisk[i]) {
                    found=true;
                    break;
                }
            if (found) {
                verifyDirectory(catalogPath+"/"+namesOnDisk[i], diskPath+"/"+namesOnDisk[i],
                                pathPrefixLen, pDifferences, pAbortFlag);
            }
            else {
                diff.type=eDiffOnlyOnDisk;
                diff.name=(diskPath+"/"+namesOnDisk[i]).substr(pathPrefixLen+1);
                pDifferences->push_back(diff);
                gLog<<xfcInfo<<"Difference (only on disk) for: "<<namesOnDisk[i]<<eol;
                namesOnDisk.erase(namesOnDisk.begin()+i);
                i--;
            }
        }

    for (unsigned int i=0;i<namesInCatalog.size();i++) {
        gLog<<xfcDebug<<__FUNCTION__<<": checking (catalog name): "<<namesInCatalog[i]<<eol;
        bool found=false;
        for (unsigned int j=0;j<namesOnDisk.size();j++) {
            if (pAbortFlag && *pAbortFlag) {
                gLog<<xfcInfo<<__FUNCTION__<<": stopped by user"<<eol;
                return 0;
            }
            if (namesInCatalog[i]==namesOnDisk[j]) {
                gLog<<xfcDebug<<__FUNCTION__<<": comparing with (disk name): "<<namesOnDisk[j]<<eol;
                found=true;
                std::string diskPathCurrent=diskPath+"/"+namesOnDisk[j];
                diff=compareItems(namesInCatalog[i], entitiesInCatalog[i], diskPathCurrent,
                                  pathPrefixLen, shaWasMissing);
                if (eDiffIdentical != diff.type) {
                    if (eDiffErrorOnDisk == diff.type) {
                        throw std::string("Error comparing")+namesInCatalog[i];
                    }
                    else {
                        gLog<<xfcInfo<<"Difference (type="<<(int)diff.type<<") for: "<<diff.name<<eol;
                        gLog<<"  catalog value="<<diff.catalogValue<<", disk value="<<diff.diskValue<<eol;
                        pDifferences->push_back(diff);
                    }
                }
                namesOnDisk.erase(namesOnDisk.begin()+j);
                break;
            }
        }
        if (!found) { // only in catalog
            diff.type=eDiffOnlyInCatalog;
            diff.name=catalogPath+"/"+namesInCatalog[i];
            gLog<<xfcInfo<<"Difference (only in catalog) for: "<<diff.name<<eol;
            pDifferences->push_back(diff);
        }
    }

    // the remaining elements are only on disk
    diff.type=eDiffOnlyOnDisk;
    for (unsigned int i=0;i<namesOnDisk.size();i++)  {
        diff.name=(diskPath+"/"+namesOnDisk[i]).substr(pathPrefixLen+1);
        gLog<<xfcInfo<<"Difference (only on disk) for: "<<diff.name<<eol;
        pDifferences->push_back(diff);
    }

    if (shaWasMissing)
        return 2;
    else
        return 1;
}


xmlNodePtr Xfc::getSubelementByName(xmlNodePtr pNode, const std::string &name)
{
    xmlNodePtr pNameNode = pNode->xmlChildrenNode;
    while (pNameNode != NULL) {
        if (!strcmp((const char*)pNameNode->name, name.c_str())) {
            return pNameNode;
        }
        pNameNode = pNameNode->next;
    }
    return NULL;
}


std::string Xfc::getNodeText(xmlNodePtr pNode)
{
    std::string s;
    if (pNode) {
        xmlChar *pStr = xmlNodeListGetString(mpDoc, pNode->xmlChildrenNode, 1);
        s = (char*)pStr;
        xmlFree(pStr);
    }
    return s;
}


void Xfc::setNodeText(xmlNodePtr p_node, const std::string &text)
{
    if (p_node) {
        xmlNodeSetContent(p_node, (xmlChar*)text.c_str());
    }
}
