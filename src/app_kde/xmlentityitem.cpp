/***************************************************************************
 *   Copyright (C) 2005 by Tudor Pristavu                                  *
 *   me@clapautius                                                         *
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
#include "xmlentityitem.h"
#include "main.h"
#include "xfcEntity.h"

#if defined(XFC_DEBUG)
    #include <iostream>
    using std::cout;
    using std::endl;
#endif

using std::string;
using std::vector;
using std::map;


XmlEntityItem::XmlEntityItem(Q3ListView *lpListView, QString lS,
                             QString lS1, QString lS2, QString lS3)
    : K3ListViewItem(lpListView, lS, lS1, lS2, lS3)
{
    mAlreadyOpened=false;
}


XmlEntityItem::XmlEntityItem(XmlEntityItem *lpItem, QString lS,
                             QString lS1, QString lS2, QString lS3)
    : K3ListViewItem(lpItem, lS, lS1, lS2, lS3)
{
    mAlreadyOpened=false;
}


XmlEntityItem::~XmlEntityItem()
{
}


void XmlEntityItem::setXmlNode(xmlNodePtr lpNode)
{
    mpNode=lpNode;
}


void XmlEntityItem::redisplay(bool first)
{
    XfcEntity lEnt(mpNode, mspCatalog);
    map<string, string> details;
    string labelsString;
    setText(NAME_COLUMN, str2qstr(lEnt.getName()));
    details=lEnt.getDetails();
    if( !details["description"].empty())
        setText(DESCRIPTION_COLUMN, details["description"].c_str());
    else
        setText(DESCRIPTION_COLUMN, "");
    labelsString=lEnt.getLabelsAsString();
    setText(LABELS_COLUMN, labelsString.c_str());

#if defined(XFC_DEBUG)
    cout<<":debug:"<<__FUNCTION__<<": this="<<this<<endl;
#endif
    
    XmlEntityItem *pCurrentItem;
    pCurrentItem=(XmlEntityItem*)firstChild();
    if (pCurrentItem) {
        pCurrentItem->redisplay(false);
    }
    if (!first) {
        pCurrentItem=(XmlEntityItem*)nextSibling();
        if (pCurrentItem) {
            pCurrentItem->redisplay(false);
        }
    }
}


void XmlEntityItem::setOpen(bool lOpen)
{
    map<string, string> details;
    if(lOpen && !mAlreadyOpened)
    {
        XfcEntity ent;
#if defined(XFC_DEBUG)
        XfcEntity thisEnt(mpNode, mspCatalog);
        cout<<":debug:"<<__FUNCTION__<<": opening entity "<<
            thisEnt.getName()<<endl;
#endif
        XmlEntityItem *lpItem;
        EntityIterator *lpTempIterator;
        // :fixme: - assert(mpNode)
        EntityIterator *lpIterator=new EntityIterator(*mspCatalog, mpNode);
        while(lpIterator->hasMoreChildren())
        {
            ent=lpIterator->getNextChild();
            details=ent.getDetails();
            QString labelsString=str2qstr(ent.getLabelsAsString());
            if( !details["description"].empty())
                lpItem=new XmlEntityItem(this, str2qstr(ent.getName()),
                                         details["description"].c_str(),
                                         labelsString);
            else
                lpItem=new XmlEntityItem(this, str2qstr(ent.getName()), "",
                                         labelsString);
            lpItem->setXmlNode(ent.getXmlNode());
#if defined(XFC_DEBUG)
            cout<<":debug:"<<__FUNCTION__<<": adding child: "<<
                ent.getName()<<endl;
            cout<<":debug:"<<__FUNCTION__<<":   labels: "<<
                qstr2cchar(labelsString)<<endl;
#endif
            switch (ent.getElementType()) {
            case Xfc::eFile:
                lpItem->setPixmap(0, *gpFilePixmap);
                break;
            case Xfc::eDir:
                lpItem->setPixmap(0, *gpDirPixmap);
                break;
            case Xfc::eDisk:
                lpItem->setPixmap(0, *gpDiskPixmap);
                break;
            default:
                throw std::string("You've just found a bug! (The application shouldn't be here)");
            }
            lpTempIterator=new EntityIterator(*mspCatalog, ent.getXmlNode());
            if(lpTempIterator->hasMoreChildren())
                lpItem->setExpandable(true);
            delete lpTempIterator;
        }
        mAlreadyOpened=true;
    }
    
    K3ListViewItem::setOpen(lOpen);
}

Xfc* XmlEntityItem::mspCatalog=NULL;
