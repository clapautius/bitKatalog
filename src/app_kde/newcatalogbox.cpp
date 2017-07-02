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
#include "newcatalogbox.h"

#include <kmessagebox.h>

#include "main.h"
#include "xfc.h"

NewCatalogBox::NewCatalogBox() : KPageDialog()
{
    setCaption(QString("New catalog"));
    setButtons(KDialog::Cancel | KDialog::User1);
    setModal(true);
    layout();
}

NewCatalogBox::~NewCatalogBox() {}

void NewCatalogBox::layout()
{
    resize(300, 250);
    setButtonText(KDialog::User1, "Create");  // :fixme: - replace with a kguiitem
    mpLayoutBox1 = new KVBox();
    KPageWidgetItem *pPage = addPage(mpLayoutBox1, QString("New catalog"));
    pPage->setHeader(QString("New catalog"));
    mpLayoutBox2 = new KHBox(mpLayoutBox1);
    mpTmpLabel1 = new QLabel("Catalog description: ", mpLayoutBox2);
    mpCatalogNameEdit = new KLineEdit(mpLayoutBox2);
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotUser1()));
}

void NewCatalogBox::slotUser1()
{
    Xfc *lpXfc = new Xfc();
    lpXfc->createNew(qstr2cchar(mpCatalogNameEdit->text()));
    gpView->setCatalog(lpXfc);
    gCatalogState = 1;
    gpMainWindow->setCatalogPath(std::string(""));
    gpMainWindow->updateTitle(true);
    close();
}
