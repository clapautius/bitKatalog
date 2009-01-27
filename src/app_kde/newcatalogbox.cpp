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
#include "newcatalogbox.h"


#include <kmessagebox.h>

#include "main.h"
#include "xfc.h"

NewCatalogBox::NewCatalogBox()
    : KDialog()
{
    setCaption(QString("New catalog"));
    setButtons(KDialog::Cancel | KDialog::User1);
    layout();
}


NewCatalogBox::~NewCatalogBox()
{
}


void NewCatalogBox::layout()
{
    resize(300,250);
        
    setButtonText(KDialog::User1, "Create"); // :fixme: - replace with a kguiitem
    
    QWidget *pPage=new QWidget(this); //=plainPage();    
    mpLayout1 = new QVBoxLayout(pPage);
    
    mpLayoutBox1=new Q3HBox(pPage);
    mpLayout1->addWidget(mpLayoutBox1);
    mpTmpLabel1=new QLabel("Catalog description: ", mpLayoutBox1);
    mpCatalogNameEdit=new KLineEdit(mpLayoutBox1);
}


void NewCatalogBox::slotUser1()
{
    Xfc *lpXfc=new Xfc();
    lpXfc->createNew(QString2string(mpCatalogNameEdit->text()));
    gpView->setCatalog(lpXfc);   
    gCatalogState=1;
    gpMainWindow->setCatalogPath(std::string(""));
    gpMainWindow->updateTitle(true);
    close();
}

