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
#ifndef NEWCATALOGBOX_H
#define NEWCATALOGBOX_H


#include <kdialog.h>
#include <klineedit.h>
#include <qlabel.h>
#include <Qt3Support/q3vgroupbox.h>
#include <Qt3Support/q3hbox.h>
#include <k3listbox.h>
#include <qlayout.h>


/**
@author Tudor Pristavu
*/
class NewCatalogBox : public KDialog
{
Q_OBJECT

public:
    NewCatalogBox();

    ~NewCatalogBox();

protected slots:  

    virtual void slotUser1();
        
private:

    void layout();
    
    QFrame *mpPage; // on heap

    QVBoxLayout *mpLayout1; // on heap

    Q3HBox *mpLayoutBox1;
    
    QLabel *mpTmpLabel1;
    
    KLineEdit *mpCatalogNameEdit;

};


#endif
