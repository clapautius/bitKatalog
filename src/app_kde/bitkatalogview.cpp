/***************************************************************************
 *   Copyright (C) 2009 by Tudor Pristavu   *
 *   clapautiuAtGmaliDotCom   *
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


#include "bitkatalogview.h"

#include <Qt/qpainter.h>
#include <Qt/qlayout.h>

#include <kurl.h>

#include <kmimetypetrader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kmenu.h>
#include <kfiledialog.h>
#include <kprogressdialog.h>
#include <kinputdialog.h>
     
#include "bitkatalogview.h"
#include "xmlentityitem.h"
#include "main.h"
#include "detailsbox.h"
#include "outputwindow.h"
#include "verifythread.h"
#include "fs.h"
#include "misc.h"
#include "xfcEntity.h"

#if defined(MTP_DEBUG)
#include <iostream>
using namespace std;
#endif


bitKatalogView::bitKatalogView(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *top_layout = new QHBoxLayout(this);
    setupListView();
    mRootItem=new K3ListViewItem(mListView, "[ no catalog ]");
    mCatalog=NULL;
    top_layout->addWidget(mListView);
    
    /*
    connect(m_html, SIGNAL(setWindowCaption(const QString&)),
            this,   SLOT(slotSetTitle(const QString&)));
    connect(m_html, SIGNAL(setStatusBarText(const QString&)),
            this,   SLOT(slotOnUrl(const QString&)));
    */

}

bitKatalogView::~bitKatalogView()
{
}

void bitKatalogView::print(QPainter *p, int height, int width)
{
    // do the actual printing, here
    // p->drawText(etc..)
}

QString bitKatalogView::currentUrl()
{
    return m_html->url().url();
}

void bitKatalogView::openUrl(QString url)
{
    openUrl(KUrl(url));

}

void bitKatalogView::openUrl(const KUrl& url)
{
    //m_html->openURL(url);
    mCatalog=new Xfc;
    try {
        QString lPath=url.path();
        if (lPath!=QString::null)
            mCatalog->loadFile(url.path().toStdString());
        else {
            delete mCatalog;
            mCatalog=NULL;
            KMessageBox::error(this, QString("Invalid path: ")+lPath);
            return;
        }
    }
    catch (std::string e) {
        delete mCatalog;
        mCatalog=NULL;
        KMessageBox::error(this, QString("Error loading ")+url.path());
        return;
    }
    XmlEntityItem::mspCatalog=mCatalog;
    populateTree(mCatalog);
    if (mCatalog) {
        gCatalogState=2;
        gpMainWindow->setCatalogPath(url.path());
        gpMainWindow->updateTitle(false);
    }
}


void bitKatalogView::slotOnUrl(const QString& url)
{
    emit signalChangeStatusbar(url);
}


void bitKatalogView::slotSetTitle(const QString& title)
{
    emit signalChangeCaption(title);
}


void
bitKatalogView::setupListView()
{
    mListView=new K3ListView(this);    
    mListView->setRootIsDecorated(true);
    mListView->setAllColumnsShowFocus(true);
    mListView->addColumn("Name", 350);
    mListView->addColumn("Description", 250);
    connect(mListView, 
            SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
            this, SLOT(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)));
}    


void bitKatalogView::contextMenu(K3ListView *l, Q3ListViewItem *i, const QPoint &p)
{
    std::string lCompletePath;
    std::string lS;
    Q3ListViewItem *lpItem;

    KMenu *pContextMenu=new KMenu();
    QAction *pAct=NULL;
    pAct=pContextMenu->addAction("Details");
    connect(pAct, SIGNAL(triggered()), this, SLOT(details()));
    pContextMenu->addSeparator();
    
    mpCurrentItem=i;
    
    // this code appears also in details function - make a function from it
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    
    lCompletePath=mpCurrentItem->text(0).toStdString();
    lpItem=mpCurrentItem;
    while(1) {
        lpItem=lpItem->parent();
        if(lpItem==NULL)
            break;
        lS=lpItem->text(0).toStdString();
        lS+="/";
        lS+=lCompletePath;
        lCompletePath=lS;
    }
    mCurrentItemPath=lCompletePath;

    try {
        msgInfo("Context menu for: ", lCompletePath);
        // check if this is a disk
        XfcEntity lEnt(mCatalog->getNodeForPath(lCompletePath), mCatalog);
        if (lEnt.isDisk()) {
            pAct=pContextMenu->addAction("Verify disk");
            connect(pAct, SIGNAL(triggered()), this, SLOT(verifyDisk()));
            
            pAct=pContextMenu->addAction("Rename disk");
            connect(pAct, SIGNAL(triggered()), this, SLOT(renameDisk()));
            
            pAct=pContextMenu->addAction("Delete disk");
            connect(pAct, SIGNAL(triggered()), this, SLOT(deleteDisk()));
        }
        pContextMenu->exec(p);
    }
    catch(std::string e) {
        msgWarn("Hmmm, cannot display info about this item (exception in XfcEntity()! completePath=", lCompletePath);
        KMessageBox::error(this, "Hmmm, cannot display informations about this item!");
    }
}


