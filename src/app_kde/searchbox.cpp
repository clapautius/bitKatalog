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

#include <fstream>
#include <qpainter.h>
#include <qlayout.h>
#include <qprogressbar.h>
#include <kprogressdialog.h>
#include <kmessagebox.h>
#include <kvbox.h>
#include <kpushbutton.h>
#include <QHeaderView>
#include <kmenu.h>

#include "main.h"
#include "misc.h"
#include "localfiles.h"
#include "fs.h"
#include "plugins.h"

using std::string;
using std::vector;
using std::ifstream;


// helper functions (lambda functions :) )

/**
 * @param[in] pCatalog - not used
 **/
static bool
matchesName(const QFileInfo &rFsFileInfo, const string &rXmlPath,
            const xmlNodePtr xmlNode, Xfc *pCat, KProgressDialog*)
{
    bool fsIsFile=rFsFileInfo.isFile();
    bool catalogIsFile=(Xfc::eFile == pCat->getTypeOfElement(xmlNode));
    return
        fsIsFile==catalogIsFile &&
        rFsFileInfo.fileName().toStdString()==getLastComponentOfPath(rXmlPath);
}


static bool
matchesSize(const QFileInfo &rFsFileInfo, const string &,
            const xmlNodePtr xmlNode, Xfc *pCatalog, KProgressDialog*)
{
    string size=pCatalog->getDetailsForNode(xmlNode)["size"];
    if (size.empty()) { // size unknown, skip element
        return false;
    }
    else {
        return rFsFileInfo.size() == stringToUint(size);
    }
}


static bool
matchesSha256sum(const QFileInfo &rFsFileInfo, const string &,
                 const xmlNodePtr xmlNode, Xfc *pCat, KProgressDialog *pProgress)
{
    string sha256InCatalog=pCat->getDetailsForNode(xmlNode)[SHA256LABEL];
    unsigned long int progressPos=0;
    QString path=rFsFileInfo.filePath();
    if (sha256InCatalog.empty()) {
        return true;
    }
    QString origText=pProgress->labelText();
    QString newText="Computing sha256 for ";
    if (newText.size() + path.size() > 65) {
        newText+="...";
        newText+=path.right(65 - newText.size());
    }
    else {
        newText+=path;
    }
    pProgress->setLabelText(newText);
    ifstream fin;
    bool firstTime=true, lastTime=false;
    char readBuf[102400];
    string param, value;
    gkLog<<xfcInfo<<__FUNCTION__<<": computing sha256sum for file "<<
        path.toStdString()<<eol;
    fin.open(path.toStdString().c_str(), std::ios::binary);
    if (!fin.good()) {
        gkLog<<xfcError<<"Error opening "<<path.toStdString()<<eol;
    }
    while (fin.good()) {
        fin.read(readBuf, 102400);
        if (fin.eof())
            lastTime=true;
        sha256UsingBufCallback(readBuf, fin.gcount(), firstTime, lastTime,
                               &param, &value, NULL);
        if (lastTime) {  // final iteration
            gkLog<<xfcInfo<<__FUNCTION__<<"sha256sum is "<<value<<eol;
        }
        firstTime=false;

        // progress dialog stuff
        if (pProgress->wasCancelled()) {
            value="";
            break;
        }
        pProgress->progressBar()->setValue(progressPos++);
        gpApplication->processEvents();

    } // end while(fin.good())
    fin.close();
    pProgress->setLabelText(origText);
    gkLog<<xfcDebug<<"sha256 in catalog is "<<sha256InCatalog<<
        ", sha256 on fs is "<<value<<eol;
    if (value==sha256InCatalog) {
        return true;
    }
    else {
        return false;
    }
}



SearchStruct::SearchStruct()
{
    mpProgressDialog=NULL;
}


SearchStruct::~SearchStruct()
{
    clear(false);
}


void
SearchStruct::clear(bool freeMemory)
{
    if (freeMemory) {
        if (mpProgressDialog)
            delete mpProgressDialog;
    }
    mpProgressDialog=NULL;
    mString.clear();
    mLabels.clear();
    mSearchResultsPaths.clear();
    mSearchResultsNodes.clear();
}


