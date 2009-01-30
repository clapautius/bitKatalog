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

using namespace std;

/**
@author Tudor Marian Pristavu
*/


class XfcEntity;
class Xfc;


typedef int(*ParserFuncType)(unsigned int, std::string, Xfc&, xmlNodePtr, void*);
// when func ret -1 = abandon parsing

    
class Xfc
{
public:

    typedef enum {
        eUnknown=0,
        eDisk,
        eFile,
        eDir
    } ElementType;

    typedef int (*XmlParamForFileCallback)(
        std::string fileName,
        std::string &xmlParam, std::string &xmlValue, std::vector<std::string> &xmlAttrs);

    typedef int (*XmlParamForFileChunkCallback)(
        const char *buf, unsigned int bufLen, bool isFirstChunk, bool isLastChunk,
        std::string &xmlParam, std::string &xmlValue, std::vector<std::string> &xmlAttrs);
    
    static std::string getVersionString();
  
    friend class EntityIterator;
    
    Xfc();

    ~Xfc();
    
    int getState() const throw();
    
    void createNew(std::string lCatalogName="") throw();
    // creates a new catalog in memory
    // if there is a catalog, IT IS DELETED
    // the program must check first the state
    
    void loadFile(std::string lPath) throw (std::string);
    // if a file is already loaded throw excp
    
    void saveToFile(std::string lPath, int lParams)
            throw (std::string);
    // lParams - param. to xmlSaveFormatFile
    // overwrite without warning
    
    xmlDocPtr getXmlDocPtr() throw (std::string);
    
    void parseFileTree(ParserFuncType callBackFunc,
                      void *lpParam=NULL)
            throw (std::string);
    // when callBackFunc ret -1 = abandon parsing    
    
    void parseDisk(ParserFuncType callBackFunc,
                   std::string lDiskName, void *lpParam=NULL);
    
    std::vector<std::string> getDiskList();
    
    bool isFileOrDir(xmlNodePtr) const throw();
    
    bool isDisk(xmlNodePtr) const throw();
        
    std::string getNameOfFile(xmlNodePtr) const throw (std::string);
    
    std::string getNameOfDisk(xmlNodePtr) const throw (std::string);
    
    std::string getNameOfElement(xmlNodePtr) const throw (std::string);
    
    void setNameOfElement(xmlNodePtr, std::string newName) throw (std::string);

    ElementType getTypeOfElement(xmlNodePtr lpNode) const throw (std::string);

    std::string getDescriptionOfNode(xmlNodePtr lpNode, xmlNodePtr *lpDescriptionNode=NULL) 
            const throw (std::string);
    // ret "" if no description
    // if lpDescriptionNode!=NULL - lpDescriptionNode will have the address of descr. node
        
    void addPathToXmlTree(
        std::string lPath, int lMaxDepth,
        std::vector<XmlParamForFileCallback>, std::vector<XmlParamForFileChunkCallback>,
        std::string lDiskId, std::string lDiskCategory="", std::string lDiskDescription="",
        std::string lDiskLabel="")
        throw (std::string);
    // maxDepth - -1 
    
    //std::string getXmlDocPath() const throw (std::string);
    
    xmlNodePtr getNodeForPath(std::string lPath) const throw ();
    // path is '/disk/file1/file2/...'
    // return NULL if it doesnt exist
    // path may or may not start with '/'

    /**
     * Get all the details of a node.
     * The node must be a file, dir ar disk.
     *
     * Map keys: description, size, cdate, sha1sum, sha256sum,
     * label0, label1, ..., label9.
     **/
    std::map<std::string, std::string> getDetailsForNode(xmlNodePtr lpNode) throw (std::string);

    xmlNodePtr addNewDiskToXmlTree(
        std::string diskName, std::string diskCategory="", std::string diskDescription="",
        std::string diskLabel="", std::string diskCDate="")
        throw (std::string);
    
    void addLabelTo(std::string lPath, std::string lLabel)
        throw (std::string);
    
    void removeLabelFrom(std::string lPath, std::string lLabel)
        throw (std::string);

    void setDescriptionOf(std::string lPath, std::string lDescription)
        throw (std::string);
            
    std::string getCDate(std::string lDiskName, xmlNodePtr *lpCDateNode=NULL)
        const throw (std::string);
    
    void setCDate(std::string lDiskName, std::string lCDate)
        throw (std::string);
    
    std::string getChecksumOf(std::string lPath, std::string sumType="")
        const throw (std::string);
    // ret "" if no shasum
    
    std::string getChecksumOf(xmlNodePtr lpNode, std::string sumType="")
        const throw (std::string);
    // ret "" if no shasum

    XfcEntity getEntityFromNode(xmlNodePtr lpNode) throw (std::string);

private:    

    std::string getValueOfNode(xmlNodePtr) throw (std::string);
    
    xmlNodePtr addFileToXmlTree(
        xmlNodePtr lpParent, std::string lPath,
        std::vector<XmlParamForFileCallback>, std::vector<XmlParamForFileChunkCallback>)
        throw (std::string);
    // lPath - fs path

    xmlNodePtr addDirToXmlTree(xmlNodePtr lpParent, std::string lPath)
      throw (std::string);
    // lPath - fs path
    
    void parseRec(unsigned int lDepth, std::string lPath, xmlNodePtr lpNode, 
                  ParserFuncType lFunc, void *lpParam)
            throw (std::string);               
    
    void recAddPathToXmlTree(
        xmlNodePtr lpCurrentNode, std::string lPath, int lLevel, int lMaxDepth,
        std::vector<XmlParamForFileCallback>, std::vector<XmlParamForFileChunkCallback>,
        bool lSkipFirstLevel=false)
        throw (std::string);
    // root level is 0
    // maxDepth = -1 -> ignore max depth
    
    xmlNodePtr getNodeForPathRec(std::string lPath, xmlNodePtr lpNode) const
            throw ();

    string getParamValueForNode(xmlNodePtr pNode, string param);
    
    xmlNodePtr getFirstFileNode(xmlNodePtr lpNode);
    // ret null if no such node

    xmlNodePtr getFirstLabelNode(xmlNodePtr lpNode);
    // ret null if no such node
    
    xmlNodePtr getDescriptionNode(xmlNodePtr lpNode);
    // ret null if no such node

    xmlNodePtr getNameNode(xmlNodePtr lpNode);
    // ret null if no such node
    
    bool isLabel(xmlNodePtr) const throw();
    
    bool isDescription(xmlNodePtr) const throw();
    
    std::string getCDate(xmlNodePtr lpDiskNode, xmlNodePtr *lpCDateNode=NULL)
    const
    throw (std::string);

    
    int mState;
    // 0 - catalog not initialized
    // 1 - initialized
    
    xmlDocPtr mpDoc;

};

#endif
