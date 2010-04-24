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
#ifndef XFC_H
#define XFC_H

#include <string.h>

#include <string>
#include <vector>
#include <map>

#include <libxml/parser.h>

#include "xfclib.h"

using namespace std;

#define SHA256LABEL "sha256"
#define SHA1LABEL "sha1"

#define READ_BUF_LEN 102400

class XfcEntity;
class Xfc;


typedef int(*ParserFuncType)(unsigned int, string, Xfc&, xmlNodePtr, void*);
// when func ret -1 = abandon parsing

    
class Xfc
{
public:

    typedef enum {
        eUnknown=0,
        eDisk,
        eFile,
        eDir,
        eRoot
    } ElementType;

    typedef int (*XmlParamForFileCallback)(
        string fileName, string *pParam, string *pValue, volatile const bool *pAbortFlag);

    typedef int (*XmlParamForFileChunkCallback)(
        const char *buf, unsigned int bufLen, bool isFirstChunk, bool isLastChunk,
        string *paramName, string *paramValue,
        volatile const bool *pAbortFlag);
    
    static string getVersionString();
  
    friend class EntityIterator;
    
    Xfc();

    ~Xfc();
    
    int getState() const throw();
    
    // creates a new catalog in memory
    // if there is a catalog, IT IS DELETED
    // the program must check first the state
    void createNew(string catalogName="") throw();
    
    // if a file is already loaded throw excp
    void loadFile(string path) throw (string);
    
    // lParams - param. to xmlSaveFormatFile
    // overwrite without warning
    void saveToFile(string path, int params) throw (string);
    
    xmlDocPtr getXmlDocPtr() throw (string);
    
    // when callBackFunc ret -1 = abandon parsing    
    void parseFileTree(ParserFuncType callBackFunc, void *pParam=NULL)
        throw (string);
    
    void parseDisk(ParserFuncType callBackFunc, string diskName, void *pParam=NULL);
    
    std::vector<string> getDiskList();
    
    bool isFileOrDir(xmlNodePtr) const throw();
    
    bool isDisk(xmlNodePtr) const throw();

    bool isRoot(xmlNodePtr) const throw();
        
    string getNameOfFile(xmlNodePtr) const throw (string);
    
    string getNameOfDisk(xmlNodePtr) const throw (string);
    
    string getNameOfElement(xmlNodePtr) const throw (string);
    
    void setNameOfElement(xmlNodePtr, string newName) throw (string);

    ElementType getTypeOfElement(xmlNodePtr pNode) const throw (string);

    // ret "" if no description
    // if lpDescriptionNode!=NULL - lpDescriptionNode will have the address of descr. node
    string getDescriptionOfNode(xmlNodePtr pNode, xmlNodePtr *pDescriptionNode=NULL) 
            const throw (string);
        
    // maxDepth - -1 
    void addPathToXmlTree(
        string lPath, int lMaxDepth, volatile const bool *pAbortFlag,
        std::vector<XmlParamForFileCallback>, std::vector<XmlParamForFileChunkCallback>,
        string diskId, string diskCategory="", string diskDescr="", string diskLabel="")
        throw (string);
    
    //string getXmlDocPath() const throw (string);
    
    // path is '/disk/file1/file2/...'
    // return NULL if it doesnt exist
    // path may or may not start with '/'
    xmlNodePtr getNodeForPath(string path) const throw ();

    /**
     * Get all the details of a node.
     * The node must be a file, dir ar disk.
     *
     * Map keys: description, size, cdate, sha1sum, sha256sum,
     * label0, label1, ..., label9.
     **/
    std::map<string, string> getDetailsForNode(xmlNodePtr lpNode) throw (string);

    xmlNodePtr addNewDiskToXmlTree(
        string diskName, string diskCategory="", string diskDescription="",
        string diskLabel="", string diskCDate="")
        throw (string);
    
    void addLabelTo(string path, string label) throw (string);
    
    void removeLabelFrom(string path, string label) throw (string);

    void addLabelRecTo(string path, string label) throw (string);

    void removeLabelRecFrom(string path, string label) throw (string);
    
    void setDescriptionOf(string path, string description) throw (string);
            
    string getCDate(string diskName, xmlNodePtr *pCDateNode=NULL) const throw (string);
    
    void setCDate(string diskName, string cDate) throw (string);
    
    // ret "" if no shasum
    string getChecksumOf(string path, string sumType="") const throw (string);
    
    // ret "" if no shasum
    string getChecksumOf(xmlNodePtr pNode, string sumType="") const throw (string);

    XfcEntity getEntityFromNode(xmlNodePtr lpNode) throw (string);

    int verifyDirectory(string catalogPath, string diskPath, unsigned int pathPrefixLen,
                        vector<EntityDiff> *pDiffs, volatile const bool *pAbortFlag=NULL);

private:    

    string getValueOfNode(xmlNodePtr) throw (string);
    
    // lPath - fs path
    xmlNodePtr addFileToXmlTree(
        xmlNodePtr lpParent, string lPath, vector<XmlParamForFileCallback>,
        vector<XmlParamForFileChunkCallback>, volatile const bool *pAbortFlag)
        throw (string);

    // lPath - fs path
    xmlNodePtr addDirToXmlTree(xmlNodePtr pParent, string path) throw (string);
    
    void parseRec(unsigned int depth, string path, xmlNodePtr pNode, 
                  ParserFuncType func, void *pParam)
            throw (string);               
    
    void recAddPathToXmlTree(
        xmlNodePtr pCurrentNode, string path, int level, int maxDepth,
        volatile const bool *pAbortFlag,
        std::vector<XmlParamForFileCallback>, std::vector<XmlParamForFileChunkCallback>,
        bool lSkipFirstLevel=false)
        throw (string);
    // root level is 0
    // maxDepth = -1 -> ignore max depth
    
    xmlNodePtr getNodeForPathRec(string path, xmlNodePtr pNode) const
            throw ();

    string getParamValueForNode(xmlNodePtr pNode, string param);
    
    xmlNodePtr getFirstFileNode(xmlNodePtr pNode);
    // ret null if no such node

    xmlNodePtr getFirstLabelNode(xmlNodePtr pNode);
    // ret null if no such node
    
    xmlNodePtr getDescriptionNode(xmlNodePtr pNode);
    // ret null if no such node

    xmlNodePtr getNameNode(xmlNodePtr pNode);
    // ret null if no such node
    
    bool isLabel(xmlNodePtr) const throw();
    
    bool isDescription(xmlNodePtr) const throw();
    
    string getCDate(xmlNodePtr pDiskNode, xmlNodePtr *pCDateNode=NULL)
        const throw (string);

    EntityDiff compareItems(
        string catalogName, XfcEntity &rEnt, string diskPath, unsigned int pathPrefixLen,
        bool &rShaWasMissing, volatile const bool *pAbortFlag=NULL);


    map<string, string> bufferCallbacks(string path,
                                        vector<XmlParamForFileChunkCallback> &cbList,
                                        volatile const bool *pAbortFlag);

    
    int mState;
    // 0 - catalog not initialized
    // 1 - initialized
    
    xmlDocPtr mpDoc;

};

#endif