SearchBox::SearchBox(Xfc *lpCatalog, const vector<string> &rAllLabels)
    : KPageDialog(),
      mrAllLabels(rAllLabels)
{
    setCaption(QString("Search"));
    setButtons(KDialog::Close | KDialog::User1);
    //| KDialog::User2 | KDialog::User3);
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
    button(KDialog::User1)->setDefault(true);
    //setButtonText(KDialog::User2, "Find local by name ...");
    //setButtonText(KDialog::User3, "Find local exactly ...");

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
    mpSearchResults=new QTreeWidget(pBox1);
    mpSearchResults->setColumnCount(1);
    mpSearchResults->setSortingEnabled(false);
    mpSearchResults->setRootIsDecorated(false);
    mpSearchResults->header()->hide();
    mpSearchResults->setAlternatingRowColors(true);
    mpSearchResults->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connectButtons();
    disableButtons(); // all
    enableButtons();
    mpTextEdit->setFocus();
}


void SearchBox::connectButtons()
{
    connect(this, SIGNAL(user1Clicked()), this, SLOT(search())); // search    
    connect(mpEditLabelsButton, SIGNAL(clicked()), this, SLOT(editLabels()));
    //connect(this, SIGNAL(user2Clicked()), this, SLOT(findLocalFilesByName()));
    //connect(this, SIGNAL(user3Clicked()), this,SLOT(findLocalFilesExactly()));
}


void SearchBox::search()
{
    vector<string> *pResultsPaths=NULL;
    vector<xmlNodePtr> *pResultsNodes;

    if (mpCatalog==NULL) {
        KMessageBox::error(this, "No catalog!");
        return;
    }
    if (mpTextEdit->text()=="" && 0==mSearchLabels.size()) {
        KMessageBox::error(this, "Please specify a search string or a label");
        return;
    }
    
    // prepare search struct
    mSearchStruct.clear();
    mSearchStruct.mString=qstr2str(mpTextEdit->text());
    mSearchStruct.mLabels=mSearchLabels;
    pResultsPaths=&mSearchStruct.mSearchResultsPaths;
    pResultsNodes=&mSearchStruct.mSearchResultsNodes;

    // prepare progress dialog
    mpProgress=new KProgressDialog(this, "Searching ...", "Searching");
    mpProgress->progressBar()->setRange(0, 0);
    mpProgress->setMinimumDuration(500);
    mpProgress->setAutoClose(true);
    mpProgress->setAllowCancel(true);
    mpProgress->setButtonText("Stop");
    mSearchStruct.mpProgressDialog=mpProgress;

    mpSearchResults->clear();
    disableButtons();
    msgDebug("Starting to search. Search string is: ", mSearchStruct.mString);
    msgDebug("Labels: ", vectorWStringsToString(mSearchStruct.mLabels));
    msgDebug("Start time: ", getTimeSinceMidnight());
    mpCatalog->parseFileTree(findInTree, (void*)&mSearchStruct);
    msgDebug("Finish time: ", getTimeSinceMidnight());
    mpProgress->progressBar()->setValue(0);
    delete mpProgress;
    mSearchStruct.mpProgressDialog=NULL;
    msgDebug("Search finished");

    QTreeWidgetItem *pItem=NULL;
    if (pResultsPaths->size()>0) {
        for (unsigned int i=0;i<pResultsPaths->size();i++) {
            pItem=new QTreeWidgetItem(
                QStringList(str2qstr(pResultsPaths->at(i))));
            mpSearchResults->addTopLevelItem(pItem);
        }
        enableButtons();
    }
    else {
        pItem=new QTreeWidgetItem(QStringList("Nothing found!"));
        mpSearchResults->addTopLevelItem(pItem);
        enableButtons();
    }
    mpSearchResults->resizeColumnToContents(0);
} 


