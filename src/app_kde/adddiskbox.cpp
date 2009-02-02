/***************************************************************************
 *   Copyright (C) 2005 by Tudor Pristavu                                  *
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
#include <time.h>

#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <qcheckbox.h>
#include <kdatepicker.h>
#include <khbox.h>
#include <kvbox.h>

#include <sstream>
 
#include "adddiskbox.h"
#include "main.h"
#include "scanthread.h"
#include "misc.h"


AddDiskBox::AddDiskBox(Xfc *lpCatalog)
    : KPageDialog()
{
    setButtons(KDialog::Close | KDialog::User1);
    setCaption("Add disk");
    mpCatalog=lpCatalog;
    mCatalogWasModified=false;
    setModal(true);
    layout();
}


AddDiskBox::~AddDiskBox()
{
}


void AddDiskBox::layout()
{
    resize(800,450);
        
    setButtonText(KDialog::User1, "Add"); // :fixme: - replace with a kguiitem

    KVBox *pLayoutBox= new KVBox();
    KPageWidgetItem *pPage=addPage(pLayoutBox, QString("Add disk"));
    pPage->setHeader(QString("Add disk"));

    mpLayoutBox1=new KHBox(pLayoutBox);
    //mpLayout1->addWidget(mpLayoutBox1);
    mpTmpLabel1=new QLabel("Path to add: ", mpLayoutBox1);
    mpPathLabel=new QLabel("", mpLayoutBox1);
    mpBrowseButton=new KPushButton("Browse ...", mpLayoutBox1);
    mpBrowseButton->setMaximumSize(mpBrowseButton->sizeHint());
    connect(mpBrowseButton, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));
    
    mpLayoutBox2=new KHBox(pLayoutBox);
    //mpLayout1->addWidget(mpLayoutBox2);
    mpTmpLabel2=new QLabel("Disk name: ", mpLayoutBox2);
    mpDiskNameEdit=new KLineEdit(mpLayoutBox2);
    
    mpGroupBox=new Q3VGroupBox(" Parameters ", pLayoutBox);
    //mpLayout1->addWidget(mpGroupBox);

    mpLayoutBox3=new KHBox(mpGroupBox);
    //mpLayout1->addWidget(mpLayoutBox3);
    mpTmpLabel3=new QLabel("Disk description: ", mpLayoutBox3);
    mpDiskDescriptionEdit=new KLineEdit(mpLayoutBox3);
    
    // creation date
    mpLayoutBox4=new KHBox(mpGroupBox);
    //mpLayout1->addWidget(mpLayoutBox4);
    mpTmpLabel4=new QLabel("Disk creation date: ", mpLayoutBox4);
    
    mpDiskCDateEdit=new KLineEdit(mpLayoutBox4);
    mpTodayCDateButton=new KPushButton("Today", mpLayoutBox4);
    connect(mpTodayCDateButton, SIGNAL(clicked()), this, SLOT(todayCDateButtonClicked()));
    mpSomeDayCDateButton=new KPushButton("Other creation date", mpLayoutBox4);
    connect(mpSomeDayCDateButton, SIGNAL(clicked()), this, SLOT(someDayCDateButtonClicked()));
    
    mpAddRootCheckBox=new QCheckBox("Add also the root dir. to catalog", mpGroupBox);
    mpAddRootCheckBox->setChecked(false);

    KHBox *pShaBox=new KHBox(mpGroupBox);

    mpComputeSha256SumCheckBox=
        new QCheckBox("Compute sha256 sums for this disk", pShaBox);
    mpComputeSha256SumCheckBox->setChecked(true);

    mpComputeSha1SumCheckBox=
        new QCheckBox("Compute sha1 sums for this disk", pShaBox);
    mpComputeSha1SumCheckBox->setChecked(false);

    // connect button
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotUser1())); // add
} 


void AddDiskBox::slotUser1()
{
    std::string lPath;
    std::string lDiskName;
    std::string lDiskDescription;
    std::string lDiskCDate;
    bool computeSha1=false, computeSha256=false;
    volatile bool abortScan=false;
    
    lPath=mpPathLabel->text().toStdString();
    lDiskName=mpDiskNameEdit->text().toStdString();
    lDiskDescription=mpDiskDescriptionEdit->text().toStdString();
    lDiskCDate=mpDiskCDateEdit->text().toStdString();
    
    if (lPath=="") {
        KMessageBox::error(this, "Invalid path");
        return;
    }
    if (lDiskName=="") {
        KMessageBox::error(this, "Please enter a name for disk");
        return;
    }
    if (mpCatalog->getNodeForPath(std::string("/")+lDiskName)!=NULL) {
        KMessageBox::error(this, "Disk already exists");
        return;
    }
    if (!mpAddRootCheckBox->isChecked()) {
        lPath=lPath+"/";
    }
    if (mpComputeSha1SumCheckBox->isChecked()) {
        computeSha1=true;
    }
    if (mpComputeSha256SumCheckBox->isChecked()) {
        computeSha256=true;
    }
    msgDebug("Adding path to disk. Path=", lPath);

    KProgressDialog *lpProgress=new KProgressDialog(this, "Scanning ...", "Scanning");
    lpProgress->progressBar()->setRange(0, 0);
    lpProgress->progressBar()->setValue(0);
    lpProgress->progressBar()->reset();
    lpProgress->progressBar()->setTextVisible(false);
    lpProgress->setMinimumDuration(1000);
    lpProgress->setAutoClose(true);
    lpProgress->setAllowCancel(true);
    lpProgress->setButtonText("Stop");
    int i=0;

    disableButtons();
    ScanThread::ScanThreadParams params;
    params.computeSha1=computeSha1;
    params.computeSha256=computeSha256;
    params.pAbortFlag=&abortScan;
    ScanThread *lpScanThread=new ScanThread(mpCatalog, lPath, lDiskName, params);
    msgDebug("Start time: ", getTimeSinceMidnight());
    lpScanThread->start();
    mCatalogWasModified=true;

    msgDebug("Add thread started, progress bar created, waiting for scan thread ...");
    while (!lpScanThread->isFinished()) {
        if (lpProgress->wasCancelled()) {
            if (KMessageBox::questionYesNo(this, "Are you sure?")==KMessageBox::Yes) {
                abortScan=true;
                msgInfo("Waiting for scanthread to finish");
                while (!lpScanThread->isFinished())
                    usleep(250000);
                msgInfo("Scanthread is finished");
                delete lpProgress;
                lpProgress=NULL;
                KMessageBox::information(
                    this, "Scanning was cancelled. You should not save this catalog!");
                break;
            }
            else {
                delete lpProgress;
                lpProgress=new KProgressDialog(this, "Scanning ...", "Scanning");
                lpProgress->progressBar()->setRange(0, 0);
                lpProgress->progressBar()->setTextVisible(false);
                lpProgress->setMinimumDuration(1000);
                lpProgress->setAutoClose(true);
                lpProgress->setAllowCancel(true);
                lpProgress->setButtonText("Stop");
            }    
        }
        lpProgress->progressBar()->setValue(i+=10);
        gpApplication->processEvents();
        usleep(250000);
    }
    msgDebug("Finish time: ", getTimeSinceMidnight());
    if (lpScanThread->returnValue()!=0) {        
        std::string err;
        err="Error adding path to catalog. ";
        err+=lpScanThread->getErrorMessage();
        delete lpProgress;
        delete lpScanThread;
        KMessageBox::error(this, err.c_str());
        close();
        return;
    }
    if (lDiskDescription!="") {
        try {
            mpCatalog->setDescriptionOf(std::string("/")+lDiskName, lDiskDescription);
        }
        catch(std::string e) {
            KMessageBox::error(this, "Cannot add description to disk");
        }
    }
    if (lDiskCDate!="") {
        try {
            mpCatalog->setCDate(lDiskName, lDiskCDate);
        }
        catch(std::string e) {
            KMessageBox::error(this, "Cannot set cdate for disk");
        }
    }
    gCatalogState=1;
    gpMainWindow->updateTitle(true);
    delete lpProgress;
    delete lpScanThread;
    close();    
} 


void AddDiskBox::browseButtonClicked()
{
    QString lDir = KFileDialog::getExistingDirectory(QString("/"), this, i18n("Path to add"));
    // :todo: - replace '/'
    if (lDir!="") {
        mpPathLabel->setText(lDir);
    }
} 


void AddDiskBox::todayCDateButtonClicked()
{
    char lAux[128];
    struct tm *lpLocalTime;
    time_t lTime=time(NULL);
    lpLocalTime=localtime(&lTime);
    sprintf(lAux, "%04d-%02d-%02d", lpLocalTime->tm_year+1900, lpLocalTime->tm_mon+1, 
        lpLocalTime->tm_mday);
    mpDiskCDateEdit->setText(lAux);
}


void AddDiskBox::someDayCDateButtonClicked()
{
    KMessageBox::error(this, "Not ready yet"); // :todo:
}


bool AddDiskBox::catalogWasModified()
{
    return mCatalogWasModified;
}


void
AddDiskBox::disableButtons()
{
    enableButton(KDialog::Close, false);
    enableButton(KDialog::User1, false);
    mpTodayCDateButton->setEnabled(false);
    mpBrowseButton->setEnabled(false);
    mpSomeDayCDateButton->setEnabled(false);
}
