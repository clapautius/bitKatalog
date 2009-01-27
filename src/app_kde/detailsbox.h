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
#ifndef DETAILSBOX_H
#define DETAILSBOX_H

#include <kpagedialog.h>

#include <k3buttonbox.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <k3listbox.h>
#include <kvbox.h>

#include "xfcapp.h"
#include "xmlentityitem.h"

/**
@author Tudor Pristavu
*/
class DetailsBox : public KPageDialog
{
    Q_OBJECT
            
public:
    DetailsBox();
    
    DetailsBox(Xfc*, std::string, XfcEntity*, Q3ListViewItem*);

    virtual ~DetailsBox();
    
    bool catalogWasModified();

public slots:
    void addLabel();
   
    void editLabel();
    
    void deleteLabel();

protected slots:  
    
    virtual void accept();
    virtual void reject();
        
private:
    
    void connectButtons();
    
    void layout();

    Xfc *mpCatalog;
    
    XfcEntity *mpXmlItem;

    std::string mCompletePath;
    
    bool mCatalogWasModified;

    Q3ListViewItem *mpListItem;
      
    QFrame *mpPage1, *mpPage2, *mpPage3;
    
    QVBoxLayout *top_layout1, *top_layout2; // on heap
                
    K3ButtonBox *mpButtonBox;
    K3ButtonBox *mpLabelButtonBox;
    
    QPushButton *mpOkButton;
    QPushButton *mpCancelButton;
    QPushButton *mpAddLabelButton;
    QPushButton *mpEditLabelButton;
    QPushButton *mpDeleteLabelButton;
    
    QLabel *mpName; // entity name - on heap
//    QLabel *mpDescription;

    Q3VGroupBox *mpLabelGroup; // on heap
      
    Q3HBox *mpDescriptionBox; // entity description on heap
    Q3HBox *mpLabelsBox; // on heap
    Q3HBox *mpLabelButtons; // on heap
    Q3HBox *mpCdateBox;
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3; // on heap

    QLabel *mpShaLabel;
    
    KLineEdit *mpDescriptionEdit; // on heap
    KLineEdit *mpCdateEdit;
            
    K3ListBox *mpLabels;
      
};

#endif
