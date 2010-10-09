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

#if defined(XFC_DEBUG)
    #include <iostream>
    using std::cout;
    using std::endl;
#endif

using std::string;
using std::vector;
using std::map;


bitKatalogView::bitKatalogView(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *top_layout = new QHBoxLayout(this);
    setupListView();
    mRootItem=new XmlEntityItem(mListView, "[ no catalog ]");
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
    mCatalogLabels.clear();
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
    populateCatalogLabels();
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
    std::string completePath;
    std::string lS;
    Q3ListViewItem *lpItem;
    bool isDisk=false, isDir=false;

    // this code appears also in details function - make a function from it
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }

    KMenu *pContextMenu=new KMenu();
    QAction *pAct=NULL;

    mpCurrentItem=(XmlEntityItem*)i;
    completePath=qstr2str(mpCurrentItem->text(0));
    lpItem=mpCurrentItem;
    if (NULL == lpItem->parent()) { // element is root
        completePath="/";
    }
    else {
        while(1) {
            lpItem=lpItem->parent();
            if(lpItem==NULL)
                break;
            if (NULL == lpItem->parent()) { // element is root
                lS="";
            }
            else {
                lS=qstr2cchar(lpItem->text(0));
            }
            lS+="/";
            lS+=completePath;
            completePath=lS;
        }
    }
    mCurrentItemPath=completePath;
    
    try {
        msgInfo("Context menu for: ", completePath);
        // check if this is a disk
        XfcEntity lEnt(mCatalog->getNodeForPath(completePath), mCatalog);
        Xfc::ElementType type=lEnt.getElementType();
        switch (type) {
        case Xfc::eDisk:
            isDisk=true;
            break;
        case Xfc::eDir:
            isDir=true;
            break;
        default: // make compiler happy
            break;
        }
    }
    catch(std::string e) {
        msgWarn("Hmmm, cannot display info about this item (exception in XfcEntity()! completePath=", completePath);
        msgWarn("  exception is: ", e);
        KMessageBox::error(this, "Hmmm, cannot display informations about this item!");
    }
    
    pAct=pContextMenu->addAction("Details");
    connect(pAct, SIGNAL(triggered()), this, SLOT(details()));
    pContextMenu->addSeparator();

    // add label rec.
    if (isDir || isDisk) {
        pAct=pContextMenu->addAction("Add label recursive");
        connect(pAct, SIGNAL(triggered()), this, SLOT(addLabelRec()));
        
        pContextMenu->addSeparator();
    }
    
    if (isDisk) {
        pAct=pContextMenu->addAction("Verify disk");
        connect(pAct, SIGNAL(triggered()), this, SLOT(verifyDisk()));
        
        pAct=pContextMenu->addAction("Rename disk");
        connect(pAct, SIGNAL(triggered()), this, SLOT(renameDisk()));
        
        pAct=pContextMenu->addAction("Delete disk");
        connect(pAct, SIGNAL(triggered()), this, SLOT(deleteDisk()));
    }

    pContextMenu->exec(p);
}


void
bitKatalogView::details() throw()
{
    std::string completePath;
    DetailsBox *pDetailsBox=NULL;
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    completePath=mCurrentItemPath;
    try {
        XfcEntity lEnt(mCatalog->getNodeForPath(completePath), mCatalog);
        vector<QString> allLabels=vectWstringToVectWQString(mCatalogLabels);
        pDetailsBox=new DetailsBox(mCatalog, completePath, &lEnt, mpCurrentItem,
                                   allLabels);
        pDetailsBox->exec(); 
        if (pDetailsBox->catalogWasModified()) {
            gCatalogState=1;
            if (pDetailsBox->labelsWereModified()) {
                vector<string> labels=lEnt.getLabels();
                addCatalogLabels(labels);
            }
            gpMainWindow->updateTitle(true);
        }
    }
    catch(std::string e) {
        msgWarn("Hmmm, cannot display info about this item (exception in XfcEntity()! completePath=", completePath);
        msgWarn("  exception is: ", e);
        KMessageBox::error(this, "Hmmm, cannot display informations about this item!");
    }
}


