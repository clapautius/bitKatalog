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
#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <kpagedialog.h>
#include <klineedit.h>
#include <qlabel.h>
#include <kprogressdialog.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <QTreeWidget>
#include <khbox.h>
#include <QDirIterator>

#include "xfcapp.h"
#include "xmlentityitem.h"

/**
@author Tudor Pristavu
*/


typedef bool (*MatchFuncType)(const QFileInfo &, const std::string &,
                              const xmlNodePtr, Xfc *);


class SearchStruct
{
public:
    SearchStruct();
    
    ~SearchStruct();
    
    void clear(bool freeMemory=true);

    // public members - not very nice :fixme:
    KProgressDialog *mpProgressDialog;
    std::string mString;
    std::vector<std::string> mLabels;
    std::vector<std::string> mSearchResultsPaths;
    std::vector<xmlNodePtr> mSearchResultsNodes;

};


class SearchBox : public KPageDialog
{
    Q_OBJECT
            
public:
    SearchBox(Xfc *, const std::vector<std::string>&);

    ~SearchBox();

protected:

    void disableButtons();
    void enableButtons();
    virtual void findLocalFiles(bool exactly=false);
    
protected slots:  
    
    virtual void search();
    
    virtual void findLocalFilesByName();

    virtual void findLocalFilesExactly();

    virtual void editLabels();

    virtual void contextMenuEvent(QContextMenuEvent* event);

private:

    void connectButtons();
    
    void layout();

    void matchByRelation(std::vector<MatchFuncType>,
                         QDirIterator &rIt,
                         std::vector<QFileInfo> &rResult,
                         KProgressDialog *pProgress=NULL);
    
    void matchByName(QDirIterator &rIt,
                     std::vector<QFileInfo> &rResult,
                     KProgressDialog *pProgress=NULL);

    void matchBySizeAndSha256(QDirIterator &rIt,
                              std::vector<QFileInfo> &rResult,
                              KProgressDialog *pProgress=NULL);

    Xfc *mpCatalog;
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3; // on heap
    
    KLineEdit *mpTextEdit, *mpLabelsEdit; // on heap
    
    KHBox *mpSimpleSearchBox; // on heap
    
    QTreeWidget *mpSearchResults;
    
    KProgressDialog *mpProgress;

    QPushButton *mpEditLabelsButton;

    std::vector<std::string> mSearchLabels;
    
    const std::vector<std::string> & mrAllLabels;

    SearchStruct mSearchStruct;

    std::vector<bool> mSearchEltSelected;
};


int findInTree(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
                void *lpParam);

#endif
