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

#include <kdialog.h>
#include <klineedit.h>
#include <qlabel.h>
#include <kprogressdialog.h>
#include <QBoxLayout>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <k3listbox.h>

#include "xfcapp.h"
#include "xmlentityitem.h"

/**
@author Tudor Pristavu
*/
class SearchBox : public KDialog
{
    Q_OBJECT
            
public:
    SearchBox(Xfc *);

    ~SearchBox();

protected slots:  
    
//    virtual void close();
    void search();
    
    virtual void slotUser1();

private:

    void connectButtons();
    
    void layout();
    
    
    Xfc *mpCatalog;
    
    QFrame *mpPage1, *mpPage2; // on heap
    
    QVBoxLayout *mpLayout1; // on heap
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3; // on heap
    
    KLineEdit *mpSimpleSearchEdit; // on heap
    
    Q3HBox *mpSimpleSearchBox; // on heap
    
    K3ListBox *mpSimpleSearchResults;
    
    KProgressDialog *mpProgress;
};
    
class SearchStruct
{
public:
    KProgressDialog *mpProgressDialog;
    const char *mpString;
    std::vector<std::string> *mpSearchResultsPaths;
    std::vector<xmlNodePtr> *mpSearchResultsNodes;
};

int findInTree(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
                void *lpParam);

#endif
