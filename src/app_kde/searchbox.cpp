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

#include <qpainter.h>
#include <qlayout.h>

#include <qprogressbar.h>
#include <kprogressdialog.h>
#include <kmessagebox.h>
#include <kvbox.h>

#include "main.h"

// :todo: - make a tabbed version

SearchBox::SearchBox(Xfc *lpCatalog)
    : KPageDialog()
{
    //setFaceType(KPageDialog::Tabbed);
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
    setButtonText(KDialog::User1, "Search"); // :fixme: - replace with a kguiitem
    
    // page1
    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Simple search"));
    pPage1->setHeader(QString("Simple search"));
    
    //mpPage1=addPage(QString("Simple search"));    
    //mpLayout1 = new QVBoxLayout(this);

    mpSimpleSearchBox=new KHBox(pBox1);
    //mpLayout1->addWidget(mpSimpleSearchBox);
    mpTmpLabel1=new QLabel("Search for: ", mpSimpleSearchBox);
    mpSimpleSearchEdit=new KLineEdit(mpSimpleSearchBox);    

    // ...    
    mpSimpleSearchResults=new K3ListBox(pBox1);
    //mpLayout1->addWidget(mpSimpleSearchResults, 5);

    // page2
    //mpPage2=addPage(QString("Advanced search"));

}


void SearchBox::connectButtons()
{
//    connect(this, SIGNAL(user1Clicked()), this, SLOT(search())); // search    
}


void SearchBox::search()
{
    SearchStruct lSearchStruct;
    std::vector<std::string> lSearchResultsPaths;
    std::vector<xmlNodePtr> lSearchResultsNodes;

//    KMessageBox::error(this, "Bau");
    
    //QProgressDialog *lpProgress=new QProgressDialog("Searching ...", "Stop", 0, this, "Dialog", true);
    //lpProgress->setTotalSteps(0);
    //lpProgress->setMinimumDuration(1);

    if(mpCatalog==NULL)
    {
        KMessageBox::error(this, "No catalog!");
        return;
    }
        
    if(mpSimpleSearchEdit->text()=="")
    {
        KMessageBox::error(this, "Search string is empty!");
        return;
    }
    
    mpSimpleSearchResults->clear();
        
    mpProgress=new KProgressDialog(this, "Searching ...", "Searching");
    mpProgress->progressBar()->setRange(0, 0);
    mpProgress->setMinimumDuration(1);
    mpProgress->setAutoClose(true);
    mpProgress->setAllowCancel(true);
    mpProgress->setButtonText("Stop");
    
    lSearchStruct.mpProgressDialog=mpProgress;
    lSearchStruct.mpString=QString2string(mpSimpleSearchEdit->text());
    lSearchStruct.mpSearchResultsNodes=&lSearchResultsNodes;
    lSearchStruct.mpSearchResultsPaths=&lSearchResultsPaths;
    
    mpCatalog->parseFileTree(findInTree, (void*)&lSearchStruct);        
        
    mpProgress->progressBar()->setValue(0);
    delete mpProgress;
    //lpProgress->setProgress(0);
    
    for(int i=0;i<lSearchResultsPaths.size();i++)
        mpSimpleSearchResults->insertItem(lSearchResultsPaths[i].c_str());
} 


void SearchBox::slotUser1()
{
    search();
} 


int findInTree(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
            void *lpParam)
{
    static int lPosInProgressBar=1;
    std::string lName;
    SearchStruct *lpSearchStruct=(SearchStruct*)lpParam;
    const char *lpPtr=lpSearchStruct->mpString;
    KProgressDialog *lpProgressDialog=lpSearchStruct->mpProgressDialog;

    if(lpProgressDialog->wasCancelled())
        return -1;
        
    lName=lrXfc.getNameOfElement(lpNode); // :fixme: utf8 -> string ?
    if(xmlStrcasestr((const xmlChar*)lName.c_str(),
       (xmlChar*)lpPtr)!=NULL)
    {
#if defined(_debug_)
        cout<<":debug: found matching node: path="<<lPath.c_str();
        cout<<", name="<<lName.c_str()<<endl;
#endif 
        lpSearchStruct->mpSearchResultsPaths->push_back(lPath+"/"+lName);
        lpSearchStruct->mpSearchResultsNodes->push_back(lpNode);
    }
    // :fixme: - check labes and other stuff
    
    lpProgressDialog->progressBar()->setValue(lPosInProgressBar/10);
    lPosInProgressBar++;
    gpApplication->processEvents();
    
    return 0;
}
