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
#if !defined(_XFCENTITY_H_)
#define _XFCENTITY_H_

#include <map>
#include <string>
#include <vector>

#include <libxml/parser.h>

#include "xfc.h"

/// wrapper class for an xml item from the catalog
/// may be a disk or a file
class XfcEntity
{
   public:
    XfcEntity(xmlNodePtr pNode, Xfc *pXfc);

    XfcEntity &operator=(const XfcEntity &rhs);

    std::string getName() const;

    xmlNodePtr getXmlNode() const;

    void setName(std::string);

    // void setXmlNode(xmlNodePtr) throw();

    Xfc::ElementType getElementType() const;

    // DEPRECATED
    bool isFileOrDir() const;

    // DEPRECATED
    bool isDisk() const;

    // DEPRECATED
    int getTypeOfFile() const throw(std::string);
    // 0 - regular file
    // 1 - directory

    std::map<std::string, std::string> getDetails();

    std::vector<std::string> getLabels();

    /* not used atm
    void setParams(std::vector<std::string> lVect, bool lIsFile=false, bool
    lIsDisk=false);
    */

    std::string getLabelsAsString() const;

    void setComment(const std::string &comment);

    std::string getComment() const;

    void setStorageDev(const std::string &storage_dev);

    std::string getStorageDev() const;

    void setOrAddParam(const std::string &elt_name, const std::string &elt_value);

    /**
     * @return The value of the specified parameter (sub-element), or empty string if no
     * such sub-element exists.
     */
    std::string getParamValue(const std::string &elt_name) const;

   private:
    void checkValidData(bool xfcMustBeValid = true) const throw(std::string);

    Xfc *mpXfc;

    xmlNodePtr mpXmlNode;

    // std::string mName;

    // bool mIsFile;

    // bool mIsDisk;
};

class EntityIterator
{
   public:
    EntityIterator(Xfc &lrXfc, std::string lPath) throw(std::string);

    EntityIterator(Xfc &lrXfc, xmlNodePtr lpNode) throw(std::string);

    bool hasMoreChildren();

    XfcEntity getNextChild();

   private:
    xmlNodePtr mpParentNode;
    xmlNodePtr mpCurrentNode;
    Xfc &mrXfc;
};

#endif
