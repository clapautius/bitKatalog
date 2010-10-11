/***************************************************************************
 *   Copyright (C) 2005 by Tudor Marian Pristavu                           *
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
#include "detailsbox.h"

#include <qpainter.h>
#include <qlayout.h>
#include <vector>
#include <string>
#include <klineedit.h>

#include <kmessagebox.h>
#include <kinputdialog.h>

#include "bitkatalogview.h"
#include "xfcEntity.h"
#include "misc.h"
#include "labelsbox.h"
#include "main.h"

#if defined(XFC_DEBUG)
    #include <iostream>
    using std::cout;
    using std::endl;
#endif

using std::string;
using std::vector;
using std::map;


DetailsBox::DetailsBox(Xfc *lpCatalog, std::string lCompletePath,
                       XfcEntity *lpXmlItem, Q3ListViewItem *pListItem,
                       const vector<QString> &rAllLabels)
  : KPageDialog(),
    mrAllLabels(rAllLabels)

{
    setFaceType(KPageDialog::Tabbed);
    setCaption(QString("Details"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);
    mCompletePath=lCompletePath;
    mpXmlItem=lpXmlItem;
    mpCatalog=lpCatalog;
    mpListItem=pListItem;
    mCatalogWasModified=false;
    mLabelsWereModified=false;

    layout();    
    connectButtons();
    setModal(true);
}


DetailsBox::~DetailsBox()
{
}


void DetailsBox::editLabels()
{
#if defined(XFC_DEBUG)
    cout<<":debug:"<<__FUNCTION__<<endl;
#endif
    vector<QString> allCurrentLabels; // all catalog labels + current labels
    // we need this to avoid bugs when editing an element multiple times
    // (without hitting 'OK').
    allCurrentLabels=mrAllLabels;
    for(uint i=0; i<mCurrentLabels.size(); i++) {
        if (!contains(allCurrentLabels, mCurrentLabels[i])) {
            allCurrentLabels.push_back(mCurrentLabels[i]);
        }
    }
    LabelsBox *pLabelsBox=new LabelsBox(allCurrentLabels, mCurrentLabels);
    if (QDialog::Accepted == pLabelsBox->exec()) {
        mCurrentLabels=pLabelsBox->getSelectedLabels();
        mpLabels->clear();
        for (uint i=0; i<mCurrentLabels.size(); i++) {
            mpLabels->insertItem(mCurrentLabels[i]);
        }
    }
}


void DetailsBox::connectButtons()
{
    connect(mpEditLabelsButton, SIGNAL(clicked()), this, SLOT(editLabels()));
} 


void DetailsBox::layout()
{
    QLabel *pTmpLabel;
    std::string str;
    QFont lFont;
    map<string, string> details;
    QSize lSize;
    QFontMetrics *lpFontMetrics;

    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("General"));
    pPage1->setHeader(QString("General"));
    
    resize(500,500);
    
    mpName=new QLabel(str2qstr(mpXmlItem->getName()), pBox1);
    mpName->setAlignment(Qt::AlignHCenter);
    lFont=mpName->font();
    lFont.setBold(true);
    mpName->setFont(lFont);

    KHBox *pDescriptionBox=new KHBox(pBox1);
    mpTmpLabel1=new QLabel("Description: ", pDescriptionBox);
    mpDescriptionEdit=new KLineEdit(pDescriptionBox);
    details=mpXmlItem->getDetails();
    mpDescriptionEdit->setText(str2qstr(details["description"]));

    // labels group box
    mpLabelGroup=new Q3VGroupBox("Labels", pBox1);
    
    KHBox *pLabelsBox=new KHBox(mpLabelGroup);
    mpLabels=new K3ListBox(pLabelsBox);
    lSize=mpLabels->size();
    lFont=mpLabels->font();
    lpFontMetrics=new QFontMetrics(lFont);
    mpLabels->resize(lSize.width(), 2*lpFontMetrics->height());
    delete lpFontMetrics;

    mCurrentLabels=vectWstringToVectWQString(mpXmlItem->getLabels());
    for (uint i=0; i<mCurrentLabels.size(); i++) {
        mpLabels->insertItem(mCurrentLabels[i]);
    }
    
    KHBox *pLabelButtons=new KHBox(mpLabelGroup);
    mpEditLabelsButton=new QPushButton("Edit labels", pLabelButtons);

    // end labels group box
    
    // page2
    KPageWidgetItem *pPage2=NULL;
    KVBox *pBox2=NULL;
    if (mpXmlItem->isFileOrDir()) {
      int fileType;
      fileType=mpXmlItem->getTypeOfFile();
      if (0 == fileType) // not good
        ;
      else if (1 == fileType) {
          pBox2= new KVBox();
          pPage2=addPage(pBox2, QString("File details"));
          pPage2->setHeader(QString("File details"));
          KVBox *pLayoutBox2=new KVBox(pBox2);
          str="Sha1 sum: ";
          if (details[SHA1LABEL].empty())
              str+="-";
          else
              str+=details[SHA1LABEL];
          mpSha1Label=new QLabel(str.c_str(), pLayoutBox2);
          str="Sha256 sum: ";
          if (details[SHA256LABEL].empty())
              str+="-";
          else
              str+=details[SHA256LABEL];
          mpSha256Label=new QLabel(str.c_str(), pLayoutBox2);
          str="Size: ";
          str+=sizeToHumanReadableSize(details["size"]);
          mpSizeLabel=new QLabel(str.c_str(), pLayoutBox2);
      }
      else if (2 == fileType) {
          pBox2= new KVBox();
          pPage2=addPage(pBox2, QString("Directory details"));
          pPage2->setHeader(QString("Directory details"));
      }
    }
    else if (mpXmlItem->isDisk()) { // disk
        pBox2= new KVBox();
        pPage2=addPage(pBox2, QString("Disk details"));
        pPage2->setHeader(QString("Disk details"));
        mpCdateBox=new KHBox(pBox2);
        pTmpLabel=new QLabel("Creation date: ", mpCdateBox);
        mpCdateEdit=new KLineEdit(mpCdateBox);
        mpCdateEdit->setText(details["cdate"].c_str());
    }
}


void DetailsBox::accept()
{
    QString lQString;
    //std::vector<std::string> lDetails;
    map<string, string> details;
    bool lModifiedLabels=false;
    
    details=mpXmlItem->getDetails();
    
    // update description
    lQString=mpDescriptionEdit->text();
    if (lQString!=str2qstr(details["description"])) {
        mpListItem->setText(DESCRIPTION_COLUMN, lQString);
        mpCatalog->setDescriptionOf(mCompletePath, qstr2cchar(lQString));
        mCatalogWasModified=true;
    }

    // check labels and update if modified
    vector<QString> labels=vectWstringToVectWQString(mpXmlItem->getLabels());
    if (mCurrentLabels.size()!=labels.size())
        lModifiedLabels=true;
    else {
        // :fixme: optimize - use set or something else
        for (uint i=0;i<mCurrentLabels.size() && !lModifiedLabels;i++) {
            if (!contains(labels, mCurrentLabels[i])) {
                lModifiedLabels=true;
            }
        }
        for (uint i=0;i<labels.size() && !lModifiedLabels;i++) {
            if (!contains(mCurrentLabels, labels[i])) {
                lModifiedLabels=true;
            }
        }
    }
    if (lModifiedLabels) {
        // remove all labels
        for(unsigned int i=0; i<labels.size(); i++)
            mpCatalog->removeLabelFrom(mCompletePath, qstr2cchar(labels[i]));
        // add new labels
        for(unsigned int i=0;i<mCurrentLabels.size();i++) {
            gkLog<<xfcDebug<<"adding new label to element: ";
            gkLog<<qstr2cchar(mCurrentLabels[i])<<eol;
            mpCatalog->addLabelTo(mCompletePath, qstr2cchar(mCurrentLabels[i]));
        }
        mCatalogWasModified=true;
        mLabelsWereModified=true;
        string labelsString=mpXmlItem->getLabelsAsString();
        mpListItem->setText(LABELS_COLUMN, str2qstr(labelsString));
    }

    if (mpXmlItem->isDisk()) {
      // update cdate
      lQString=mpCdateEdit->text();
      if (lQString!=details["cdate"].c_str()) {
        try {
            mpCatalog->setCDate(mCompletePath, qstr2cchar(lQString));
            mCatalogWasModified=true;
        }
        catch(...) {
            KMessageBox::error(this, "Error setting cdate");
        }
      }
    }
    
    KPageDialog::accept();
} 


void
DetailsBox::reject()
{
    KPageDialog::reject();
} 


bool
DetailsBox::catalogWasModified() const
{
    return mCatalogWasModified;
} 


bool
DetailsBox::labelsWereModified() const
{
    return mLabelsWereModified;
}