void
SearchBox::editLabels()
{
    gkLog<<xfcDebug<<__FUNCTION__<<eol;
    string str;
    LabelsBox *pLabelsBox=new LabelsBox(
        vectWstringToVectWQString(mrAllLabels),
        vectWstringToVectWQString(mSearchLabels), false);
    if (QDialog::Accepted == pLabelsBox->exec()) {
        mSearchLabels=vectWQStringToVectWstring(pLabelsBox->getSelectedLabels());
        mpLabelsEdit->setText(str2qstr(vectorWStringsToString(mSearchLabels)));
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
    static unsigned long int lPosInProgressBar=1;
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
        lpSearchStruct->mSearchResultsPaths.push_back(lPath+"/"+lName);
        lpSearchStruct->mSearchResultsNodes.push_back(lpNode);
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
                        lpSearchStruct->mSearchResultsPaths.push_back(fName);
                        lpSearchStruct->mSearchResultsNodes.push_back(lpNode);
                        found=true;
                    }
        }
    }
    if (lPosInProgressBar%100) {
        lpProgressDialog->progressBar()->setValue(lPosInProgressBar/100);
        gpApplication->processEvents();
    }
    lPosInProgressBar++;
    return 0;
}


void
SearchBox::findLocalFiles(bool exactly)
{
    if (mSearchStruct.mSearchResultsPaths.size()>0) {
        vector<QFileInfo> results;
        QString startPath=cfgGetSearchStartPath();

        // prepare progress dialog
        KProgressDialog *pProgress=new KProgressDialog(
            this, "Searching ...",
            QString("Searching in folder ")+startPath);
        pProgress->progressBar()->setRange(0, 0);
        pProgress->setMinimumDuration(500);
        pProgress->setAutoClose(true);
        pProgress->setAllowCancel(true);
        pProgress->setButtonText("Stop");

        gkLog<<xfcInfo<<"Start searching local storage, starting from "
             <<qstr2cchar(startPath)<<eol;
        QDirIterator it(startPath,
                  QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
        try {
            if (exactly)
                matchBySizeAndSha256(it, results, pProgress);
            else
                matchByName(it, results, pProgress);
        }
        catch (string e) {
            KMessageBox::error(this,
                               QString("Something bad happened. Error is: ")+
                               e.c_str());
        }
        catch (...) {
            KMessageBox::error(this,
                               QString("Something bad happened. Unknown error"));
        }

        delete pProgress;
        gkLog<<xfcInfo<<"Search finished, displaying results"<<eol;
        if (results.size()>0) {
            LocalFilesBox *pLocalFilesBox = new LocalFilesBox(results);
            pLocalFilesBox->exec();
        }
        else {
            KMessageBox::information(this, "No local files found");
        }
    }
    else {
        KMessageBox::error(this, "Nothing to search");
    }
}


void
SearchBox::matchByName(QDirIterator &rIt, vector<QFileInfo> &rResult,
                       KProgressDialog *pProgressDlg)
{
    vector<MatchFuncType> functions;
    functions.push_back(matchesName);
    return matchByRelation(functions, rIt, rResult, pProgressDlg);
}


void
SearchBox::matchBySizeAndSha256(QDirIterator &rIt, vector<QFileInfo> &rResult,
                                KProgressDialog *pProgressDlg)
{
    vector<MatchFuncType> functions;
    functions.push_back(matchesSize);
    functions.push_back(matchesSha256sum);
    return matchByRelation(functions, rIt, rResult, pProgressDlg);
}


/**
 * @param[in] funcs : vector with pointers to lambda functions that check if
 *   local files match xml parameters.
 * @param[in] rIt : a QDirIterator to get files from a directory recursively.
 * @param[out] rResults : the files that match the required conditions.
 * @param[in] pProgressDlg : optional - a KProgressDialog that should be updated
 *   periodically.
 **/
void
SearchBox::matchByRelation(vector<MatchFuncType> funcs,
                           QDirIterator &rIt, vector<QFileInfo> &rResult,
                           KProgressDialog *pProgressDlg)
{
    unsigned long int progressPos=1;
    vector<string> filesToSearch;
    vector<xmlNodePtr> nodesToSearch;

    // get only selected items
    for (uint i=0; i<mSearchStruct.mSearchResultsPaths.size(); i++) {
        if (mSearchEltSelected[i]) {
            filesToSearch.push_back(mSearchStruct.mSearchResultsPaths[i]);
            nodesToSearch.push_back(mSearchStruct.mSearchResultsNodes[i]);
        }
    }
    while (rIt.hasNext() && !filesToSearch.empty()) {
        // progress stuff
        progressPos++;
        if (pProgressDlg && progressPos%100==0) {
            if (pProgressDlg->wasCancelled()) {
                gkLog<<xfcInfo<<__FUNCTION__<<": cancelled."<<eol;
                return;
            }
            pProgressDlg->progressBar()->setValue(progressPos/100);
            gpApplication->processEvents();
        }

        QString file=rIt.next();
        QFileInfo fInfo=rIt.fileInfo();
        for (uint i=0; i<filesToSearch.size(); i++) {
            bool matchesAll=true;
            if (funcs[0](fInfo, filesToSearch[i], nodesToSearch[i], mpCatalog,
                         pProgressDlg)) {
                gkLog<<xfcDebug<<"Catalog file "<<filesToSearch[i];
                gkLog<<" matches local file "<<file.toStdString()<<eol;
                for (uint j=1; j<funcs.size(); j++)
                    if (!funcs[j](fInfo, filesToSearch[i], nodesToSearch[i],
                                  mpCatalog, pProgressDlg)) {
                        matchesAll=false;
                        break;
                    }
                if (matchesAll) {
                    gkLog<<xfcDebug<<"  matches all, good"<<eol;
                    rResult.push_back(fInfo);
                    filesToSearch.erase(filesToSearch.begin()+i);
                    nodesToSearch.erase(nodesToSearch.begin()+i);
                    i--;
                    if (filesToSearch.size()==0) {
                        gkLog<<xfcDebug<<"No more files to search"<<eol;
                        break;
                    }
                }
                else {
                    gkLog<<xfcDebug<<"  some elements do not match"<<eol;
                }
            }
        }
    }
    pProgressDlg->progressBar()->setValue(0);
}


void
SearchBox::findLocalFilesByName()
{
    return findLocalFiles(false);
}


void
SearchBox::findLocalFilesExactly()
{
    return findLocalFiles(true);
}


void
SearchBox::contextMenuEvent(QContextMenuEvent* event)
{
    QList<QTreeWidgetItem *> listSel=mpSearchResults->selectedItems();
    bool haveSelected=(listSel.size()>0);
    KMenu *pContextMenu=new KMenu();
    QAction *pAct=NULL;
    pAct=pContextMenu->addAction("Select all");
    connect(pAct, SIGNAL(triggered()), this, SLOT(selectAll()));
    pAct=pContextMenu->addAction("Unselect all");
    if (haveSelected)
        connect(pAct, SIGNAL(triggered()), this, SLOT(unselectAll()));
    else
        pAct->setEnabled(false);
    pContextMenu->addSeparator();
    pAct=pContextMenu->addAction("Find selected by name (on local storage)");
    if (haveSelected)
        connect(pAct, SIGNAL(triggered()), this, SLOT(findLocalFilesByName()));
    else
        pAct->setEnabled(false);
    pAct=pContextMenu->addAction("Find selected exactly (on local storage)");
    if (haveSelected)
        connect(pAct, SIGNAL(triggered()), this, SLOT(findLocalFilesExactly()));
    else
        pAct->setEnabled(false);
    if (haveSelected) {
        // mark selected items
        vector<string> vectSel;
        for (int i=0; i<listSel.size(); i++)
            vectSel.push_back(qstr2str(listSel[i]->text(0)));
        mSearchEltSelected.clear();
        for (uint i=0; i<mSearchStruct.mSearchResultsPaths.size(); i++) {
            if (contains(vectSel, mSearchStruct.mSearchResultsPaths[i])) {
                mSearchEltSelected.push_back(true);
            }
            else {
                mSearchEltSelected.push_back(false);
            }
        }
    }
    pContextMenu->exec(event->globalPos());
}


void
SearchBox::selectAll()
{
    mpSearchResults->selectAll();
}


void
SearchBox::unselectAll()
{
    mpSearchResults->clearSelection();
}
