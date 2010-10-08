/***************************************************************************
 *   Copyright (C) 2010 by Tudor Marian Pristavu                           *
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
#include "localfiles.h"

#include <qpainter.h>
#include <qlayout.h>
#include <vector>
#include <string>
#include <klineedit.h>
#include <QHeaderView>
#include <kmessagebox.h>
#include <kinputdialog.h>

#include "misc.h"
#include "main.h"

using std::string;
using std::vector;



LocalFilesBox::LocalFilesBox(vector<QFileInfo> &rFiles)
    : KPageDialog()
{
    setCaption(QString("Files on local storage"));
    setButtons(KDialog::Ok);
    setModal(true);
    mFiles=rFiles;
    layout();
    connectButtons();
}


LocalFilesBox::~LocalFilesBox()
{
}


void
LocalFilesBox::connectButtons()
{
} 


void LocalFilesBox::layout()
{
    resize(600,500);
    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Local files"));
    pPage1->setHeader(QString("Files on local storage"));

    QTreeWidget *pFileList=new QTreeWidget(pBox1);
    pFileList->setColumnCount(1);
    pFileList->sortByColumn(0, Qt::AscendingOrder);
    pFileList->setSortingEnabled(true);
    pFileList->setRootIsDecorated(false);
    pFileList->header()->hide();
    pFileList->setAlternatingRowColors(true);
    for(uint i=0; i<mFiles.size(); i++) {
        gkLog<<xfcDebug<<"adding local element to list, complete path: ";
        gkLog<<mFiles[i].filePath().toStdString()<<eol;
        gkLog<<xfcDebug<<"path="<<mFiles[i].path().toStdString();
        gkLog<<", fileName="<<mFiles[i].fileName().toStdString()<<eol;
        QTreeWidgetItem *pItem=NULL;
        if (mFiles[i].isDir()) {
            pItem=new QTreeWidgetItem(
                QStringList(mFiles[i].filePath()+"\n"));
            pItem->setIcon(0, KIcon("folder"));
            gkLog<<xfcDebug<<"element is dir"<<eol;
        }
        else {
            pItem=new QTreeWidgetItem(
                QStringList(mFiles[i].path()+"/\n"+mFiles[i].fileName()));
            pItem->setIcon(0, KIcon("application-octet-stream"));
            gkLog<<xfcDebug<<"element is not dir"<<eol;
        }
        pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        pFileList->addTopLevelItem(pItem);
    }
    pFileList->resizeColumnToContents(0);
}


void LocalFilesBox::accept()
{
    KPageDialog::accept();
} 