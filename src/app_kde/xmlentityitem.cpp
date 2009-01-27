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


XmlEntityItem::XmlEntityItem(Q3ListView *lpListView, QString lS)
    : K3ListViewItem(lpListView, lS)
{
    mAlreadyOpened=false;
}


XmlEntityItem::XmlEntityItem(Q3ListViewItem *lpItem, QString lS,
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


void XmlEntityItem::setOpen(bool lOpen)
{
    std::vector<std::string> lVect;
    if(lOpen && !mAlreadyOpened)
    {
        XfcEntity lEnt;
        XmlEntityItem *lpItem;
        EntityIterator *lpTempIterator;
        EntityIterator *lpIterator=new EntityIterator(*mspCatalog, mpNode); // :fixme: - assert(mpNode)        
        while(lpIterator->hasMoreChildren())
        {
            lEnt=lpIterator->getNextChild();
            lVect=lEnt.getDetails();
            if(lVect.size()>0)
                lpItem=new XmlEntityItem(this, lEnt.getName().c_str(), lVect[0].c_str());
            else
                lpItem=new XmlEntityItem(this, lEnt.getName().c_str());
            lpItem->setXmlNode(lEnt.getXmlNode());
            if(lEnt.isFile())
                lpItem->setPixmap(0, *gpFilePixmap);
            if(lEnt.isDisk())
                lpItem->setPixmap(0, *gpDiskPixmap);            
            
            lpTempIterator=new EntityIterator(*mspCatalog, lEnt.getXmlNode());
            if(lpTempIterator->hasMoreChildren())
                lpItem->setExpandable(true);
            delete lpTempIterator;
        }
        mAlreadyOpened=true;
    }
    
    K3ListViewItem::setOpen(lOpen);
}

Xfc* XmlEntityItem::mspCatalog=NULL;
