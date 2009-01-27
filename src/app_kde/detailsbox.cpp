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
        mpLabels->insertItem(QString2string(lS));    
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
    std::vector<std::string> lDetails;
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
    mpTmpLabel1=new QLabel("Description: ", mpDescriptionBox);
    mpDescriptionEdit=new KLineEdit(pDescriptionBox);
    lDetails=mpXmlItem->getDetails();
    mpDescriptionEdit->setText(lDetails[0].c_str());
    //top_layout->addWidget(lpDescription);
    //mpDescription=new QLabel(lDetails[0].c_str(), this);
    //mpDescription->setAlignment(Qt::AlignHCenter);

    // labels group box
    mpLabelGroup=new Q3VGroupBox("Labels", mpPage1);
    //top_layout1->addWidget(mpLabelGroup);
    
    KHBox *pLabelsBox=new KHBox(mpLabelGroup);
    //top_layout1->addWidget(mpLabelsBox);
    //lpTempLabel=new QLabel("Labels: ", lpLabelsBox);
    mpLabels=new K3ListBox(mpLabelsBox);
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
    
    for (unsigned int i=10;i<lDetails.size();i++) {
        mpLabels->insertItem(lDetails[i].c_str());
    }
    
    KHBox *pLabelButtons=new KHBox(mpLabelGroup);
    //top_layout1->addWidget(mpLabelButtons);
    //mpLabelButtonBox=new KButtonBox(lpLabelButtons);
    
    mpAddLabelButton=new QPushButton("Add a label", mpLabelButtons);
    mpEditLabelButton=new QPushButton("Edit current label", mpLabelButtons);
    mpDeleteLabelButton=new QPushButton("Delete current label", mpLabelButtons);
    
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

#if 0
    // page2
    if(mpXmlItem->isFile())
    {
      int fileType;
      fileType=mpXmlItem->getTypeOfFile();
      if(0 == fileType) // not good
        ;
      else if(1 == fileType)
      {
        mpPage2=addPage(QString("File details"));
        top_layout2 = new QVBoxLayout(mpPage2, 5);

        str="Sha1 sum: ";
        if("" == lDetails[2])
          str+="-";
        else
          str+=lDetails[2];
        mpShaLabel=new QLabel(str.c_str(), mpPage2);
        top_layout2->addWidget(mpShaLabel);
      }
      else if(2 == fileType)
      {
        mpPage2=addPage(QString("Directory details"));
        top_layout2 = new QVBoxLayout(mpPage2, 5);
      }
    }
    else // disk
    {
        mpPage2=addPage(QString("Disk details"));
        top_layout2 = new QVBoxLayout(mpPage2, 5);

        mpCdateBox=new QHBox(mpPage2);
        top_layout2->addWidget(mpCdateBox);
        pTmpLabel=new QLabel("Creation date: ", mpCdateBox);
        mpCdateEdit=new KLineEdit(mpCdateBox);
        mpCdateEdit->setText(lDetails[1].c_str());
    }

    // page3
    //mpPage3=addPage(QString("Misc"));
#endif
}


void DetailsBox::accept()
{
    QString lQString;
    std::vector<std::string> lDetails;
    bool lModifiedLabels=false;
    
    lDetails=mpXmlItem->getDetails();
    
    // update description
    lQString=mpDescriptionEdit->text();
    if (lQString!=lDetails[0].c_str()) {
        //KMessageBox::error(this, "Description modified:");
        mpListItem->setText(DESCRIPTION_COLUMN, lQString);
        mpCatalog->setDescriptionOf(mCompletePath, QString2string(lQString));
        mCatalogWasModified=true;
    }
    
    // check labels and update if modified
    if(mpLabels->count()!=lDetails.size()-10)
        lModifiedLabels=true;
    else
        for(unsigned int i=0;i<mpLabels->count() && !lModifiedLabels;i++)
        {
            if(mpLabels->text(i)!=lDetails[i+10].c_str())
                lModifiedLabels=true;
        }
    if (lModifiedLabels) {
        // remove all labels
        for(unsigned int i=1;i<lDetails.size();i++)
            mpCatalog->removeLabelFrom(mCompletePath, lDetails[i]);
        
        // add new labels
        for(unsigned int i=0;i<mpLabels->count();i++)
            mpCatalog->addLabelTo(mCompletePath, QString2string(mpLabels->text(i)));
        
        mCatalogWasModified=true;
    }

    if (mpXmlItem->isDisk()) {
      // update cdate
      lQString=mpCdateEdit->text();
      if (lQString!=lDetails[1].c_str()) {
        try {
            mpCatalog->setCDate(mCompletePath, QString2string(lQString));
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
