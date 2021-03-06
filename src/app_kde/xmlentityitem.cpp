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

// :tmp:
#include "kmessagebox.h"

#if defined(XFC_DEBUG)
#include <iostream>
using std::cout;
using std::endl;
#endif

using std::string;
using std::vector;
using std::map;

XmlEntityItem::XmlEntityItem(QTreeWidget *pTreeWidget, QStringList list)
    : QTreeWidgetItem(pTreeWidget, list)
{
    mAlreadyOpened = false;
}

XmlEntityItem::XmlEntityItem(XmlEntityItem *lpItem, QStringList list)
    : QTreeWidgetItem(lpItem, list)
{
    mAlreadyOpened = false;
}

XmlEntityItem::~XmlEntityItem() {}

void XmlEntityItem::setXmlNode(xmlNodePtr lpNode) { mpNode = lpNode; }

void XmlEntityItem::redisplay(bool first)
{
    XfcEntity lEnt(mpNode, mspCatalog);
    map<string, string> details = mspCatalog->getDetailsForNode(mpNode);
    updateVisualTexts(lEnt, details);

#if defined(XFC_DEBUG)
    cout << ":debug:" << __FUNCTION__ << ": this=" << this << endl;
#endif

    XmlEntityItem *pCurrentItem;
    pCurrentItem = (XmlEntityItem *)child(0);
    if (pCurrentItem) {
        pCurrentItem->redisplay(false);
    }
    if (!first) {
        pCurrentItem = (XmlEntityItem *)nextSibling();
        if (pCurrentItem) {
            pCurrentItem->redisplay(false);
        }
    }
}

void XmlEntityItem::setOpened(bool lOpen)
{
    map<string, string> details;
    gkLog << xfcDebug << __FUNCTION__ << (lOpen ? ": expanding" : ": collapsing")
          << " entity " << xmlName() << eol;
    if (lOpen && !mAlreadyOpened) {
        XmlEntityItem *lpItem;
        EntityIterator *lpTempIterator;
        // :fixme: - assert(mpNode)
        EntityIterator *lpIterator = new EntityIterator(*mspCatalog, mpNode);
        while (lpIterator->hasMoreChildren()) {
            XfcEntity ent = lpIterator->getNextChild();
            details = ent.getDetails();
            QString labelsString = str2qstr(ent.getLabelsAsString());
            QStringList columns;
            gpView->fillColumnValues(columns, str2qstr(ent.getName()),
                                     str2qstr(details["description"]), labelsString, ent);
            lpItem = new XmlEntityItem(this, columns);
            lpItem->setXmlNode(ent.getXmlNode());
#if defined(XFC_DEBUG)
            cout << ":debug:" << __FUNCTION__ << ": adding child: " << ent.getName()
                 << endl;
            cout << ":debug:" << __FUNCTION__
                 << ":   labels: " << qstr2cchar(labelsString) << endl;
#endif
            switch (ent.getElementType()) {
                case Xfc::eFile:
                    lpItem->setIcon(0, *gpFileIcon);
                    break;
                case Xfc::eDir:
                    lpItem->setIcon(0, *gpDirIcon);
                    break;
                case Xfc::eDisk:
                    lpItem->setIcon(0, *gpDiskIcon);
                    break;
                default:
                    throw std::string(
                        "You've just found a bug! (The application shouldn't be here)");
            }
            lpTempIterator = new EntityIterator(*mspCatalog, ent.getXmlNode());
            if (lpTempIterator->hasMoreChildren())
                lpItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            delete lpTempIterator;
            lpItem->updateVisualTexts(ent, details);
        }
        mAlreadyOpened = true;
    }
    QTreeWidgetItem::setExpanded(lOpen);
}

QTreeWidgetItem *XmlEntityItem::nextSibling()
{
    QTreeWidgetItem *pParent = parent();
    QTreeWidgetItem *next = NULL;
    if (pParent) {
        next = pParent->child(pParent->indexOfChild(this) + 1);
    } else {
        QTreeWidget *pTreeWidget = treeWidget();
        next = pTreeWidget->topLevelItem(pTreeWidget->indexOfTopLevelItem(this) + 1);
    }
    return next;
}

std::string XmlEntityItem::xmlName() const
{
    return mspCatalog->getNameOfElement(mpNode);
}


void XmlEntityItem::updateVisualTexts(const XfcEntity &ent,
                                      const map<string, string> &details,
                                      bool skip_columns)
{
    if (!skip_columns) {
        // columns
        string labelsString = ent.getLabelsAsString();
        setText(gpView->getNameColumnIndex(), str2qstr(ent.getName()));
        std::map<string, string>::const_iterator elt_pos =
          details.find("description");
        if (elt_pos != details.end()) {
            setText(gpView->getDescriptionColumnIndex(), str2qstr(elt_pos->second));
        }
        setText(gpView->getLabelsColumnIndex(), str2qstr(labelsString));
        setText(gpView->getStorageDevColumnIndex(), ent.getStorageDev().c_str());
    }
    updateTooltip(ent, details);
}

void XmlEntityItem::updateTooltip(const XfcEntity &ent, const map<string, string> &details)
{
    QString tooltip;
    if (ent.getName().empty()) {
        tooltip = "Unnamed entity";
    } else {
        tooltip = ent.getName().c_str();
    }
    tooltip += "\n\nDescription: ";
    std::map<string, string>::const_iterator elt_pos = details.find("description");
    if(elt_pos != details.end()) {
        tooltip += elt_pos->second.c_str();
    }

    tooltip += "\nComment: ";
    tooltip += ent.getComment().c_str();

    tooltip += "\nStorage dev.: ";
    tooltip += ent.getStorageDev().c_str();

    elt_pos = details.find("cdate");
    if (elt_pos != details.end() && !(elt_pos->second.empty())) {
        tooltip += "\nC. date: ";
        tooltip += elt_pos->second.c_str();
    }
    elt_pos = details.find("size");
    if (elt_pos != details.end() && !(elt_pos->second.empty())) {
        tooltip += "\nSize: ";
        tooltip += elt_pos->second.c_str();
    }

    for (int i = 0; i < gpView->getNoColumns(); i++) {
        setToolTip(i, tooltip);
    }
}


Xfc *XmlEntityItem::mspCatalog = NULL;
