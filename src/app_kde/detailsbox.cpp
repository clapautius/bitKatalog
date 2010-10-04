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
                       vector<string> &rAllLabels)
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
    mrAllLabels=rAllLabels;

    layout();    
    connectButtons();
    setModal(true);
}


DetailsBox::~DetailsBox()
{
}


void DetailsBox::editLabels()
{
#if defined(MTP_DEBUG)
    cerr<<__FUNCTION__<<endl;
#endif

    std::vector<std::string> list;
    list=mpXmlItem->getLabels();
    LabelsBox *pLabelsBox=new LabelsBox(mrAllLabels, list);
    pLabelsBox->exec();

    list=pLabelsBox->getSelectedLabels();
    mpLabels->clear();
    for (uint i=0; i<list.size(); i++) {
        mpLabels->insertItem(list[i].c_str());
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

    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("General"));
    pPage1->setHeader(QString("General"));
    
    QFont lFont;
    map<string, string> details;
    QSize lSize;
    QFontMetrics *lpFontMetrics;
    
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
    delete lpFontMetrics;

    char labelsBuf[7]= { "labelX" };
    for (char i='0'; i<='9'; i++) {
        labelsBuf[5]=i;
        if (!details[labelsBuf].empty())
            mpLabels->insertItem(details[labelsBuf].c_str());
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
    bool found=false;
    
    details=mpXmlItem->getDetails();
    
    // update description
    lQString=mpDescriptionEdit->text();
    if (lQString!=details["description"].c_str()) {
        mpListItem->setText(DESCRIPTION_COLUMN, lQString);
        mpCatalog->setDescriptionOf(mCompletePath, lQString.toStdString());
        mCatalogWasModified=true;
    }

    // :fixme: - replace bad design (should not be limited to 9)
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
        // :fixme: optimize - use set or something else
        for (uint i=0;i<mpLabels->count() && !lModifiedLabels;i++) {
            found=false;
            for (uint j=0; j<labels.size(); j++) {
                if (mpLabels->text(i)!=labels[j].c_str())
                    found=true;
            }
            if (!found) {
                lModifiedLabels=true;
                break;
            }
        }
    if (lModifiedLabels) {
        // remove all labels
        for(unsigned int i=0; i<labels.size(); i++)
            mpCatalog->removeLabelFrom(mCompletePath, labels[i]);
        // add new labels
        for(unsigned int i=0;i<mpLabels->count();i++)
            mpCatalog->addLabelTo(mCompletePath, mpLabels->text(i).toStdString());
        mCatalogWasModified=true;
        string labelsString=mpXmlItem->getLabelsAsString();
        mpListItem->setText(LABELS_COLUMN, labelsString.c_str());
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
