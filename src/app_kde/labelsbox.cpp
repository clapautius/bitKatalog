/***************************************************************************
 *   Copyright (C) 2010 by Tudor Marian Pristavu                           *
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
#include "labelsbox.h"

#include <qpainter.h>
#include <qlayout.h>
#include <vector>
#include <string>
#include <klineedit.h>

#include <kmessagebox.h>
#include <kinputdialog.h>

#if defined(MTP_DEBUG)
  #include <iostream>

  using namespace std;
#endif

LabelsBox::LabelsBox()
    : KPageDialog()
{
    // :fixme: - do something
} 


LabelsBox::LabelsBox(XfcEntity *lpXmlItem, Q3ListViewItem *pListItem)
    : KPageDialog()
{
    setCaption(QString("Labels"));
    setButtons(KDialog::Ok | KDialog::Cancel);
    setModal(true);

    layout();    
    connectButtons();
}


LabelsBox::~LabelsBox()
{
}




void LabelsBox::connectButtons()
{

} 


void LabelsBox::layout()
{
    std::string str;

    resize(500,250);

    KVBox *pBox1= new KVBox();
    KPageWidgetItem *pPage1=addPage(pBox1, QString("Labels"));
    pPage1->setHeader(QString("Labels"));

    mpLabels=new QTableWidget(pBox1);
    mpLabels->setColumnCount(2);
    
    KHBox *pBox2=new KHBox(pBox1);
    mpLabelEdit=new KLineEdit(pBox2);
    mpAddButton=new QPushButton("Add", pBox2);
}


void LabelsBox::accept()
{
    KPageDialog::accept();
} 


void LabelsBox::reject()
{
    KPageDialog::reject();
} 
