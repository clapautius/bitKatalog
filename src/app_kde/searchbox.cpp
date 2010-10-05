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
#include "searchbox.h"
#include "labelsbox.h"

#include <qpainter.h>
#include <qlayout.h>

#include <qprogressbar.h>
#include <kprogressdialog.h>
#include <kmessagebox.h>
#include <kvbox.h>
#include <kpushbutton.h>

#include "main.h"
#include "misc.h"

using std::string;
using std::vector;


SearchBox::SearchBox(Xfc *lpCatalog, const vector<string> &rAllLabels)
    : KPageDialog(),
      mrAllLabels(rAllLabels)
{
    setCaption(QString("Search"));
    setButtons(KDialog::Close | KDialog::User1);
    setModal(true);
    mpCatalog=lpCatalog;
    layout();
}


SearchBox::~SearchBox()
{
}


void SearchBox::layout()
{
    resize(800,450);
    button(KDialog::User1)->setGuiItem(KStandardGuiItem::Find);
    
    // page1
    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Search"));
    pPage1->setHeader(QString("Search"));
    
    mpSimpleSearchBox=new KHBox(pBox1);
    mpTmpLabel1=new QLabel("Text: ", mpSimpleSearchBox);
    mpTextEdit=new KLineEdit(mpSimpleSearchBox);    

    // labels
    KHBox *pLabelsBox=new KHBox(pBox1);
    mpTmpLabel2=new QLabel("Labels: ", pLabelsBox);
    mpLabelsEdit=new KLineEdit(pLabelsBox);
    mpLabelsEdit->setReadOnly(true);
    mpEditLabelsButton=new QPushButton("Select labels", pLabelsBox);
    
    // ...    
    mpSimpleSearchResults=new K3ListBox(pBox1);

    mpTextEdit->setFocus();
    connectButtons();
}


void SearchBox::connectButtons()
{
    connect(this, SIGNAL(user1Clicked()), this, SLOT(search())); // search    
    connect(mpEditLabelsButton, SIGNAL(clicked()), this, SLOT(editLabels()));
}


void SearchBox::search()
{
    SearchStruct lSearchStruct;
    std::vector<std::string> lSearchResultsPaths;
    std::vector<xmlNodePtr> lSearchResultsNodes;

    if (mpCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    if (mpTextEdit->text()=="" && 0==mSearchLabels.size()) {
        KMessageBox::error(this, "Please specify a search string or a label");
        return;
    }
    
    // prepare progress dialog
    mpProgress=new KProgressDialog(this, "Searching ...", "Searching");
    mpProgress->progressBar()->setRange(0, 0);
    mpProgress->setMinimumDuration(1000);
    mpProgress->setAutoClose(true);
    mpProgress->setAllowCancel(true);
    mpProgress->setButtonText("Stop");
    
    lSearchStruct.mpProgressDialog=mpProgress;
    lSearchStruct.mString=mpTextEdit->text().toStdString();
    lSearchStruct.mLabels=mSearchLabels;
    lSearchStruct.mpSearchResultsNodes=&lSearchResultsNodes;
    lSearchStruct.mpSearchResultsPaths=&lSearchResultsPaths;
    mpSimpleSearchResults->clear();
    disableButtons();
    msgDebug("Starting to search. Search string is: ", lSearchStruct.mString);
    msgDebug("Labels: ", vectorWStringsToString(lSearchStruct.mLabels));
    msgDebug("Start time: ", getTimeSinceMidnight());
    mpCatalog->parseFileTree(findInTree, (void*)&lSearchStruct);
    msgDebug("Finish time: ", getTimeSinceMidnight());
    mpProgress->progressBar()->setValue(0);
    delete mpProgress;
    msgDebug("Search finished");

    if (lSearchResultsPaths.size()>0)
        for (unsigned int i=0;i<lSearchResultsPaths.size();i++)
            mpSimpleSearchResults->insertItem(lSearchResultsPaths[i].c_str());
    else
        mpSimpleSearchResults->insertItem("Nothing found!");
    enableButtons();
} 


void SearchBox::slotUser1()
{
    search();
} 


void
SearchBox::editLabels()
{
    gkLog<<xfcDebug<<__FUNCTION__<<eol;
    string str;
    LabelsBox *pLabelsBox=new LabelsBox(mrAllLabels, mSearchLabels, false);
    if (QDialog::Accepted == pLabelsBox->exec()) {
        mSearchLabels=pLabelsBox->getSelectedLabels();
        mpLabelsEdit->setText(vectorWStringsToString(mSearchLabels).c_str());
    }
}


void
SearchBox::enableButtons()
{
    enableButton(KDialog::Close, true);
    enableButton(KDialog::User1, true);
    mpTextEdit->setEnabled(true);
}


void
SearchBox::disableButtons()
{
    enableButton(KDialog::Close, false);
    enableButton(KDialog::User1, false);
    mpTextEdit->setEnabled(false);
}


/**
 * helper function
 * @retval 0 : OK;
 * @retval -1 : user abort / stop;
 * @retval -2 : error;
 **/
int
findInTree(unsigned int depth __attribute__((unused)), std::string lPath, Xfc& lrXfc,
           xmlNodePtr lpNode, void *lpParam)
{
    static int lPosInProgressBar=1;
    std::string lName;
    SearchStruct *lpSearchStruct=(SearchStruct*)lpParam;
    const char *lpPtr=lpSearchStruct->mString.c_str();
    KProgressDialog *lpProgressDialog=lpSearchStruct->mpProgressDialog;
    bool found=false;

    if (lpProgressDialog->wasCancelled()) {
        gkLog<<xfcInfo<<__FUNCTION__<<": cancelled."<<eol;
        return -1;
    }
        
    lName=lrXfc.getNameOfElement(lpNode); // :fixme: utf8 -> string ?
    if ( lpPtr[0] &&
         xmlStrcasestr((const xmlChar*)lName.c_str(), (xmlChar*)lpPtr)!=NULL) {
        string fName=lPath;
        if (lPath!="/")
            fName+"/";
        fName+=lName;
        gkLog<<xfcDebug<<__FUNCTION__<<": found matching node: path="<<lPath;
        gkLog<<__FUNCTION__<<":  name="<<lName<<eol;
        lpSearchStruct->mpSearchResultsPaths->push_back(lPath+"/"+lName);
        lpSearchStruct->mpSearchResultsNodes->push_back(lpNode);
        found=true;
    }

    // check labels
    if (!found && lpSearchStruct->mLabels.size() > 0) {
        vector<string> eltLabels=lrXfc.getLabelsForNode(lpNode);
        vector<string> &rSList=lpSearchStruct->mLabels;
        if (eltLabels.size() > 0) {
            for (uint i=0; i<eltLabels.size() && !found; i++)
                for (uint j=0; j<rSList.size() && !found; j++)
                    if (xmlStrcasecmp((const xmlChar*)eltLabels[i].c_str(),
                                      (const xmlChar*)rSList[j].c_str())==0) {
                        string fName=lPath;
                        if (lPath!="/")
                            fName+="/";
                        fName+=lName;
                        gkLog<<xfcDebug<<__FUNCTION__;
                        gkLog<<": found matching label ("<<eltLabels[i];
                        gkLog<<") for node: "<<fName<<eol;
                        lpSearchStruct->mpSearchResultsPaths->push_back(fName);
                        lpSearchStruct->mpSearchResultsNodes->push_back(lpNode);
                        found=true;
                    }
        }
    }
    
    lpProgressDialog->progressBar()->setValue(lPosInProgressBar/10);
    lPosInProgressBar++;
    gpApplication->processEvents();
    return 0;
}
