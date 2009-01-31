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

#if defined(MTP_DEBUG)
  #include <iostream>

  using namespace std;
#endif

DetailsBox::DetailsBox()
    : KPageDialog()
{
    // :fixme: - do something
} 


DetailsBox::DetailsBox(Xfc *lpCatalog, std::string lCompletePath,
                       XfcEntity *lpXmlItem, Q3ListViewItem *pListItem)
    : KPageDialog()
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

    layout();    
    connectButtons();
    setModal(true);
}


DetailsBox::~DetailsBox()
{
}


void DetailsBox::addLabel()
{
#if defined(MTP_DEBUG)
    cerr<<"addLabel()"<<endl;
#endif

    QString lS;
    bool lRetButton; 
    lS=KInputDialog::getText("New label", "Label: ", "", &lRetButton);
    //mpCatalog->addLabelTo(mCompletePath, lS);
    if(lRetButton)
        mpLabels->insertItem(lS);
}


void DetailsBox::editLabel()
{
    QString lS, lOldLabel;
    int lInt;
    lInt=mpLabels->currentItem();
    if(lInt==-1)
    {
        KMessageBox::error(this, "No label selected");
        return;
    }
    bool lRetButton;
    lOldLabel=mpLabels->currentText();
    lS=KInputDialog::getText("Edit label", "Label: ", lOldLabel, &lRetButton);    
    if(lRetButton)
        mpLabels->changeItem(lS, lInt);
}


void DetailsBox::deleteLabel()
{
    int lInt;
    lInt=mpLabels->currentItem();
    if(lInt==-1)
    {
        KMessageBox::error(this, "No label selected");
        return;
    }
    mpLabels->removeItem(lInt);    
}


void DetailsBox::connectButtons()
{
    //connect(mpOkButton, SIGNAL(clicked()), this, SLOT(accept()));
    //connect(mpCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
   
    connect(mpAddLabelButton, SIGNAL(clicked()), this, SLOT(addLabel()));
    connect(mpEditLabelButton, SIGNAL(clicked()), this, SLOT(editLabel()));
    connect(mpDeleteLabelButton, SIGNAL(clicked()), this, SLOT(deleteLabel()));
    
} 


