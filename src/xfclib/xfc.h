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

#define SHA256LABEL "sha256"
#define SHA1LABEL "sha1"

#define READ_BUF_LEN 102400

class XfcEntity;
class Xfc;


/**
 * Type definition for the function used for recursive parsing of various trees
 * (catalogs, disks, etc.).
 * When func ret -1 = abandon parsing
 * It is called for elements of type dir, disk, file or root.
 **/
typedef int(*ParserFuncType)(uint depth, std::string path, Xfc& rCatalog,
                             xmlNodePtr pNode, void *pParam);

    
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
        std::string fileName, std::string *pParam, std::string *pValue,
        volatile const bool *pAbortFlag);

    typedef int (*XmlParamForFileChunkCallback)(
        const char *buf, uint len, bool isFirstChunk, bool isLastChunk,
        std::string *paramName, std::string *paramValue,
        volatile const bool *pAbortFlag);
    
    static std::string getVersionString();
  
    friend class EntityIterator;
    
    Xfc();

    ~Xfc();
    
    int getState() const throw();
    
    // creates a new catalog in memory
    // if there is a catalog, IT IS DELETED
    // the program must check first the state
    void createNew(std::string catalogName="") throw();
    
    // if a file is already loaded throw excp
    void loadFile(std::string path) throw (std::string);
    
    // lParams - param. to xmlSaveFormatFile
    // overwrite without warning
    void saveToFile(std::string path, int params) throw (std::string);
    
    xmlDocPtr getXmlDocPtr() throw (std::string);
    
    void parseFileTree(ParserFuncType callBackFunc, void *pParam=NULL)
        throw (std::string);
    
    void parseDisk(ParserFuncType callBackFunc, std::string diskName, void *pParam=NULL);
    
    std::vector<std::string> getDiskList();
    
    bool isFileOrDir(xmlNodePtr) const throw();
    
    bool isDisk(xmlNodePtr) const throw();

    bool isRoot(xmlNodePtr) const throw();

    bool isDirDiskFileOrRoot(xmlNodePtr) const throw();
    
    std::string getNameOfFile(xmlNodePtr) const throw (std::string);
    
    std::string getNameOfDisk(xmlNodePtr) const throw (std::string);
    
    std::string getNameOfElement(xmlNodePtr) const throw (std::string);
    
    void setNameOfElement(xmlNodePtr, std::string newName) throw (std::string);

    ElementType getTypeOfElement(xmlNodePtr pNode) const throw (std::string);

    // ret "" if no description
    // if lpDescriptionNode!=NULL - lpDescriptionNode will have the address of descr. node
    std::string getDescriptionOfNode(xmlNodePtr pNode,
                                     xmlNodePtr *pDescriptionNode=NULL)
        const throw (std::string);
        
    // maxDepth - -1 
    void addPathToXmlTree(
        std::string lPath, int lMaxDepth, volatile const bool *pAbortFlag,
        std::vector<XmlParamForFileCallback>,
        std::vector<XmlParamForFileChunkCallback>,
        std::string diskId, std::string diskCat="", std::string diskDescr="",
        std::string diskLabel="")
        throw (std::string);
    
    // path is '/disk/file1/file2/...'
    // return NULL if it doesnt exist
    // path may or may not start with '/'
    xmlNodePtr getNodeForPath(std::string path) const throw ();

    std::map<std::string, std::string> getDetailsForNode(xmlNodePtr lpNode)
        throw (std::string);

    std::vector<std::string> getLabelsForNode(xmlNodePtr lpNode)
        throw (std::string);

    xmlNodePtr addNewDiskToXmlTree(
        std::string diskName, std::string diskCategory="",
        std::string diskDescription="", std::string diskLabel="",
        std::string diskCDate="")
        throw (std::string);
    
    void addLabelTo(std::string path, std::string label) throw (std::string);
    
    void removeLabelFrom(std::string path, std::string label)
        throw (std::string);

    void addLabelRecTo(std::string path, std::string label) throw (std::string);

    void removeLabelRecFrom(std::string path, std::string label)
        throw (std::string);
    
    void setDescriptionOf(std::string path, std::string description) throw (std::string);
            
    std::string getCDate(std::string diskName, xmlNodePtr *pCDateNode=NULL)
        const throw (std::string);
    
    void setCDate(std::string diskName, std::string cDate) throw (std::string);
    
    // ret "" if no shasum
    std::string getChecksumOf(std::string path, std::string sumType="")
        const throw (std::string);
    
    // ret "" if no shasum
    std::string getChecksumOf(xmlNodePtr pNode, std::string sumType="")
        const throw (std::string);

    XfcEntity getEntityFromNode(xmlNodePtr lpNode) throw (std::string);

    int verifyDirectory(
        std::string catalogPath, std::string diskPath, uint pathPrefixLen,
        std::vector<EntityDiff> *pDiffs, volatile const bool *pAbortFlag=NULL);

private:    

    std::string getValueOfNode(xmlNodePtr) throw (std::string);
    
    // lPath - fs path
    xmlNodePtr
        addFileToXmlTree(xmlNodePtr lpParent, std::string lPath,
                         std::vector<XmlParamForFileCallback>,
                         std::vector<XmlParamForFileChunkCallback>,
                         volatile const bool *pAbortFlag)
        throw (std::string);

    // lPath - fs path
    xmlNodePtr addDirToXmlTree(xmlNodePtr pParent, std::string path)
        throw (std::string);
    
    void parseRec(uint depth, std::string path, xmlNodePtr pNode,
                  ParserFuncType func, void *pParam)
            throw (std::string);               
    
    /// root level is 0
    /// maxDepth = -1 -> ignore max depth
    void recAddPathToXmlTree(
        xmlNodePtr pCurrentNode, std::string path, int level, int maxDepth,
        volatile const bool *pAbortFlag, std::vector<XmlParamForFileCallback>,
        std::vector<XmlParamForFileChunkCallback>, bool lSkipFirstLevel=false)
        throw (std::string);
    
    xmlNodePtr getNodeForPathRec(std::string path, xmlNodePtr pNode) const
            throw ();

    std::string getParamValueForNode(xmlNodePtr pNode, std::string param);
    
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
    
    std::string getCDate(xmlNodePtr pDiskNode, xmlNodePtr *pCDateNode=NULL)
        const throw (std::string);

    EntityDiff compareItems(
        std::string catalogName, XfcEntity &rEnt, std::string diskPath,
        uint pathPrefixLen, bool &rShaMissing,
        volatile const bool *pAbortFlag=NULL);


    std::map<std::string, std::string> bufferCallbacks(
        std::string path, std::vector<XmlParamForFileChunkCallback> &cbList,
        volatile const bool *pAbortFlag);

    
    int mState;
    // 0 - catalog not initialized
    // 1 - initialized
    
    xmlDocPtr mpDoc;

};

#endif