void
bitKatalogView::addLabelRec() throw()
{
    std::string completePath;
    if (mCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    completePath=mCurrentItemPath;
    if (completePath!="/") {
        QString str;
        bool retButton; 
        str=KInputDialog::getText("New label", "Label: ", "", &retButton);
        //mpCatalog->addLabelTo(mCompletePath, lS);
        if(retButton) {
#if defined(XFC_DEBUG)
            cout<<":debug: adding new label (rec.): "<<qstr2cchar(str)<<endl;
#endif 
            mCatalog->addLabelRecTo(completePath, qstr2cchar(str));
            mpCurrentItem->redisplay(); // to update the window
            gCatalogState=1; // :fixme: - check result
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
        mCatalog, completePath, qstr2cchar(dir), &differences);
        
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
  std::string completePath;
  QString oldName, newName;
  
  if (mCatalog==NULL) {
    KMessageBox::error(this, "No catalog!");
    return;
  }
  
  lpItem=mpCurrentItem;
  completePath=mCurrentItemPath;
  
  if (completePath!="/") {
    XfcEntity lEnt(mCatalog->getNodeForPath(completePath), mCatalog);
    oldName=lEnt.getName().c_str();

    bool lRetButton;
    newName=KInputDialog::getText("Rename disk", "Name: ", oldName, &lRetButton);
    if (lRetButton && newName != oldName) {
        lEnt.setName(qstr2cchar(newName));
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
  std::string completePath;
  std::string lOldName, lNewName;
  
  if (mCatalog==NULL) {
    KMessageBox::error(this, "No catalog!");
    return;
  }
  lpItem=mpCurrentItem;
  completePath=mCurrentItemPath;
  
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
    map<string, string> details;
    QString labelsString;

    mListView->clear();

    // setup root element
    xmlNodePtr pRootNode=mpCatalog->getNodeForPath("/");
    string catalogName="/";
    XfcEntity rootEnt(pRootNode, mpCatalog);
    details=rootEnt.getDetails();
    labelsString=QString::fromUtf8(rootEnt.getLabelsAsString().c_str());
    catalogName+=rootEnt.getName();
    if (!details["description"].empty())
        mRootItem=new XmlEntityItem(mListView, str2qstr(catalogName),
                                    details["description"].c_str(),
                                    labelsString);
    else
        mRootItem=new XmlEntityItem(mListView, str2qstr(catalogName), "",
                                    labelsString);
    mRootItem->setXmlNode(pRootNode);
    
    EntityIterator *lpIterator;
    EntityIterator *lpTempIterator;
    std::string lS="/";
    lpIterator=new EntityIterator(*mpCatalog, lS);
    XfcEntity lEnt;
    XmlEntityItem *lpItem;

    // add first level elements without opening root element
    while (lpIterator->hasMoreChildren()) {
        lEnt=lpIterator->getNextChild();
        details=lEnt.getDetails();
        labelsString=QString::fromUtf8(lEnt.getLabelsAsString().c_str());
        if (!details["description"].empty())
            lpItem=new XmlEntityItem(
                mRootItem, str2qstr(lEnt.getName()),
                details["description"].c_str(), labelsString);
        else
            lpItem=new XmlEntityItem(mRootItem, str2qstr(lEnt.getName()), "", labelsString);
#if defined(XFC_DEBUG)
        cout<<":debug:"<<__FUNCTION__<<": adding element with name ";
        cout<<lEnt.getName()<<endl;
        cout<<":debug:"<<__FUNCTION__<<": labelsString: "<<labelsString<<endl;
#endif
        lpItem->setXmlNode(lEnt.getXmlNode());
        lpItem->setPixmap(0, *gpDiskPixmap);
        lpTempIterator=new EntityIterator(*mpCatalog, lEnt.getXmlNode());
        if (lpTempIterator->hasMoreChildren())
            lpItem->setExpandable(true);
        delete lpTempIterator;
    }
    mRootItem->setAsOpened();
}    


void bitKatalogView::setCatalog(Xfc *lpNewCatalog)
{
    if(mCatalog!=NULL)
        delete mCatalog;
    mCatalog=lpNewCatalog;
    XmlEntityItem::mspCatalog=mCatalog;
    populateTree(mCatalog);
}


/**
 * Helper function used for collectin all labels in the catalog.
 *
 * @sa bitKatalogView::populateCatalogLabels
 **/
int collectLabels(uint, std::string, Xfc& rCatalog, xmlNodePtr pNode,
                  void *pParam)
{
    vector<string> labels;
    labels=rCatalog.getLabelsForNode(pNode);
    for (uint i=0; i<labels.size(); i++) {
        static_cast<bitKatalogView*>(pParam)->addCatalogLabel(labels[i]);
    }
    return 0;
}


void
bitKatalogView::populateCatalogLabels()
{
    mCatalog->parseFileTree(collectLabels, this);
}


void
bitKatalogView::addCatalogLabels(std::vector<std::string> newLabels)
{
    for (uint i=0; i<newLabels.size(); i++) {
        addCatalogLabel(newLabels[i]);
    }
}


/**
 * Add a new label to the list of all the labels of the current catalog.
 * Check if already exists.
 **/
void
bitKatalogView::addCatalogLabel(std::string newLabel)
{
    // :fixme: - optimize - use a set or something
    bool found=false;
    for (uint i=0; i<mCatalogLabels.size(); i++) {
        if (newLabel == mCatalogLabels[i]) {
            found=true;
            break;
        }
    }
    if (!found) {
        mCatalogLabels.push_back(newLabel);
    }
}


const std::vector<std::string>&
bitKatalogView::getCatalogLabels() const
{
    return mCatalogLabels;
}