void
bitKatalogView::details() throw()
{
    Q3ListViewItem *lpItem=NULL;
    std::string lCompletePath;
    std::string lS;
    
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    
    lpItem=mpCurrentItem;
    lCompletePath=mCurrentItemPath;

    if (lCompletePath!="/") {
        XfcEntity lEnt(mCatalog->getNodeForPath(lCompletePath), mCatalog);
    
        DetailsBox *lpDetailsBox=new DetailsBox(mCatalog, lCompletePath, &lEnt,
          mpCurrentItem);
        lpDetailsBox->exec(); 
    
        if (lpDetailsBox->catalogWasModified()) {
            gCatalogState=1;
            gpMainWindow->updateTitle(true);
        }
    }
    else {
        KMessageBox::information(this, "Special case - not ready yet");
    }
}


void 
bitKatalogView::verifyDisk() throw()
{
    Q3ListViewItem *lpItem;
    std::string lCompletePath;
    std::string lS;
    unsigned int ui;
    std::vector<std::string> lOnlyInCatalog;
    std::vector<std::string> lOnlyOnDisk;
    std::vector<std::string> lWrongSum;
    std::vector<std::string> lDifferent;
    
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    lpItem=mpCurrentItem;
    lCompletePath=mCurrentItemPath;
    KMessageBox::information(this, "Not properly tested (and size is not checked)");

    QString lDir = KFileDialog::getExistingDirectory(KUrl("/"), this, QString("Path to verify"));
    if ("" == lDir) // cancel
        return;
    
    VerifyThread *lpVerifyThread=new VerifyThread(
        mCatalog, lCompletePath, lDir.toStdString(), lOnlyInCatalog,
        lOnlyOnDisk, lWrongSum, lDifferent);
        
    KProgressDialog *lpProgress=new KProgressDialog(this, "Verifying ...", "");
    //lpProgress->progressBar()->setTotalSteps(0);
    lpProgress->progressBar()->setRange(0, 0);
    //lpProgress->progressBar()->setPercentageVisible(false);
    lpProgress->progressBar()->setTextVisible(false);
    lpProgress->setMinimumDuration(1000);
    lpProgress->setAutoClose(true);
    lpProgress->setAllowCancel(true);
    lpProgress->setButtonText("Stop");
    lpProgress->setLabelText(QString(""));
    int i=0;

    lpVerifyThread->start();
    while (!lpVerifyThread->isFinished()) {
        std::string lLabel;
        std::string lOldLabel;
        lLabel=lpVerifyThread->getCurrentFile();
        if (lLabel!=lOldLabel) {
            if (lLabel.size()>60) {
                std::string lS=lLabel.substr(0, 28);
                lS+="....";
                lS+=lLabel.substr(lLabel.size()-28);
                lLabel=lS;
            }
            lpProgress->setLabelText(QString(lLabel.c_str()));
            lOldLabel=lLabel;
        }
        if (lpProgress->wasCancelled()) {
            lpVerifyThread->stopThread();
            KProgressDialog *lpProgress2=new KProgressDialog(this, "Waiting ...", "");
            lpProgress2->progressBar()->setRange(0, 0);
            lpProgress2->progressBar()->setTextVisible(false);
            lpProgress2->setMinimumDuration(1000);
            lpProgress2->setAutoClose(false);
            lpProgress2->setAllowCancel(false);
            lpProgress2->setLabelText(QString("Waiting for verify thread to finish"));
            int i=0;
            while (!lpVerifyThread->isFinished()) {
                lpProgress2->progressBar()->setValue(i+=10);
                gpApplication->processEvents();
                usleep(100000);
            }
            delete lpProgress2;
            break;
        }
        lpProgress->progressBar()->setValue(i+=10);
        gpApplication->processEvents();
        usleep(250000);
    }

    int rc=lpVerifyThread->getResultCode();

    delete lpProgress;
    delete lpVerifyThread;
    OutputWindow lResults;
    bool lAtLeastOneError=false;

    if(2 == rc || 4 == rc)
        KMessageBox::information(this, "Some (or all) files did not have sha1 sum.");

    if (lOnlyInCatalog.size()>0) {
        lAtLeastOneError=true;
        lResults.addText( std::string("Only in catalog (missing from disk):"));
        for(ui=0; ui<lOnlyInCatalog.size(); ui++)
            lResults.addText( lOnlyInCatalog[ui]);
    }
    if (lOnlyOnDisk.size()>0) {
        lAtLeastOneError=true;
        lResults.addText( std::string("Only on disk (missing from catalog):"));
        for(ui=0; ui<lOnlyOnDisk.size(); ui++)
            lResults.addText( lOnlyOnDisk[ui]);
    }
    if (lDifferent.size()>0) {
        lAtLeastOneError=true;
        lResults.addText( std::string("Different"));
        for(ui=0; ui<lDifferent.size(); ui++)
            lResults.addText( lDifferent[ui]);
    }
    if (lWrongSum.size()>0) {
        lAtLeastOneError=true;
        lResults.addText( std::string("Wrong sum"));
        for(ui=0; ui<lWrongSum.size(); ui++)
            lResults.addText( lWrongSum[ui]);
    }
    if(!lAtLeastOneError)
        lResults.addText( "Disk OK");
    lResults.exec();
}


