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
#include <k3listbox.h>
#include <khbox.h>

#include "xfcapp.h"
#include "xmlentityitem.h"

/**
@author Tudor Pristavu
*/
class SearchBox : public KPageDialog
{
    Q_OBJECT
            
public:
    SearchBox(Xfc *, const std::vector<std::string>&);

    ~SearchBox();

protected:

    void disableButtons();
    void enableButtons();
    
protected slots:  
    
    void search();
    
    virtual void slotUser1();

    virtual void editLabels();

private:

    void connectButtons();
    
    void layout();
    
    
    Xfc *mpCatalog;
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3; // on heap
    
    KLineEdit *mpTextEdit, *mpLabelsEdit; // on heap
    
    KHBox *mpSimpleSearchBox; // on heap
    
    K3ListBox *mpSimpleSearchResults;
    
    KProgressDialog *mpProgress;

    QPushButton *mpEditLabelsButton;

    std::vector<std::string> mSearchLabels;
    
    const std::vector<std::string> & mrAllLabels;
};


class SearchStruct
{
public:
    KProgressDialog *mpProgressDialog;
    std::string mString;
    std::vector<std::string> mLabels;
    std::vector<std::string> *mpSearchResultsPaths;
    std::vector<xmlNodePtr> *mpSearchResultsNodes;
};

int findInTree(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
                void *lpParam);

#endif