void DetailsBox::layout()
{
    QLabel *pTmpLabel;
    std::string str;

    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("General"));
    pPage1->setHeader(QString("General"));
    
    QFont lFont;
    map<string, string> details;
    QSize lSize;
    QFontMetrics *lpFontMetrics;
    
    //top_layout1 = new QVBoxLayout(mpPage1, 5);
    //top_layout->setAutoAdd(true);

    resize(500,250);
    
    mpName=new QLabel(mpXmlItem->getName().c_str(), pBox1);
    mpName->setAlignment(Qt::AlignHCenter);
    lFont=mpName->font();
    lFont.setBold(true);
    mpName->setFont(lFont);
    //top_layout1->addWidget(mpName);

    KHBox *pDescriptionBox=new KHBox(pBox1);
    //top_layout1->addWidget(mpDescriptionBox);
    mpTmpLabel1=new QLabel("Description: ", pDescriptionBox);
    mpDescriptionEdit=new KLineEdit(pDescriptionBox);
    details=mpXmlItem->getDetails();
    mpDescriptionEdit->setText(details["description"].c_str());
    //top_layout->addWidget(lpDescription);
    //mpDescription=new QLabel(lDetails[0].c_str(), this);
    //mpDescription->setAlignment(Qt::AlignHCenter);

    // labels group box
    mpLabelGroup=new Q3VGroupBox("Labels", pBox1);
    //top_layout1->addWidget(mpLabelGroup);
    
    KHBox *pLabelsBox=new KHBox(mpLabelGroup);
    //top_layout1->addWidget(mpLabelsBox);
    //lpTempLabel=new QLabel("Labels: ", lpLabelsBox);
    mpLabels=new K3ListBox(pLabelsBox);
    lSize=mpLabels->size();
    lFont=mpLabels->font();
    lpFontMetrics=new QFontMetrics(lFont);
    mpLabels->resize(lSize.width(), 2*lpFontMetrics->height());
    //mpLabels->resize(lSize.width()/2, lSize.height()/2);
    delete lpFontMetrics;
    //top_layout1->addWidget(mpLabels);
    //QSize lSize;
    //lSize=mpLabels->size();
    //mpLabels->resize(lSize.width(), 50);

    char labelsBuf[7]= { "labelX" };
    for (char i='0'; i<='9'; i++) {
        labelsBuf[5]=i;
        if (!details[labelsBuf].empty())
            mpLabels->insertItem(details[labelsBuf].c_str());
    }
    
    KHBox *pLabelButtons=new KHBox(mpLabelGroup);
    //top_layout1->addWidget(mpLabelButtons);
    //mpLabelButtonBox=new KButtonBox(lpLabelButtons);
    
    mpAddLabelButton=new QPushButton("Add a label", pLabelButtons);
    mpEditLabelButton=new QPushButton("Edit current label", pLabelButtons);
    mpDeleteLabelButton=new QPushButton("Delete current label", pLabelButtons);
    
    //mpAddLabelButton=mpLabelButtonBox->addButton("Add a label");
    //mpEditLabelButton=mpLabelButtonBox->addButton("Edit current label");
        
    // end labels group box
    
    /*
    QHBox *lpButtons=new QHBox(mpPage1);
    top_layout->addWidget(lpButtons, 5);
    //top_layout->addLayout(lpButtons, 5);
    mpButtonBox=new KButtonBox(lpButtons);
    //lpButtons->addWidget(mpButtonBox);
    
    mpOkButton=mpButtonBox->addButton("OK");
    mpOkButton->setFocus();
    mpCancelButton=mpButtonBox->addButton("Cancel");
    */

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
          str+=details["size"];
          mpSizeLabel=new QLabel(str.c_str(), pLayoutBox2);
      }
      else if (2 == fileType) {
          pBox2= new KVBox();
          pPage2=addPage(pBox2, QString("Directory details"));
          pPage2->setHeader(QString("Directory details"));
      }
    }
    else { // disk
        pBox2= new KVBox();
        pPage2=addPage(pBox2, QString("Disk details"));
        pPage2->setHeader(QString("Disk details"));
        mpCdateBox=new KHBox(pBox2);
        pTmpLabel=new QLabel("Creation date: ", mpCdateBox);
        mpCdateEdit=new KLineEdit(mpCdateBox);
        mpCdateEdit->setText(details["cdate"].c_str());
    }

    // page3
    //mpPage3=addPage(QString("Misc"));
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
    if (lQString!=details["description"].c_str()) {
        mpListItem->setText(DESCRIPTION_COLUMN, lQString);
        mpCatalog->setDescriptionOf(mCompletePath, lQString.toStdString());
        mCatalogWasModified=true;
    }

    vector<string> labels;
    char labelBuf[7] = { "labelX" };
    for (char c='0'; c<='9'; c++) {
        labelBuf[5]=c;
        if (!details[labelBuf].empty())
            labels.push_back(details[labelBuf]);
    }
    // check labels and update if modified
    if (mpLabels->count()!=labels.size())
        lModifiedLabels=true;
    else
        for (unsigned int i=0;i<mpLabels->count() && !lModifiedLabels;i++) {
            if(mpLabels->text(i)!=labels[i].c_str())
                lModifiedLabels=true;
        }
    if (lModifiedLabels) {
        // remove all labels
        for(unsigned int i=0; i<labels.size(); i++)
            mpCatalog->removeLabelFrom(mCompletePath, labels[i]);
        // add new labels
        for(unsigned int i=0;i<mpLabels->count();i++)
            mpCatalog->addLabelTo(mCompletePath, mpLabels->text(i).toStdString());
        mCatalogWasModified=true;
    }

    if (mpXmlItem->isDisk()) {
      // update cdate
      lQString=mpCdateEdit->text();
      if (lQString!=details["cdate"].c_str()) {
        try {
            mpCatalog->setCDate(mCompletePath, lQString.toStdString());
            mCatalogWasModified=true;
        }
        catch(...) {
            KMessageBox::error(this, "Error setting cdate");
        }
      }
    }
    
    KPageDialog::accept();
} 


void DetailsBox::reject()
{
    KPageDialog::reject();
} 


bool DetailsBox::catalogWasModified()
{
    return mCatalogWasModified;
} 
