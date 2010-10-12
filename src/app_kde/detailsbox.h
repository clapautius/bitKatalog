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
#ifndef DETAILSBOX_H
#define DETAILSBOX_H

#include <kpagedialog.h>

#include <k3buttonbox.h>
#include <kpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <QTreeWidget>
#include <k3listbox.h>
#include <kvbox.h>
#include <klineedit.h>

#include "main.h"
#include "xfcapp.h"
#include "xmlentityitem.h"


class DetailsBox : public KPageDialog
{
    Q_OBJECT
            
public:
    DetailsBox();
    
    DetailsBox(Xfc*, std::string, XfcEntity*, QTreeWidgetItem*,
               const std::vector<QString> &rAllLabels);

    virtual ~DetailsBox();
    
    bool catalogWasModified() const;

    bool labelsWereModified() const;

public slots:
   
    void editLabels();
    
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
    bool mLabelsWereModified;

    QTreeWidgetItem *mpListItem;
      
    QVBoxLayout *top_layout1, *top_layout2; // on heap
                
    K3ButtonBox *mpButtonBox;
    K3ButtonBox *mpLabelButtonBox;
    
    QPushButton *mpOkButton;
    QPushButton *mpCancelButton;
    QPushButton *mpEditLabelsButton;

    QLabel *mpName; // entity name - on heap

    Q3VGroupBox *mpLabelGroup; // on heap
      
    KHBox *mpCdateBox;
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3; // on heap

    QLabel *mpSha1Label, *mpSha256Label, *mpSizeLabel;
    
    KLineEdit *mpDescriptionEdit; // on heap
    KLineEdit *mpCdateEdit;
            
    K3ListBox *mpLabels;

    const std::vector<QString> & mrAllLabels;
    std::vector<QString> mCurrentLabels;
};


bool contains(std::vector<QString> vect, QString elt);


#endif