void
bitKatalogView::renameDisk() throw()
{
  Q3ListViewItem *lpItem;
  std::string lCompletePath;
  QString oldName, newName;
  
  if (mCatalog==NULL) {
    KMessageBox::error(this, "No catalog!");
    return;
  }
  
  lpItem=mpCurrentItem;
  lCompletePath=mCurrentItemPath;
  
  if (lCompletePath!="/") {
    XfcEntity lEnt(mCatalog->getNodeForPath(lCompletePath), mCatalog);
    oldName=lEnt.getName().c_str();

    bool lRetButton;
    newName=KInputDialog::getText("Rename disk", "Name: ", oldName, &lRetButton);
    if (lRetButton && newName != oldName) {
        lEnt.setName(newName.toStdString());
        lpItem->setText(NAME_COLUMN, newName);
        gCatalogState=1;
        gpMainWindow->updateTitle(true);
    }
  }
  else {
    // :fixme:
    KMessageBox::information(this, "Special case - not ready yet");
  }
}


void
bitKatalogView::deleteDisk() throw()
{
  Q3ListViewItem *lpItem;
  std::string lCompletePath;
  std::string lOldName, lNewName;
  
  if (mCatalog==NULL) {
    KMessageBox::error(this, "No catalog!");
    return;
  }
  lpItem=mpCurrentItem;
  lCompletePath=mCurrentItemPath;
  
  // :todo:
  KMessageBox::information(this, "Not ready yet");
}


Xfc*
bitKatalogView::getCatalog()
{
    return mCatalog;
}


bool
bitKatalogView::catalogWasModified() // :todo: - remove this function
{
    return mModifiedCatalog;
}


void
bitKatalogView::resetModifiedFlag() // :todo: - remove this function
{
    mModifiedCatalog=false;
}


void
bitKatalogView::populateTree(Xfc *mpCatalog)
{
    mListView->clear();
    mRootItem=new K3ListViewItem(mListView, "/"); // :fixme: - use catalog name?

    EntityIterator *lpIterator;
    EntityIterator *lpTempIterator;
    std::string lS="/";
    lpIterator=new EntityIterator(*mpCatalog, lS);
    XfcEntity lEnt;
    XmlEntityItem *lpItem;
    std::vector<std::string> lVect;
    
    while (lpIterator->hasMoreChildren()) {
        lEnt=lpIterator->getNextChild();
        lVect=lEnt.getDetails();
        if (lVect.size()>0)
            lpItem=new XmlEntityItem(mRootItem, lEnt.getName().c_str(), lVect[0].c_str());
        else
            lpItem=new XmlEntityItem(mRootItem, lEnt.getName().c_str());
        //lpItem=new XmlEntityItem(mRootItem, lEnt.getName().c_str());
        lpItem->setXmlNode(lEnt.getXmlNode());
        lpItem->setPixmap(0, *gpDiskPixmap);
        lpTempIterator=new EntityIterator(*mpCatalog, lEnt.getXmlNode());
        if (lpTempIterator->hasMoreChildren())
            lpItem->setExpandable(true);
        delete lpTempIterator;
    }
}    


void bitKatalogView::setCatalog(Xfc *lpNewCatalog)
{
    if(mCatalog!=NULL)
        delete mCatalog;
    mCatalog=lpNewCatalog;
    XmlEntityItem::mspCatalog=mCatalog;
    populateTree(mCatalog);
}
