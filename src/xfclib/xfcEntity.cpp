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
#include "xfcEntity.h"
#include "xfc.h"

/*
XfcEntity::XfcEntity()
    throw ()
{
    mpXmlNode=NULL;
}
*/


XfcEntity::XfcEntity(xmlNodePtr lpNode, Xfc *pXfc)
{
  mpXmlNode=lpNode;
  mpXfc=pXfc;
}


void
XfcEntity::setName(std::string lS)
{
  checkValidData();
  return mpXfc->setNameOfElement(mpXmlNode, lS);
}


std::string
XfcEntity::getName() const
{
  checkValidData();
  return mpXfc->getNameOfElement(mpXmlNode);
}


xmlNodePtr
XfcEntity::getXmlNode() const
{
    return mpXmlNode;
}


Xfc::ElementType
XfcEntity::getElementType() const
{
    checkValidData();
    return mpXfc->getTypeOfElement(mpXmlNode);
}


bool
XfcEntity::isFileOrDir() const
{
  checkValidData();
  return mpXfc->isFileOrDir(mpXmlNode);
}
        

bool
XfcEntity::isDisk() const
{
  checkValidData();
  return mpXfc->isDisk(mpXmlNode);
}


int
XfcEntity::getTypeOfFile() const
  throw (std::string)
// 0 - unknown
// 1 - regular file
// 2 - directory
{
    Xfc::ElementType type;
    if (!isFileOrDir())
        throw std::string("Not a file/dir");
    type=mpXfc->getTypeOfElement(mpXmlNode);
    switch(type) {
    case Xfc::eFile:
        return 1;
    case Xfc::eDir:
        return 2;
    default:
        return 0;
    }
}


std::vector<std::string>
XfcEntity::getDetails() 
{
  checkValidData();
  return mpXfc->getDetailsForNode(mpXmlNode);
}

        
void
XfcEntity::setParams(std::vector<std::string> lVect, bool lIsFile, bool lIsDisk)
{

}


void
XfcEntity::checkValidData(bool xfcMustBeValid) const
  throw (std::string)
{
  if (mpXmlNode==NULL)
    throw std::string("Xml node is NULL");
  if (xfcMustBeValid && mpXfc==NULL)
    throw std::string("Xfc is NULL");
}



EntityIterator::EntityIterator(Xfc &lrXfc, std::string lPath)
         throw (std::string)
    : mrXfc(lrXfc)
{
    mpParentNode=mrXfc.getNodeForPath(lPath);
    if (mpParentNode)
        mpCurrentNode=mpParentNode->xmlChildrenNode;
    else
        mpCurrentNode=NULL;
}    


EntityIterator::EntityIterator(Xfc &lrXfc, xmlNodePtr lpNode)
            throw (std::string)
    : mrXfc(lrXfc)
{
    mpParentNode=lpNode;
    mpCurrentNode=mpParentNode->xmlChildrenNode;
}
    

bool EntityIterator::hasMoreChildren()
{
    while (mpCurrentNode!=NULL) {
        if (mrXfc.isFileOrDir(mpCurrentNode) || mrXfc.isDisk(mpCurrentNode))
           return true;
        mpCurrentNode=mpCurrentNode->next;
    }    
    return false;
}


XfcEntity EntityIterator::getNextChild()
{
    if (mpCurrentNode) {
        XfcEntity lEnt(mpCurrentNode, &mrXfc);
        mpCurrentNode=mpCurrentNode->next;
        hasMoreChildren(); // go to next child
        return lEnt;
    }
    else
        // :fixme: - make o proper exception hierarchy
        throw std::string("No more children in EntityIterator");
}
