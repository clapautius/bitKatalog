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
#ifndef ADDDISKBOX_H
#define ADDDISKBOX_H

#include <kpagedialog.h>
#include <klineedit.h>
#include <qlabel.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <k3listbox.h>
#include <kprogressdialog.h>
#include <kpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <kdatepicker.h>

#include "xfcapp.h"
#include "xmlentityitem.h"


class AddDiskBox : public KPageDialog
{      
Q_OBJECT
            
public:
    AddDiskBox(Xfc *);

    ~AddDiskBox();
    
    bool catalogWasModified();

    const std::string addedDiskPath() const
    {
        return mAddedDiskPath;
    }

    bool resultOk() const
    {
        return mResultOk;
    }
    
protected:

    void disableButtons();
    
protected slots:  
    
//    virtual void close();
//    void add();
   
    virtual void slotUser1();
    
    virtual void browseButtonClicked();
    
    virtual void todayCDateButtonClicked();
    
    virtual void someDayCDateButtonClicked();
    
private:

    void layout();
    
    Xfc *mpCatalog;
    
    bool mCatalogWasModified;

    bool mResultOk;

    std::string mAddedDiskPath;
    
    //QFrame *mpPage; // on heap

    //QVBoxLayout *mpLayout1; // on heap
    
    QLabel *mpTmpLabel1, *mpTmpLabel2, *mpTmpLabel3,
        *mpTmpLabel4; // on heap
    
    QLabel *mpPathLabel; // on heap
    //KLineEdit *mpSimpleSearchEdit; // on heap
    
    KHBox *mpLayoutBox1, *mpLayoutBox2, *mpLayoutBox3,
        *mpLayoutBox4; // on heap

    // on heap
    KPushButton *mpBrowseButton, *mpTodayCDateButton, *mpSomeDayCDateButton;
            
    KProgressDialog *mpProgress;
    
    Q3VGroupBox *mpGroupBox;
    
    KLineEdit *mpDiskNameEdit, *mpDiskDescriptionEdit,
        *mpDiskCDateEdit;
    
    QCheckBox *mpAddRootCheckBox;
    QCheckBox *mpComputeSha1SumCheckBox, *mpComputeSha256SumCheckBox;
    
    KDatePicker *mpCDatePicker;

};

#endif
