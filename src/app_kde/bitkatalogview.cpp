/***************************************************************************
 *   Copyright (C) 2009 by Tudor Marian Pristavu                           *
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


// QPainter *p, int height, int width)
void
bitKatalogView::print(QPainter *, int, int)
{
    // do the actual printing, here
    // p->drawText(etc..)
}


QString
bitKatalogView::currentUrl()
{
    return m_html->url().url();
}


void
bitKatalogView::openUrl(QString url)
{
    openUrl(KUrl(url));

}


void
bitKatalogView::openUrl(const KUrl& url)
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
    mListView->addColumn("Name", 320);
    mListView->addColumn("Description", 250);
    mListView->addColumn("Labels", 320);
    connect(mListView, 
            SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)),
            this, SLOT(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint &)));
}    


void bitKatalogView::contextMenu(K3ListView *, Q3ListViewItem *i, const QPoint &p)
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
    std::string completePath;
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    completePath=mCurrentItemPath;
    if (completePath!="/") {
        XfcEntity lEnt(mCatalog->getNodeForPath(completePath), mCatalog);
        DetailsBox *pDetailsBox=new DetailsBox(mCatalog, completePath, &lEnt,
          mpCurrentItem);
        pDetailsBox->exec(); 
        if (pDetailsBox->catalogWasModified()) {
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
    std::string completePath;
    std::string str;
    vector<EntityDiff> differences;
    
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    completePath=mCurrentItemPath;

    QString previousDir;
    previousDir=gpConfig->group("").readEntry("LastDirVerify", "/");
    QString dir=KFileDialog::getExistingDirectory(KUrl(previousDir), this, QString("Path to verify"));
    if ("" == dir) // cancel
        return;
    gpConfig->group("").writeEntry("LastDirVerify", dir);
    gpConfig->sync();

    VerifyThread *pVerifyThread=new VerifyThread(
        mCatalog, completePath, dir.toStdString(), &differences);
        
    KProgressDialog *pProgress=new KProgressDialog(this, "Verifying ...", "");
    //pProgress->progressBar()->setTotalSteps(0);
    pProgress->progressBar()->setRange(0, 0);
    //pProgress->progressBar()->setPercentageVisible(false);
    pProgress->progressBar()->setTextVisible(false);
    // show it immediately
    pProgress->setMinimumDuration(0);
    pProgress->setAutoClose(true);
    pProgress->setAllowCancel(true);
    pProgress->setButtonText("Stop");
    pProgress->setLabelText(QString(""));
    int i=0;

    pVerifyThread->start();
    while (!pVerifyThread->isFinished()) {
        std::string lLabel;
        std::string lOldLabel;
        lLabel=pVerifyThread->getCurrentFile();
        if (lLabel!=lOldLabel) {
            if (lLabel.size()>60) {
                str=lLabel.substr(0, 28);
                str+="....";
                str+=lLabel.substr(lLabel.size()-28);
                lLabel=str;
            }
            pProgress->setLabelText(QString(lLabel.c_str()));
            lOldLabel=lLabel;
        }
        if (pProgress->wasCancelled()) {
            pVerifyThread->stopThread();
            KProgressDialog *pProgress2=new KProgressDialog(this, "Waiting ...", "");
            pProgress2->progressBar()->setRange(0, 0);
            pProgress2->progressBar()->setTextVisible(false);
            pProgress2->setMinimumDuration(0);
            pProgress2->setAutoClose(false);
            pProgress2->setAllowCancel(false);
            pProgress2->setLabelText(QString("Waiting for verify thread to finish"));
            int i=0;
            while (!pVerifyThread->isFinished()) {
                pProgress2->progressBar()->setValue(i+=10);
                gpApplication->processEvents();
                usleep(100000);
            }
            delete pProgress2;
            break;
        }
        pProgress->progressBar()->setValue(i+=10);
        gpApplication->processEvents();
        usleep(250000);
    }

    int rc=pVerifyThread->getResultCode();
    delete pProgress;
    delete pVerifyThread;
    if(2 == rc)
        KMessageBox::information(this, "Some (or all) files did not have checksums.");

    if (differences.size()>0) {
        string str1, str2;
        DiffOutputWindow *pResults=new DiffOutputWindow(differences.size());
        for (unsigned int i=0; i<differences.size(); i++) {
            switch (differences[i].type) {
            case eDiffOnlyInCatalog:
                // strip disk name
                pResults->addText(differences[i].name.substr(completePath.length()+1),
                                  "!", "-");
                break;
            case eDiffOnlyOnDisk:
                pResults->addText("-", "!", differences[i].name);
                break;
            case eDiffSize:
                str1=differences[i].name+"\n (size: "+differences[i].catalogValue+")";
                str2=differences[i].name+"\n (size: "+differences[i].diskValue+")";
                pResults->addText(str1, " ! size ", str2, 2);
                str1.clear();
                str2.clear();
                break;
            case eDiffSha256Sum:
                str1=differences[i].name+"\n (sha256: "+differences[i].catalogValue+")";
                str2=differences[i].name+"\n (sha256: "+differences[i].diskValue+")";
                pResults->addText(str1, " ! sha256 ", str2, 2);
                str1.clear();
                str2.clear();
                break;
            case eDiffSha1Sum:
                str1=differences[i].name+"\n (sha1: "+differences[i].catalogValue+")";
                str2=differences[i].name+"\n (sha1: "+differences[i].diskValue+")";
                pResults->addText(str1, " ! sha1 ", str2, 2);
                str1.clear();
                str2.clear();
                break;
            case eDiffErrorOnDisk:
                str1=differences[i].name;
                str2=differences[i].name+"\n error reading details from disk: "+differences[i].diskValue;
                pResults->addText(str1, " ! IO error ", str2, 2);
                str1.clear();
                str2.clear();
                break;
            default:
                pResults->addText(differences[i].name, " ! ", differences[i].name);
                break;
            }                
        }
        pResults->finishedText();
        pResults->setModal(true);
        pResults->show();
    }
    else if(0 != rc) {
        KMessageBox::information(this, "Disk OK");
    }
    // else stopped by user
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
    map<string, string> details;
    string labelsString;
    
    while (lpIterator->hasMoreChildren()) {
        lEnt=lpIterator->getNextChild();
        details=lEnt.getDetails();
        labelsString=lEnt.getLabelsAsString();
        if (!details["description"].empty())
            lpItem=new XmlEntityItem(mRootItem, lEnt.getName().c_str(),
                                     details["description"].c_str(), labelsString.c_str());
        else
            lpItem=new XmlEntityItem(mRootItem, lEnt.getName().c_str(), "", labelsString.c_str());
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
